/**
 * Copyright (c) 2000-2001 Charles Samuels <charles@kde.org>
 *               2000-2001 Neil Stevens <neil@qualityassistant.com>
 *
 * Copyright (c) from the patches of:
 *               2001 Klas Kalass <klas.kalass@gmx.de>
 *               2001 Anno v. Heimburg <doktor.dos@gmx.de>
 **/


// Abandon All Hope, Ye Who Enter Here


#include <iostream>
#include <qmap.h>

#include <qlayout.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <qheader.h>
#include <qdragobject.h>
#include <kaction.h>
#include <kmenubar.h>
#include <kfiledialog.h>
#include <qtextstream.h>
#include <noatunapp.h>
#include <player.h>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>
#include <kurldrag.h>

#include "playlist.h"
#include "view.h"
#include "find.h"

#define SPL SplitPlaylist::SPL()

SafeListViewItem::SafeListViewItem(QListView *parent, QListViewItem *after, const KURL &text)
	: QCheckListItem(parent,0, QCheckListItem::CheckBox), PlaylistItem(text)
{
	
	static_cast<KListView*>(parent)->moveItem(this, 0, after);
	setOn(true);
	setText(0,title());
	if (!isDownloaded()) setText(1, "0%");

	SplitPlaylist *p=SPL;
	if (!(p->currentItem || p->nextItem || p->previousItem))
		p->setCurrent(this);
	if (p->currentItem==(SafeListViewItem*)itemAbove())
		p->setNext(this);
	if (p->currentItem==(SafeListViewItem*)itemBelow())
		p->setPrevious(this);
}

SafeListViewItem::~SafeListViewItem()
{
	if (SPL->nextItem==this)
		SPL->setNext(static_cast<SafeListViewItem*>(itemBelow()));
	if (SPL->currentItem==this)
		SPL->setCurrent(0), SPL->setNext(static_cast<SafeListViewItem*>(itemBelow()));
	if (SPL->previousItem==this)
		SPL->setPrevious(static_cast<SafeListViewItem*>(itemAbove()));
}

void SafeListViewItem::downloaded(int percent)
{
	setText(1, QString::number(percent)+'%');
}

void SafeListViewItem::modified()
{
	setText(0, title());
	if (isDownloaded() && length()!=-1)
		setText(1, lengthString());
	else if (isDownloaded())
		setText(1, "");
}

void SafeListViewItem::stateChange(bool s)
{
	// if you uncheck this, uncheck thet others that
	// are selected too

	QList<QListViewItem> list=SPL->view->listView()->selectedItems();

	// but not if I'm not selected
	if (list.containsRef(this))
		for (QListViewItem *i=list.first(); i != 0; i=list.next())
			static_cast<QCheckListItem*>(i)->setOn(s);
	else
		QCheckListItem::stateChange(s);
}

List::List(View *parent)
	: KListView(parent), recursiveAddAfter(0), listJob(0)
{
	addColumn(i18n("File"));
	addColumn(i18n("Time"));
	setAcceptDrops(true);
	setSorting(-1);
	setDropVisualizer(true);
	setDragEnabled(true);
	setItemsMovable(true);
	setSelectionMode(QListView::Extended);
	connect(this, SIGNAL(dropped(QDropEvent*, QListViewItem*)), SLOT(dropEvent(QDropEvent*, QListViewItem*)));
	connect(this, SIGNAL(moved(QList<QListViewItem>&,QList<QListViewItem>&,QList<QListViewItem>&)), SLOT(move(QList<QListViewItem>&,QList<QListViewItem>&,QList<QListViewItem>&)));
	connect(this, SIGNAL(aboutToMove()), parent, SLOT(setNoSorting()));
}

List::~List()
{
}

void List::move(QList<QListViewItem>& item, QList<QListViewItem>&, QList<QListViewItem>&)
{
	bool bidimerge=static_cast<bool>(item.containsRef(static_cast<SafeListViewItem*>(SPL->currentItem)));
	bool current=SPL->currentItem;
	if (current)
	{
		if (bidimerge || item.containsRef(static_cast<SafeListViewItem*>(SPL->previousItem))
		              || (current && item.containsRef(static_cast<SafeListViewItem*>(SPL->currentItem)->itemAbove())))
			SPL->setPrevious(static_cast<SafeListViewItem*>(static_cast<SafeListViewItem*>(SPL->currentItem)->itemAbove()));
	
		if (bidimerge || item.containsRef(static_cast<SafeListViewItem*>(SPL->nextItem))
		              || item.containsRef(static_cast<SafeListViewItem*>(static_cast<SafeListViewItem*>(SPL->currentItem)->itemBelow())))
			SPL->setNext(static_cast<SafeListViewItem*>(static_cast<SafeListViewItem*>(SPL->currentItem)->itemBelow()));
	}

	
	emit modified();
}

