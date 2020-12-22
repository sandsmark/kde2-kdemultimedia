/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#ifndef ITEM_H
#define ITEM_H

#include <qlistview.h>
#include <qstring.h>

class CharlatanItem : public QListViewItem
{
  public:

    CharlatanItem(QListView * parent, QString name)
      : QListViewItem(parent, name),
        sunken_(-1)
    {
    }

    CharlatanItem(QListViewItem * parent, QString name)
      : QListViewItem(parent, name),
        sunken_(-1)
    {
    }

    virtual ~CharlatanItem()
    {
    }

    void setSunken(int i) { sunken_ = i; repaint(); }

    virtual void increaseScore() = 0;
    virtual void decreaseScore() = 0;

    virtual void paintCell(QPainter *, const QColorGroup &, int, int, int a);

  private:

    int sunken_;
};

#endif
