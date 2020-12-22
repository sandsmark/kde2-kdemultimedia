    /*

    Copyright (C) 2000 Stefan Westerfeld
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

#ifndef ARTS_GUI_KPOTI_IMPL_H
#define ARTS_GUI_KPOTI_IMPL_H
#include "kwidget_impl.h"
#include "kpoti.h"
#include "qobject.h"


namespace Arts {

class KPoti_impl;

class PotiIntMapper :public QObject {
	Q_OBJECT
	KPoti_impl *impl;
public:
	PotiIntMapper(KPoti_impl *impl, KPoti *kp);
public slots:
	void valueChanged(int x);
};

class KPoti_impl : virtual public Arts::Poti_skel,
						virtual	public Arts::KWidget_impl
{
protected:
	KPoti *kpoti;
	std::string _text, _color;
	float _min, _max, _value;

	void applyValue();
public:
	KPoti_impl(KPoti *kpoti = 0);

	std::string text();
	void text(const std::string& newText);
	std::string color();
	void color(const std::string& newColor);

	float min();
	void min(float newMin);
	float max();
	void max(float newMax);
	float value();
	void value(float newValue);

	/* from qt */
	void valueChanged(int newValue);
};

};
#endif /* ARTS_GUI_KPOTI_IMPL_H */
