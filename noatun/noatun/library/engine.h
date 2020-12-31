#ifndef _ENGINE_H
#define _ENGINE_H

#include <qobject.h>
#include <kurl.h>
#include <kmedia2.h>

class Visualization;
class PlaylistItem;

namespace Arts
{
	class SoundServerV2;

	class Synth_AMAN_PLAY;
}

namespace Noatun
{
	class StereoEffectStack;
	class StereoVolumeControl;
	class Equalizer;
	class Session;
}

class NoatunApp;

/**
 * Handles all playing, connecting to aRts.
 * Does almost everything related to multimedia.
 * Most interfacing should be done with Player
 **/
class Engine : public QObject
{
Q_OBJECT
friend class NoatunApp;
public:
	Engine(QObject *parent=0);
	~Engine();
	void setInitialized();
	bool initialized() const;
	
public slots:
	/**
	 * opens the file, use play() to start playing
	 **/
	bool open(const PlaylistItem *file);
	/**
	 * Continues playing
	 **/
	bool play();
	/**
	 * Terminates playing, does not close the file
	 **/
	void pause();
	/**
	 * resets the engine
	 **/
	void stop();
	/**
	 * skips to a timecode
	 * unit is milliseconds
	 **/
	void seek(int msec);

	void setVolume(int percent);

signals:
	void done();
	/**
	 * emitted when arts dies and noatun has to start 
	 * it again. This is called when the new arts
	 * is already initialized
	 **/
	void artsError();
	
public:
	int state();
	int position(); // return position in milliseconds
	int length(); // return track-length in milliseconds
	int volume() const;

private:
	int openMixerFD();
	void closeMixerFD(int);
	void useHardwareMixer(bool);
	bool initArts();

public:
	Arts::SoundServerV2 *server() const;
	Arts::PlayObject playObject() const;
	Arts::SoundServerV2 *simpleSoundServer() const;
	Noatun::StereoEffectStack *effectStack() const;
	Noatun::Equalizer *equalizer() const;
	Noatun::StereoEffectStack *visualizationStack() const;
	Noatun::StereoEffectStack *globalEffectStack() const;
	Noatun::Session *session() const;

private:
	class EnginePrivate;
	EnginePrivate *d;
	bool mPlay;
};

#endif
