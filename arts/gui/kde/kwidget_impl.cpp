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

#include "kwidget_impl.h"
#include "kwidgetrepo.h"
#include "debug.h"

using namespace Arts;

KWidget_impl::KWidget_impl(QWidget *qwidget /* = 0 */)
{
	if(!qwidget)
		qwidget = new QWidget();

	this->qwidget = qwidget;
	_widgetID = KWidgetRepo::the()->add(this,qwidget);

	/*
	 * KWidgetGuard will protect us against deleting the widget if Qt already
	 * has done so (for instance if our widget was inside a panel, and
	 * the panel got deleted, our widget will be gone, too)
	 */
	guard = new KWidgetGuard(this);
	QWidget::connect(qwidget, SIGNAL(destroyed()),
					 guard, SLOT(widgetDestroyed()));
}

KWidget_impl::~KWidget_impl()
{
	if(qwidget)
	{
		delete qwidget;
		arts_assert(qwidget == 0);	// should be true due to KWidgetGuard
	}
	delete guard;
}

void KWidget_impl::widgetDestroyed()
{
	KWidgetRepo::the()->remove(_widgetID);
	_widgetID = 0;
	qwidget = 0;
}

long KWidget_impl::widgetID()
{
	return _widgetID;
}

Widget KWidget_impl::parent()
{
	return KWidgetRepo::the()->lookupWidget(_parentID);
}

void KWidget_impl::parent(Arts::Widget newParent)
{
	if(!newParent.isNull())
	{
		_parentID = newParent.widgetID();

		QWidget *qparent;
		qparent = KWidgetRepo::the()->lookupQWidget(newParent.widgetID());
		if(qparent != 0)
		{
			QPoint pos(x(),y());
			bool showIt = visible();
			qwidget->reparent(qparent, pos, showIt);
		}
	}
	else
	{
		_parentID = 0;
	}
}

long KWidget_impl::x()
{
	return qwidget->x();
}

void KWidget_impl::x(long newX)
{
	qwidget->move(newX,y());
}

long KWidget_impl::y()
{
	return qwidget->y();
}

void KWidget_impl::y(long newY)
{
	qwidget->move(x(),newY);
}

long KWidget_impl::width()
{
	return qwidget->width();
}

void KWidget_impl::width(long newWidth)
{
	qwidget->resize(newWidth,height());
}

long KWidget_impl::height()
{
	return qwidget->height();
}

void KWidget_impl::height(long newHeight)
{
	qwidget->resize(width(),newHeight);
}

bool KWidget_impl::visible()
{
	return qwidget->isVisible();
}

void KWidget_impl::visible(bool newVisible)
{
	if(newVisible) show(); else hide();
}

void KWidget_impl::show()
{
	qwidget->show();
}

void KWidget_impl::hide()
{
	qwidget->hide();
}

REGISTER_IMPLEMENTATION(KWidget_impl);
#include "kwidget_impl.moc"
