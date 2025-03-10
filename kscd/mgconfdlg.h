
/*
 *
 * kscd -- A simple CD player for the KDE project           
 *
 * $Id: mgconfdlg.h 50820 2000-05-26 21:45:16Z dfoerste $
 * 
 * Copyright (C) 1997 Bernd Johannes Wuebben wuebben@math.cornell.edu
 * Copyright (C) 2000 Dirk Försterling <milliByte@gmx.de>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#ifndef _MGCONFIG_DLG_H_
#define _MGCONFIG_DLG_H_

#include <qgroupbox.h> 
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qbutton.h> 
#include <qbuttongroup.h> 

#include "kscd.h"


class QSpinBox;

class MGConfigDlg : public QDialog 
{
  
Q_OBJECT
  
public:

  MGConfigDlg(QWidget *parent, struct mgconfigstruct * data,const char *name);
  ~MGConfigDlg() {}


  struct mgconfigstruct * getData();

private slots:

  void help();
  void brightness_changed(int value);
  void width_changed(const QString &);
  void height_changed(const QString &);
  void pointsAreDiamonds_clicked(bool flag);
  //  void fadeMode_changed();
  //  void starSize_changed();

  
private:

  struct mgconfigstruct mgconfigdata;

  QGroupBox *box;
  
  QLabel *label1;
  QSpinBox *bspin;

  QLabel *label2;
  QLineEdit *height_edit;

  QLabel *label3;
  QLineEdit *width_edit;

  QCheckBox *pointsAreDiamondsCB;
  

  QPushButton *helpbutton;
};
#endif