bool List::acceptDrag(QDropEvent *event) const
{
	return QUriDrag::canDecode(event) || KListView::acceptDrag(event);
}

void List::dropEvent(QDropEvent *event, QListViewItem *after)
{
	static_cast<View*>(parent())->setNoSorting();
	KURL::List textlist;
	if (!KURLDrag::decode(event, textlist)) return;
	event->acceptAction();
	
	for (KURL::List::Iterator i=textlist.begin(); i != textlist.end(); ++i)
	{
		after= addFile(*i, false, after);
	}

	emit modified();
}

void List::viewportPaintEvent(QPaintEvent *e)
{
	KListView::viewportPaintEvent(e);
	
	static bool recurse=false;
	if (!recurse)
	{
		QListViewItem *current=static_cast<SafeListViewItem*>(SPL->current());
		QRect item(itemRect(current).intersect(e->rect()));
		if (item.isEmpty()) return;

		QPixmap pix(e->rect().width(), e->rect().height());
		pix.fill(QColor(255,255,255));
		
		recurse=true;
		viewport()->repaint(item,true);
		recurse=false;
	
		bitBlt(viewport(), item.topLeft(), &pix, QRect(0,0, item.width(), item.height()), XorROP,false);
	}
}



// for m3u files
QListViewItem *List::openGlobal(const KURL &u, QListViewItem *after)
{
	QString local;
	if(KIO::NetAccess::download(u, local))
	{
		QFile saver(local);
		saver.open(IO_ReadOnly);
		QTextStream t(&saver);
		QString file;
		while (!t.eof())
		{
			file=t.readLine();
			if (!file.isNull())
			{
				KURL u1;
				if (file.find('/')) // we have to deal with a relative path
				{
					u1.setPath(u.path(0));
					u1.setFileName(file);
				}
				else
					u1.setPath(file);
				after=addFile(u1, false, after);
			}
		}
		
		KIO::NetAccess::removeTempFile(local);
	}
	return after;
}

QListViewItem *List::addFile(const KURL& url, bool play, QListViewItem *after)
{
	// when a new item is added, we don't want to sort anymore
	SPL->view->setNoSorting();

	// TODO: there must be a better way.. this isn't windows
	if (url.path().right(4).lower()==".m3u")
	{
		// a playlist is requested
		return openGlobal(url, after);
	}
	else
	{
		if (!after) after=lastItem();
		KFileItem fileItem(-1,-1,url);
		if (fileItem.isDir())
		{
			addDirectoryRecursive(url, after);
			return after; // don't (and can't) know better!?
		}
		else
		{
			QListViewItem *i=new SafeListViewItem(this, after, url);
			if (play)
				SPL->listItemSelected(i);
			emit modified();
			return i;
		}
	}
}

// starts a new listJob if there is no active but work to do
void List::addNextPendingDirectory()
{
	KURL::List::Iterator pendingIt= pendingAddDirectories.begin();
	if (!listJob &&	(pendingIt!= pendingAddDirectories.end()))
	{
		currentJobURL= *pendingIt;
		listJob= KIO::listRecursive(currentJobURL, false);
		connect(listJob, SIGNAL(entries(KIO::Job*, const KIO::UDSEntryList&)),
				SLOT(slotEntries(KIO::Job*, const KIO::UDSEntryList&)));
		connect(listJob, SIGNAL(result(KIO::Job *)),
				SLOT(slotResult(KIO::Job *)));
		connect(listJob, SIGNAL(redirection(KIO::Job *, const KURL &)),
				SLOT(slotRedirection(KIO::Job *, const KURL &)));
		pendingAddDirectories.remove(pendingIt);
	}
}

void List::addDirectoryRecursive(const KURL &dir, QListViewItem *after)
{
	if (!after) after=lastItem();
	recursiveAddAfter= after;
	pendingAddDirectories.append(dir);
	addNextPendingDirectory();
}

void List::slotResult(KIO::Job *job)
{
	listJob= 0;
	if (job && job->error())
		job->showErrorDialog();
	addNextPendingDirectory();
}

