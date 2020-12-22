// userinterface.cpp
//
// Copyright (C) 2000, 2001 Neil Stevens <multivac@fcmail.com>
// Copyright (C) 1999 Charles Samuels <charles@kde.org>
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

#include <effects.h>
#include <noatunapp.h>
#include "controls.h"
#include "noatunpref.h"
#include "player.h"
#include "userinterface.h"
#include "noatunstdaction.h"

#include <kbuttonbox.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kpixmapeffect.h>
#include <kpopupmenu.h>
#include <kstdaction.h>
#include <kwin.h>
#include <qbitmap.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qlcdnumber.h>
#include <qobjcoll.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>

Excellent::Excellent()
	: KMainWindow()
	, UserInterface()
{
	NOATUNPLUGINC(Excellent);
	setAcceptDrops(true);

	KStdAction::quit(napp, SLOT(quit()), actionCollection());
	KStdAction::open(napp, SLOT(fileOpen()), actionCollection());
	mToolbar = KStdAction::showToolbar(this, SLOT(showToolbar()), actionCollection());
	mMenubar = KStdAction::showMenubar(this, SLOT(showMenubar()), actionCollection());
	KStdAction::preferences(napp, SLOT(preferences()), actionCollection());
	NoatunStdAction::back(actionCollection(), "back");
	NoatunStdAction::stop(actionCollection(), "stop");
	NoatunStdAction::play(actionCollection(), "play");
	NoatunStdAction::pause(actionCollection(), "pause");
	NoatunStdAction::forward(actionCollection(), "forward");
	NoatunStdAction::playlist(actionCollection(), "show_playlist");
	mVolume = new KToggleAction(i18n("Show &Volume Control"), 0, this, SLOT(showVolumeControl()), actionCollection(), "show_volumecontrol");
	NoatunStdAction::effects(actionCollection(), "effects");
	NoatunStdAction::equalizer(actionCollection(), "equalizer");
	mLoop = new KAction(i18n("L&oop Style"), 0, napp->player(), SLOT(loop()), actionCollection(), "loop_style");
	mLoop->setIcon("noatunloopnone");

	createGUI("excellentui.rc");

	menuBar()->insertItem(i18n("&Plugins"), napp->pluginMenu(), -1, 3);
	toolBar("mainToolBar")->hide();

	// Who needs Qt Designer?
	mainFrame = new QHBox(this);
	mainFrame->setSpacing(KDialog::spacingHint());
	mainFrame->setMargin(KDialog::marginHint());
	mSeeker = new L33tSlider(0,1000,10,0, L33tSlider::Horizontal, mainFrame);
//	mSeeker->setTickInterval(100); OOLD
//	mSeeker->setTickmarks(QSlider::Below);
	mSeeker->setTickmarks(QSlider::NoMarks);
	mElapsed = new QLCDNumber(5, mainFrame);
	mElapsed->setSegmentStyle(QLCDNumber::Flat);
	mElapsed->setFrameStyle(QFrame::Panel);
	mElapsed->setFrameShadow(QFrame::Sunken);
	mElapsed->setFixedWidth(mElapsed->width());
	mElapsed->setFixedHeight(mElapsed->height());
	mElapsed->display("");
	
	setCentralWidget(mainFrame);

	mTotal = new QLCDNumber(5, statusBar());
	mTotal->setSegmentStyle(QLCDNumber::Flat);
	mTotal->display("");
	statusBar()->addWidget(mTotal, 0, true);
	statusBar()->show();

	connect( napp->player(), SIGNAL(hideYourself()), this, SLOT(hide()) );
	connect( napp->player(), SIGNAL(showYourself()), this, SLOT(show()) );

	connect(napp->player(), SIGNAL(playing()), this, SLOT(slotPlaying()));
	connect(napp->player(), SIGNAL(stopped()), this, SLOT(slotStopped()));
	connect(napp->player(), SIGNAL(paused()), this, SLOT(slotPaused()));
	napp->player()->handleButtons();
	
	connect(napp->player(), SIGNAL(timeout()), this, SLOT(slotTimeout()));
	connect(napp->player(), SIGNAL(loopTypeChange(int)), this, SLOT(changeLoopType(int)));

	/* This skipToWrapper is needed to pass milliseconds to Player() as everybody
	 * below the GUI is based on milliseconds instead of some unprecise thingy
	 * like seconds or mille */
	connect(seeker(), SIGNAL(userChanged(int)), this, SLOT(skipToWrapper(int)));
	connect(this, SIGNAL(skipTo(int)), napp->player(), SLOT(skipTo(int)));

	connect(seeker(), SIGNAL(sliderMoved(int)), SLOT(sliderMoved(int)));

	// Why + 25?  so the last button doesn't get cut off after a loop mode change
	// Better would be for toolbar icons to resize themselves when needed, then I
	// could dynamically catch a signal and resize the window as appropriate.
	setCaption("Noatun");
	setIcon(SmallIcon("noatun"));
	changeLoopType(Player::None);
	if(napp->playlist()->current())
		changeStatusbar(napp->playlist()->current()->title());
	handleLengthString("00:00/00:00");

	KConfig &config = *KGlobal::config();

	toolBar("main")->applySettings(&config, "Excellent main");
	mToolbar->setChecked( !toolBar("main")->isHidden() );

	config.setGroup("excellent");

	mVolumeControl = 0;
	mVolume->setChecked( config.readBoolEntry("volumeShown", false) );
	showVolumeControl();

	mMenubar->setChecked( config.readBoolEntry("menuShown", true) );
	showMenubar();

	if(config.readLongNumEntry("width"))
		resize(config.readLongNumEntry("width"), config.readLongNumEntry("height"));
	else
		resize( toolBar("main")->sizeHint().width() + 25, minimumHeight() );

	switch((NET::MappingState)config.readNumEntry("mappingState", (int)NET::Visible))
	{	
	case NET::Visible:
		showNormal();
		break;
	case NET::Withdrawn:
		hide();
		break;
	case NET::Iconic:
		showMinimized();
		break;
	}
}

