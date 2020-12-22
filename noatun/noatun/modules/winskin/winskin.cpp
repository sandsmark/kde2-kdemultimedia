/*
 * winskin.cpp
 *
 * This file is distributed under the terms of the Artistic License
 *
 * Copyright (C) 1999 Charles Samuels <charles@kde.org>
 * Copyright (C) 2000 Neil Stevens <multivac@fcmail.com>
 */

/*
  2001 Joseph Wenninger
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kiconloader.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kstdaction.h>
#include <kaction.h>
#include <klocale.h>
#include <arts/kmedia2.h>
#include <qdragobject.h>
#include <kstddirs.h>
#include <kwin.h>
#include <kconfig.h>
#include <qstringlist.h>
#include <kglobal.h>

#include "winskin.h"
#include "winSkinConfig.h"
#include "waSkin.h"
#include "l33tFileInfo.h"

#include <noatunplaylist.h>
#include <noatunapp.h>
#include "noatunstdaction.h"  

winskin::winskin() :  WaSkin(0, 0), UserInterface(),i_b_move(false)
{

	NOATUNPLUGINC(winskin);

        KWin::setType( this->winId(), NET::Override );
        setBackgroundMode( NoBackground );

	setAcceptDrops(true);

	i18n("Winamp skin selection");
	mLength = 0;
	mJumpPressed = false;

	QStringList skins=KGlobal::dirs()->findDirs("data", "noatun/winskin");

	cout << "\n" << skins[0].latin1() << "\n" << skins[1].latin1() << endl;
	QString selectedSkin;
	selectedSkin = skins[0];
	selectedSkin.append("sketchamp");
	loadSkin(selectedSkin.latin1());
	//	mWaSkin = new WaSkin(s1, this);
	mWaSkin = this;
	//	setCentralWidget(mWaSkin);
	//	setFixedSize(size());
	
	setCaption("Winskin - Noatun");
	setIcon(SmallIcon("noatun"));

	show();
	
	//	Ok now we connect the signals from the winamp widgets to our slots,
	//	so we can update the player whent he users does something.
	
	//	buttons
	//
	QObject::connect(mWaSkin, SIGNAL(playCurrentEvent()), 	this, SLOT(playCurrentEvent()));
	QObject::connect(mWaSkin, SIGNAL(playNextEvent()),		this, SLOT(playNextEvent()));
	QObject::connect(mWaSkin, SIGNAL(playPrevEvent()),		this, SLOT(playPrevEvent()));
	QObject::connect(mWaSkin, SIGNAL(playStopEvent()),		this, SLOT(playStopEvent()));
	QObject::connect(mWaSkin, SIGNAL(playPauseEvent()), 	this, SLOT(playPauseEvent()));
	QObject::connect(mWaSkin, SIGNAL(ejectEvent()), 		this, SLOT(ejectEvent()));

	//	slider
	//
	QObject::connect(mWaSkin, SIGNAL(jump(int)),			this, SLOT(jump(int)));
	QObject::connect(mWaSkin, SIGNAL(jumpSliderPressed()),	this, SLOT(jumpSliderPressed()));
	QObject::connect(mWaSkin, SIGNAL(jumpSliderReleased()),	this, SLOT(jumpSliderReleased()));
	QObject::connect(mWaSkin, SIGNAL(jumpValueChanged(int)),this, SLOT(jumpValueChanged(int)));
	
	//	balance slider
	//
	QObject::connect(mWaSkin, SIGNAL(balanceSetValue(int)),	this, SLOT(balanceSetValue(int)));
	
	//	volume slider
	//
	QObject::connect(mWaSkin, SIGNAL(volumeSetValue(int)),	this, SLOT(volumeSetValue(int)));
	
	//	toggles
	//
	QObject::connect(mWaSkin, SIGNAL(repeatClickedEvent(int)),	this, SLOT(repeatClickedEvent(int)));
	QObject::connect(mWaSkin, SIGNAL(shuffleClickedEvent(int)),	this, SLOT(shuffleClickedEvent(int)));
	QObject::connect(mWaSkin, SIGNAL(playlistClickedEvent(int)),this, SLOT(playlistClickedEvent(int)));
	
	// connect to players signals - so we can update our display if someone else
	// changes settings...
	
	connect(napp->player(), SIGNAL(timeout()), this, SLOT(timetick()));
	connect(napp->player(), SIGNAL(stopped()), this, SLOT(slotStopped()));
	connect(napp->player(), SIGNAL(playing()), this, SLOT(slotPlaying()));
	connect(napp->player(), SIGNAL(paused()), this, SLOT(slotPaused()));
	connect(napp->player(), SIGNAL(loopTypeChange(int)), this, SLOT(loopChange(int)));
	connect(napp->player(), SIGNAL(playlistShown()), this, SLOT(playlistShown()));
	connect(napp->player(), SIGNAL(playlistHidden()), this, SLOT(playlistHidden()));
	connect(napp->player(), SIGNAL(newSongLen(int, int)), this, SLOT(newSongLength(int, int)));
	connect(napp->player(), SIGNAL(newSong()), this, SLOT(newSong()));
	connect(napp->player(), SIGNAL(songModified()), this, SLOT(songModified()));	
	
	napp->player()->handleButtons();
	
	// now set demo values
	//
	l33tFileInfo *lfi = new l33tFileInfo(napp->playlist()->current()->file());
	setBPS(lfi->getBps());
	setFreq(44100);
	setJumpValue(0);
	setVolumeValue(napp->player()->volume());
	setBalanceValue(0);
	setStatus(_STATUS_PLAYING);
	setPlaylist(false);
	setShuffle(false);
	setRepeat(false);
	setStereo(true);
	if (napp->player()->current())
		setSong(napp->player()->current()->title());
	setTime(QString("-00:00"));
	
	// set values for the spectrum
	float aArray[75];
	int i;
	for(i=0;i<75;i++) {
		aArray[i]=i%20;
	}
	setSpectrum(aArray);
        new WinSkinConfig(this, guiSpectrumAnalyser);
}

void winskin::fileOpen()
{
	napp->fileOpen();
}
void winskin::quit()
{
	napp->quit();
}
void winskin::preferences()
{
	napp->preferences();
}
void winskin::effects()
{
	napp->effects();
}
void winskin::mouseReleaseEvent(QMouseEvent *e)
{
	cout << "mouseReleaseEvent" << endl;
	QWidget::mouseReleaseEvent(e);
	if (e->button()!=RightButton)
	{
		i_b_move = false;
		return;
	}
}
void winskin::mousePressEvent(QMouseEvent *e)
{
	cout << "mousePressEvent" << endl;
	QWidget::mousePressEvent(e);
	if (e->button()!=RightButton)
	{
    		if (!i_b_move) {
                        i_b_move=true;
        		i_point_dragStart = e->pos();
      		  	i_point_lastPos = e->globalPos();
    		}
		return;

	}
        showContextMenu();
}

void winskin::showContextMenu(const QPoint &p)
{
//        NoatunStdAction::ContextMenu::showContextMenu();

//	napp->showContextMenu(p);
}

void winskin::showContextMenu()
{
        NoatunStdAction::ContextMenu::showContextMenu();
//	napp->showContextMenu();
}


winskin::~winskin()
{
//	delete mContextMenu; Huh ?
}

void winskin::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
	event->accept(QUriDrag::canDecode(event));
}

void winskin::dropEvent(QDropEvent *event)
{
	QStrList uri;
	if (QUriDrag::decode(event, uri))
	{
		for (char *file=uri.first(); file != 0; file=uri.next() )
			napp->player()->openFile(KURL(file), false);
	}
}

// public slots
//
void winskin::repeatClickedEvent(int val) {
	int loopVal = ( val == 1 ) ? Player::Playlist : Player::None;
	napp->player()->loop(loopVal);
}

void winskin::shuffleClickedEvent(int val) {
	cout << "shuffleClickedEvent:"<<val<<endl;
}


void winskin::playlistClickedEvent(int /*val*/) {
	napp->playlist()->toggleList();
}


