#include <engine.h>
#include <player.h>
#include <noatunapp.h>

#include <qlayout.h>

#include <kglobalaccel.h>
#include <kkeydialog.h>
#include <klocale.h>

#include "keyz.h"

extern "C"
{
	Plugin *create_plugin()
	{
		return new Keyz();
	}
}

KGlobalAccel * Keyz::s_accel = 0L;

Keyz::Keyz() : QObject( 0L, "Keyz" ), Plugin()
{
	NOATUNPLUGINC(Keyz);
	Player *player = napp->player();

	if ( !s_accel )
	{
		s_accel = new KGlobalAccel();
		s_accel->insertItem( i18n("Play/Pause"), "PlayPause", "CTRL+ALT+P" );
		s_accel->insertItem( i18n("Stop playing"), "Stop", "CTRL+ALT+S" );
		s_accel->insertItem( i18n("Back"), "Back", "CTRL+ALT+Left" );
		s_accel->insertItem( i18n("Forward"), "Forward", "CTRL+ALT+Right" );
		s_accel->insertItem( i18n("Show Playlist"), "Playlist", "CTRL+ALT+L" );
		s_accel->insertItem( i18n("Open file to play"), "OpenFile",
				     "CTRL+ALT+O" );
		s_accel->insertItem( i18n("Effects configuration"), "Effects",
				     "CTRL+ALT+E" );
		s_accel->insertItem( i18n("Preferences"), "Preferences", "CTRL+ALT+F");
		s_accel->insertItem( i18n("Volume up"), "VolumeUp",
				     "CTRL+ALT+SHIFT+Up" );
		s_accel->insertItem( i18n("Volume Down"), "VolumeDown",
				     "CTRL+ALT+SHIFT+Down" );
		s_accel->insertItem( i18n("Seek forward"), "SeekForward",
				     "CTRL+ALT+SHIFT+Right" );
		s_accel->insertItem( i18n("Seek backward"), "SeekBackward",
				     "CTRL+ALT+SHIFT+Left" );

		s_accel->connectItem( "PlayPause", player, SLOT( playpause() ));
		s_accel->connectItem( "Stop", player, SLOT( stop() ));
		s_accel->connectItem( "Back", player, SLOT( back() ));
		s_accel->connectItem( "Forward", player, SLOT( fastForward() ));
		s_accel->connectItem( "Playlist", player, SLOT( toggleListView() ));
		s_accel->connectItem( "OpenFile", napp, SLOT( fileOpen() ));
		s_accel->connectItem( "Effects", napp, SLOT( effectView() ));
		s_accel->connectItem( "Preferences", napp, SLOT( preferences() ));
		s_accel->connectItem( "VolumeUp", this, SLOT( slotVolumeUp() ));
		s_accel->connectItem( "VolumeDown", this, SLOT( slotVolumeDown() ));
		s_accel->connectItem( "SeekForward", this, SLOT( slotForward() ));
		s_accel->connectItem( "SeekBackward", this, SLOT( slotBackward() ));

		s_accel->readSettings();
	}

	new KeyzPrefs(this);
}

Keyz::~Keyz()
{
	delete s_accel;
	s_accel = 0L;
}

void Keyz::slotVolumeUp()
{
	int vol = napp->player()->volume();
	if ( vol >= 100 )
		return;

	napp->player()->setVolume( vol + 4 );
}

void Keyz::slotVolumeDown()
{
	int vol = napp->player()->volume();
	if ( vol <= 0 )
		return;

	napp->player()->setVolume( vol - 4 );
}

void Keyz::slotForward()
{
	napp->player()->skipTo( QMIN(napp->player()->getLength(), napp->player()->getTime() + 3000) ); // + 3 seconds
}

void Keyz::slotBackward()
{
	napp->player()->skipTo( QMAX( 0, napp->player()->getTime() - 3000 )); // - 3 seconds
}

///////////////////////////////////////////////////////////////////

KeyzPrefs::KeyzPrefs( QObject *parent ) :
	CModule( i18n("Keyz"), i18n("Shortcut Configuration"), parent )
{
	m_keys = Keyz::accel()->keyDict();
	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->addWidget( new KKeyChooser( &m_keys, this ) );
}

void KeyzPrefs::save()
{
	Keyz::accel()->setKeyDict( m_keys );
	Keyz::accel()->writeSettings();
}

#include "keyz.moc"
