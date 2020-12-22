/***************************************************************************
                          rgb2yuvdefs.h  -  description
                             -------------------
    begin                : Tue Nov 2 2000
    copyright            : (C) 2000 by Christian Gerlach
    email                : cgerlach@rhrk.uni-kl.de
 ***************************************************************************/
 
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __RGB2YUVDEFS_H
#define __RGB2YUVDEFS_H

// hicolor mode (16 bit) with r(5) g(6) b(5) bits (reverse order b, g, r)
#define RED(rgb)      (unsigned char) ((rgb) << 3)
#define GREEN(rgb)    (((rgb) & 0x7e0) >> 3)
#define BLUE(rgb)     (((rgb) & 0xf800) >> 8)

#define YUV_SHIFT       15
#define YUV_HALF        (1<<(YUV_SHIFT-1))
#define YUV_ONE         (1<<YUV_SHIFT)
#define Y_R   ((int)( 0.299 * YUV_ONE ))
#define Y_G   ((int)( 0.587 * YUV_ONE ))
#define Y_B   ((int)( 0.114 * YUV_ONE ))
#define U_R   ((int)(-0.146 * YUV_ONE ))
#define U_G   ((int)(-0.288 * YUV_ONE ))
#define U_B   ((int)( 0.434 * YUV_ONE ))
#define V_R   ((int)( 0.617 * YUV_ONE ))
#define V_G   ((int)(-0.517 * YUV_ONE ))
#define V_B   ((int)(-0.100 * YUV_ONE ))
  
#define Y_RGB(R,G,B) (( Y_R * (R) + Y_G * (G) + Y_B * (B)) >> YUV_SHIFT) 
#define U_RGB(R,G,B) ((( U_R * (R) + U_G * (G) + U_B * (B)) >> YUV_SHIFT) + 128)
#define V_RGB(R,G,B) ((( V_R * (R) + V_G * (G) + V_B * (B)) >> YUV_SHIFT) + 128) 

static unsigned char CLEARX[8]  = { 255, 0, 255, 0, 255, 0, 255, 0 };
static short ZEROSX[4]  = { 0, 0, 0, 0 };

static short OFFSETDX[4] = { 0, 64, 0, 64 };
static short OFFSETWX[4] = { 128, 0, 128, 0 };
static short OFFSETBX[4] = { 128, 128, 128, 128 };

static short YR0GRX[4] = { Y_R, Y_G, 0, Y_R };
static short YBG0BX[4] = { Y_B, 0, Y_G, Y_B };

static short UR0GRX[4] = { U_R, U_G, 0, U_R };
static short UBG0BX[4] = { U_B, 0, U_G, U_B };

static short VR0GRX[4] = { V_R, V_G, 0, V_R };
static short VBG0BX[4] = { V_B, 0, V_G, V_B };
 
#endif
