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

#include "autorouter.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arts/debug.h>

#ifndef HAVE_LIBPTHREAD
#define pthread_create(a,b,c,d) ;
#define pthread_join(a,b) ;
#define pthread_mutex_lock(a) ;
#define pthread_mutex_unlock(a) ;
#define pthread_mutex_init(a,b) ;
#define pthread_attr_init(a) ;
#endif

using namespace std;

void *startroute(void *where)
{
	((AutoRouter *)where)->thread_command_loop();
	// just to prevent the compiler warning
	return 0;
}

AutoRouter::AutoRouter(int width, int height)
{
	int i;
	const int PRSIZE = 16; // size of the pseudo random table

//	field = new Field[width][height];	  anybody knows how to do this correct?

	field = (Field **)malloc(sizeof(Field *)*width);
	completefield = (Field **)malloc(sizeof(Field *)*width);

	for(i=0;i<width;i++)
	{
		field[i] = (Field *)malloc(sizeof(Field)*height);
		completefield[i] = (Field *)calloc(1,sizeof(Field)*height);
	}

	for(i=0;i<255;i++) directionmask[i] = 0xffff;

	directionmask['d'] = ~(left | right);
	directionmask['u'] = ~(left | right);
	directionmask['l'] = ~(up | down);
	directionmask['r'] = ~(up | down);

	ownerindex['d'] = OWNER_UD;
	ownerindex['u'] = OWNER_UD;
	ownerindex['l'] = OWNER_LR;
	ownerindex['r'] = OWNER_LR;

	prstart = (long *)malloc((PRSIZE + 1) * sizeof(long));
	prp = prstart;

	int remaining[4];	
	for(i=0;i<PRSIZE;i++)
	{
		if((i & 3) == 0)
		{
			int j;
			for (j=0;j<4;j++) remaining[j] = j+1;
#ifdef AR_DEBUG
			arts_debug("");
#endif
		}

		int rnd;
		do
		{
			rnd = rand()&3;
		} while(!remaining[rnd]);

		*prp++ = remaining[rnd];
#ifdef AR_DEBUG
		arts_debug("%d ",remaining[rnd]);
#endif
		remaining[rnd]=0;
	}
	*prp = 0;
#ifdef AR_DEBUG
	arts_debug("");
#endif
	
	this->width = width;
	this->height = height;

	thread_terminate_now = false; 		// not yet
	newowner = 1;

	thread_clear();

#ifdef HAVE_LIBPTHREAD
	pthread_attr_t attrs;

	pthread_attr_init(&attrs);
	pthread_mutex_init(&mutex_sync,0);
	pthread_mutex_init(&mutex_queue,0);

	pthread_create(&route_thread,&attrs,startroute,this);
#endif

	arts_debug("AR UP...");
}

AutoRouter::~AutoRouter()
{
	void *rc;
	pthread_mutex_lock(&mutex_queue);
	thread_terminate_now = true;
	pthread_mutex_unlock(&mutex_queue);
	pthread_join(route_thread,&rc);
	arts_debug("AR DOWN...");
}

void AutoRouter::queue(ARCommand *command)
{
#ifdef HAVE_LIBPTHREAD
	// threaded execution
	pthread_mutex_lock(&mutex_queue);
	if(command->isDestructive())
	{
		// ok, then we can kill the whole list, since this will clear
		// the whole results anyway

		while(command_queue.size() > 0)
		{
			delete *command_queue.begin();
			command_queue.pop_front();
		}
	}
	command_queue.push_back(command);
	pthread_mutex_unlock(&mutex_queue);
#else
	// immediate execution
	command->execute(this);
	delete command;
#endif
}

void AutoRouter::thread_command_loop()
{
	ARCommand *command;
	while(1)
	{
		command = 0;


		pthread_mutex_lock(&mutex_queue);
		if(command_queue.size() > 0)
		{
			command = (*command_queue.begin());
			command_queue.pop_front();
		}
		else
		{
			if(thread_terminate_now) return;
		}
		pthread_mutex_unlock(&mutex_queue);

		if(command)
		{
			command->execute(this);
			delete command;
		} 
		else usleep(40000);
	}
}

void AutoRouter::sync()
{
	queue(new ARSyncCommand());
}

void ARSyncCommand::execute(AutoRouter *router)
{
	router->thread_sync();
}

void AutoRouter::thread_sync()
{
	int i;
	pthread_mutex_lock(&mutex_sync);
	for(i=0;i<width;i++) memcpy(completefield[i],field[i],sizeof(Field)*height);
	_needRedraw = true;
	pthread_mutex_unlock(&mutex_sync);
}

