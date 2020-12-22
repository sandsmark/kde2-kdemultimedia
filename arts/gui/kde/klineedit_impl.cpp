/*
	Copyright (C) 2000 Stefan Westerfeld <stefan@space.twc.de>
	              2001 Charles Samuels <charles@kde.org>

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

#include "klineedit_impl.h"

using namespace Arts;
using namespace std;

KLineEdit_impl::KLineEdit_impl(KLineEdit *le)
	: KWidget_impl(le ? le : new KLineEdit(0))
{
	this->klineedit = static_cast<KLineEdit*>(qwidget);
}

string KLineEdit_impl::text()
{
	return klineedit->text().utf8().data();
}

void KLineEdit_impl::text(const string& newText)
{
	klineedit->setText(QString::fromUtf8(newText.c_str()));
}

string KLineEdit_impl::caption()
{
	return ""; // FIXME
}

void KLineEdit_impl::caption(const string& newCaption)
{
	// FIXME
}

REGISTER_IMPLEMENTATION(KLineEdit_impl);
