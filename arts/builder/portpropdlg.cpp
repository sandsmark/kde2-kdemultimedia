	/*

	Copyright (C) 1998-1999 Stefan Westerfeld
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

#include "portpropdlg.h"
#include "portablekde.h"

#include <qlayout.h>
#include <qlabel.h>
#include <kbuttonbox.h>
#include <kseparator.h>
#include <qbttngrp.h>
#include <qradiobt.h>
#include <kapp.h>
#include <qlined.h>
#include <stdio.h>
#include <algorithm>
#include <ctype.h> 

using namespace std;

static void min_size(QWidget *w) {
  w->setMinimumSize(w->sizeHint());
} 

PortPropDlg::PortPropDlg(QWidget *parent, ModulePort *port) :QDialog(parent,"Props", TRUE)
{
	this->port = port;

	newconntype = ctNone;

	if(port->PortDesc.isConnected())
		newconntype = ctConnection;

	if(port->PortDesc.hasValue())
		newconntype = ctValue;
	
	newvalue = "";

	setCaption(i18n("Arts port properties"));

	QVBoxLayout *mainlayout = new QVBoxLayout(this);
	QHBoxLayout *contentslayout = new QHBoxLayout;

// object type

	mainlayout->addSpacing(5);
	QLabel *objectlabel = new QLabel(this);
	QFont labelfont(objectlabel->font());
	labelfont.setPointSize(labelfont.pointSize()*3/2);
	objectlabel->setFont(labelfont);
	objectlabel->setText(QString(" ")+i18n("Object Type: ")+QString(port->owner->name())+QString(" "));
	objectlabel->setAlignment(AlignCenter);
	min_size(objectlabel);
	mainlayout->addWidget(objectlabel);

// port description

	mainlayout->addSpacing(5);
	QLabel *portlabel = new QLabel(this);
	labelfont.setPointSize(labelfont.pointSize()*4/5);
	portlabel->setFont(labelfont);
	portlabel->setText(i18n("Port Description: ")+ port->description);
	min_size(portlabel);
	portlabel->setAlignment(AlignCenter);
	mainlayout->addWidget(portlabel);

	int labelwidth = max(portlabel->sizeHint().width(),objectlabel->sizeHint().width());

	portlabel->setMinimumWidth(labelwidth);
	objectlabel->setMinimumWidth(labelwidth);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler);

	mainlayout->addSpacing(5);
	mainlayout->addLayout(contentslayout);

// icon

	contentslayout->addSpacing(5);
	if(port->owner->pixmap())
	{
		QLabel *iconlabel = new QLabel(this);

		iconlabel->setPixmap(*port->owner->pixmap());
		min_size(iconlabel);
		contentslayout->addWidget(iconlabel);
	}
	QVBoxLayout *connlayout = new QVBoxLayout;
	contentslayout->addSpacing(5);
	contentslayout->addLayout(connlayout);
	contentslayout->addSpacing(5);

// radio group

	QButtonGroup *rb_group = new QButtonGroup(this);
	rb_group->hide();

	rb_noconn = new QRadioButton(i18n("not connected"), this);
	rb_group->insert(rb_noconn);
	min_size(rb_noconn);
	connlayout->addWidget(rb_noconn);
	connect( rb_noconn, SIGNAL( clicked() ), SLOT(conn_none() ) );

	rb_connto = new QRadioButton(i18n("connection"), this);
	rb_group->insert(rb_connto);
	min_size(rb_connto);
	connlayout->addWidget(rb_connto);
	connect( rb_connto, SIGNAL( clicked() ), SLOT(conn_to() ) );

	rb_const = new QRadioButton(i18n("constant value"), this);
	rb_group->insert(rb_const);
	min_size(rb_const);
	connlayout->addWidget(rb_const);
	connect( rb_const, SIGNAL( clicked() ), SLOT(conn_value() ) );

	QHBoxLayout *valuelayout = new QHBoxLayout;
	connlayout->addLayout(valuelayout);
	
	valuelabel = new QLabel("value:", this);
	valuelayout->addSpacing(20);
	min_size(valuelabel);
	valuelayout->addWidget(valuelabel);	

	valuelayout->addSpacing(5);

	valueedit = new QLineEdit(this);

	if(port->PortDesc.hasValue())
	{
		Arts::Any value = port->PortDesc.value();
		Arts::Buffer b;
		b.write(value.value);

		if(value.type == "float")
			newvalue.sprintf("%2.4f", b.readFloat());
		else if(value.type == "long")
			newvalue.sprintf("%ld", b.readLong());
		else if(value.type == "string")
		{
			string s;
			b.readString(s);
			newvalue = s.c_str();
		}
		else if(value.type == "boolean")
		{
			if(b.readBool())
				newvalue = "true";
			else
				newvalue = "false";
		}
		else newvalue = ("*unknown type* " + value.type).c_str();
	}
	valueedit->setText(newvalue);

	min_size(valueedit);
	connect(valueedit, SIGNAL( textChanged(const QString&) ),
	                   SLOT( connvalue_changed(const QString&) ));
	valuelayout->addWidget(valueedit);


//---- configurable
/*
	rb_conf = new QRadioButton(i18n("configurable"), this);
	rb_group->insert(rb_conf);
	min_size(rb_conf);
	connlayout->addWidget(rb_conf);
	connect( rb_conf, SIGNAL( clicked() ), SLOT(conn_conf() ) );
*/
// hruler

	valuelayout->addStretch(1);
	valuelayout->addSpacing(5);

	mainlayout->addSpacing(5);
	KSeparator *ruler2 = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler2);