void List::slotEntries(KIO::Job *job, const KIO::UDSEntryList &entries)
{
	QMap<QString,KURL> __list; // temp list to sort entries
	
	KIO::UDSEntryListConstIterator it = entries.begin();
	KIO::UDSEntryListConstIterator end = entries.end();

	for (; it != end; ++it)
	{
		KFileItem file(*it, currentJobURL, false /* no mimetype detection */, true);
		// "prudhomm:
		// insert the path + url in the map to sort automatically by path
		// note also that you use audiocd to rip your CDs then it will be sorted the right way
		// now it is an easy fix to have a nice sort BUT it is not the best
		// we should sort based on the tracknumber"
		// - copied over from old kdirlister hack <hans_meine@gmx.de>
		if (!file.isDir())
			__list.insert(file.url().path(), file.url());
	}
	QMap<QString,KURL>::Iterator __it;
	for( __it = __list.begin(); __it != __list.end(); ++__it )
	{
		recursiveAddAfter= addFile(__it.data(), false, recursiveAddAfter);
	}
}

void List::slotRedirection(KIO::Job *, const KURL & url)
{
	currentJobURL= url;
}

View::View(SplitPlaylist *)
	: KMainWindow(0,0)
{
	list=new List(this);
	list->show();
	setCentralWidget(list);
	connect(list, SIGNAL(modified(void)), this, SLOT(setModified(void)) );
	// connect the click on the header with sorting
	connect(list->header(),SIGNAL(clicked(int)),this,SLOT(headerClicked(int)) );

	tb = new KToolBar( this );
	addToolBar(tb);

	(mOpen=new KAction(i18n("Add &Files"), "queue", 0, this, SLOT(addFiles()),this, "open"))->plug(tb);
	(mDelete=new KAction(i18n("Delete"), "editdelete", Qt::Key_Delete, this, SLOT(deleteSelected()), this, "delete"))->plug(tb);
	(new KAction(i18n("Add &Directories"), "folder", 0, this, SLOT(addDirectory()), this, "add_dir"))->plug(tb);

	(new KActionSeparator(this))->plug(tb);

	(mFind=KStdAction::find(this, SLOT(find()), this, "find"))->plug(tb);

	(new KActionSeparator(this))->plug(tb);

	(mSave=KStdAction::save(this, SLOT(save()), this, "save"))->plug(tb);
	(mSaveAs=KStdAction::saveAs(this, SLOT(saveAs()), this, "saveas"))->plug(tb);
	(mOpenpl=KStdAction::open(this, SLOT(open()), this, "open"))->plug(tb);
	(mOpenNew=KStdAction::openNew(this, SLOT(openNew()), this, "openNew"))->plug(tb);
	(new KAction(i18n("Shuffle"), "misc", 0, SPL, SLOT( randomize() ), this, "clear"))->plug(tb);
//	(new KAction(i18n("Sort"),"queue",0,SPL,SLOT(sort()),this))->plug(tb);
	(new KAction(i18n("Clear"), "fileclose", 0, this, SLOT( clearView() ), this, "clear"))->plug(tb);
	list->setFocus();
}

void View::find()
{
	Finder *f=new Finder(this);
	f->show();
	connect(f, SIGNAL(search(Finder*)), SLOT(findIt(Finder*)));
}

static bool testWord(QListViewItem *i, const QString &finder)
{
	PlaylistItem *item=static_cast<SafeListViewItem*>(i);
	if (item->title().find(finder, 0, false) >=0)
		return true;
	if (item->file().find(finder, 0, false) >=0)
		return true;
	if (item->url().path().find(finder.local8Bit(), 0, false) >=0)
		return true;
	if (item->lengthString().find(finder, 0, false) >=0)
		return true;
	if (item->mimetype().find(finder.local8Bit(), 0, false) >=0)
		return true;
	return false;
}

static bool testWord(QListViewItem *i, const QRegExp &finder)
{
	PlaylistItem *item=static_cast<SafeListViewItem*>(i);
	if (item->title().find(finder) >=0)
		return true;
	if (item->file().find(finder) >=0)
		return true;
	if (item->url().path().find(finder) >=0)
		return true;
	if (item->lengthString().find(finder) >=0)
		return true;
	if (item->mimetype().find(finder) >=0)
		return true;
	return false;
}

void View::findIt(Finder *f)
{
	QListViewItem *search=list->currentItem();
		
	if (!search)
		if (f->isForward())
			search=list->firstChild();
		else
			search=list->lastChild();

	while (search)
	{
		if (f->isForward())
			search=search->itemBelow();
		else
			search=search->itemAbove();

		if (!search) return;
		if (f->regexp())
		{
			if (testWord(search, QRegExp(f->string(), false)))
				break;
		}
		else
		{	
			if (testWord(search, f->string()))
				break;
		}

	}

	if (search)
	{
		{ // select none
			QList<QListViewItem> sel=list->selectedItems();
			for (QListViewItem *i=sel.first(); i!=0; i=sel.next())
				list->setSelected(i, false);
		}
		list->setSelected(search, true);
		list->setCurrentItem(search);
		list->ensureItemVisible(search);
	}
}

