/*
  creates an image of the spectrum analyser peaks
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __SPECTRUMIMAGE_H
#define __SPECTRUMIMAGE_H

#include <iostream>
#include <qpixmap.h> 
#include <qpainter.h> 


/*
  This class creates an image in which each column
  represent a spectrum analyser peak with.
  A 100%peak is a smooth color gradient from green-yellow-red.
  The 99% peak is the 100 percent peak shifted to left with a
  black pixel on the right.
  And so on.

  A spectrum drawer can access the resulting image very fast.

  The class has 101 entries. Each column has <width> RGB elements.
  the 0th entry is the 100% peak the 100th entry is black.

 

*/


class SpectrumImage {
  
  QPixmap* qPixmap;
  int height;
  int width;

 public:
  SpectrumImage(int height, int width);
  ~SpectrumImage();

  QPixmap* getPixmap();
  int getHeight();
  void fillImage(float *data);

};


#endif