// buttons

	QHBoxLayout *buttonlayout = new QHBoxLayout;
	mainlayout->addSpacing(5);
	mainlayout->addLayout(buttonlayout);
	mainlayout->addSpacing(5);

	buttonlayout->addSpacing(5);
	KButtonBox *bbox = new KButtonBox(this);

	QButton *helpbutton = bbox->addButton(i18n("Help"));
	connect( helpbutton, SIGNAL( clicked() ), SLOT(showHelp()));
	bbox->addStretch(1);

	QButton *okbutton = bbox->addButton(i18n("Okay"));
	connect( okbutton, SIGNAL( clicked() ), SLOT(apply_changes() ) );
	connect( okbutton, SIGNAL( clicked() ), SLOT(accept() ) );

	QButton *cancelbutton = bbox->addButton(i18n("Cancel"));
	connect( cancelbutton, SIGNAL( clicked() ), SLOT(reject() ) );
	bbox->layout();
	//min_size(bbox);

	buttonlayout->addWidget(bbox);
	buttonlayout->addSpacing(5);

	//mainlayout->activate();
	mainlayout->freeze();

	updatestatus();
};

void PortPropDlg::updatestatus()
{
	rb_noconn->setChecked(newconntype == ctNone);

	valuelabel->setEnabled(newconntype == ctValue);
	valueedit->setEnabled(newconntype == ctValue);
	rb_const->setChecked(newconntype == ctValue);

	rb_connto->setChecked(newconntype == ctConnection);
}

void PortPropDlg::conn_value()
{
	newconntype = ctValue;
	if(port->PortDesc.type().direction == Arts::output)
		newconntype = ctNone;	// yes, confusing, should be done better
	updatestatus();
}

void PortPropDlg::conn_to()
{
	if(!port->PortDesc.isConnected())		// no connection there?
		newconntype = ctNone;
	else
		newconntype = ctConnection;
	updatestatus();
}

void PortPropDlg::conn_conf()
{
	newconntype = ctNone;	// no conf supported
	updatestatus();
}

void PortPropDlg::conn_none()
{
	newconntype = ctNone;
	updatestatus();
}

void PortPropDlg::connvalue_changed(const QString& _newvalue)
{
	newvalue = _newvalue;
}

void PortPropDlg::showHelp()
{
	// transforms port->owner->name() to an anchor for the right documentation
	// example: "Arts::Synth_MULTI_ADD" => "MREF-SYNTH-MULTI-ADD"
	string s = port->owner->name();
	string anchor = "MREF-";

	string::iterator si = s.begin();
	if(strncmp(s.c_str(),"Arts::",6) == 0) si += 6;

	while(si != s.end())
	{
		if(*si == '_') anchor += '-'; else anchor += toupper(*si);
		si++;
	}
	kapp->invokeHelp(anchor.c_str());
}

void PortPropDlg::apply_changes()
{
	if(newconntype != ctConnection)
	{
		if(port->PortDesc.isConnected())
			port->PortDesc.disconnectAll();
	}

	if(newconntype == ctValue)
	{
		string type = port->PortDesc.type().dataType;

		Arts::Any a;
		a.type = type;
		Arts::Buffer b;
		if(type == "float")
			b.writeFloat(newvalue.toFloat());
		else if(type == "long")
			b.writeLong(newvalue.toLong());
		else if(type == "string")
			b.writeString(newvalue.ascii());
		else if(type == "boolean")
		{
			b.writeBool(newvalue.upper() == "TRUE" || newvalue.upper() == "T"
					  || newvalue == "1");
		}

		if(b.size() > 0)
		{
			b.read(a.value, b.size());
			port->PortDesc.value(a);
		}
	}

	if(newconntype == ctNone)
		port->PortDesc.hasValue(false);
}
#include "portpropdlg.moc"