Excellent::~Excellent()
{
	KConfig &config = *KGlobal::config();
	toolBar("main")->saveSettings(&config, "Excellent main");
	config.setGroup("excellent");
	config.writeEntry("volumeShown", mVolume->isChecked());
	config.writeEntry("menuShown", mMenubar->isChecked());
	config.writeEntry("width", width());
	config.writeEntry("height", height());
	config.sync();
}

void Excellent::closeEvent(QCloseEvent *)
{
	unload();
}

void Excellent::showEvent(QShowEvent *e)
{
	KConfig *config = KGlobal::config();
	config->setGroup("excellent");
	config->writeEntry("mappingState", NET::Visible);
	config->sync();

	KMainWindow::showEvent(e);
}

void Excellent::hideEvent(QHideEvent *e)
{
	KConfig *config = KGlobal::config();
	config->setGroup("excellent");
	config->writeEntry("mappingState", NET::Withdrawn);
	config->sync();

	KMainWindow::hideEvent(e);
}

void Excellent::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    event->accept(QUriDrag::canDecode(event));
}

void Excellent::dropEvent(QDropEvent *event)
{
	QStrList uri;
	if (QUriDrag::decode(event, uri))
	{
		for (char *file=uri.first(); file != 0; file=uri.next() )
			napp->player()->openFile(KURL(file), false);
	}
}

void Excellent::slotPlaying()
{
	changeStatusbar(napp->player()->current()->title());
}

void Excellent::slotStopped()
{
	if (!napp->player()->current()) return;
	changeStatusbar(napp->player()->current()->title());
	seeker()->setValue(0);
	handleLengthString("00:00/00:00");
}

void Excellent::slotPaused()
{
	changeStatusbar(napp->player()->current()->title());
}

void Excellent::slotTimeout()
{
	if(mVolumeControl)
		mVolumeControl->setValue(100 - napp->player()->volume());
	if (!napp->player()->current()) return;
	if (static_cast<L33tSlider*>(seeker())->currentlyPressed()) return;
	if (seeker())
	{
		seeker()->setRange ( 0, (int)napp->player()->getLength()/1000 );
		seeker()->setValue ( (int)napp->player()->getTime()/1000 );
	}
	handleLengthString(napp->player()->lengthString());
}

void Excellent::sliderMoved(int seconds)
{
	if(napp->player()->current())
		handleLengthString(napp->player()->lengthString(seconds*1000));
}

void Excellent::skipToWrapper(int second) // wrap int to int _and_ seconds to mseconds
{
	emit skipTo((int)(second*1000));
}

void Excellent::changeLoopType(int t)
{
	static const int time = 2000;
	switch (t)
	{
	case Player::None:
		statusBar()->message(i18n("No looping"), time);
		mLoop->setIcon("noatunloopnone");
		mLoop->setText(i18n("No looping"));
		break;
	case Player::Song:
		statusBar()->message(i18n("Song looping"), time);
		mLoop->setIcon("noatunloopsong");
		mLoop->setText(i18n("Song looping"));
		break;
	case Player::Playlist:
		statusBar()->message(i18n("Playlist looping"), time);
		mLoop->setIcon("noatunloopplaylist");
		mLoop->setText(i18n("Playlist looping"));
	}
}

void Excellent::showToolbar(void)
{
	if(mToolbar->isChecked())
		toolBar("main")->show();
	else
		toolBar("main")->hide();
}

void Excellent::showMenubar(void)
{
	if(mMenubar->isChecked())
		menuBar()->show();
	else
		menuBar()->hide();
}

void Excellent::showVolumeControl(void)
{
	if(mVolume->isChecked())
		growVolumeControl();
	else
		shrinkVolumeControl();
}

void Excellent::changeCaption(const QString& text)
{
	setCaption(text);
}

void Excellent::changeStatusbar(const QString& text)
{
	statusBar()->message(!text.isNull() ? text : napp->player()->current()->title());
}

void Excellent::handleLengthString(const QString &text)
{
	QString elapsed = text.left(5);
	QString total = text.right(5);
	mElapsed->display(elapsed);
	mTotal->display(total);
}

void Excellent::growVolumeControl(void)
{
	mVolumeControl = new L33tSlider(0, 100, 10, 0, Vertical, mainFrame);
	mVolumeControl->setValue(100 - napp->player()->volume());
	mVolumeControl->show();
	connect(mVolumeControl, SIGNAL(sliderMoved(int)), this, SLOT(changeVolume(int)));
	connect(mVolumeControl, SIGNAL(userChanged(int)), this, SLOT(changeVolume(int)));
}

void Excellent::shrinkVolumeControl(void)
{
	if(mVolumeControl)
	{
		delete mVolumeControl;
		mVolumeControl = 0;
	}
}

void Excellent::changeVolume(int slider)
{
	napp->player()->setVolume(100 - slider);
}

#include "userinterface.moc"
