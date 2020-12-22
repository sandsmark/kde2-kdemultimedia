/*
  creates an image of the spectrum analyser peaks
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <spectrumImage.h>




SpectrumImage::SpectrumImage(int height, int width) {
  this->height=height;
  this->width=width;
  qPixmap=new QPixmap(width,height);
}

  
SpectrumImage::~SpectrumImage() {
  delete qPixmap;
}



QPixmap* SpectrumImage::getPixmap() {
  return qPixmap;
}


int SpectrumImage::getHeight() {
  return height;
}

void SpectrumImage::fillImage(float *data) {
  QPainter painter;
  int row;
  int col;
  float colorHeight=(float)height/5.0;
  int currentRow=0;

  painter.begin(qPixmap);

  // draw the first entry



  // now draw the rest 1..
/*  for(col=0;col<width;col++) {
    // first draw the black part
    for(row=0;row<height-col;row++) {
      painter.setPen(QColor(0,0,0));
      painter.drawPoint(col,row);
    }
    currentRow=0;
    for(row=width-col;row<height;row++) {
      // now we must creat a smooth gradient from red to yellow to green
      if (currentRow < 0.4*colorHeight) {
	float x=(float)currentRow/(5.0*colorHeight);
	float a=x;
	float b=73.0*x;
	float c=15.0*x;

	painter.setPen(QColor(208.0-a,39.0+b,c));
      } else if (currentRow < 1.2*colorHeight) {
	float x=(float)currentRow/(5.0*colorHeight);
	float a=16.0*x;
	float b=56.0*x;
	float c=31.0*x;

	painter.setPen(QColor(192.0+a,120.0+b,c));
      } else if (currentRow < 2.2*colorHeight) {
	float x=(float)currentRow/(5.0*colorHeight);
	float a=47.0*x;
	float b=56.0*x;
	float c=31.0*x;
	
	painter.setPen(QColor(191.0-a,223.0,32));
      } else if (currentRow < 3.0*colorHeight) {
	float x=(float)currentRow/(5.0*colorHeight);
	float a=16.0*x;
	float b=31.0*x;
	float c=31.0*x;
	
	painter.setPen(QColor(32.0+a,207.0-b,15));
      } else if (currentRow < 5*colorHeight) {
	float x=(float)currentRow/(5.0*colorHeight);
	float a=16.0*x;
	float b=31.0*x;
	float c=31.0*x;
	
	painter.setPen(QColor(47.0,152.0,0));
      }

    }
  }
  */
  painter.end();
}

