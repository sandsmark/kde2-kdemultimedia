#include "dcopiface.h"
#include <player.h>
#include <noatunapp.h>
#include <noatunarts.h>
#include <engine.h>
#include <dcopclient.h>

extern "C"
{
	NIF *create_plugin()
	{
		return new NIF();
	}
}


NIF::NIF() : Plugin(), DCOPObject("Noatun")
{
//	connect(napp->player(), SIGNAL(newSong()), SLOT(newSongPlaying()));
}

NIF::~NIF()
{
	kapp->dcopClient()->emitDCOPSignal("exiting()", QByteArray());
}

void NIF::toggleListView()
{
	napp->player()->toggleListView();
}

void NIF::handleButtons()
{
	napp->player()->handleButtons();
}

void NIF::removeCurrent()
{
	napp->player()->removeCurrent();
}

void NIF::back()
{
	napp->player()->back();
}

void NIF::stop()
{
	napp->player()->stop();
}

void NIF::play()
{
	napp->player()->play();
}

void NIF::playpause()
{
	napp->player()->playpause();
}

void NIF::fastForward()
{
	napp->player()->fastForward();
}

void NIF::skipTo(int msec)
{
	napp->player()->skipTo(msec);
}

void NIF::loop()
{
	napp->player()->loop();
}

void NIF::newCurrent()
{
	napp->player()->newCurrent();
}

void NIF::playCurrent()
{
	napp->player()->playCurrent();
}

void NIF::setVolume(int i)
{
	napp->player()->setVolume(i);
}

int NIF::length() // returns -1 if there's no playobject
{
	return napp->player()->getLength();
}

int NIF::position() // returns -1 if there's no playobject
{
    return napp->player()->getTime();
}

int NIF::state()
{
	if (napp->player()->isPlaying())
		return 2;
	if (napp->player()->isPaused())
		return 1;
	if (napp->player()->isStopped());
		return 0;
}

QString NIF::lengthString()
{
	return napp->player()->current() ? napp->player()->current()->lengthString() : "";
}

QString NIF::title()
{
	return napp->player()->current() ? napp->player()->current()->title() : "";
}

QCString NIF::visStack()
{
	return napp->player()->engine()->visualizationStack()->toString().c_str();
}

QCString NIF::session()
{
	return napp->player()->engine()->session()->toString().c_str();
}

// adds one file to the playlist
void NIF::addFile(const QString& f, bool autoplay)
{
	napp->player()->openFile(f, false, autoplay);
}

// Adds a bunch of files to the playlist
void NIF::addFile(const QStringList &f, bool autoplay)
{
	for (QStringList::ConstIterator it = f.begin(); it != f.end(); ++it )
		napp->player()->openFile(*it, false, autoplay);
}

void NIF::loadPlugin(const QString &spec)
{
	napp->libraryLoader()->add(spec);
}

void NIF::newSongPlaying()
{
	kapp->dcopClient()->emitDCOPSignal("newFile()", QByteArray());
}