View::~View()
{
	hide();
	saveState();
	delete list;
	delete tb;
	mOpen->unplug(tb);
	mDelete->unplug(tb);
}

void View::init()
{
	KURL internalURL;
	internalURL.setPath(napp->dirs()->saveLocation("data", "noatun/") + "splitplaylistdata");
	list->openGlobal(internalURL);

	KConfig &config = *KGlobal::config();
	config.setGroup("splitplaylist");

	// this has to come after openGlobal, since openGlobal emits modified()
	setModified(config.readBoolEntry("modified", false));
	mPlaylistFile.setPath(config.readEntry("file", QString::null));

	SPL->reset();
	int saved = config.readNumEntry("current", 0);

	PlaylistItem *item=SPL->getFirst();
	for(int i = 0 ; i < saved ; i++)
	{
		item=SPL->getAfter(item);
	}
	if (item)
		SPL->setCurrent(item);
}

void View::save()
{
	if (mPlaylistFile.isMalformed()) { saveAs(); return; }
	saveToURL(mPlaylistFile);
	setModified(false);
}

void View::saveAs()
{
	KURL u=KFileDialog::getSaveURL(0, "*.m3u\n*", this, i18n("Save Playlist"));
	if (u.isMalformed()) return;
	mPlaylistFile = u;
	save();
}

void View::open()
{
	KURL u=KFileDialog::getOpenURL(0, "*.m3u\n*", this, i18n("Insert Playlist"));
	if (u.isMalformed()) return;
	list->openGlobal(u);
	setModified(false);
}

void View::openNew()
{
	KURL u=KFileDialog::getOpenURL(0, "*.m3u\n*", this, i18n("Open New Playlist"));
	if (u.isMalformed()) return;
	mPlaylistFile = u;

	listView()->clear();
	list->openGlobal(mPlaylistFile);

	setModified(false);
}

void View::deleteSelected()
{
	QList<QListViewItem> items(list->selectedItems());
	for (QListIterator<QListViewItem> it(items); it.current(); ++it)
		delete *it;

	setModified(true);
}

void View::addFiles()
{
	KURL::List files=KFileDialog::getOpenURLs(0, napp->mimeTypes(), this, i18n("Select a File to Play"));

	for(KURL::List::Iterator it=files.begin(); it!=files.end(); ++it)
		addFile(KURL(*it));
	
	setModified(true);
}

void View::addDirectory()
{
	QString file=KFileDialog::getExistingDirectory(0, this, i18n("Select a Directory"));

	if (!file) return;
	KURL url;
	url.setPath(file);
	list->addDirectoryRecursive(url);
	
	setModified(true);
}

void View::closeEvent(QCloseEvent*)
{
	hide();
}

void View::showEvent(QShowEvent *)
{
	emit shown();
}

void View::hideEvent(QHideEvent *)
{
	emit hidden();
}

void View::setModified(bool b)
{
	modified = b;
	setCaption(i18n("Playlist"), modified);
}

void View::setModified(void)
{
	setModified(true);
}

void View::saveToURL(const KURL &url)
{
	QString local(napp->tempSaveName(url.path()));
	QFile saver(local);
	saver.open(IO_ReadWrite | IO_Truncate);
	QTextStream t(&saver);
	// navigate the list
	for (SafeListViewItem *i=static_cast<SafeListViewItem*>(listView()->firstChild());
	     i != 0; i=static_cast<SafeListViewItem*>(i->itemBelow()))
		t<< *i << '\n';
	saver.close();

	KIO::NetAccess::upload(local, url);

	saver.remove();
}

void View::saveState(void)
{
	KConfig &config = *KGlobal::config();
	config.setGroup("splitplaylist");

	config.writeEntry("modified", modified);
	config.writeEntry("file", mPlaylistFile.path());
	saveToURL(napp->dirs()->saveLocation("data", "noatun/") + "splitplaylistdata");

	int i;
	PlaylistItem *item=SPL->getFirst();
	for(i = 0; item != SPL->current(); )
		item=SPL->getAfter(item), i++;

	config.writeEntry("current", i);

	config.sync();
}

// turns the sorting on or off
void View::setSorting(bool on, int column)
{
	if (on)
	{
		list->setSorting(column,true);
		list->setShowSortIndicator(true);
	}
	else
	{
		list->setShowSortIndicator(false);
		list->setSorting(-1);
	}
}

void View::headerClicked(int column)
{
	/* this is to avoid that if we already have it sorted, we sort it again ascendingly
	   this way, clicking on the header a second time will correctly toggle ascending/descending sort */
	if (list-> showSortIndicator())
	{
		return;
	}
	else
	{
		setSorting(true,column);
	}
}

#include "view.moc"

