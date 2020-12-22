#ifndef _NOATUNSTDACTION_H_
#define _NOATUNSTDACTION_H_

#include <kaction.h>

class KPopupMenu;

namespace NoatunStdAction
{

class PlayAction : public KAction
{
Q_OBJECT
public:
	PlayAction(QObject *parent, const char *name);
private slots:
	void playing();
	void notplaying();
};

class PlaylistAction : public KToggleAction
{
Q_OBJECT
public:
	PlaylistAction(QObject *parent, const char *name);
private slots:
	void shown();
	void hidden();
};

KAction *effects(QObject *parent = 0, const char *name = 0);
KAction *equalizer(QObject *parent = 0, const char *name = 0);

KAction *back(QObject *parent = 0, const char *name = 0);
KAction *stop(QObject *parent = 0, const char *name = 0);
KAction *playpause(QObject *parent = 0, const char *name = 0);
KAction *forward(QObject *parent = 0, const char *name = 0);
KToggleAction *playlist(QObject *parent = 0, const char *name = 0);
KAction *loop(QObject *parent = 0, const char *name = 0);
KAction *play(QObject *parent = 0, const char *name = 0);
KAction *pause(QObject *parent = 0, const char *name = 0);

class ContextMenu
{
public:
	static KPopupMenu *createContextMenu(QWidget *p);

	/**
	 * One menu to show them all, One menu to find them
	 * One menu to bring them all and in the darkness bind them
	 *
	 * In the land of Noatun where the oceans die
	 */
	static KPopupMenu *contextMenu();

	/**
	 * Show the context menu at point
	 **/
	static void showContextMenu(const QPoint &);

	/**
	 * show the context menu at the mouse's current position
	 **/
	static void showContextMenu();
private:
	static KPopupMenu *mContextMenu;
};

}

#endif
