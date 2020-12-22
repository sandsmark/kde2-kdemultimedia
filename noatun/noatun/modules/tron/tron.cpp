// tron.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <kaction.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <keditcl.h>
#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kstdaction.h>
#include <kstddirs.h>
#include <kurldrag.h>
#include <qfileinfo.h>
#include <qframe.h>
#include <qheader.h>
#include <qlayout.h>
#include <qurl.h>

#include "cmodule.h"
#include "mcp.h"
#include "tron.h"
#include <player.h>

//////////////////////
// TronListViewItem //
//////////////////////

QColor *TronListViewItem::specialFG = 0;
QColor *TronListViewItem::specialBG = 0;

TronListViewItem::TronListViewItem(QListView *parent, QListViewItem *after, QString t1, QString t2)
	: QListViewItem(parent, after, t1, t2)
{
}

TronListViewItem::~TronListViewItem()
{
}

void TronListViewItem::setColors(QColor _specialfg, QColor _specialbg)
{
	QColor *oldFG, *oldBG;
	oldFG = specialFG;
	oldBG = specialBG;

	specialFG = new QColor(_specialfg);
	specialBG = new QColor(_specialbg);

	if(oldFG)
	{
		delete oldFG;
		delete oldBG;
	}
}

static QColor desaturate(QColor orig)
{
	int h, s, v;
	orig.hsv(&h, &s, &v);
	return QColor(h, s / 2, v, orig.QColor::Hsv);
}

void TronListViewItem::paintCell(QPainter *painter, const QColorGroup &_group, int column, int width, int alignment)
{
	QColorGroup group = _group;
	if(static_cast<TronListView *>(listView())->specialItem() == this)
	{
		if(!specialFG)
			setColors(qRgb(255, 255, 255), qRgb(64, 64, 192));

		group.setColor(QColorGroup::Base, *specialBG);
		group.setColor(QColorGroup::Text, *specialFG);
		group.setColor(QColorGroup::Highlight, desaturate(*specialBG));
		group.setColor(QColorGroup::HighlightedText, *specialFG);
	}
	QListViewItem::paintCell(painter, group, column, width, alignment);
}

//////////////////
// TronListView //
//////////////////

TronListView::TronListView(QWidget *parent, const char *name)
	: KListView(parent, name)
	, special(0)
{
	connect( this, SIGNAL(dropped(QDropEvent *, QListViewItem *)), this, SLOT(myDropEvent(QDropEvent *, QListViewItem *)) );
}

TronListView::~TronListView()
{
}

void TronListView::setSpecialItem(TronListViewItem *item)
{
	if(!item || item->listView() != this)
		special = 0;
	else
		special = item;

	updateContents();
}

TronListViewItem *TronListView::specialItem(void)
{
	return special;
}

void TronListView::clear(void)
{
	special = 0;
	KListView::clear();
}

void TronListView::takeItem(QListViewItem *item)
{
	if(item == special) special = 0;
	KListView::takeItem(item);
}

bool TronListView::acceptDrag(QDropEvent *e) const
{
	KURL::List urls;
	bool retval = QUriDrag::canDecode(e) || KListView::acceptDrag(e);
	return retval;
}

void TronListView::myDropEvent(QDropEvent *e, QListViewItem *after)
{
	KURL::List urls;
	if(KURLDrag::decode(e, urls))
	{
		emit dropped(urls, after);
	}
}

//////////
// Tron //
//////////

Tron *Tron::tronical = 0;