void winskin::playCurrentEvent() {
	if( napp->player()->isPaused() )
		napp->player()->playpause();
	else
		napp->player()->playCurrent();
}

void winskin::playNextEvent() {
	napp->player()->fastForward();
}


void winskin::playPrevEvent() {
	napp->player()->back();
}

void winskin::playStopEvent() {
	setJumpValue(0);
	napp->player()->stop();
}


void winskin::playPauseEvent() {
	if( ! napp->player()->isStopped() )
		napp->player()->playpause();
}


void winskin::ejectEvent(){
	//	napp->player()->removeCurrent();
}


void winskin::jump(int val) {
	if( napp->player()->isStopped() )
	{
		setJumpValue(0);
	}
	else
	{
//		int mille = val * 10;
		napp->player()->skipTo((int)(val*1000));
	}
}

void winskin::jumpSliderPressed() {
	
	mJumpPressed = true;
}


void winskin::jumpSliderReleased() {
	
	mJumpPressed = false;
	if (napp->player()->current())
		setSong(napp->player()->current()->title());
}

void winskin::jumpValueChanged(int val) {
	if(mJumpPressed && !napp->player()->isStopped()) {
//		int position = (mLength * val) / 100;
		QString timeStr = "Seek to: ";
		timeStr += napp->player()->lengthString(val); //(position);
		timeStr += QString(" ( ");
		timeStr += QString::number(val);
		timeStr += QString("% )");
		setSong(timeStr);
	}
}
void winskin::balanceSetValue(int val) {
	cout << "balanceSetValue:"<<val<<endl;
}


