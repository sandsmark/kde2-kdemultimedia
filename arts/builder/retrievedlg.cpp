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

#include "retrievedlg.h"
#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <kbuttonbox.h>
#include <qbttngrp.h>
#include <qradiobt.h>
#include <qlined.h>
#include <kapp.h>
#include <kseparator.h>
#include <stdio.h>
#include <set>
#include <arts/debug.h>

#ifndef KDE_USE_FINAL
static void min_size(QWidget *w) {
  w->setMinimumSize(w->sizeHint());
} 
#endif

RetrieveDlg::RetrieveDlg(QWidget *parent) :QDialog(parent,"X", TRUE)
{
	setCaption(i18n("Retrieve structure from server"));

	QVBoxLayout *mainlayout = new QVBoxLayout(this);

// caption label: "Synthesis running..."

	mainlayout->addSpacing(5);
	QLabel *captionlabel = new QLabel(this);
	QFont labelfont(captionlabel->font());
	labelfont.setPointSize(labelfont.pointSize()*3/2);
	captionlabel->setFont(labelfont);
	captionlabel->setText(QString(" ")+i18n("Published structures")+QString(" "));
	captionlabel->setAlignment(AlignCenter);
	min_size(captionlabel);
	mainlayout->addWidget(captionlabel);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler2 = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler2);
	mainlayout->addSpacing(5);

// listwidget

	listbox = new QListBox(this);
    listbox->setMinimumSize(300,200);

	arts_debug("TODO:PORT:get available structures");
#if 0
	// sort the result:
	vector<StructureDesc> *structures = Synthesizer->publishedStructures();
	set<string> names;

	unsigned long i;
	for(i=0;i<structures->length();i++)
		names.insert(structures[i]->Name());

	set<string>::iterator ni;
	for(ni=names.begin();ni!=names.end();ni++)
		listbox->insertItem((*ni).c_str());
#endif

	mainlayout->addWidget(listbox);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler);
	mainlayout->addSpacing(5);

// buttons

	QHBoxLayout *buttonlayout = new QHBoxLayout;
	mainlayout->addSpacing(5);
	mainlayout->addLayout(buttonlayout);
	mainlayout->addSpacing(5);

	buttonlayout->addSpacing(5);
	KButtonBox *bbox = new KButtonBox(this);

	bbox->addButton(i18n("Help"));
	bbox->addStretch(1);

	QButton *cancelbutton = bbox->addButton(i18n("Cancel"));
	connect( cancelbutton, SIGNAL( clicked() ), SLOT(reject() ) );

	QButton *okbutton = bbox->addButton(i18n("Okay"));
	connect( okbutton, SIGNAL( clicked() ), SLOT(accept() ) );

	bbox->layout();

	buttonlayout->addWidget(bbox);
	buttonlayout->addSpacing(5);

	mainlayout->freeze();
};

const char * RetrieveDlg::result()
{
	if(listbox->currentItem() != -1)
	{
		return(listbox->text(listbox->currentItem()));
	}
	return(0);
}
#include "retrievedlg.moc"
