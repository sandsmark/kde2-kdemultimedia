/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#ifndef PLAYLIST_EDIT_WIDGET_H
#define PLAYLIST_EDIT_WIDGET_H

#include <qwidget.h>

#include "Track.h"

class ScoreWidget;
class PreviewWidget;

class EditWidget : public QWidget
{
  Q_OBJECT

  public:

    EditWidget();
    virtual ~EditWidget();

    void reset();

  protected:

    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

    void closeEvent(QCloseEvent *);

  signals:

    void skipToTrack(long);
    void closed();

  private:

    ScoreWidget * scoreWidget_;
    PreviewWidget * previewWidget_;
};

#endif // PLAYLIST_EDIT_WIDGET_H