void winskin::volumeSetValue(int val) {
	napp->player()->setVolume( val );
}

void winskin::slotPlaying() {
	setStatus( _STATUS_PLAYING );
}

void winskin::slotStopped() {
	setStatus( _STATUS_STOPPED );
	setTime( "" );	
}

void winskin::slotPaused() {
	setStatus( _STATUS_PAUSED );	
}

void winskin::timetick() {
	
	if (!napp->player()->current()) return;
	QString timeStr = napp->player()->lengthString();
	timeStr = timeStr.left(5);
	setTime( timeStr );

	// hack until napp->player()->length() is available.
	//
/*
	timeStr = napp->player()->lengthString();
	timeStr = timeStr.right(5);
	int min = timeStr.left(2).toInt();
	int sec = timeStr.right(2).toInt();
	mLength = (min * 60) + sec;
*/
	mLength = (int)napp->player()->getLength()/1000;
	if ( mLength < 0 )
		mLength = 0;

//	mLength = mLength ? mLength:1;

	int time=0;
	if (napp->player()->current())
//		time = napp->player()->current()->fromMille(napp->player()->getTime());
		time = (int)napp->player()->getTime()/1000;

	setJumpRange(mLength);

	if(!mJumpPressed)
//		setJumpValue((time * 100 / mLength));
		setJumpValue(time);

	setVolumeValue( napp->player()->volume() );
}

void winskin::loopChange(int loopType) {	
	int loopVal = ( loopType == Player::Playlist ) ? 1 : 0;
	setRepeat(loopVal);
}

void winskin::playlistShown() {
	setPlaylist(1);
}

void winskin::playlistHidden() {
	setPlaylist(0);
}

void winskin::newSongLen(int min, int sec) {
	mLength = (min * 60) + sec;
}

void winskin::newSong() {
	if (!napp->player()->current()) return; 
	QString title = napp->player()->current()->title();
	setSong( title );
	
	timetick();
}

void winskin::songModified() {
	setVolumeValue(napp->player()->volume());
}


void winskin::mouseMoveEvent(QMouseEvent *qme)
{
    QPoint diff = qme->globalPos() - i_point_lastPos;
    if ( abs(diff.x()) > 10 || abs(diff.y()) > 10) {
        // Moving starts only, when passing a drag border
        i_b_move = true;
    }

    if ( i_b_move ) {
//        QWidget *p = parentWidget()->parentWidget();
  //      if ( !p ) p = parentWidget();

        move( qme->globalPos() - i_point_dragStart );
    }
 
}
void winskin::configureDialog() {

}

void winskin::shadeEvent()
{
        cout << "Shade mode";
}
 

#include "winskin.moc"
