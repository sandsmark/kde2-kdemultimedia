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

#include "kpoti_impl.h"
#include "kpoti_impl.moc"
#include "anyref.h"
#include "stdio.h"

using namespace Arts;
using namespace std;

PotiIntMapper::PotiIntMapper(KPoti_impl *impl, KPoti *kp) :impl(impl)
{
	connect(kp, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
}

void PotiIntMapper::valueChanged(int pos)
{
	impl->valueChanged(pos);
}

KPoti_impl::KPoti_impl(KPoti *newKPoti /* = 0 */)
	: KWidget_impl(kpoti = (newKPoti?newKPoti:new KPoti(0,100,1,0)))
{
	kpoti->setFixedSize(46,40); 
	new PotiIntMapper(this, kpoti);
	_min = 0; _max = 1; _value = 0;
}

string KPoti_impl::text()
{
	return _text;
}

void KPoti_impl::text(const string& newText)
{
	_text = newText;
	kpoti->setName(_text.c_str());
}

string KPoti_impl::color()
{
	return _color;
}

void KPoti_impl::color(const string& newColor)
{
	_color = newColor;
    if(strlen(_color.c_str()))
    {
        QColor qc(_color.c_str());
        kpoti->setColor(qc);
    }                                                                           
}


float KPoti_impl::min()
{
	return _min;
}

void KPoti_impl::min(float newMin)
{
	_min = newMin;
	applyValue();
}

float KPoti_impl::max()
{
	return _max;
}

void KPoti_impl::max(float newMax)
{
	_max = newMax;
	applyValue();
}

float KPoti_impl::value()
{
	return _value;
}

void KPoti_impl::value(float newValue)
{
	if(newValue != _value)
	{
		_value = newValue;
		applyValue();
		if(visible())
			value_changed(newValue);
	}
}

void KPoti_impl::valueChanged(int newvalue)
{
	value((((float)newvalue/100.0) * (_max - _min))+_min);
}
	
void KPoti_impl::applyValue()
{
	if(_max - _min > 0 & _value >= _min && _value <= _max)
	{
    	kpoti->setValue((long)(((_value-_min)/(_max-_min))*100.0 /*+0.5 */));
	}
	else kpoti->setValue(0);
}

REGISTER_IMPLEMENTATION(KPoti_impl);
