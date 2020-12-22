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

#ifndef __NAMEDLG_H_
#define __NAMEDLG_H_

#include "structure.h"
#include <qdialog.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qscrbar.h>
#include <qlined.h>
#include <qlayout.h>

class NameDlg :public QDialog {
	Q_OBJECT
	QLineEdit *edit;
public:
	NameDlg(QWidget *parent, const char *oldname, const char *title);
	const char *name();
};
#endif
