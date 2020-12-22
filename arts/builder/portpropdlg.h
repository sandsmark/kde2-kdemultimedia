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

#ifndef __PORTPROPDLG_H_
#define __PORTPROPDLG_H_

#include "module.h"

#include <qdialog.h>
#include <qradiobt.h>
#include <qlabel.h>
#include <qlined.h>

class PortPropDlg :public QDialog {
	Q_OBJECT
protected:
	ModulePort *port;
	QRadioButton *rb_noconn,*rb_connto,*rb_const,*rb_conf;
	QLineEdit *valueedit;
	QLabel *valuelabel;

	enum ConnType { ctNone, ctValue, ctConnection };
//
	int		newconntype;

	QString	newvalue;

	void updatestatus();
public slots:
	void conn_value();
	void conn_none();
	void conn_to();
	void conn_conf();
	void connvalue_changed(const QString& newvalue);
	void showHelp();

	void apply_changes();

public:
	PortPropDlg(QWidget *parent, ModulePort *port);
};

#endif
