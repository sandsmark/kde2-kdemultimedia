/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qheader.h>
#include <klocale.h>

#include "ScoreWidget.h"
#include "CharlatanItem.h"
#include "ArtistItem.h"
#include "AlbumItem.h"
#include "TrackItem.h"
#include "Track.h"
#include "PlayLiszt.h"

ScoreWidget::ScoreWidget(QWidget * parent)
  : QListView(parent, "Charlatan ScoreWidget"),
    itemDown_(0)
{
  addColumn(i18n("Track"));
  addColumn(i18n("Score"));
  addColumn("+");
  addColumn("-");
  setRootIsDecorated(true);

  header()->resizeSection(2, 16);
  header()->resizeSection(3, 16);
  header()->setResizeEnabled(false, 2);
  header()->setResizeEnabled(false, 3);

  connect(
      PlayLiszt::instance(),  SIGNAL(trackAdded(Track *)),
      this,                   SLOT(slotTrackAdded(Track *)));

  connect(
      PlayLiszt::instance(),  SIGNAL(trackListAdded(TrackList &)),
      this,                   SLOT(slotTrackListAdded(TrackList &)));
}

ScoreWidget::~ScoreWidget()
{
}

  ArtistItem *
ScoreWidget::_artistItem(const QString & name)
{
  QListViewItemIterator it(this);

  for (; it.current(); ++it) {

    if (it.current()->depth() != 0)
      continue;

    ArtistItem * aItem = static_cast<ArtistItem *>(it.current());

    if (aItem->name() == name)
      return aItem;
  }

  ArtistItem * i = new ArtistItem(this, name);
  return i;
}

  void
ScoreWidget::slotTrackAdded(Track * t)
{
  _artistItem(t->artist())->albumItem(t->album())->trackItem(t);
}

  void
ScoreWidget::slotTrackListAdded(TrackList & l)
{
  setUpdatesEnabled(false);

  for (TrackListIterator it(l); it.current(); ++it) {
    Track * t(it.current());
    _artistItem(t->artist())->albumItem(t->album())->trackItem(t);
  }

  setUpdatesEnabled(true);

  triggerUpdate();
}

  void
ScoreWidget::contentsMousePressEvent(QMouseEvent * e)
{
  QPoint pos = contentsToViewport(e->pos());

  QListViewItem * item = itemAt(pos);

  if (0 == item)
    return QListView::contentsMousePressEvent(e);

  int section = header()->sectionAt(pos.x());

  if (section < 2)
    return QListView::contentsMousePressEvent(e);

  CharlatanItem * i = static_cast<CharlatanItem *>(item);

  i->setSunken(section);
  itemDown_ = i;

  QListView::contentsMousePressEvent(e);
}

  void
ScoreWidget::contentsMouseReleaseEvent(QMouseEvent * e)
{
  if (0 != itemDown_)
    itemDown_->setSunken(false);

  CharlatanItem * id = itemDown_;
  itemDown_ = 0;

  QPoint pos = contentsToViewport(e->pos());
  QListViewItem * item = itemAt(pos);

  if (0 == item)
    return QListView::contentsMouseReleaseEvent(e);

  CharlatanItem * i = static_cast<CharlatanItem *>(item);

  int section = header()->sectionAt(pos.x());

  if (i != id || section < 2)
    return QListView::contentsMouseReleaseEvent(e);

  switch (section) {

    case 2:
      i->increaseScore();
      break;

    case 3:
      i->decreaseScore();
      break;

    default:
      break;
  }

  QListView::contentsMouseReleaseEvent(e);
}

#include "ScoreWidget.moc"
