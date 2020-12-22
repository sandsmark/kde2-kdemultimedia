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

#include "kbutton_impl.h"

using namespace Arts;
using namespace std;

KButton_impl::KButton_impl(QPushButton *qpushbutton /* = 0 */)
	: KWidget_impl(qpushbutton?qpushbutton:new QPushButton(0))
{
	this->qpushbutton = (QPushButton *)qwidget;
}

string KButton_impl::text()
{
	return qpushbutton->text().utf8().data();
}

void KButton_impl::text(const string& newText)
{
	qpushbutton->setText(QString::fromUtf8(newText.c_str()));
}

REGISTER_IMPLEMENTATION(KButton_impl);
