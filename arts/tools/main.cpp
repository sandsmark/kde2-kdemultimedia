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

#include "main.h"
#include "main.moc"
#include "levelmeters.h"
#include "objectmanager.h"
#include "midimanagerview.h"
#include "debug.h"
#include "artsversion.h"
#include "kartsfloatwatch.h"
#include <math.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <kaction.h>
#include <kled.h>
#include <kmessagebox.h>
#include <klistview.h>
#include <kdebug.h>
#include <audiomanager.h>
#include <iostream>
#include <map>

using namespace std;
using namespace Arts;

void FreeVerbView::closeEvent(QCloseEvent *e)
{
	e->accept();
	emit closed();
}

FreeVerbView::FreeVerbView(SimpleSoundServer server) : server(server)
{
	freeverb = DynamicCast(server.createObject("Arts::Synth_FREEVERB"));
	arts_assert(!freeverb.isNull());
	freeverb.start();

	// put it into the effect chain
	effectID = server.outstack().insertTop(freeverb,"FreeVerb");

	GenericGuiFactory factory;
	Widget gui = factory.createGui(freeverb);

	QBoxLayout * l = new QHBoxLayout( this );
	if(!gui.isNull())
		l->add(new KArtsWidget(gui, this));
	else
		l->add(new QLabel(i18n("No GUI found for this effect."), this));
	l->freeze();
	show();
}

FreeVerbView::~FreeVerbView()
{
	// remove effect
	server.outstack().remove(effectID);
}

void FFTScopeView::updateScopeData()
{
	if(scopeData) delete scopeData;
	scopeData = scopefx.scope();
}

void FFTScopeView::updateScope()
{
	updateScopeData();

	for(unsigned int i=0;i<scopeData->size();i++)
	{
		scopeDraw[i] /= 1.25;
		if((*scopeData)[i] > scopeDraw[i]) scopeDraw[i] = (*scopeData)[i];
		scopeScales[i]->setValue(scopeDraw[i]);
	}
}

void FFTScopeView::closeEvent(QCloseEvent *e)
{
	e->accept();
	cout << "close()" << endl;
	emit closed();
}

FFTScopeView::FFTScopeView(SimpleSoundServer server)
	: server(server), scopeData(0)
{
	/*
	   create a stereo fft scope on the server and push it into the
	   effect chain
	*/
	{
		scopefx = DynamicCast(server.createObject("Arts::StereoFFTScope"));
		assert(!scopefx.isNull());
		scopefx.start();

		// put it into the effect chain
		effectID = server.outstack().insertBottom(scopefx,"FFT Scope");
	}

	updateScopeData();
	QBoxLayout * l = new QHBoxLayout( this );
	l->setAutoAdd(TRUE);

	for(unsigned int i=0;i<scopeData->size();i++)
	{
		scopeScales.push_back(new LedMeter(this,true));
		//scopeScales.push_back(new PeakBar(this));
		scopeDraw.push_back(0.0);
	}

	l->activate();
	show();
	setFixedSize(300,200);

	QTimer *updatetimer = new QTimer(this);
	updatetimer->start(100);
	connect(updatetimer,SIGNAL(timeout()),this,SLOT(updateScope()));
}

FFTScopeView::~FFTScopeView()
{
	// remove effect
	server.outstack().remove(effectID);
}

MediaTypesView::MediaTypesView()
{
	QBoxLayout *l= new QHBoxLayout(this);
	l->setAutoAdd(true);

	KListView *listView = new KListView(this);
	listView->addColumn(i18n("Media Type"));

	Arts::TraderQuery q;
	std::vector<Arts::TraderOffer> *results = q.query();
	std::map<std::string, bool> done;
	QString str;

	for(std::vector<Arts::TraderOffer>::iterator i = results->begin(); i != results->end(); i++)
	{
		std::vector<string> *ext = (*i).getProperty("Extension");

		for(vector<string>::iterator it = ext->begin(); it != ext->end(); it++)
		{
			if(!(*it).length() || done[*it])
			    continue;

			done[*it] = true;
			(void) new QListViewItem(listView, (*it).c_str());
		}
		delete ext;
	}
	delete results;

	l->activate();
	show();
	setFixedSize(300,200);
}

MediaTypesView::~MediaTypesView()
{
}

void MediaTypesView::closeEvent(QCloseEvent *e)
{
	e->accept();
	emit closed();
}



