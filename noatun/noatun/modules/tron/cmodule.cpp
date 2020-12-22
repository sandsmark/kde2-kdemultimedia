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

#include <kcolorbutton.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <qcolor.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>

#include <noatunapp.h>
#include <pluginloader.h>

#include "cmodule.h"
#include "tron.h"

TronModule::TronModule(QObject *_parent)
	: CModule(i18n("Tron Playlist"), i18n("Configure The Playlist"), _parent)
{
	highlightFGColor = new KColorButton(this);
	highlightBGColor = new KColorButton(this);

	QHBoxLayout *highlightFGColorLayout = new QHBoxLayout(0, 0, KDialog::spacingHint());
	highlightFGColorLayout->addWidget( new QLabel(i18n("Current Track Text Color"), this) );
	highlightFGColorLayout->addWidget( highlightFGColor );

	QHBoxLayout *highlightBGColorLayout = new QHBoxLayout(0, 0, KDialog::spacingHint());
	highlightBGColorLayout->addWidget( new QLabel(i18n("Current Track Background Color"), this) );
	highlightBGColorLayout->addWidget( highlightBGColor );

	columnButtons = new QButtonGroup(2, Horizontal, i18n("Columns"), this);
	(void)new QCheckBox(i18n("Title"), columnButtons);
	(void)new QCheckBox(i18n("Length"), columnButtons);
	(void)new QCheckBox(i18n("Artist"), columnButtons);
	(void)new QCheckBox(i18n("Album"), columnButtons);
	(void)new QCheckBox(i18n("Date"), columnButtons);
	(void)new QCheckBox(i18n("Comment"), columnButtons);

	QVBoxLayout *layout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
	layout->addLayout(highlightFGColorLayout);
	layout->addLayout(highlightBGColorLayout);
	layout->addWidget(columnButtons);
	layout->addStretch();
}

void TronModule::save(void)
{
	int columns = 0;
	for(int i = 0; i < 6; i++)
	{
		if(static_cast<QCheckBox *>(columnButtons->find(i))->isChecked())
			columns |= 1 << i;
	}

	KConfig &config = *(KGlobal::config());
	config.setGroup("tron");
	config.writeEntry("highlightFGColor", highlightFGColor->color());
	config.writeEntry("highlightBGColor", highlightBGColor->color());
	config.writeEntry("columns", columns);
	config.sync();

	Tron *tron = Tron::tronical;
	if(tron)
	{
		tron->setColors(highlightFGColor->color(), highlightBGColor->color());
		tron->setColumns(columns);
	}
}

void TronModule::reopen(void)
{
	KConfig &config = *(KGlobal::config());
	config.setGroup("tron");
	QColor solaris(255, 255, 255), mustDie(64, 64, 192);
	highlightFGColor->setColor(config.readColorEntry("highlightFGColor", &solaris));
	highlightBGColor->setColor(config.readColorEntry("highlightBGColor", &mustDie));

	int columns = config.readLongNumEntry("columns", 63);
	for(int i = 0; i < 6; i++)
	{
		if(columns & (1 << i))
			static_cast<QCheckBox *>(columnButtons->find(i))->setChecked(true);
		else
			static_cast<QCheckBox *>(columnButtons->find(i))->setChecked(false);
	}
}

#include "cmodule.moc"
