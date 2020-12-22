#ifndef PLAYER_H
#define PLAYER_H

#include <qobject.h>
#include <qtimer.h>
#include <kurl.h>
#include <noatunplaylist.h>

class Engine;
class Playlist;
class KLibrary;

/**
 * This class has slots for all the common media player buttons
 * The slots are called, and it queries the Playlist for the appropriate
 * file.
 **/
class Player : public QObject
{
Q_OBJECT
friend class Effects;
friend class PlaylistItem;

public:
	enum LoopType { None=0, Song, Playlist };
	
public:
	Player(QObject *parent=0);
	~Player();

	/**
	 * returns a string with the time that can
	 * be used in the UI:
	 * CC:CC/LL:LL (mm:ss)
	 **/
	QString lengthString(int _position=-1);
	/**
	 * returns the LoopType enum
	 **/
	int loopStyle() const { return mLoopStyle; }
	/**
	 * the volume from 0-100
	 * use volume() instead
	 **/
	int volume() const;
	/**
	 * the position in milliseconds
	 **/
	int getTime() const { return position; }
	/**
	 * the track-length in milliseconds
	 **/
	int getLength();
	/**
	 * true if we're playing
	 **/
	bool isPlaying();
	/**
	 * true if paused
	 **/
	bool isPaused();
	/**
	 * true if stopped
	 **/
	bool isStopped();

	/**
	 * get the current playlist
	 * this may be null
	 * And this may not be necessarily an item allocated
	 * by playlist()
	 **/
	PlaylistItem *current() const { return mCurrent;}

	/**
	 * load the @param file, clear the playlist if @param purg is set
	 * and if autoplay is set, play the item.
	 **/
	void openFile(const KURL &file, bool purge=true, bool autoplay=false);
	
	Engine *engine() const { return mEngine; }
	
public slots:
	/**
	 * show or hide the playlist
	 **/
	void toggleListView();
	/**
	 * force the playing/paused/stopped/playlist shown signals to
	 * be sent out
	 **/
	void handleButtons();
	/**
	 * remove current from playlist
	 **/
	void removeCurrent();
	
	/**
	 * go back a track
	 **/
	void back();
	/**
	 * stop playing
	 **/
	void stop();
	/**
	 * start playing
	 **/
	void play();
	/**
	 * start playing, or pause if we're currently playing
	 **/
	void playpause();
	/**
	 * go forward a track
	 **/
	void fastForward(bool allowLoop);
	/**
	 * same as fastForward(true)
	 **/
	void fastForward() { fastForward(true); }
	
	/**
	 * skip to the position, unit is milliseconds
	 **/
	void skipTo(int msec);
	void loop();
	void loop(int i);

	void newCurrent();
	void playCurrent();
	void setVolume(int);

	void toggleInterfaces();
	void showInterfaces();
	void hideInterfaces();

private slots:
	void posTimeout();

signals:
	/**
	 * Tells you to update the seekbar and volume
	 **/
	void timeout();

	/**
	 * Tells you to hide
	 */
	void hideYourself();

	/**
	 * Tells you to show again
	 */
	void showYourself();

	void stopped();
	void playing();
	void paused();
	void loopTypeChange(int t);
	void playlistShown();
	void playlistHidden();

	/**
	 * called at the same time as newSong, but
	 * maybe easier to work with
	 **/
	void newSongLen(int mins, int sec);

	/**
	 * when a new song is currently playing
	 **/
	void newSong();

	/**
	 * Called when a new song is about to be played, but
	 * hasn't started.  player->current() is the
	 * next song
	 **/
	void changed();
	
	void volumeChanged(int);

private:
	Engine *mEngine;
	QTimer filePos;
	int position;
	int mLoopStyle;
	bool firstTimeout;
	PlaylistItem *mCurrent;
	bool mDeleteMe;
	bool showingInterfaces;
};

#endif

