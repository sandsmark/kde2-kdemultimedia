/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qbrush.h>
#include "CharlatanItem.h"
#include "Static.h"

  void
CharlatanItem::paintCell(
    QPainter * p,
    const QColorGroup & g,
    int c,
    int w,
    int a
)
{
  if (c < 2)
    return QListViewItem::paintCell(p, g, c, w, a);

  p->fillRect(0, 0, 16, height(), Qt::white);
  QBrush b(g.button());

  int top = height() / 2 - 8;
  qDrawShadeRect(p, 0, top, 16, 16, g, sunken_ == c, 1, 1, &b);

  int ofs = sunken_ == c ? 3 : 2;

  switch (c) {

    case 2:
      p->drawPixmap(ofs, top + ofs, Static::instance()->pixmap("plus"));
      break;

    case 3:
      p->drawPixmap(ofs, top + ofs, Static::instance()->pixmap("minus"));
      break;

    default:
      break;
  }
}