bool AutoRouter::needRedraw()
{
	bool result;

	pthread_mutex_lock(&mutex_sync);
	result = _needRedraw;
	_needRedraw = false;
#ifdef AR_DEBUG
	if(result) arts_debug("NEED REDRAW NOW!");
#endif
	pthread_mutex_unlock(&mutex_sync);

	return result;
}

void AutoRouter::clear()
{
	queue(new ARClearCommand());
}

bool ARCommand::isDestructive()
{
	return false;
}

bool ARClearCommand::isDestructive()
{
	return true;
}

void ARClearCommand::execute(AutoRouter *router)
{
	router->thread_clear();
}

void AutoRouter::thread_clear()
{
	int x,y;

	// if there is lots of movement on the screen this usleep will prevent
	// lots of calculations to be done; normally you shouldn't need it as
	// the calculations will take place in a seperate thread and don't
	// bother the gui
	//
	// usleep(300000);

	for (x=0;x<width;x++)
		for (y=0;y<height;y++)
		{
			field[x][y].data = none;
			field[x][y].penalty = 0;
			field[x][y].owner[0] = -1;
			field[x][y].owner[1] = -1;
		}

	//newowner = 1;
}

long AutoRouter::get(int x,int y)
{
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);

	pthread_mutex_lock(&mutex_sync);
	long result = completefield[x][y].data;
	pthread_mutex_unlock(&mutex_sync);
	return(result);
}

void AutoRouter::getowners(int x, int y, long& ud_owner, long& lr_owner)
{
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);

	pthread_mutex_lock(&mutex_sync);
	ud_owner = completefield[x][y].owner[OWNER_UD];
	lr_owner = completefield[x][y].owner[OWNER_LR];
	pthread_mutex_unlock(&mutex_sync);
}

void AutoRouter::set(int x1,int y1, int x2, int y2, long lt)
{
	queue(new ARSetCommand(x1,y1,x2,y2,lt));
}

ARSetCommand::ARSetCommand(int x1,int y1, int x2, int y2, long lt)
{
	_x1 = x1; _y1 = y1; _x2 = x2; _y2 = y2; _lt = lt;
}

void ARSetCommand::execute(AutoRouter *router)
{
	router->thread_set(_x1,_y1,_x2,_y2,_lt);
}

void AutoRouter::thread_set(int x1,int y1, int x2, int y2, long lt)
{
	int x,y;
	for(x=x1; x <= x2; x++)
	{
		for(y=y1; y <= y2; y++)
		{
			assert(x >= 0 && x < width);
			assert(y >= 0 && y < height);

			if(lt & solid)
			{
				if((y-1) >= 0)
					field[x][y-1].penalty += 5;

				if((y-2) >= 0)
					field[x][y-2].penalty += 2;

				if((y+1) < height)
					field[x][y+1].penalty += 5;

				if((y+2) < height)
					field[x][y+2].penalty += 2;
			}
			field[x][y].data = lt;
			field[x][y].owner[0] = 0;
			field[x][y].owner[1] = 0;		// don't pass
		}
	}
}


// gms = global min score: score of the best connection that was achieved
// new routing steps are only tried if their score does not exceed gms

int gms, gmsd;

long AutoRouter::connect(int x1, int y1, int x2, int y2, long owner)
{
	if(owner == 0) owner = newowner++;

	queue(new ARConnectCommand(x1,y1,x2,y2,owner));

	return(owner);
}

ARConnectCommand::ARConnectCommand(int x1, int y1, int x2, int y2, long owner)
{
	_x1 = x1; _y1 = y1; _x2 = x2; _y2 = y2; _owner = owner;
}

void ARConnectCommand::execute(AutoRouter *router)
{
	router->thread_connect(_x1,_y1,_x2,_y2,_owner);
}

