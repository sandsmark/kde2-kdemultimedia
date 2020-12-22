/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qpixmap.h>
#include <qpainter.h>

#include "Static.h"
#include "CharlatanItem.h"
#include "ArtistItem.h"
#include "AlbumItem.h"

ArtistItem::ArtistItem(
    QListView * parent,
    QString artist
                      )
  : CharlatanItem(parent, artist),
    artist_(artist)
{
  setExpandable(true);
  setPixmap(0, Static::instance()->pixmap("artist"));

#if 0
  KConfig * c(KGlobal::config());

  c->setGroup("General");
  QStringList l(c->readListEntry("OpenArtists"));

  if (0 != l.contains(artist_))
    setOpen(true);
#endif
}

ArtistItem::~ArtistItem()
{
}

  void
ArtistItem::setOpen(bool b)
{
  CharlatanItem::setOpen(b);
#if 0

  KConfig * c(KGlobal::config());

  c->setGroup("General");

  QStringList l(c->readListEntry("OpenArtists"));

  if (b) {
    if (0 == l.contains(artist_)) {
      l.append(artist_);
      c->writeEntry("OpenArtists", l);
    }
  } else {
    QStringList::Iterator it = l.find(artist_);
    if (it != l.end()) {
      l.remove(it);
      c->writeEntry("OpenArtists", l);
    }
  }
#endif
}

  AlbumItem *
ArtistItem::albumItem(QString name)
{
  AlbumItem * i = static_cast<AlbumItem *>(firstChild());

  while (i) {

    if (i->name() == name)
      return i;

    i = static_cast<AlbumItem *>(i->nextSibling());
  }

  i = new AlbumItem(this, artist_, name);
  return i;
}

  void
ArtistItem::increaseScore()
{
  AlbumItem * i = static_cast<AlbumItem *>(firstChild());

  while (0 != i) {
    i->increaseScore();
    i = static_cast<AlbumItem *>(i->nextSibling());
  }
}

  void
ArtistItem::decreaseScore()
{
  AlbumItem * i = static_cast<AlbumItem *>(firstChild());

  while (0 != i) {
    i->decreaseScore();
    i = static_cast<AlbumItem *>(i->nextSibling());
  }
}


