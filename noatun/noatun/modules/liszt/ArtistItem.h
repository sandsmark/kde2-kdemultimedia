/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#ifndef ARTIST_ITEM
#define ARTIST_ITEM

#include <qlistview.h>
#include <qstring.h>

#include "CharlatanItem.h"

class AlbumItem;

class ArtistItem : public CharlatanItem
{
  public:

    ArtistItem(QListView *, QString artist);
    virtual ~ArtistItem();

    AlbumItem * albumItem(QString);

    void setOpen(bool);

    void increaseScore();
    void decreaseScore();

    QString name() const { return artist_; }

  private:

    QString artist_;
};

#endif
