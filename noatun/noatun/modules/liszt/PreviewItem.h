/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#ifndef PREVIEW_ITEM
#define PREVIEW_ITEM

#include <qlistview.h>
#include <qstring.h>

class Track;

class PreviewItem : public QObject, public QListViewItem
{
  Q_OBJECT

  public:

    PreviewItem(QListView *, Track *);

    QString key(int, bool) const;

  protected slots:

    void slotScoreChanged(int);

  private:

    bool visible_;
    QListView * listView_;

    Track * track_;
};

#endif
