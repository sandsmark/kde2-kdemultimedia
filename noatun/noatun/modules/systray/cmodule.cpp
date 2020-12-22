// cmodule.cpp
//
// Copyright (C) 2001 Neil Stevens <multivac@fcmail.com>
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

#include <kconfig.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qradiobutton.h>

#include <noatunapp.h>
#include <pluginloader.h>

#include "cmodule.h"
#include "systray.h"

YHModule::YHModule(QObject *_parent)
	: CModule(i18n("Young Hickory"), i18n("Configure The System Tray Icon"), _parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());

	tip = new QCheckBox(i18n("Show a tooltip for the current track"), this);

	icon = new QButtonGroup(1, Horizontal, this);
	icon->setExclusive(true);
	new QRadioButton(i18n("Blink the icon for the current state"), icon);
	new QRadioButton(i18n("Show the icon for the current state"), icon);
	new QRadioButton(i18n("Do neither"), icon);

	layout->addWidget(tip);
	layout->addWidget(icon);
	layout->addStretch();

	reopen();
	save();
}

void YHModule::save(void)
{
	KConfig &config = *(KGlobal::config());
	config.setGroup("Young Hickory");
	int iconPressed = icon->id(icon->selected());
	config.writeEntry("icon", iconPressed);
	config.writeEntry("tip", tip->isChecked());
	config.sync();

	NoatunSystray *yh = NoatunSystray::jasonkb;
	if(yh)
	{
		yh->setBlink((NoatunSystray::Blink)iconPressed);
		yh->setTip(tip->isChecked());
	}
}

void YHModule::reopen(void)
{
	KConfig &config = *(KGlobal::config());
	config.setGroup("Young Hickory");
	static_cast<QRadioButton *>(icon->find(config.readLongNumEntry("icon", 0)))->setChecked(true);
	tip->setChecked(config.readBoolEntry("tip", true));
}

#include "cmodule.moc"