Tron::Tron()
	: KMainWindow()
	, Plugin()
{
	tronical = this;

	(void)KStdAction::openNew(this, SLOT(file_opennew()), actionCollection());
	(void)KStdAction::open(this, SLOT(file_open()), actionCollection());
	(void)KStdAction::save(this, SLOT(file_save()), actionCollection());
	(void)KStdAction::saveAs(this, SLOT(file_saveas()), actionCollection());
	(void)KStdAction::close(this, SLOT(close()), actionCollection());
	(void)KStdAction::find(this, SLOT(edit_find()), actionCollection());
	(void)new KAction(i18n("&Add..."), Key_Plus, this, SLOT(edit_add()), actionCollection(), "edit_add");
	options_toolbarAction = KStdAction::showToolbar(this, SLOT(options_toolbar()), actionCollection());
	options_menubarAction = KStdAction::showMenubar(this, SLOT(options_menubar()), actionCollection());
	edit_removeAction = new KAction(i18n("&Remove"), Key_Delete, this, SLOT(edit_remove()), actionCollection(), "edit_remove");
	(void)new KAction(i18n("Add &Directory..."), 0, this, SLOT(edit_adddirectory()), actionCollection(), "edit_adddirectory");
	options_shuffleAction = new KToggleAction(i18n("&Shuffle"), 0, this, SLOT(options_shuffle()), actionCollection(), "options_shuffle");
	options_volumeAction = new KToggleAction(i18n("Save &Volume"), 0, this, SLOT(options_volume()), actionCollection(), "options_volume");
	(void)KStdAction::preferences(napp, SLOT(preferences()), actionCollection());
	(void)KStdAction::configureToolbars(this, SLOT(options_configuretoolbars() ), actionCollection(), "configuretoolbars");

	edit_removeAction->setEnabled(false);

	createGUI("tronui.rc");

	list = new MCP(this);
}

Tron::~Tron()
{
	KConfig &config = *KGlobal::config();
	toolBar("mainToolBar")->saveSettings(&config, "Tron mainToolBar");
	config.setGroup("tron");
	config.writeEntry("modified", currentPlaylistModified);
	config.writeEntry("currentPlaylistURL", currentPlaylistURL.url());
	config.writeEntry("width", width());
	config.writeEntry("height", height());
	config.writeEntry("shuffle", options_shuffleAction->isChecked());
	config.writeEntry("volume", options_volumeAction->isChecked());
	config.writeEntry("menuShown", options_menubarAction->isChecked());

	if(listView->specialItem())
	{
		int specialNum = 0;
		for(QListViewItem *cur = listView->firstChild(); cur != listView->specialItem(); cur = cur->itemBelow())
			specialNum++;
		config.writeEntry("current", specialNum);
	}
	else
	{
		config.writeEntry("current", -1);
	}

	KURL url(QUrl(napp->dirs()->saveLocation("data", "noatun/") + "trondata"));
	list->save(url);

	delete list;
	list = 0;
}

