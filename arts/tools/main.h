    /*

    Copyright (C) 2000-2001 Stefan Westerfeld
	                        <stefan@space.twc.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

#ifndef MAIN_H
#define MAIN_H
#include <qiomanager.h>
#include <dispatcher.h>
#include <qmessagebox.h>
#include <kapp.h>
#include <qframe.h>
#include <kmainwindow.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kstdaction.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <artsflow.h>
#include <soundserver.h>
#include <qobject.h>
#include "kartswidget.h"
#include "flowsystem.h"
#include "artsmodules.h"

class LevelMeter;

class PeakBar;

class PeakLevelMeter;

class VScale;

class FFTScopeView : public QWidget {
	Q_OBJECT
protected:
	Arts::StereoFFTScope scopefx;
	Arts::SimpleSoundServer server;
	long effectID;

	std::vector<float> *scopeData;
	std::vector<float> scopeDraw;
	std::vector<LevelMeter *> scopeScales;

	void closeEvent(QCloseEvent *e);
public:
	void updateScopeData();
	FFTScopeView(Arts::SimpleSoundServer server);
	~FFTScopeView();

public slots:
	void updateScope();
signals:
	void closed();
};

class MediaTypesView : public QWidget {
	Q_OBJECT
protected:
	void closeEvent(QCloseEvent *e);
public:
	MediaTypesView();
	~MediaTypesView();

signals:
	void closed();
};

class FreeVerbView : public QWidget {
	Q_OBJECT
protected:
	Arts::Synth_FREEVERB freeverb;
	Arts::SimpleSoundServer server;
	long effectID;

	void closeEvent(QCloseEvent *e);
public:
	FreeVerbView(Arts::SimpleSoundServer server);
	~FreeVerbView();
signals:
	void closed();
};

class ArtsStatusView : public QWidget {
	Q_OBJECT
public:
	ArtsStatusView(Arts::SoundServer server);
	
public slots:
	void updateStatus();
	void suspendButtonClicked();
	
signals:
	void closed();

protected:
	QTimer *artsPollStatusTimer;
	Arts::SoundServer server;
	QLabel *suspendLabel;
	QButton *suspendButton;
	
	void closeEvent(QCloseEvent *e);
};

class VControl : public QFrame {
	Q_OBJECT
protected:
	class StereoLevelMeter *stereoMeter;
	class FFTScopeView *scopeView;
	class MediaTypesView *mediaView;
	class FreeVerbView *freeVerbView;
	class Gui_AUDIO_MANAGER *audioManager;
	class ArtsStatusView *artsStatusView;
	class MidiManagerView *midiManagerView;
	class QSlider *slider;
	Arts::StereoVolumeControl svc;
	Arts::SoundServerV2 server;
	bool inUpdate;
	
	QBoxLayout *boxLayout;

public:
	VControl(QWidget *parent);
	~VControl();

public slots:
	void updateVolume(float newVolume);
	void updateScales();
	void setVolume(int vol);
	void useOldVolumeBar(int old);
	void showScopeView();
	void removeScopeView();
	void showMediaView();
	void removeMediaView();
	void showAudioManager();
	void removeAudioManager();
	void showMidiManager();
	void removeMidiManager();
	void showFreeVerbView();
	void removeFreeVerbView();
	void showArtsStatusView();
	void removeArtsStatusView();
};

class MainWindow : public KMainWindow {
	Q_OBJECT
protected:
	VControl *vc;
	KToggleAction *showOldVolumeDisplay;

public slots:
	void toggleVolumeBar();

public:
	MainWindow();
};

#endif /* MAIN_H */
