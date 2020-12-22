// $Id: kitsystemtray.cpp 74694 2000-12-13 14:51:34Z malte $
//
// Kit
//
// Copyright (C) 1999 Neil Stevens <multivac@fcmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "noatunapp.h"
#include "player.h"
#include "kitsystemtray.h"
#include "kmainwindow.h"
#include "kpopupmenu.h"
#include "kxmlgui.h"
#include "kiconloader.h"

KitSystemTray::KitSystemTray(const QString &contextMenu, KMainWindow *parent, const char *name)
	: KSystemTray(parent, name)
{
	setAlignment(AlignHCenter | AlignVCenter);
	menu = (KPopupMenu *)parent->guiFactory()->container(contextMenu, parent); 
	menu->insertTitle(SmallIcon("noatun"), QString::null, 0, 0);
}

void KitSystemTray::changeTitle(const QPixmap &pixmap, const QString &title)
{
	menu->changeTitle(0, pixmap, title);
}

void KitSystemTray::showEvent(QShowEvent *)
{
	// empty
}

void KitSystemTray::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == LeftButton)
		napp->player()->toggleInterfaces();
	else
		menu->popup(event->globalPos());
}

#include "kitsystemtray.moc"