void Tron::init(void)
{
	connect( list, SIGNAL(cleared()), this, SLOT(cleared()) );
	connect( list, SIGNAL(loaded()), this, SLOT(loaded()) );
	connect( list, SIGNAL(saved()), this, SLOT(saved()) );
	connect( list, SIGNAL(added(PlaylistItem *)), this, SLOT(tronadded(PlaylistItem *)) );
	connect( list, SIGNAL(removed(PlaylistItem *)), this, SLOT(removed(PlaylistItem *)) );
	connect( list, SIGNAL(modified(PlaylistItem *)), this, SLOT(modified(PlaylistItem *)) );
	connect( list, SIGNAL(current(PlaylistItem *)), this, SLOT(current(PlaylistItem *)) );
	connect( list, SIGNAL(moved(PlaylistItem *, PlaylistItem *)), this, SLOT(moved(PlaylistItem *, PlaylistItem *)) );

	QFrame *box = new QFrame(this);
	setCentralWidget(box);

	listView = new TronListView(box);
	listView->setAcceptDrops(true);
	listView->setDragEnabled(true);
	listView->setSorting(-1);
	listView->setMultiSelection(true);
	listView->setSelectionModeExt(KListView::Extended);
	listView->setAllColumnsShowFocus(true);
	listView->addColumn(i18n("Title"));
	listView->addColumn(i18n("Length"));
	listView->addColumn(i18n("Artist"));
	listView->addColumn(i18n("Album"));
	listView->addColumn(i18n("Date"));
	listView->addColumn(i18n("Comment"));
	connect( listView, SIGNAL(moved(QListViewItem *, QListViewItem *, QListViewItem *)), this, SLOT(itemMoved(QListViewItem *, QListViewItem *, QListViewItem *)) );
	connect( listView, SIGNAL(executed(QListViewItem *)), this, SLOT(itemExecuted(QListViewItem *)) );
	connect( listView, SIGNAL(dropped(KURL::List, QListViewItem *)), this, SLOT(itemDropped(KURL::List, QListViewItem *)) );
	connect( listView, SIGNAL(selectionChanged()), this, SLOT(itemSelectionChanged(void)) );

	QVBoxLayout *layout = new QVBoxLayout(box, 0, KDialog::spacingHint());
	layout->addWidget(listView);

	hide();

	KURL url(QUrl(napp->dirs()->saveLocation("data", "noatun/") + "trondata"));
	list->load(url, true);

	// this has to come after the load, because of the modified flag
	KConfig &config = *KGlobal::config();

	toolBar("mainToolBar")->applySettings(&config, "Tron mainToolBar");
	options_toolbarAction->setChecked( !toolBar("mainToolBar")->isHidden() );

	config.setGroup("tron");
	options_menubarAction->setChecked( config.readBoolEntry("menuShown", true) );
	options_menubar();

	currentPlaylistURL = KURL();
	QString playlistURL = config.readEntry("currentPlaylistURL", QString::null);
	if(!playlistURL.isNull())
	{
		currentPlaylistURL = KURL(playlistURL);
	}
	else
	{
		// 2.1 compatibility behavior
		playlistURL = config.readEntry("currentPlaylist", QString::null);
		if(!playlistURL.isNull())
		{
			currentPlaylistURL = KURL(QUrl(config.readEntry("currentPlaylist", QString::null)));
		}
	}

	setModified(config.readBoolEntry("modified", false));
	
	int savedWidth = config.readLongNumEntry("width", 0);
	int savedHeight = config.readLongNumEntry("height", 0);
	if(savedWidth && savedHeight) resize(savedWidth, savedHeight);

	list->reset();
	int savedCurrent = config.readLongNumEntry("current", -1);
	for(int i = 0; i < savedCurrent; i++)
		list->next();

	options_shuffleAction->setChecked(config.readBoolEntry("shuffle", false));
	options_shuffle();

	options_volumeAction->setChecked(config.readBoolEntry("volume", false));
	options_volume();

	TronModule *module = new TronModule(this);
	module->reopen();
	module->save();
}

Playlist *Tron::playlist(void) const
{
	return list;
}

void Tron::setColors(QColor _highlightfg, QColor _highlightbg)
{
	TronListViewItem::setColors(_highlightfg, _highlightbg);
}

static inline void hideColumn(KListView *listView, int column)
{
	listView->setColumnWidthMode(column, QListView::Manual);
	listView->setColumnWidth(column, 0);
	listView->header()->setResizeEnabled(false, column);
}

static inline void showColumn(KListView *listView, int column)
{
	listView->header()->setResizeEnabled(true, column);
	listView->setColumnWidthMode(column, QListView::Maximum);
	if(listView->columnWidth(column) < 50)
		listView->setColumnWidth(column, 50);
}

void Tron::setColumns(int columns)
{
	if(columns & colTitle)
		showColumn(listView, 0);
	else
		hideColumn(listView, 0);

	if(columns & colLength)
		showColumn(listView, 1);
	else
		hideColumn(listView, 1);

	if(columns & colArtist)
		showColumn(listView, 2);
	else
		hideColumn(listView, 2);

	if(columns & colAlbum)
		showColumn(listView, 3);
	else
		hideColumn(listView, 3);

	if(columns & colDate)
		showColumn(listView, 4);
	else
		hideColumn(listView, 4);

	if(columns & colComment)
		showColumn(listView, 5);
	else
		hideColumn(listView, 5);
}

void Tron::closeEvent(QCloseEvent *)
{
	if(list) list->hideList();
	hide();
}

void Tron::hideEvent(QHideEvent *e)
{
	KMainWindow::hideEvent(e);
}

void Tron::showEvent(QShowEvent *e)
{
	if(list) list->showList();
	KMainWindow::showEvent(e);
}

void Tron::itemMoved(QListViewItem *item, QListViewItem *, QListViewItem *newSibling)
{
	list->moveAfter(viewMap[(TronListViewItem *)item], viewMap[(TronListViewItem *)newSibling]);
}

void Tron::itemExecuted(QListViewItem *item)
{
	list->play(viewMap[(TronListViewItem *)item]);
}

