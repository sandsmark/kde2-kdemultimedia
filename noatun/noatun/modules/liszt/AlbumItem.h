/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#ifndef ALBUM_ITEM
#define ALBUM_ITEM

#include <qlistview.h>
#include <qstring.h>

#include "CharlatanItem.h"

class Track;
class ArtistItem;
class TrackItem;
class PlaylistEditWidget;
class QPainter;
class QColorGroup;

class AlbumItem : public CharlatanItem
{
  public:

    AlbumItem(ArtistItem *, QString artist, QString album);
    virtual ~AlbumItem();

    void setOpen(bool);

    void increaseScore();
    void decreaseScore();

    TrackItem * trackItem(QString);
    TrackItem * trackItem(Track *);

    QString name() const { return album_; }

  private:

    PlaylistEditWidget * lv_;
    QString artist_;
    QString album_;
};

#endif
