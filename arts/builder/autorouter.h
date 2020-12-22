	/*

	Copyright (C) 1998 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

#ifndef __AUTOROUTER_H_
#define __AUTOROUTER_H_

// If you get into trouble with threading (random crashes), you can configure
// things with --disable-threading, which should fix everything by not using
// threads
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qstring.h>
#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#endif

#include <list>

class tryinfo
{
public:
	int x1,x2,y1,y2,score,depth;
	QString history;
};

class ARCommand;

class AutoRouter
{
public:
	enum {none=0,up=1,down=2,left=4,right=8,head=16,tail=32,solid=64};

protected:
	enum {OWNER_UD=0, OWNER_LR=1};

	struct Field
	{
		long data;
		long minscore;
		long penalty;
		long owner[2];
	} **field, **completefield;

	long directionmask[256];
	int ownerindex[256];

	long newowner;
	long currentowner;

	bool _needRedraw;

	QString minhistory;
	std::list<tryinfo> trylist[1024];
	int triesleft;

	int width,height;
	long *prp, *prstart;

/****** thread stuff *****/
#ifdef HAVE_LIBPTHREAD
	pthread_mutex_t mutex_sync;
	pthread_mutex_t mutex_queue;

	pthread_t		route_thread;
#endif
	std::list<ARCommand *> command_queue;

	bool			thread_terminate_now;
/*************************/

	void try_connect(int x1, int y1, int x2, int y2, int score, int depth, QString history);
	void q_try_connect(int x1, int y1, int x2, int y2, int score, int depth, QString history);

public:
	long get(int x,int y);
	void getowners(int x,int y, long& ud_owner, long& lr_owner);
	void set(int x1,int y1, int x2, int y2, long lt);
	void clear();
	void sync();
	long connect(int x1, int y1, int x2, int y2, long owner);

	void queue(ARCommand *command);
	void thread_connect(int x1, int y1, int x2, int y2, long owner);
	void thread_set(int x1,int y1, int x2, int y2, long lt);
	void thread_clear();
	void thread_sync();

	void thread_command_loop();

	// queries _needRedraw flag and deletes it
	// (assuming that the client is smart and redraws when getting true ;)
	bool needRedraw();

	AutoRouter(int width, int height);
	~AutoRouter();
};

class ARCommand
{
public:
	virtual void execute(AutoRouter *autorouter) = 0;
	virtual bool isDestructive();
};

class ARClearCommand :public ARCommand
{
public:
	void execute(AutoRouter *autorouter);
	bool isDestructive();
};

class ARSyncCommand :public ARCommand
{
public:
	void execute(AutoRouter *autorouter);
};

class ARConnectCommand :public ARCommand
{
	int _x1, _y1, _x2, _y2;
	long _owner;
public:
	ARConnectCommand(int x1, int y1, int x2, int y2, long owner);
	void execute(AutoRouter *autorouter);
};

class ARSetCommand :public ARCommand
{
private:
	int _x1,_y1, _x2, _y2;
	long _lt;
public:
	ARSetCommand(int x1,int y1, int x2, int y2, long lt);
	void execute(AutoRouter *autorouter);
};

#endif
