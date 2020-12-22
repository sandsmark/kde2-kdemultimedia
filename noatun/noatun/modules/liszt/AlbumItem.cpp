/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qpixmap.h>
#include <qpainter.h>

#include "Static.h"
#include "AlbumItem.h"
#include "ArtistItem.h"
#include "TrackItem.h"
#include "Track.h"

AlbumItem::AlbumItem(
    ArtistItem * parent,
    QString artist,
    QString album
                                      )
  : CharlatanItem(parent, album),
    artist_(artist),
    album_(album)
{
  setExpandable(true);
  setPixmap(0, Static::instance()->pixmap("album"));

#if 0
  KConfig * c(KGlobal::config());

  c->setGroup("General");

  QStringList l(c->readListEntry("OpenAlbums - " + artist_));

  if (0 != l.contains(album_))
    setOpen(true);
#endif
}

AlbumItem::~AlbumItem()
{
}

  void
AlbumItem::setOpen(bool b)
{
  CharlatanItem::setOpen(b);
#if 0

  KConfig * c(KGlobal::config());

  c->setGroup("General");

  QString key("OpenAlbums - " + artist_);

  QStringList l(c->readListEntry(key));

  if (b) {
    if (0 == l.contains(album_)) {
      l.append(album_);
      c->writeEntry(key, l);
    }
  } else {
    QStringList::Iterator it = l.find(album_);
    if (it != l.end()) {
      l.remove(it);
      c->writeEntry(key, l);
    }
  }
#endif
}

  void
AlbumItem::increaseScore()
{
  TrackItem * i = static_cast<TrackItem *>(firstChild());

  while (0 != i) {
    i->increaseScore();
    i = static_cast<TrackItem *>(i->nextSibling());
  }
}

  void
AlbumItem::decreaseScore()
{
  TrackItem * i = static_cast<TrackItem *>(firstChild());

  while (0 != i) {
    i->decreaseScore();
    i = static_cast<TrackItem *>(i->nextSibling());
  }
}

  TrackItem *
AlbumItem::trackItem(QString name)
{
  TrackItem * i = static_cast<TrackItem *>(firstChild());

  while (i) {

    if (i->track()->name() == name)
      return i;

    i = static_cast<TrackItem *>(i->nextSibling());
  }

  return 0;
}

  TrackItem *
AlbumItem::trackItem(Track * t)
{
  TrackItem * i = static_cast<TrackItem *>(firstChild());

  while (i) {

    if (i->track()->name() == t->name())
      return i;

    i = static_cast<TrackItem *>(i->nextSibling());
  }

  return (new TrackItem(this, t));
}

