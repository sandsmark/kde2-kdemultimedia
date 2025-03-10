/*  This file is part of the KDE libraries
    Copyright (C) 2002 Simon MacMullen

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef _VIDEOCREATOR_H_
#define _VIDEOCREATOR_H_ "$Id: videocreator.h 160640 2002-06-12 22:48:23Z macmullen $"

#include <kio/thumbcreator.h>
#include <kartsserver.h>
#include <kplayobjectfactory.h>
#include <kartsdispatcher.h>

class VideoCreator : public QObject, public ThumbCreator
{
    Q_OBJECT
public:
    VideoCreator();
    virtual ~VideoCreator();
    virtual bool create(const QString &path, int width, int height, QImage &img);
    virtual Flags flags() const;

protected:
    virtual void timerEvent(QTimerEvent *);

private slots:
    void slotCompleted();

private:
    void pause (int ms);
    double imageVariance (QImage&);
    void mutePlayObject (Arts::PlayObject object);
    
    static const int maximumAttempts = 1;

    QPixmap m_sprocketSmall;
    QPixmap m_sprocketMedium;
    QPixmap m_sprocketLarge;
    KPlayObject* m_playobj;
    KArtsServer m_server;
    KArtsDispatcher m_dispatcher;
    
    bool m_completed;
};

#endif
