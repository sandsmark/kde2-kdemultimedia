    /*

    Copyright (C) 1999 Stefan Westerfeld
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

#ifndef _AUDIOMANAGER_H_
#define _AUDIOMANAGER_H_

#include <artsflow.h>
#include "qwidget.h"
#include "qlabel.h"
#include "qlayout.h"
#include "qpixmap.h"
#include "qbitmap.h"
#include <qlistview.h>
#include "qobject.h"
#include "qframe.h"

class GuiAudioManagerProxy;

class Gui_AUDIO_MANAGER : public QWidget
{
	Q_OBJECT
protected:
	QWidget *ParentWidget;
	QListView *listview;
	GuiAudioManagerProxy *proxy;
	Arts::AudioManager AudioManager;

	bool inDialog;
	unsigned long changes;
	int x,y;
public:
	Gui_AUDIO_MANAGER();
	~Gui_AUDIO_MANAGER();

	void setParent(QWidget *Parent,QBoxLayout *layout);

	void edit(QListViewItem *item);
	void closeEvent(QCloseEvent *e);
signals:
	void closed();
public slots:
	void tick();
};

class GuiAudioManagerProxy :public QObject {
	Q_OBJECT
	Gui_AUDIO_MANAGER *gim;
public:
	GuiAudioManagerProxy(Gui_AUDIO_MANAGER *gim);
public slots:
	void edit(QListViewItem *item);
};

class AudioManagerItem : public QListViewItem {
	long _ID;
public:
	AudioManagerItem(QListView *parent, QString a, QString b,
			              QString c, long ID);
	~AudioManagerItem();

	long ID();
};

#endif
