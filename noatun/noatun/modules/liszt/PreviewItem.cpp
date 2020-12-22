/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qpixmap.h>

#include "Static.h"
#include "Track.h"
#include "PreviewItem.h"

PreviewItem::PreviewItem(QListView * lv, Track * t)
  : QObject       (),
    QListViewItem (lv),
    visible_      (true),
    listView_     (lv),
    track_        (t)
{
  setPixmap(0, Static::instance()->pixmap("track"));
  setText(0, track_->artist());
  setText(1, track_->name());

  connect(t, SIGNAL(scoreChanged(int)), this, SLOT(slotScoreChanged(int)));

  if (t->score() <= 0) {
    listView_->takeItem(this);
    visible_ = false;
  } else {
    listView_->sort();
  }
}

  void
PreviewItem::slotScoreChanged(int i)
{
  if ((i <= 0) && visible_) {

    listView_->takeItem(this);
    visible_ = false;
    return;

  } else {

    if (!visible_) {
      listView_->insertItem(this);
      visible_ = true;
    }
  }

  listView_->sort();
}

  QString
PreviewItem::key(int, bool) const
{
  return track_->key();
}
#include "PreviewItem.moc"
