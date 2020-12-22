/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qheader.h>

#include <klocale.h>

#include "PlayLiszt.h"
#include "PreviewWidget.h"
#include "PreviewItem.h"
#include "Track.h"

PreviewWidget * PreviewWidget::instance_ = 0L;

PreviewWidget::PreviewWidget(QWidget *parent)
  : QListView(parent, "Charlatan playlist preview widget")
{
  instance_ = this;

  addColumn(i18n("Artist"));
  addColumn(i18n("Track"));

  setColumnWidthMode(0, QListView::Manual);
  setColumnWidthMode(1, QListView::Maximum);

  setAllColumnsShowFocus(true);

  connect(
      this,                   SIGNAL(currentChanged(QListViewItem *)),
      this,                   SLOT(slotCurrentChanged(QListViewItem *)));

  connect(
      PlayLiszt::instance(),  SIGNAL(currentTrackChanged(long)),
      this,                   SLOT(slotTrackChange(long)));

  connect(
      PlayLiszt::instance(),  SIGNAL(trackAdded(Track *)),
      this,                   SLOT(slotTrackAdded(Track *)));

  connect(
      PlayLiszt::instance(),  SIGNAL(trackListAdded(TrackList &)),
      this,                   SLOT(slotTrackListAdded(TrackList &)));
}

PreviewWidget::~PreviewWidget()
{
}

  void
PreviewWidget::slotTrackAdded(Track * t)
{
  new PreviewItem(this, t);
}

  void
PreviewWidget::slotTrackListAdded(TrackList & l)
{
  setUpdatesEnabled(false);

  for (TrackListIterator it(l); it.current(); ++it)
    new PreviewItem(this, it.current());

  setUpdatesEnabled(true);

  triggerUpdate();
}

  void
PreviewWidget::reset()
{
  QListViewItemIterator it(this);
  if (0 != it.current())
    setCurrentItem(it.current());
}

  void
PreviewWidget::slotTrackChange(long t)
{
  long height(0);

  for (QListViewItemIterator it(this); it.current(); ++it) {

    if (height == t) {

      blockSignals(true);
      setCurrentItem(it.current());
      ensureItemVisible(it.current());
      blockSignals(false);

      break;
    }

    ++height;
  }
}

  void
PreviewWidget::slotCurrentChanged(QListViewItem * item)
{
  long height(0);

  for (QListViewItemIterator it(this); it.current(); ++it) {

    if (it.current() == item) {
      ensureItemVisible(item);
      emit(skipToTrack(height));
      break;
    }

    ++height;
  }
}

#include "PreviewWidget.moc"
