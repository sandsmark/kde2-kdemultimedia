#include "noatunstdaction.h"
#include "noatunapp.h"
#include "player.h"
#include "stereobuttonaction.h"

#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kstdaction.h>

/**
 * A namespace to have all of noatun's standard actions
 * This is treated like KStdAction
 **/
namespace NoatunStdAction	
{
/////////////////////////////////////////////////////
PlayAction::PlayAction(QObject *parent, const char *name)
	: KAction(i18n("Play"), 0, napp->player(), SLOT(playpause()), parent, name)
{
	connect(napp->player(), SIGNAL(playing()), SLOT(playing()));
	connect(napp->player(), SIGNAL(paused()), SLOT(notplaying()));
	connect(napp->player(), SIGNAL(stopped()), SLOT(notplaying()));
	if (napp->player()->isPlaying())
		playing();
	else if (napp->player()->isPaused() || napp->player()->isStopped())
		notplaying();
}

void PlayAction::playing()
{
	setIconSet(QIconSet(SmallIcon("noatunpause")));
	setText(i18n("Pause"));
}

void PlayAction::notplaying()
{
	setIconSet(QIconSet(SmallIcon("noatunplay")));
	setText(i18n("Play"));
}
/////////////////////////////////////////////////////

PlaylistAction::PlaylistAction(QObject *parent, const char *name)
	: KToggleAction(i18n("Show Playlist"), 0, napp->player(), SLOT(toggleListView()), parent, name)
{
	connect(napp->player(), SIGNAL(playlistShown()),  SLOT(shown()));
	connect(napp->player(), SIGNAL(playlistHidden()), SLOT(hidden()));
	setChecked(napp->playlist()->listVisible());
}

void PlaylistAction::shown()
{
	setChecked(true);
}

void PlaylistAction::hidden()
{
	setChecked(false);
}

////////////////////////////////////////////////////

KAction *playpause(QObject *parent, const char *name)
{
	return new PlayAction(parent, name);
}

KAction *effects(QObject *parent, const char *name)
{
	return new KAction(i18n("&Effects..."), "effect", 0, napp, SLOT(effectView()), parent, name);
}

KAction *equalizer(QObject *parent, const char *name)
{
	return new KAction(i18n("E&qualizer..."), 0, napp, SLOT(equalizerView()), parent, name);
}

KAction *back(QObject *parent, const char *name)
{
	return new KAction(i18n("&Back"), "noatunback", 0, napp->player(), SLOT(back()), parent, name);
}

KAction *stop(QObject *parent, const char *name)
{
	StereoButtonAction *action = new StereoButtonAction(i18n("Stop"), "noatunstop", 0, napp->player(), SLOT(stop()), parent, name);
	QObject::connect(napp->player(), SIGNAL(playing()), action, SLOT(enable()));
	QObject::connect(napp->player(), SIGNAL(paused()), action, SLOT(enable()));
	QObject::connect(napp->player(), SIGNAL(stopped()), action, SLOT(disable()));
	if(napp->player()->isStopped())
		action->disable();
	else
		action->enable();
	return action;
}
	
KAction *forward(QObject *parent, const char *name)
{
	return new KAction(i18n("&Forward"), "noatunforward", 0, napp->player(), SLOT(fastForward()), parent, name);
}

KAction *play(QObject *parent, const char *name)
{
	StereoButtonAction *action = new StereoButtonAction(i18n("&Play"), "1rightarrow", 0, napp->player(), SLOT(playpause()), parent, name);
	QObject::connect(napp->player(), SIGNAL(playing()), action, SLOT(disable()));
	QObject::connect(napp->player(), SIGNAL(paused()), action, SLOT(enable()));
	QObject::connect(napp->player(), SIGNAL(stopped()), action, SLOT(enable()));
	if(napp->player()->isPlaying())
		action->disable();
	else
		action->enable();
	return action;
}

KAction *pause(QObject *parent, const char *name)
{
	StereoButtonAction *action = new StereoButtonAction(i18n("&Pause"), "player_pause", 0, napp->player(), SLOT(playpause()), parent, name);
	QObject::connect(napp->player(), SIGNAL(playing()), action, SLOT(enable()));
	QObject::connect(napp->player(), SIGNAL(paused()), action, SLOT(disable()));
	QObject::connect(napp->player(), SIGNAL(stopped()), action, SLOT(disable()));
	if(napp->player()->isPlaying())
		action->enable();
	else
		action->disable();
	return action;
}

KToggleAction *playlist(QObject *parent, const char *name)
{
	return new PlaylistAction(parent, name);
}

KPopupMenu *ContextMenu::mContextMenu = 0;

KPopupMenu *ContextMenu::contextMenu()
{
	if(!mContextMenu) mContextMenu = createContextMenu(0);

	return mContextMenu;
}

KPopupMenu *ContextMenu::createContextMenu(QWidget *p)
{
	KPopupMenu *contextMenu = new KPopupMenu(p, 0);
	KHelpMenu *helpmenu = new KHelpMenu(contextMenu, kapp->aboutData());

	KStdAction::open(napp, SLOT(fileOpen()), contextMenu)->plug(contextMenu);
	KStdAction::quit(napp, SLOT(quit()), contextMenu)->plug(contextMenu);
	contextMenu->insertItem( i18n("&Help"), helpmenu->menu() );
	contextMenu->insertSeparator();
	KStdAction::preferences(napp, SLOT(preferences()), contextMenu)->plug(contextMenu);
	NoatunStdAction::effects(contextMenu)->plug(contextMenu);
	NoatunStdAction::equalizer(napp)->plug(contextMenu);
	contextMenu->insertItem(i18n("&Actions"), napp->pluginMenu());
	return contextMenu;
}

void ContextMenu::showContextMenu(const QPoint &p)
{
	contextMenu()->exec(p);
}

void ContextMenu::showContextMenu()
{
	showContextMenu(QCursor::pos());
}

}

#include "noatunstdaction.moc"