void AutoRouter::thread_connect(int x1, int y1, int x2, int y2, long owner)
{
	int x,y;
	long dat_source, dat_dest;

	currentowner = owner;

	// clear data(source) & data(dest) first and restore later, since they
	// might be solid

#ifdef AR_DEBUG
	arts_debug("-field[x1][y1].owner[0..1] = %ld,%ld",field[x1][y1].owner[0],
													field[x1][y1].owner[1]);
	arts_debug("-field[x2][y2].owner[0..1] = %ld,%ld",field[x2][y2].owner[0],
													field[x2][y2].owner[1]);
#endif
	dat_source = field[x1][y1].data; field[x1][y1].data = none;
	dat_dest   = field[x2][y2].data; field[x2][y2].data = none;

	for (x=0;x<width;x++)
		for (y=0;y<height;y++)
			field[x][y].minscore = 20000;

#ifdef AR_DEBUG
	arts_debug("autorouter: trying to connect %d,%d with %d,%d (owner %ld)",
								x1,y1,x2,y2,owner);
#endif
	QString history("");

	gmsd = 0; gms=20000;

	int activelist = 0;
	triesleft = 0;

	prp = prstart;
	q_try_connect(x1,y1,x2,y2,0,0,history);

	while(triesleft)
	{
		list<tryinfo>::iterator ti,minti;
		int mintry=20000;

		while(!trylist[activelist].size()) activelist++;
		list<tryinfo>& activetrylist = trylist[activelist];

		minti = activetrylist.begin();
		ti = activetrylist.begin();
		while(ti != activetrylist.end())
		{
			if((*ti).score < mintry)
			{
				mintry = (*ti).score;
				minti = ti;
			}
			ti++;
		}

		try_connect((*minti).x1, (*minti).y1,
					(*minti).x2, (*minti).y2, (*minti).score, (*minti).depth, (*minti).history);
		activetrylist.erase(minti);
		triesleft--;
	}
	//arts_debug("gmsd= %d",gmsd);

	field[x1][y1].data = dat_source;
	field[x2][y2].data = dat_dest;

	if(gms != 20000)
	{
		//arts_debug("minhistory for this connection is %s",minhistory.data());
		//arts_debug("minscore for that was %d",gms);

		char *walk = (char*)minhistory.ascii();

		int x = x1;
		int y = y1;

		while(*walk)
		{
			field[x][y].owner[ownerindex[*walk]] = currentowner;
			switch(*walk)
			{
				case 'd':	field[x][y++].data |= down;
							field[x][y].data |= up;
						break;
				case 'u':	field[x][y--].data |= up;
							field[x][y].data |= down;
						break;
				case 'l':	field[x--][y].data |= left;
							field[x][y].data |= right;
						break;
				case 'r':	field[x++][y].data |= right;
							field[x][y].data |= left;
						break;
			}
			field[x][y].owner[ownerindex[*walk]] = currentowner;
			walk++;
		}
	}
	else
	{
#ifdef AR_DEBUG
		arts_debug("!! sorry, this connection is impossible !!");
#endif
	}
}

void AutoRouter::q_try_connect(int x1, int y1, int x2, int y2, int score, int depth, QString history)
{
	tryinfo newtry;

	newtry.x1 = x1;
	newtry.x2 = x2;
	newtry.y1 = y1;
	newtry.y2 = y2;
	newtry.score = score;
	newtry.depth = depth;
	newtry.history = history;

	int targetlist = newtry.score/5;
	if(targetlist > 1023) targetlist = 1023;

	trylist[targetlist].push_back(newtry);
	triesleft++;
}

void AutoRouter::try_connect(int x1, int y1, int x2, int y2, int score, int depth, QString history)
{
	char *walk = (char*)history.ascii();

// check if we can go here:

	if(x1 < 0 || x1 >= width) return;
	if(y1 < 0 || y1 >= width) return;

	if(depth > 0)
	{
		if(field[x1][y1].data != 0) score += 100;
		// going over a field where already connections are is bad...

		if(directionmask[walk[depth-1]] & field[x1][y1].data)
		{
			// someone already uses that field... we can continue
			// only if the connection has the same sourceport

			long fieldowner = field[x1][y1].owner[ownerindex[walk[depth-1]]];

			if(fieldowner != -1)		// used?
			{
				if(fieldowner != currentowner) return;
				score -= 100;
				// ... or a good idea, if the connections are from the
				// same owner
			}
		}
	}

	//score++;
	score += abs(x1-x2) + abs(y1-y2);
	score += field[x1][y1].penalty;

	if(depth > 2)
		if(walk[depth-2] != walk[depth-1]) score += 100;
	
	if(field[x1][y1].minscore <= score) return;
	if(score > gms) { gmsd++; return; }

	field[x1][y1].minscore = score;

// check if we are where we wanted to be:

	if(x1 == x2 && y1 == y2) {
		// success

		if(score < gms)
		{
			// best solution until now
			minhistory = history;
			gms = score;
		}
		return;
	}

// search next place to go; take some pseudo random direction order
// this method improves search speed ;)

	depth++;

	int i;
	for(i=0;i<4;i++)
	{
		switch(*prp++)
		{
			case 0: i--;
					prp = prstart;
				break;
			case 1: q_try_connect(x1-1,y1,x2,y2,score,depth,history+"l");
				break;
			case 2: q_try_connect(x1+1,y1,x2,y2,score,depth,history+"r");
				break;
			case 3: q_try_connect(x1,y1-1,x2,y2,score,depth,history+"u");
				break;
			case 4: q_try_connect(x1,y1+1,x2,y2,score,depth,history+"d");
				break;
		}
	}
}

