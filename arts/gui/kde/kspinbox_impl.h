    /*

    Copyright (C) 2000,2001 Stefan Westerfeld
                            stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#ifndef ARTS_GUI_KSPINBOX_IMPL_H
#define ARTS_GUI_KSPINBOX_IMPL_H
#include "kwidget_impl.h"
#include "qspinbox.h"
#include "qobject.h"


namespace Arts {

class KSpinBox_impl;
class SpinBoxIntMapper :public QObject {
	Q_OBJECT
	KSpinBox_impl *impl;
public:
	SpinBoxIntMapper(KSpinBox_impl *impl, QSpinBox *sp);
public slots:
	void valueChanged(int x);
};

class KSpinBox_impl : virtual public Arts::SpinBox_skel,
					  virtual public Arts::KWidget_impl
{
protected:
	QSpinBox *qspinbox;
	std::string _caption;
	long _min, _max, _value;

	void applyValue();
public:
	KSpinBox_impl(QSpinBox *kpoti = 0);

	std::string caption();
	void caption(const std::string& newCaption);

	long min();
	void min(long newMin);
	long max();
	void max(long newMax);
	long value();
	void value(long newValue);

	/* from qt */
	void valueChanged(int newValue);
};

};
#endif /* ARTS_GUI_KSPINBOX_IMPL_H */
