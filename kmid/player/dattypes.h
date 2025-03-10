/*  dattypes.h  - Some useful definitions and functions 
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

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

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/ 
#ifndef _DATTYPES_H
#define _DATTYPES_H

#include <stdio.h>
#include <sys/types.h>

#undef uchar
#undef ushort
#undef ulong

/**
 * Unsigned char
 */
typedef unsigned char uchar;
/**
 * Unsigned short
 */
typedef unsigned short ushort;
/**
 * Unsigned long
 */
typedef unsigned long ulong;

ushort readShort(FILE *fh);
ulong  readLong (FILE *fh);

void printfdebug(const char *s,int a=0,int b=0, int c=0);
void printfdebug(const char *s,int a,long b);
void printfdebug(const char *s,double a,double b=0, double c=0);

#endif
