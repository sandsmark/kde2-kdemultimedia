#ifndef DCOPIFACE_H
#define DCOPIFACE_H

#include "dcopiface.h"
#include <player.h>
#include <plugin.h>

#include <dcopobject.h>

class NIF : public Plugin, public DCOPObject
{
K_DCOP

public:
	NIF();
	~NIF();

private slots:
	void newSongPlaying();

k_dcop:
	void toggleListView();
	void handleButtons();
	void removeCurrent();

	void back();
	void stop();
	void play();
	void playpause();
	void fastForward();
	void skipTo(int);
	void loop();

	void newCurrent();
	void playCurrent();
	void setVolume(int);

	/**
	 * length in milliseconds
	 **/
	int length();
	/**
	 * position in milliseconds
	 **/
	int position();

	/**
	 * 0 stopped
	 * 1 paused
	 * 2 playing
	 **/
	int state();

	QString lengthString();
	QString title();

	QCString visStack();
	QCString session();
	
	void addFile(const QString& f, bool autoplay);
	void addFile(const QStringList &f, bool autoplay);

	void loadPlugin(const QString &specFile);

#ifdef DOCUMENTATION_BLEH_BLEH_DONT_TRY_COMPILING_THIS
signals:
	void exiting();
	void newFile();

#endif
};

#endif

