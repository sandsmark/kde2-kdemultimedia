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

#include "choosebusdlg.h"

#include <artsflow.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <kbuttonbox.h>
#include <kseparator.h>
#include <qbttngrp.h>
#include <qradiobt.h>
#include <qlined.h>
#include <klocale.h>
#include <kapp.h>

using namespace std;

static void min_size(QWidget *w) {
  w->setMinimumSize(w->sizeHint());
} 

ChooseBusDlg::ChooseBusDlg(QWidget *parent) :QDialog(parent,"X", TRUE)
{
	setCaption(i18n("Choose Destination"));

	QVBoxLayout *mainlayout = new QVBoxLayout(this);

// caption label: "Synthesis running..."

	mainlayout->addSpacing(5);
	QLabel *captionlabel = new QLabel(this);
	QFont labelfont(captionlabel->font());
	labelfont.setPointSize(labelfont.pointSize()*3/2);
	captionlabel->setFont(labelfont);
	captionlabel->setText(QString(" ")+i18n("Available busses:")+QString(" "));
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

	Arts::AudioManager aman = Arts::Reference("global:Arts_AudioManager");

	if(!aman.isNull())
	{
		vector<string> *destinations = aman.destinations();
		unsigned long i;
		for(i=0;i<destinations->size();i++)
			listbox->insertItem((*destinations)[i].c_str());
		delete destinations;
	}

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

	QButton *okbutton = bbox->addButton(i18n("OK"));
	connect( okbutton, SIGNAL( clicked() ), SLOT(accept() ) );

	bbox->layout();

	buttonlayout->addWidget(bbox);
	buttonlayout->addSpacing(5);

	mainlayout->freeze();
};

QString ChooseBusDlg::result()
{
	if(listbox->currentItem() != -1)
	{
		return(listbox->text(listbox->currentItem()));
	}
	return(0);
}
#include "choosebusdlg.moc"
