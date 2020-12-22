/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qpixmap.h>
#include <qlistview.h>

#include "Static.h"
#include "PlayLiszt.h"
#include "TrackItem.h"
#include "Track.h"
#include "AlbumItem.h"

TrackItem::TrackItem(
    AlbumItem * parent,
    Track * t
)
  : CharlatanItem(parent, t->name()),
    track_(t)
{
  setPixmap(0, Static::instance()->pixmap("track"));
  setText(1, QString::number(QABS(track_->score())));
}

TrackItem::~TrackItem()
{
}

  QString
TrackItem::key(int, bool) const
{
  QString s;
  s.sprintf("%016d", track_->fileScore());
  return s;
}

  void
TrackItem::increaseScore()
{
  PlayLiszt::instance()->increaseScore(track_);
  setText(1, QString::number(QABS(track_->score())));

  if (track_->score() == 0)
    setSelectable(false);
  else
    if (!isSelectable())
      setSelectable(true);
}

  void
TrackItem::decreaseScore()
{
  PlayLiszt::instance()->decreaseScore(track_);
  setText(1, QString::number(QABS(track_->score())));

  if (track_->score() == 0)
    setSelectable(false);
  else
    if (!isSelectable())
      setSelectable(true);
}
