/*
  operations with skinset.
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef _WASKINOPERATION_H
#define _WASKINOPERATION_H
#include <qtstream.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <waSkinModell.h>


#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>


class WaSkinOperation {


 public:
  WaSkinOperation();
  ~WaSkinOperation();

  static int loadSkinSet(QString skinDir,struct WaSkinModell* waSkinModell);
  static void makeEmptySkinModell(struct WaSkinModell* waSkinModell);
  static int getPixmap(QString dir,QString fname,QPixmap* target);

 private:
  static int getPixmapExt(QDir* dir,QString fname,
			  QString extension,QPixmap* target);
  static int isZippedFile(QString skinDir);
  static QString unzipCommand(QString skinDir,QString tempdir);
  static QString doUnzip(QString unzipCommand,QString tempdir);
  static QString baseName(QString filename);
  static void doSystemCall(QString call);
  static QString findMainBMP(QString tempdir);
};

#endif
