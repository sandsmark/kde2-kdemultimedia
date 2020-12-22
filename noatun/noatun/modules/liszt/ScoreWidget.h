/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#ifndef SCORE_WIDGET_H
#define SCORE_WIDGET_H

#include <qlistview.h>

#include "Track.h"

class ArtistItem;
class CharlatanItem;

class ScoreWidget : public QListView
{
  Q_OBJECT

  public:

    ScoreWidget(QWidget * parent);
    virtual ~ScoreWidget();

  protected slots:

    void slotTrackAdded(Track *);
    void slotTrackListAdded(TrackList &);

  protected:

    void contentsMousePressEvent(QMouseEvent *);
    void contentsMouseReleaseEvent(QMouseEvent *);

  private:

    ArtistItem * _artistItem(const QString & name);

    CharlatanItem * itemDown_;
};

#endif // SCORE_WIDGET_H