static bool fileIsDir(QString s)
{
	QFileInfo info(s);
	return info.isDir();
}

void Tron::itemDropped(KURL::List urlList, QListViewItem *viewItem)
{
	PlaylistItem *item = viewMap[static_cast<TronListViewItem *>(viewItem)];

	for (KURL::List::Iterator i = urlList.begin(); i != urlList.end(); ++i)
	{
		if((*i).fileName().right(4) == ".m3u")
			item = list->importWindowsPlaylist((*i), item);
		// I hope you're happy, Charles.. I *could* have opened the file and looked
		// for <!DOCTYPE NoatunPlaylist>, but I gave it a filename extension instead
		else if((*i).fileName().right(13) == ".tronplaylist")
			item = list->importPlaylist((*i), item);
		else if( (*i).isLocalFile() && fileIsDir((*i).path()) )
			item = list->addDirectory((*i), item);
		else
			item = list->addFile((*i), item);
	}
}

void Tron::itemSelectionChanged(void)
{
	QList<QListViewItem> selected = listView->selectedItems();
	edit_removeAction->setEnabled( selected.count() > 0 );
}

void Tron::fillView(void)
{
	listView->clear();
	viewMap.clear();
	playMap.clear();
	for(PlaylistItem *cur = list->getFirst(); cur; cur = list->getAfter(cur))
	{
		addItem(cur);
	}
}

void Tron::search(void)
{
	bool found = false;
	bool done = false;

	if(!findCurrent)
	{
		if(findDialog->get_direction())
		{
			// backward
			if(KMessageBox::Yes == KMessageBox::questionYesNo(findDialog,
			   i18n("Beginning of document reached.\nContinue from the end?"),
			   i18n("Find"))
			)
			{
				findCurrent = listView->lastChild();
			}
			else
			{
				findCurrent = listView->firstChild();
				done = true;
			}
		}
		else
		{
			// forward
			if(KMessageBox::Yes == KMessageBox::questionYesNo(findDialog,
			   i18n("End of document reached.\nContinue from the beginning?"),
			   i18n("Find"))
			)
			{
				findCurrent = listView->firstChild();
			}
			else
			{
				findCurrent = listView->lastChild();
				done = true;
			}
		}
	}
	else
	{
		for(int i = 0; i < listView->columns() && !found; i++)
		{
			if(findCurrent->text(i).contains(findDialog->getText(), findDialog->case_sensitive()))
			{
				listView->clearSelection();
				listView->setSelected(findCurrent, true);
				listView->ensureItemVisible(findCurrent);
				found = true;
			}
		}

		if(!findDialog->get_direction())
			findCurrent = findCurrent->itemBelow(); // forward
		else
			findCurrent = findCurrent->itemAbove(); // backward
			
	}

	if(done)
		KMessageBox::information(findDialog, i18n("Text not found."), i18n("Find"));
	else if(!found)
		search();
}

void Tron::searchDone(void)
{
	napp->exit_loop();
}

void Tron::setModified(bool b)
{
	currentPlaylistModified = b;

	QString caption;
	if(currentPlaylistURL.isEmpty())
		caption = i18n("Playlist");
	else
		caption = currentPlaylistURL.path();
	setCaption(caption, currentPlaylistModified);
}

void Tron::addItem(PlaylistItem *item)
{
	TronListViewItem *viewItem = new TronListViewItem(listView, listView->lastItem());
	viewMap.insert(viewItem, item);
	playMap.insert(item, viewItem);
	initializeItem(item);
}

void Tron::initializeItem(PlaylistItem *item)
{
	TronListViewItem *viewItem = playMap[item];
	viewItem->setText(0, item->property("title"));
	if(item->lengthString() == "00:-1")
		viewItem->setText(1, QString::null);
	else
		viewItem->setText(1, item->lengthString());
	viewItem->setText(2, item->property("artist"));
	viewItem->setText(3, item->property("album"));
	viewItem->setText(4, item->property("date"));
	viewItem->setText(5, item->property("comment"));
}

// List Actions
//
// These synchronize the GUI with the MCP, which includes calling setModified
void Tron::cleared(void)
{
	listView->clear();
	viewMap.clear();
	playMap.clear();
	currentPlaylistURL = KURL();
	setModified(true);
}

