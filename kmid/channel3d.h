/**************************************************************************

    channel3d.h  - KMidChannel3D widget, a Channel widget with 3D look
    Copyright (C) 1998  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to larrosa@kde.org
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/

#ifndef KMIDCHANNEL3D_H
#define KMIDCHANNEL3D_H

#include "channel.h"

class KMidChannel3D : public KMidChannel
{
public:
    KMidChannel3D(int chn,QWidget *parent);


    virtual void drawDo   (QPainter *qpaint,int x,int p);
    virtual void drawDo__ (QPainter *qpaint,int x,int p);
    virtual void drawRe   (QPainter *qpaint,int x,int p);
    virtual void drawRe__ (QPainter *qpaint,int x,int p);
    virtual void drawMi   (QPainter *qpaint,int x,int p);
    virtual void drawFa   (QPainter *qpaint,int x,int p);
    virtual void drawFa__ (QPainter *qpaint,int x,int p);
    virtual void drawSol  (QPainter *qpaint,int x,int p);
    virtual void drawSol__(QPainter *qpaint,int x,int p);
    virtual void drawLa   (QPainter *qpaint,int x,int p);
    virtual void drawLa__ (QPainter *qpaint,int x,int p);
    virtual void drawSi   (QPainter *qpaint,int x,int p);
    
    
};

#endif
