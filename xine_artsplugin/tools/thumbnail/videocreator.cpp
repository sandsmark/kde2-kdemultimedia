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

// $Id: videocreator.cpp 185171 2002-10-22 18:13:30Z macmullen $

#include <time.h>

#include <qpixmap.h>
#include <qdialog.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpaintdevice.h>

#include <iostream.h>

#include <kstandarddirs.h>
#include <kapplication.h>
#include <kvideowidget.h>

#include "videocreator.h"

extern "C"
{
    ThumbCreator *new_creator()
    {
        return new VideoCreator;
    }
};

VideoCreator::VideoCreator()
    : m_playobj(0), m_server(), m_dispatcher()
{
}

VideoCreator::~VideoCreator()
{
    delete m_playobj;
}

bool VideoCreator::create(const QString &path, int width, int height, QImage &img)
{
    if (m_sprocketSmall.isNull())
    {
        QString pixmap = locate( "data", "videothumbnail/sprocket-small.png" );
        m_sprocketSmall = QPixmap(pixmap);
        pixmap = locate( "data", "videothumbnail/sprocket-medium.png" );
        m_sprocketMedium = QPixmap(pixmap);
        pixmap = locate( "data", "videothumbnail/sprocket-large.png" );
        m_sprocketLarge = QPixmap(pixmap);
    }

    KPlayObjectFactory factory(m_server.server());
    m_playobj = factory.createPlayObject(path, false);
    m_playobj->play();
    
    // The long term plan is to seek to frame 1, create thumbnail, see if is is
    // interesting enough, if not seek to frame 2, then 4, then 8, etc.
    // "Interesting enough" means the variance of the pixel brightness is high. This
    // is because many videos fade up from black and a black rectangle is boring.
    //
    // But for the time being we can't seek so we just let it play for one second 
    // then take whatever we find.
    
    pause(1000);    

    QPixmap pix;
    QSize size; 
    
    Arts::poTime t;

    t.ms = 100;
    t.seconds = 0;    
 
    int i = 0;
//  for (int i = 0; i < maximumAttempts; i++) {
        
        // Waaaaa!
        img = KVideoWidget::snapshot( Arts::DynamicCast( m_playobj->object() ) );    
    
        if (img.isNull()) {
            delete m_playobj;
            return false;
        }
        
        double ratio = (double) img.width() / (double) img.height();

        if (ratio > 1.0)
            size = QSize(width, (int)(width / ratio));
        else
            size = QSize((int)(height * ratio), height);
   
        img = img.smoothScale(size);
        
//      if (imageVariance(img) > 500) break;
//  }

    delete m_playobj;
    
    pix.convertFromImage(img);
    QPainter painter(&pix);
    
    QPixmap sprocket;
    if (pix.height() < 60)
        sprocket = m_sprocketSmall;
    else if (pix.height() < 90)
        sprocket = m_sprocketMedium;
    else
        sprocket = m_sprocketLarge;

    for (int y = 0; y < pix.height() + sprocket.height(); y += sprocket.height()) {
        painter.drawPixmap(0, y, sprocket);
    }

    img = pix.convertToImage();

    return true;
}

// Return the variance of the brightness of the pixels
double VideoCreator::imageVariance(QImage& image)
{
    if (image.depth()<32) image = image.convertDepth(32);

    unsigned char* pixel;
    unsigned int height = image.height();
    unsigned int width = image.width();
    unsigned long sigmaX = 0;
    unsigned long sigmaXSquared = 0;

    register unsigned int y, x, gray;

    for (y = 0; y < height ; y++) {
        pixel = (unsigned char *) image.scanLine(y);
        for (x = 0; x < width ; x++) {
            gray = qGray(*(QRgb*) (pixel + sizeof(QRgb) * x));

            sigmaX += gray;
            sigmaXSquared += gray * gray;
        }
    }

    unsigned int total = height * width;

    return sigmaXSquared / total - (sigmaX / total) * (sigmaX / total);
}

void VideoCreator::pause(int ms)
{
    m_completed = false;
    startTimer(ms);
    while (!m_completed) {
        kapp->processOneEvent();
    }
    killTimers();
}

void VideoCreator::timerEvent(QTimerEvent *)
{
    m_completed = true;
}

void VideoCreator::slotCompleted()
{
    m_completed = true;
}

ThumbCreator::Flags VideoCreator::flags() const
{
    return (ThumbCreator::Flags) (DrawFrame);
}

#include "videocreator.moc"

