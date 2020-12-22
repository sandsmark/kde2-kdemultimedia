/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#ifndef TRACK_ITEM
#define TRACK_ITEM

#include <qlistview.h>
#include <qstring.h>

#include "CharlatanItem.h"

class AlbumItem;
class Track;

class TrackItem : public CharlatanItem
{
  public:

    TrackItem(AlbumItem *, Track *);

    virtual ~TrackItem();

    void increaseScore();
    void decreaseScore();

    Track * track() { return track_; }

    QString key(int, bool) const;

    void copyTo(QListView *);

  private:

    Track * track_;
    int heightScore_;
};

#endif
