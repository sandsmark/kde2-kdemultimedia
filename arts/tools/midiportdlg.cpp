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

#include "midiportdlg.h"
#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>
#include <kseparator.h>
#include <kbuttonbox.h>
#include <qlineedit.h>

MidiPortDlg::MidiPortDlg(QWidget *parent, const char *oldname, const char *title) :QDialog(parent,title,TRUE)
{
	QVBoxLayout *mainlayout = new QVBoxLayout(this);

// caption label: title

	mainlayout->addSpacing(5);
	QLabel *captionlabel = new QLabel(this);
	QFont labelfont(captionlabel->font());
	labelfont.setPointSize(labelfont.pointSize()*3/2);
	captionlabel->setFont(labelfont);
	captionlabel->setText(i18n("OSS Midi Port"));
	captionlabel->setAlignment(AlignCenter);
	//min_size(captionlabel);
	mainlayout->addWidget(captionlabel);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler2 = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler2);
	mainlayout->addSpacing(5);

// editwidget

	edit = new QLineEdit(this);
	edit->setText(oldname);
	//min_size(edit);

	mainlayout->addWidget(edit);

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

	QButton *okbutton = bbox->addButton(i18n("OK"));
	connect( okbutton, SIGNAL( clicked() ), SLOT(accept() ) );

	bbox->layout();

	buttonlayout->addWidget(bbox);
	buttonlayout->addSpacing(5);

	mainlayout->freeze();
};

const char *MidiPortDlg::device()
{
	return edit->text().ascii();
}
#include "midiportdlg.moc"
