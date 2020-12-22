/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qstring.h>
#include <qpixmap.h>
#include <qmap.h>

#ifndef STATIC_H
#define STATIC_H

class Static
{
  public:

    static Static * instance()
    {
      if (0 == instance_)
        instance_ = new Static;

      return instance_;
    }

    const QPixmap & pixmap(const QString & s) const
    {
      return pix_[s];
    }

  private:

    Static();

    static Static * instance_;

    QMap<QString, QPixmap> pix_;
};

#endif
