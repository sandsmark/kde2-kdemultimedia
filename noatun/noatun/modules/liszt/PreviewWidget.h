/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#ifndef PREVIEW_WIDGET_H
#define PREVIEW_WIDGET_H

#include <qlistview.h>
#include <qstringlist.h>

#include "Track.h"

class PreviewWidget : public QListView
{
  Q_OBJECT

  public:

    static PreviewWidget * instance()
    {
      return instance_;
    }

    PreviewWidget(QWidget *parent);
    virtual ~PreviewWidget();

    void reset();

  protected slots:

    void slotTrackAdded(Track *);
    void slotTrackListAdded(TrackList &);
    void slotTrackChange(long);
    void slotCurrentChanged(QListViewItem *);

  signals:

    void skipToTrack(long);
    void closed();

  private:

    static PreviewWidget * instance_;
};

#endif