void Tron::loaded(void)
{
	fillView();
	setModified(false);
}

void Tron::saved(void)
{
	setModified(false);
}

void Tron::tronadded(PlaylistItem *item)
{
	addItem(item);
	setModified(true);
}

void Tron::removed(PlaylistItem *item)
{
	TronListViewItem *viewItem = playMap[item];
	listView->takeItem(viewItem);
	delete viewItem;

	viewMap.remove(viewItem);
	playMap.remove(item);
	setModified(true);
}

void Tron::modified(PlaylistItem *item)
{
	initializeItem(item);
	setModified(true);
}

void Tron::current(PlaylistItem *item)
{
	TronListViewItem *viewItem = playMap[item];
	listView->setSpecialItem(viewItem);
	listView->ensureItemVisible(viewItem);
}

void Tron::moved(PlaylistItem *item, PlaylistItem *itemAfter)
{
	TronListViewItem *viewItem = playMap[item];

	if(itemAfter)
	{
		TronListViewItem *viewItemAfter = playMap[itemAfter];
		listView->moveItem(viewItem, 0, viewItemAfter);
	}
	else
	{
		listView->moveItem(viewItem, 0, 0);
	}
	setModified(true);
	// I don't know why I need to do this, but the current item seems
	// to lose its specialness if it gets dragged
	listView->setSpecialItem(playMap[list->current()]);
}

// GUI Actions
//
// These do nothing but pass orders along to MCP
void Tron::file_opennew(void)
{
	currentPlaylistURL = KURL();
	list->clear();
}

void Tron::file_open(void)
{
	KURL url = KFileDialog::getOpenURL(QString::null, "*.tronplaylist\n*");
	if(!url.isEmpty())
	{
		currentPlaylistURL = url;
		list->load(currentPlaylistURL);
	}
}

void Tron::file_save(void)
{
	if(currentPlaylistURL.isEmpty())
		file_saveas();
	else
		list->save(currentPlaylistURL);
}

void Tron::file_saveas(void)
{
	KURL url = KFileDialog::getSaveURL(currentPlaylistURL.path(), "*.tronplaylist\n*");
	if(!url.isEmpty())
	{
		currentPlaylistURL = url;
		file_save();
	}
}

void Tron::edit_add(void)
{
	KURL::List list = KFileDialog::getOpenURLs(QString::null, napp->mimeTypes());
	itemDropped(list, listView->lastItem());
}

void Tron::edit_adddirectory(void)
{
	KURL url = KFileDialog::getExistingDirectory();
	if(!url.isEmpty())
		list->addDirectory(url);
}

void Tron::edit_remove(void)
{
	QList<QListViewItem> selected = listView->selectedItems();
	for (QListIterator<QListViewItem> i(selected); i.current(); ++i)
	{
		PlaylistItem *item = viewMap[ static_cast<TronListViewItem *>(*i) ];
		if(item == list->current())
			napp->player()->stop();
		list->remove(item);
	}
}

void Tron::edit_find(void)
{
	findCurrent = listView->currentItem();
	if(!findCurrent) findCurrent = listView->firstChild();

	findDialog = new KEdFind(this);
	connect(findDialog, SIGNAL(search()), this, SLOT(search()));
	connect(findDialog, SIGNAL(done()), this, SLOT(searchDone()));

	// enter loop
	findDialog->show();

	// after loop is done
	delete findDialog;
}

void Tron::options_shuffle(void)
{
	list->setShuffle(options_shuffleAction->isChecked());
}

void Tron::options_volume(void)
{
	list->setApplyVolume(options_volumeAction->isChecked());
}

void Tron::options_toolbar(void)
{
	if(options_toolbarAction->isChecked())
		toolBar("mainToolBar")->show();
	else
		toolBar("mainToolBar")->hide();
}

void Tron::options_menubar(void)
{
	if(options_menubarAction->isChecked())
		menuBar()->show();
	else
		menuBar()->hide();
}

void Tron::options_configuretoolbars(void)
{
	KEditToolbar dlg(actionCollection(), "tronui.rc");
	if(dlg.exec())
		createGUI("tronui.rc");
}

#include "tron.moc"
