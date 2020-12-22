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

#include "artsgui.h"
#include <connect.h>
#include <qiomanager.h>
#include <qpushbutton.h>
#include <kapp.h>
#include <objectmanager.h>

using namespace Arts;

int main(int argc, char **argv)
{
	QIOManager iomanager;
	Dispatcher dispatcher(&iomanager);
	KApplication application(argc, argv, "testgui");

	ObjectManager::the()->provideCapability("kdegui");

	Widget w;
	w.width(300); w.height(100); w.show();

	Button b;
	b.parent(w);
	b.x(10);
	b.y(10);
	b.width(100);
	b.height(20);
	b.text("Hello World");
	b.show();

	Button b2;
	b2.parent(w);
	b2.x(10);
	b2.y(30);
	b2.width(100);
	b2.height(20);
	b2.text("More World");
	b2.show();

	Poti p;
	p.parent(w);
	p.x(150);
	p.y(10);
	p.text("delay (ms)");
	p.color("red");
	p.min(10);
	p.max(100);
	p.value(90);
	p.show();

	Poti q;
	q.parent(w);
	q.x(250);
	q.y(10);
	q.text("delay (ms)");
	q.color("blue");
	q.min(10);
	q.max(100);
	q.value(90);
	q.show();

	/* moves q if p is moved */
	connect(p,"value_changed",q,"value");

/*
	connect(q,"value_changed",p,"value");  

	With the current assumptions that change notifications make, it is not
	possible to connect p to q and q to p.

	The reason is that the notification will only be delivered some time
	in the future (due to the requirement that they are asynchronous, i.e.
	do not cause the sender to block) - but this means that two different
	change notifications (i.e. changed to 1.0, changed to 2.0) may move
	in a cycle between the two objects p and q, if p and q are cross
	connected. So don't cross connect ;).
*/
	return application.exec();
}
