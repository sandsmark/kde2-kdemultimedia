    /*

    Copyright (C) 2000 Stefan Westerfeld
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

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#include "midimanagerview.h"
#define protected public
#include "midimanagerwidget.h"
#undef protected
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <stdio.h>
#include <kaction.h>
#include <klocale.h>
#include <soundserver.h>
#include "artsmidi.h"
#include "artsmodules.h"
#include "midiportdlg.h"
#include "midiinstdlg.h"

using namespace Arts;
using namespace std;

class MidiManagerItem : public QListBoxText {
public:
	MidiClientInfo info;

	MidiManagerItem(const MidiClientInfo &info) :info(info)
	{
	}
	QString text () const {
		return QString::fromUtf8(info.title.c_str());
	}

};

class ConnectionWidget : public QWidget {
public:
	MidiManagerView *v;
	ConnectionWidget(MidiManagerView *v, QWidget *parent) : QWidget(parent), v(v)
	{
	}
	void paintEvent(QPaintEvent *event)
	{
		QPainter painter;
		painter.begin(this);
//		painter.fillRect(event->rect(),white);

		int in;
		for(in = 0; in < v->widget->inputsListBox->count(); in++)
		{
			vector<long>::iterator conn;
			MidiManagerItem *item = (MidiManagerItem *)v->widget->inputsListBox->item(in);
			for(conn = item->info.connections.begin();
					conn != item->info.connections.end(); conn++)
			{
				MidiManagerItem *outitem = v->itemMap[*conn];
				QRect r1 = v->widget->inputsListBox->itemRect(item);
				QRect r2 = v->widget->outputsListBox->itemRect(outitem);

				if(r1.height() > 0 && r2.height() > 0)
				{
					painter.drawLine(0,(r1.top()+r1.bottom()) / 2,
								width(), (r2.top()+r2.bottom())/2);
				}
			}
		}
		painter.end();
	}
};

MidiManagerView::MidiManagerView()
	: manager(Reference("global:Arts_MidiManager"))
{
	QTimer *updatetimer = new QTimer(this);
	updatetimer->start(5000);
	connect(updatetimer,SIGNAL(timeout()),this,SLOT(updateLists()));

	widget = new MidiManagerWidget(this);
	setCentralWidget(widget);
    setCaption(i18n("Midi Manager"));

	(void)new KAction(i18n("&System Midi Port (OSS)"), 0, this, SLOT(addOSSMidiPort()),
		  actionCollection(), "add_oss_midi_port");
	(void)new KAction(i18n("&aRts Synthesis Midi Output"), 0, this,
		SLOT(addArtsMidiOutput()), actionCollection(), "add_arts_midi_output");

	connect(widget->connectButton,SIGNAL(clicked()), this, SLOT(slotConnect()));
	connect(widget->disconnectButton,SIGNAL(clicked()), this, SLOT(slotDisconnect()));

	connectionWidget = new ConnectionWidget(this, widget->connectionFrame);
	connectionWidget->setMinimumSize(60,10);
    widget->connectionFrameLayout->addWidget( connectionWidget, 0, 0 );

	updateLists();
	createGUI("artsmidimanagerview.rc");
	show();
}

void MidiManagerView::closeEvent(QCloseEvent *e)
{
	e->accept();
	emit closed();
}

void MidiManagerView::updateLists()
{
	vector<MidiClientInfo> *clients = manager.clients();
	vector<MidiClientInfo>::iterator i;

// keep selection over updating
	MidiManagerItem *src =
		(MidiManagerItem *)widget->inputsListBox->item(widget->inputsListBox->currentItem());
	MidiManagerItem *dest =
		(MidiManagerItem *)widget->outputsListBox->item(widget->outputsListBox->currentItem());

	long srcID = src?src->info.ID:0;
	long destID = dest?dest->info.ID:0;

// clear everything, and rebuild
	itemMap.clear();
	widget->inputsListBox->clear();
	widget->outputsListBox->clear();
	for(i = clients->begin(); i != clients->end(); i++)
	{
		QListBox *box;
		if(i->direction == mcdPlay)
			box = widget->inputsListBox;
		else
			box = widget->outputsListBox;

		MidiManagerItem *item = new MidiManagerItem(*i);
		itemMap[item->info.ID] = item;
		box->insertItem(item);
	}
	delete clients;

// restore selection
	if(srcID && itemMap[srcID])
		widget->inputsListBox->setSelected(itemMap[srcID],true);
	if(destID && itemMap[destID])
		widget->outputsListBox->setSelected(itemMap[destID],true);
	connectionWidget->repaint();
}

void MidiManagerView::slotConnect()
{
	MidiManagerItem *src =
		(MidiManagerItem *)widget->inputsListBox->item(widget->inputsListBox->currentItem());
	MidiManagerItem *dest =
		(MidiManagerItem *)widget->outputsListBox->item(widget->outputsListBox->currentItem());
	if(src && dest)
	{
		manager.connect(src->info.ID,dest->info.ID);
		updateLists();
	}
}

void MidiManagerView::slotDisconnect()
{
	MidiManagerItem *src =
		(MidiManagerItem *)widget->inputsListBox->item(widget->inputsListBox->currentItem());
	MidiManagerItem *dest =
		(MidiManagerItem *)widget->outputsListBox->item(widget->outputsListBox->currentItem());
	if(src && dest)
	{
		manager.disconnect(src->info.ID,dest->info.ID);
		updateLists();
	}
}

void MidiManagerView::addOSSMidiPort()
{
    //lukas: no i18n here, QDialog's CTOR doesn't know about QString
    //lukas: can't use that with const char *, i18n()'ed in the dialog itself
	MidiPortDlg *dlg = new MidiPortDlg(0,"/dev/midi","OSS Midi Port");

	if(dlg->exec())
	{
		SoundServer s = Reference("global:Arts_SoundServer");
		if(!s.isNull())
		{
			RawMidiPort p = DynamicCast(s.createObject("Arts::RawMidiPort"));
			p.device(dlg->device());
			if(p.open())
				p._addChild(p,"avoid_delete");
		}
	}
	delete dlg;
}

void MidiManagerView::addArtsMidiOutput()
{
	MidiInstDlg *dlg = new MidiInstDlg(0);

	if(dlg->exec())
	{
		SoundServer s = Reference("global:Arts_SoundServer");
		if(!s.isNull())
		{
			Synth_MIDI_TEST t
				= DynamicCast(s.createObject("Arts::Synth_MIDI_TEST"));
			t.filename(dlg->filename().data());
			t.start();
			t._addChild(t,"avoid_delete");
		}
	}
}
#include "midimanagerview.moc"
