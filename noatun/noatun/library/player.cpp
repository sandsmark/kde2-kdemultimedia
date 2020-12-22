#include "player.h"

#include <noatunplaylist.h>
#include "engine.h"
#include "noatunapp.h"
#include <klibloader.h>
#include <knotifyclient.h>
#include <klocale.h>
#include <qfile.h>

enum ArtsPOS { posIdle=0, posPlaying, posPaused };

Player::Player(QObject *parent) : QObject(parent),
	position(-1), mLoopStyle(None), firstTimeout(true), mCurrent(0), mDeleteMe(false)
{
	mEngine=new Engine;
	connect(&filePos, SIGNAL(timeout()), SLOT(posTimeout()));
	handleButtons();
	showingInterfaces = true;
}

Player::~Player()
{
	delete mEngine;
}

bool Player::isPlaying()
{
	return mEngine->state()==posPlaying;
}

bool Player::isPaused()
{
	return mEngine->state()==posPaused;
}

bool Player::isStopped()
{
	return mEngine->state()==posIdle;
}

void Player::toggleListView()
{
	napp->playlist()->toggleList();
}

void Player::handleButtons()
{
	switch (mEngine->state())
	{
	case (posPlaying):
		emit playing();
		break;
	case (posPaused):
		emit paused();
		break;
	case (posIdle):
		emit stopped();
	}
}

void Player::back()
{
	if (napp->playlist()->previous())
		play();
}

void Player::stop()
{
	filePos.stop();
	position=0;
	mEngine->stop();
	emit stopped();
	if (mDeleteMe)
	{
		delete mCurrent;
		mDeleteMe=false;
	}
	mCurrent=0;
}

void Player::play()
{
	bool work=false;
	firstTimeout=true;
	
	if (mEngine->state()==posPaused)
		work=mEngine->play();
	else
	{
		stop();
		if (!napp->playlist()->current())
			napp->playlist()->reset();
	
		PlaylistItem *current=napp->playlist()->current();
		if (!current || (current->isDownloaded()
			 && !QFile::exists(current->file())))
			work=false;
		else
			work=mEngine->open(current);
	}
	
	if (!work)
	{
		mCurrent=0;
//		removeCurrent();  // this is a bad idea
		fastForward(false);
	}	
	else
	{
		filePos.start(500);
		if (!mDeleteMe)
			mCurrent=napp->playlist()->current();
		emit changed();
		mEngine->play();
	}
	
	handleButtons();
}

void Player::playpause()
{
	if (mEngine->state()==posPlaying)
	{
		filePos.stop();
		mEngine->pause();
//		emit paused(); NOT necessary because emitted in handleButtons()  (mETz)
		handleButtons();
	}
	else
		play();
}

void Player::fastForward(bool allowLoop)
{
	stop();
	if (napp->playlist()->next())
		play();
	else if (allowLoop && napp->loopList())
		if (napp->playlist()->reset(), napp->playlist()->current())
			play();
}

void Player::skipTo(int msec) // skip to a certain time in the track
{
	if( (current()) && (msec>=0) )
	{
		mEngine->seek(msec);
		position = mEngine->position(); // make sure position is recent
		emit timeout(); // update the UI
	}
}

void Player::posTimeout()
{
	if (mEngine->state()==posIdle)
	{
		stop();
		handleButtons();
		// If you're supposed to loop the song, don't go next
		// otherwise, do go next
		if (loopStyle()==Song || napp->playlist()->next())
			play();
		else if (loopStyle()==Playlist)
		{
			napp->playlist()->reset();
			play();
		}
		else if (napp->loopList())
			napp->playlist()->reset();

		return;
	}
	position = mEngine->position();
	current()->setLength(mEngine->length());

	if ( position < 0 )
		stop();
	if (current())
	{
		position = current()->length() ? position : 0; // does that have any effect? (mETz)

		if (current()->length() && firstTimeout)
		{
			int minutes = (int) ( current()->length() / 60 );
			int seconds = current()->length() - minutes * 60;
			emit newSongLen ( minutes, seconds );
			firstTimeout = false;
			emit newSong();
		}
	}

	emit timeout();
}

QString Player::lengthString(int _position)
{
	QString posString;
	QString lenString;
	int secs, seconds, minutes;

	if (_position < 0)
		_position = position;

	{ // get the position
		secs = _position / 1000; // convert milliseconds -> seconds
		seconds = secs % 60;
		minutes = (secs - seconds) / 60;

		// the string has to look like '00:00/00:00'
		posString.sprintf("%.2d:%.2d/", minutes, seconds);
	}

	{ // get the length
		secs = current()->length() / 1000; // convert milliseconds -> seconds
		seconds = secs % 60;
		minutes = (secs - seconds) / 60;

		// the string has to look like '00:00/00:00'
		lenString.sprintf("%.2d:%.2d", minutes, seconds);
		posString += lenString;
	}

	return posString;
}

int Player::getLength()
{
	if (!current())
		return -1;
	return current()->length(); // return track-length in milliseconds
}

void Player::openFile(const KURL &f, bool purge, bool autoplay)
{
	if (purge)
		napp->playlist()->clear();
	napp->playlist()->addFile(f, autoplay);
}

void Player::loop()
{
	mLoopStyle++;
	if (mLoopStyle>Playlist)
		mLoopStyle=None;
	emit loopTypeChange(mLoopStyle);
}
void Player::loop(int i)
{
	mLoopStyle=i;
	emit loopTypeChange(mLoopStyle);
}

void Player::removeCurrent()
{
	if (napp->playlist()->current())
		napp->playlist()->remove(napp->playlist()->current());
}

void Player::playCurrent()
{
	stop();
	play();
}

void Player::newCurrent()
{
	// the second half of the following
	if (!napp->playlist() || !mEngine->initialized())
		return; // no playlist, or squelch playing as an optimization
	if ((mEngine->state()!=posPlaying) && napp->autoPlay())
		playCurrent();
}

void Player::setVolume(int v)
{
	if (v<0) v=0;
	if (v>100) v=100;
	mEngine->setVolume(v);
	emit timeout();
	emit volumeChanged(v);
}

int Player::volume() const
{
	return mEngine->volume();
}

void Player::toggleInterfaces()
{
	showingInterfaces ^= true;

	if(showingInterfaces)
		emit showYourself();
	else
		emit hideYourself();
}

void Player::showInterfaces()
{
	showingInterfaces = true;
	emit showYourself();
}

void Player::hideInterfaces()
{
	showingInterfaces = false;
	emit hideYourself();
}

#include "player.moc"
