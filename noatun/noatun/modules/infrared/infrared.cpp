
#include <stdio.h>

#include <unistd.h>
#include <player.h>
#include <noatunapp.h>

#include <klocale.h>

#include "infrared.h"
#include "lirc.h"
#include "irprefs.h"

extern "C"
{
	Plugin *create_plugin()
	{
		return new InfraRed();
	}
}


InfraRed::InfraRed()
	: QObject(),
	  Plugin()
{
	NOATUNPLUGINC(InfraRed);
	m_lirc = new Lirc(this);
	connect(m_lirc,
	        SIGNAL(commandReceived(const QString &, const QString &, int)),
	        SLOT(slotCommand(const QString &, const QString &, int)));

	IRPrefs::s_lirc = m_lirc;
	new IRPrefs(this);
}

InfraRed::~InfraRed()
{
}

void InfraRed::slotCommand(const QString &remote, const QString &button, int repeat)
{
	switch (IRPrefs::actionFor(remote, button, repeat))
	{
	case IRPrefs::None:
		break;
	
	case IRPrefs::Play:
		napp->player()->play();
		break;
	
	case IRPrefs::Stop:
		napp->player()->stop();
		break;
	
	case IRPrefs::Previous:
		napp->player()->back();
		break;
	
	case IRPrefs::Next:
		napp->player()->fastForward();
		break;
	
	case IRPrefs::VolumeDown:
		napp->player()->setVolume(napp->player()->volume() - 1);
		break;
	
	case IRPrefs::VolumeUp:
		napp->player()->setVolume(napp->player()->volume() + 1);
		break;
	}
}

#include "infrared.moc"