ArtsStatusView::ArtsStatusView(Arts::SoundServer a_server)
	: server(a_server)
{
	QBoxLayout *l= new QVBoxLayout(this);
	//l->setAutoAdd(TRUE);

	RealtimeStatus rs= server.realtimeStatus();
    l->addWidget(new QLabel(rs==rtRealtime?
							i18n("Artsd is running with realtime scheduling."):
							rs==rtNoSupport?
							i18n("Your system does not support realtime scheduling."):
							rs==rtNoWrapper?
							i18n("Artsd is not configured for realtime scheduling\n "
								 "or was manually started without artswrapper."):
							i18n("Artsd should run with realtime scheduling,\n but it "
								 "does not (is artswrapper suid root?)."),
							this, "realtimeLabel"));
	l->addSpacing(10);

	suspendLabel= new QLabel(i18n("Determining suspend status..."),
							 this, "suspendLabel");
	l->addWidget(suspendLabel);
	l->addSpacing(6);
	l->setMargin(6);

	suspendButton= new QPushButton(this, "suspendButton");
	suspendButton->setText(i18n("&Suspend Now"));
	l->addWidget(suspendButton);
	connect(suspendButton, SIGNAL(clicked()), this, SLOT(suspendButtonClicked()));

	artsPollStatusTimer= new QTimer(this);
	connect(artsPollStatusTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
	artsPollStatusTimer->start(1000);
	//l->activate();
	show();
}

void ArtsStatusView::suspendButtonClicked()
{
	(void)server.suspend(); // TODO: error msg if suspend not possible?
}

void ArtsStatusView::updateStatus()
{
	long seconds= server.secondsUntilSuspend();
	if (seconds<0)
		suspendLabel->setText(i18n("The aRts sound daemon will not autosuspend right\n"
								  "now since there are active modules."));
	else if (seconds==0)
		suspendLabel->setText(i18n("The aRts sound daemon is suspended. Legacy\n "
								  "applications can use the sound card now."));
	else
		suspendLabel->setText(i18n("Autosuspend will happen in %1 seconds.").
							 arg(seconds));
	suspendButton->setEnabled(seconds>0);
}

void ArtsStatusView::closeEvent(QCloseEvent *e)
{
	e->accept();
	emit closed();
}



VControl::VControl(QWidget *parent) : QFrame(parent), scopeView(0), mediaView(0),
	freeVerbView(0), audioManager(0), artsStatusView(0), midiManagerView(0),
	server(Reference("global:Arts_SoundServerV2")), inUpdate(false)
{
   	if(server.isNull())
   	{
		KMessageBox::error( 0, i18n("Connection to the soundserver failed - make sure that artsd is really running and that your kdelibs version is not older than kdemultimedia."));
		exit(1);
   	}
	boxLayout = new QHBoxLayout( this );
	//l->setAutoAdd(TRUE);
	// stereoMeter= new StereoLedMeters(this);
	// setMinimumSize(80,300);
	stereoMeter= new PeakLevelMeters(this);
	boxLayout->addWidget(stereoMeter);

	// 0   => 4.0
	// 200 => 2.0
	// 400 => 1.0
	// 600 => 0.5
	// 800 => 0.25
	// 1000 => 0.125
	// 1200 => 0.0 (forced)

	slider = new QSlider(0,1200,100,400,QSlider::Vertical,this,"volumeslider");
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
	slider->setTickmarks( QSlider::Right );
	boxLayout->addWidget(slider);
	boxLayout->activate();

	show();
	svc = server.outVolume();
	assert(!svc.isNull());

	QTimer *updatescalestimer = new QTimer(this);
	updatescalestimer->start(100);
	connect(updatescalestimer,SIGNAL(timeout()),this,SLOT(updateScales()));

	KArtsFloatWatch *watch = new KArtsFloatWatch(svc, "scaleFactor_changed", this);
	connect(watch, SIGNAL(valueChanged(float)), this, SLOT(updateVolume(float)));
	updateVolume(svc.scaleFactor());
}

VControl::~VControl()
{
	removeScopeView();
	removeFreeVerbView();
	removeAudioManager();
	removeMediaView();
	removeArtsStatusView();
	removeMidiManager();
}

void VControl::updateScales()
{
	stereoMeter->setValues(svc.currentVolumeLeft(), svc.currentVolumeRight());
}

void VControl::setVolume(int vol)
{
	if(inUpdate) return;

	float volume = pow(2.0,(400.0 - vol)/200.0);
	if(vol == 1200) volume = 0.0;

	svc.scaleFactor(volume);
}

void VControl::updateVolume(float volume)
{
	/*
	 * the inUpdate thing is important, because it stops us from sending the
	 * value to the server again - if we did that, two clients could bounce
	 * volume changes between the server and the clients
	 */
	inUpdate = true;
	slider->setValue((volume < 0.001)?1200:(400 - 200 * log(volume)/log(2.0)));
	inUpdate = false;
}

void VControl::useOldVolumeBar(int old)
{
	delete stereoMeter;
	if (old) {
		stereoMeter= new StereoLedMeters(this);
	} else {
		stereoMeter= new PeakLevelMeters(this);
	}
	boxLayout->insertWidget(0, stereoMeter);
	stereoMeter->show();
}

void VControl::showScopeView()
{
	if(!scopeView)
	{
		scopeView = new FFTScopeView(server);
		connect(scopeView,SIGNAL(closed()),this,SLOT(removeScopeView()));
	}
}

void VControl::showFreeVerbView()
{
	if(!freeVerbView)
	{
		freeVerbView = new FreeVerbView(server);
		connect(freeVerbView,SIGNAL(closed()),this,SLOT(removeFreeVerbView()));
	}
}

void VControl::showAudioManager()
{
	if(!audioManager)
	{
		cout << "new GUI_AUDIO_MANAGER" << endl;
		audioManager = new Gui_AUDIO_MANAGER();
		connect(audioManager,SIGNAL(closed()),this,SLOT(removeAudioManager()));
	}
}

void VControl::showMediaView()
{
	if(!mediaView)
	{
		mediaView = new MediaTypesView();
		connect(mediaView,SIGNAL(closed()),this,SLOT(removeMediaView()));
	}
}

void VControl::showArtsStatusView()
{
	if(!artsStatusView)
	{
		artsStatusView = new ArtsStatusView(server);
		connect(artsStatusView,SIGNAL(closed()),this,SLOT(removeArtsStatusView()));
	}
}

void VControl::showMidiManager()
{
	if(!midiManagerView)
	{
		midiManagerView = new MidiManagerView();
		connect(midiManagerView,SIGNAL(closed()),this,SLOT(removeMidiManager()));
	}
}

void VControl::removeAudioManager()
{
	if(audioManager) delete audioManager;
	audioManager = 0;
}

void VControl::removeMidiManager()
{
	if(midiManagerView) delete midiManagerView;
	midiManagerView = 0;
}

void VControl::removeScopeView()
{
	if(scopeView) delete scopeView;
	scopeView = 0;
}

void VControl::removeFreeVerbView()
{
	if(freeVerbView) delete freeVerbView;
	freeVerbView = 0;
}

void VControl::removeMediaView()
{
	if(mediaView) delete mediaView;
	mediaView = 0;
}

void VControl::removeArtsStatusView()
{
	if(artsStatusView) delete artsStatusView;
	artsStatusView = 0;
}

void MainWindow::toggleVolumeBar()
{
	vc->useOldVolumeBar(showOldVolumeDisplay->isChecked());
}

MainWindow::MainWindow() : KMainWindow(0)
{
	vc = new VControl(this);
	setCentralWidget(vc);
	(void)new KAction(i18n("View &FFT Scope"), 0, vc, SLOT(showScopeView()),
		  actionCollection(), "view_fft_scope");
	(void)new KAction(i18n("View &Audio Manager"), 0, vc,
		SLOT(showAudioManager()), actionCollection(), "view_audio_manager");
	(void)new KAction(i18n("View aRts &Status"), 0, vc,
		SLOT(showArtsStatusView()), actionCollection(), "view_arts_statusview");
	(void)new KAction(i18n("View &Midi Manager"), 0, vc,
		SLOT(showMidiManager()), actionCollection(), "view_midi_manager");
	(void)new KAction(i18n("View Available Media &Types"), 0, vc,
		SLOT(showMediaView()), actionCollection(), "view_media_types");
	(void)new KAction(i18n("Free&Verb"), 0, vc,
		SLOT(showFreeVerbView()), actionCollection(), "view_freeverb");
	showOldVolumeDisplay=
		new KToggleAction(i18n("&Leds-like volume display"), 0, this,
		SLOT(toggleVolumeBar()), actionCollection(), "old_volume_display");

	createGUI("artscontrol.rc");
	resize(20,300);
	show();
}

int main(int argc, char **argv)
{
	KAboutData aboutData( "artscontrol", I18N_NOOP("aRts control"),
		ARTS_VERSION, I18N_NOOP("Control tool for the aRts server"),
			KAboutData::License_GPL, "(c) 2000 Stefan Westerfeld");

  	KCmdLineArgs::init( argc, argv, &aboutData );

	KApplication app(argc, argv);

	// setup mcop communication
	QIOManager qiomanager;
	Dispatcher dispatcher(&qiomanager);

	ObjectManager::the()->provideCapability("kdegui");

	app.setMainWidget(new MainWindow);
	return app.exec();
}
