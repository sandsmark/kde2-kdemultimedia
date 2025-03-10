/**************************************************************************

    kslidertime.h  - A widget that displays time tags under a KSlider
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

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

    Send comments and bug fixes to larrosa@kde.org
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#ifndef _kslidertime_h_
#define _kslidertime_h_

#include <qslider.h>

class QWidget;

class QSliderTime : public QWidget
{
  Q_OBJECT
private:

  QSlider *kslider;

int fontheight;
public:

  QSliderTime( QSlider *ksl, QWidget *parent = NULL, const char *name = NULL );

  int getFontHeight(void) {return fontheight;};

  QSize sizeHint();
  QSizePolicy sizePolicy();

private:

  virtual void paintEvent( QPaintEvent * );  
  void drawTimeMarks(QPainter *painter);

  char *formatMillisecs(int ms,char *tmp);
};

#endif
