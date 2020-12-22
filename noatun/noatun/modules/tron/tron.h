// tron.h
//
// Copyright (C) 2000, 2001 Neil Stevens <multivac@fcmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifndef TRON_H
#define TRON_H

#include <klistview.h>
#include <kmainwindow.h>
#include <kurl.h>
#include <qcolor.h>
#include <qmap.h>

#include "plugin.h"

#include "mcp.h"

class KAction;
class KToggleAction;
class KEdFind;

/**
 * TronListViewItem is just a QListViewItem, except that it checks
 * its parent to see if it should be drawn highlighted.
 */
class TronListViewItem : public QListViewItem
{
public:
	TronListViewItem(QListView *, QListViewItem *, QString = QString::null, QString = QString::null);
	virtual ~TronListViewItem();

	static void setColors(QColor _specialfg, QColor _specialbg);
protected:
	virtual void paintCell(QPainter *, const QColorGroup &, int, int, int);

private:
	static QColor *specialFG, *specialBG;
};

/**
 * TronListView handles the drag and drop, and makes sure that only one
 * item at a time is marked "special".
 */
class TronListView : public KListView
{
Q_OBJECT

public:
	TronListView(QWidget *, const char * = 0);
	virtual ~TronListView();

	void setSpecialItem(TronListViewItem *);
	TronListViewItem *specialItem(void);

	virtual void clear(void);
	virtual void takeItem(QListViewItem *);

signals:
	void dropped(KURL::List, QListViewItem *);

protected:
	virtual bool acceptDrag(QDropEvent *) const;

protected slots:
	void myDropEvent(QDropEvent *, QListViewItem *);

private:
	TronListViewItem *special;
};

/**
 * Tron is the plugin itself, and is the view.  It is *not* the playlist.
 * It *has* a playlist, MCP *list.
 */
class Tron : public KMainWindow, public Plugin
{
Q_OBJECT

public:
	Tron();
	virtual ~Tron();

	virtual void init();
	virtual Playlist *playlist(void) const;

	void setColors(QColor _highlightfg, QColor _highlightbg);

	enum Columns
	{
		colTitle = 1,
		colLength = 2,
		colArtist = 4,
		colAlbum = 8,
		colDate = 16,
		colComment = 32
	};

	void setColumns(int);

protected:
	void closeEvent(QCloseEvent *);
	void hideEvent(QHideEvent *);
	void showEvent(QShowEvent *);

private slots:
	void itemMoved(QListViewItem *, QListViewItem *, QListViewItem *);
	void itemExecuted(QListViewItem *);
	void itemDropped(KURL::List, QListViewItem *);
	void itemSelectionChanged(void);

	void fillView(void);

	void search(void);
	void searchDone(void);

	void cleared(void);
	void loaded(void);
	void saved(void);
	void tronadded(PlaylistItem *);
	void removed(PlaylistItem *);
	void modified(PlaylistItem *);
	void current(PlaylistItem *);
	void moved(PlaylistItem *, PlaylistItem *);

	void file_opennew(void);
	void file_open(void);
	void file_save(void);
	void file_saveas(void);
	void edit_add(void);
	void edit_remove(void);
	void edit_adddirectory(void);
	void edit_find(void);
	void options_shuffle(void);
	void options_volume(void);
	void options_menubar(void);
	void options_toolbar(void);
	void options_configuretoolbars(void);

public:
	static Tron *tronical;

private:
	void setModified(bool);

	void addItem(PlaylistItem *item);
	void initializeItem(PlaylistItem *item);
	QMap<TronListViewItem *, PlaylistItem *> viewMap;
	QMap<PlaylistItem *, TronListViewItem *> playMap;

	MCP *list;
	TronListView *listView;

	KAction *edit_removeAction;
	KToggleAction *options_shuffleAction, *options_volumeAction;
	KToggleAction *options_toolbarAction, *options_menubarAction;

	KURL currentPlaylistURL;
	bool currentPlaylistModified;

	KEdFind *findDialog;
	QListViewItem *findCurrent;
};

#endif
