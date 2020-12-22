#include "playlist.h"
#include "view.h"
#include <player.h>

#include <krandomsequence.h>

SplitPlaylist *SplitPlaylist::Self=0;

SplitPlaylist::SplitPlaylist()
	: Playlist(0, "SplitPlaylist"), Plugin(), nextItem(0), currentItem(0), previousItem(0)
{
	Self=this;
}

void SplitPlaylist::init()
{
	view=new View(this); // 195
	connect(view->listView(), SIGNAL(doubleClicked(QListViewItem*)), SLOT(listItemSelected(QListViewItem*)));
	connect(view, SIGNAL(shown()), SIGNAL(listShown()));
	connect(view, SIGNAL(hidden()), SIGNAL(listHidden()));
	
	view->init(); // 1000
}

SplitPlaylist::~SplitPlaylist()
{
	delete view;
}

void SplitPlaylist::reset()
{
	previousItem=0;
	SafeListViewItem *i;
	setCurrent(i=static_cast<SafeListViewItem*>(view->listView()->firstChild()));
	if (i && !i->isOn())
		next();
}
	
PlaylistItem *SplitPlaylist::next()
{
	if (!nextItem) return 0;
	setCurrent(nextItem);
	if (currentItem)
		if (!static_cast<SafeListViewItem*>(currentItem)->isOn())
			return next();
	
	return currentItem;
}

PlaylistItem *SplitPlaylist::current()
{
	return currentItem;
}

PlaylistItem *SplitPlaylist::previous()
{
	if (!previousItem) return 0;
	setCurrent(previousItem);
	
	if (currentItem)
		if (!static_cast<SafeListViewItem*>(currentItem)->isOn())
			return previous();

	return currentItem;
}

PlaylistItem *SplitPlaylist::getFirst() const
{
	return static_cast<SafeListViewItem*>(view->listView()->firstChild());
}

PlaylistItem *SplitPlaylist::getAfter(const PlaylistItem *item) const
{
	if (item)
		return static_cast<SafeListViewItem*>(static_cast<const SafeListViewItem*>(item)->nextSibling());
	return 0;
}
	
bool SplitPlaylist::listVisible() const
{
	return view->isVisible();
}

void SplitPlaylist::showList()
{
	view->show();
}

void SplitPlaylist::hideList()
{
	view->hide();
}

void SplitPlaylist::clear()
{
	view->listView()->clear();
}

void SplitPlaylist::addFile(const KURL &file, bool play)
{
	view->addFile(file, play);
}

void SplitPlaylist::setNext(PlaylistItem *i)
{
	nextItem=i;
}

void SplitPlaylist::setCurrent(PlaylistItem *i)
{
	bool emitC=currentItem==0;
	if (!i)
	{
		currentItem=0;
		return;
	}

	QRect rect(view->listView()->itemRect(static_cast<SafeListViewItem*>(current())));
	rect.setWidth(view->listView()->viewport()->width());
	currentItem=i;	
	view->listView()->viewport()->repaint(rect,true);

	view->listView()->ensureItemVisible(static_cast<SafeListViewItem*>(current()));
	QRect currentRect= view->listView()->itemRect(static_cast<SafeListViewItem*>(current()));
	view->listView()->viewport()->repaint(currentRect);
	
	setNext(static_cast<PlaylistItem*>(static_cast<SafeListViewItem*>(static_cast<SafeListViewItem*>(current())->itemBelow())));
	setPrevious(static_cast<PlaylistItem*>(static_cast<SafeListViewItem*>(static_cast<SafeListViewItem*>(current())->itemAbove())));
	
	if (emitC && i->playable())
		emit newCurrent();
}

void SplitPlaylist::setPrevious(PlaylistItem *i)
{
	previousItem=i;
}

void SplitPlaylist::remove(PlaylistItem *i)
{
	delete i;
}

void SplitPlaylist::listItemSelected(QListViewItem *i)
{
	setCurrent(static_cast<PlaylistItem*>(static_cast<SafeListViewItem*>(i)));
	emit playCurrent();
}

void SplitPlaylist::randomize()
{
	// turning off sorting is necessary
	// otherwise, the list will get randomized and promptly sorted again
	view->setSorting(false);
	List *lview = view->listView();
	// eeeeevil :)
	QList<void> list;
	QList<QListViewItem> items;
	for ( int i = 0; i < lview->childCount(); i++ )
	{
		list.append( (void*) i );
		items.append( lview->itemAtIndex( i ) );
	}

	KRandomSequence seq;
	seq.randomize( &list );

	for ( int i = 0; i < lview->childCount(); i++ )
	{
		lview->moveItem( items.take(), 0L,
				 lview->itemAtIndex( (int) list.take() ) );
	}
	
	setCurrent( currentItem );
}

void SplitPlaylist::sort()
{
	view->setSorting(true);
	setCurrent(currentItem);
}

#include "playlist.moc"
