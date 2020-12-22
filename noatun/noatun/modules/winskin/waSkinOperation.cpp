/*
  operations with skinset.
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <config.h>
#include <qdir.h>
#include <waSkinOperation.h>

#include "error.xpm"
#include "waSkinMapping.h"  


WaSkinOperation::WaSkinOperation() {
}


WaSkinOperation::~WaSkinOperation() {
}


int WaSkinOperation::loadSkinSet(QString skinDir,WaSkinModell* waSkinModell) {
  int i;

  // now load the pixmaps
  QPixmap loadPixmap;
  QFileInfo fileInfo;
  int lError=false;

  if (WaSkinOperation::isZippedFile(skinDir)) {
    char* tempdir=tempnam(NULL, NULL);
    QString cmd=WaSkinOperation::unzipCommand(skinDir,tempdir);
    QString dir=WaSkinOperation::doUnzip(cmd,tempdir);
    lError=WaSkinOperation::loadSkinSet(dir,waSkinModell);
    
    QString del;
    del="rm -rf ";
    del=del+tempdir;
    WaSkinOperation::doSystemCall(del);

    free(tempdir);
    waSkinModell->setSkinDirectory(skinDir);
    if (lError) {
    }
    return false;
  }

  for(i=0;i<_WA_SKIN_ENTRIES;i++) {
    WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinDesc(i);
    QPixmap* pixmap=waSkinDesc->getPixmap();
    QString file=waSkinDesc->getFilename();

    int lFound=WaSkinOperation::getPixmap(skinDir,file,&loadPixmap);

    int x=waSkinDesc->getX();
    int y=waSkinDesc->getY();
    int width=waSkinDesc->getWidth();
    int height=waSkinDesc->getHeight();
    pixmap->resize(width,height);
    if (lFound == true) {
      bitBlt(pixmap,0,0,&loadPixmap,x,y,width,height);
    } else {
      lError=true;
    }
    waSkinModell->emitChangeEvent(i);
  }
  waSkinModell->setSkinDirectory(skinDir);
  if (lError) {
  }
  return false;
}


void WaSkinOperation::makeEmptySkinModell(WaSkinModell* waSkinModell) {
  int i;
  char id[10];
  id[9]=0;

  // now make a lokal copy of global struct
  QPixmap pixmap(error_xpm);
  for(i=0;i<_WA_SKIN_ENTRIES;i++) {
    WaSkinDesc* waSkinDesc=waSkinModell->
      getWaSkinDesc(globalMapFromFile[i].posId);

    waSkinDesc->setFilename(globalMapFromFile[i].filename);
    waSkinDesc->setPosId(globalMapFromFile[i].posId);
    waSkinDesc->setX(globalMapFromFile[i].x);
    waSkinDesc->setY(globalMapFromFile[i].y);
    waSkinDesc->setWidth(globalMapFromFile[i].width);
    waSkinDesc->setHeight(globalMapFromFile[i].height);
    waSkinDesc->setPixmap(&pixmap);

    QPixmap* tmpPixmap=waSkinDesc->getPixmap();
    QPainter paint;
    paint.begin( tmpPixmap );
    snprintf((char*)&id,9,"%d",waSkinDesc->getPosId());
    paint.drawText ( 0, 10, (char*)&id);
    paint.end();  


    //
    // Now find the mapToGlobal id an add the information
    //
    int id=waSkinDesc->getPosId();
    int lFound=false;
    for(i=0;i<_WA_SKIN_ENTRIES;i++) {
      if (bmpToGui[i][0]==id) {
	int globalPos=bmpToGui[i][1];
	waSkinDesc->setWidgetId(globalPos);
	if (globalPos != -1) {
	  waSkinDesc->setWidgetX(globalMapToGui[globalPos].x);
	  waSkinDesc->setWidgetY(globalMapToGui[globalPos].y);
	  waSkinDesc->setWidgetWidth(globalMapToGui[globalPos].width);
	  waSkinDesc->setWidgetHeight(globalMapToGui[globalPos].height);
	  waSkinModell->emitChangeEvent(i);
	}
	lFound=true;
	break;
	
      }
    }
    if (lFound == false) {
      exit(0);
    }

  }
  // now load the mapping in the modell
  for(i=0;i<_WA_MAPPING_ENTRIES;i++) {
    WaSkinDesc* waSkinDesc=waSkinModell
      ->getWaSkinMapDesc(globalMapToGui[i].posId);
    waSkinDesc->setWidgetId(globalMapToGui[i].posId);
    waSkinDesc->setWidgetX(globalMapToGui[i].x);
    waSkinDesc->setWidgetY(globalMapToGui[i].y);
    waSkinDesc->setWidgetWidth(globalMapToGui[i].width);
    waSkinDesc->setWidgetHeight(globalMapToGui[i].height);
  }

}


int WaSkinOperation::getPixmap(QString dir,QString fname,QPixmap* target) {
  int back;
  QDir qdir(dir);

  if (WaSkinOperation::isZippedFile(dir)) {
    char* tempdir=tempnam(NULL, NULL);
    QString cmd=WaSkinOperation::unzipCommand(dir,tempdir);
    QString skinDir=WaSkinOperation::doUnzip(cmd,tempdir);
    back=WaSkinOperation::getPixmap(skinDir,fname,target);

    QString del;
    del="rm -rf ";
    del=del+tempdir;
    WaSkinOperation::doSystemCall(del);
    free(tempdir);
    return back;
  }


  if (qdir.exists() == false) {
    return false;
  }

  QString basename=WaSkinOperation::baseName(fname);

  back=WaSkinOperation::getPixmapExt(&qdir,basename,"bmp",target);
  if (back == true) {
    return true;
  }
  back=WaSkinOperation::getPixmapExt(&qdir,basename,"xpm",target);
  if (back == true) {
    return true;
  }
  return false;
}
  

int WaSkinOperation::getPixmapExt(QDir* dir,QString basename,
				  QString extension,QPixmap* target) {


  QFileInfo fileInfo;
  QStringList strList=dir->entryList();

  QString cmpString2=basename.upper();
  
  int n=strList.count();
  int i;
  // this strList thing is a hack for qt-1.40
  strList.first();
  for(i=0;i<n;i++) {
    QString file=strList[i];

    QFileInfo fileInfo;
    fileInfo.setFile(file);
    if (fileInfo.isDir()) {
      continue;
    }
    QString cmpString1=fileInfo.baseName().upper();
    
    if (cmpString1==cmpString2) {
      QString file=dir->absPath()+"/"+QString(fileInfo.filePath());
      target->load(file);
      return true;
    }
  }

  // now the filename mapping 1.8x -> 2.0
  if (basename.upper() == "VOLUME") {
    return WaSkinOperation::getPixmapExt(dir,QString("VOLBAR"),
					 extension,target);
  }
  if (basename.upper() == "NUMBERS") {
    return WaSkinOperation::getPixmapExt(dir,QString("NUMS_EX"),
					 extension,target);
  } 

  return false;
}


int WaSkinOperation::isZippedFile(QString skinDir) {
  QFileInfo fileInfo;
  QString ending;
  fileInfo.setFile(skinDir);
  ending=fileInfo.extension().upper();
  if (ending == "") {
    return false;
  }
  if ( (ending == "ZIP") ||
       (ending == "TAR") ||
       (ending == "TAR.GZ") ||
       (ending == "TGZ") ||
       (ending == "GZ" ) || 
       (ending == "TAR.BZ2") ||
       (ending == "BZ2") ) {
    return true;
  }
  return false;
}


QString WaSkinOperation::unzipCommand(QString skinDir,QString tempdir) {
  QString tmp;
  QString back;
  QString ending;
  QFileInfo fileInfo;
  
  fileInfo.setFile(skinDir);
  ending=fileInfo.extension().upper();
  if (ending == "ZIP") {
    back=getenv("UNZIPCMD");
    if (back == NULL) {
      back="unzip ";
    }
    tmp=back+" -o -j \""+skinDir+"\" -d "+tempdir;

    return tmp;
  }

  if ( (ending == "GZ") ) {
    back="gzip -d ";
    tmp=back+skinDir;
    return tmp;
  }

  if ( (ending == "TAR.GZ") || 
       (ending == "TGZ") ) {
    back=getenv("TARCMD");
    if (back == NULL) {
      back="gzip -cd ";
    }
    tmp=back+"\""+skinDir+"\" >"+tempdir+"/tmp.tar";
    tmp=tmp+";"+unzipCommand(QString(tempdir)+"/tmp.tar",tempdir);
    return tmp;
  }

  if (ending == "TAR") {
    back=getenv("TARCMD");
    if (back == NULL) {
      back="tar xvf ";
    }
    tmp=back+"\""+skinDir+"\" -C "+tempdir;
    return tmp;
  }


  if (ending == "BZ2") {
    if (back == NULL) {
      back="bunzip2 ";
    }
    tmp=back+" \""+skinDir+"\"";
    return tmp;
  }

  if (ending == "TAR.BZ2") {
    if (back == NULL) {
      back="bunzip2 ";
    }
    tmp=back+"-cdf \""+skinDir+"\" >"+tempdir+"/tmp.tar";
    tmp=tmp+";"+unzipCommand(QString(tempdir)+"/tmp.tar",tempdir);
    return tmp;
  }

  return NULL;
}


QString WaSkinOperation::doUnzip(QString unzipCommand,QString tempdir) {
  mkdir(tempdir.latin1(), 0x1ed);  /* Permissions: 755 */
  
  
  WaSkinOperation::doSystemCall(unzipCommand);
  return findMainBMP(tempdir);
}


QString WaSkinOperation::findMainBMP(QString tempdir) {
  QDir dir;
  dir.setPath(tempdir);
  // this strList is a big hack to compile with qt-1.40
  QStringList strList=dir.entryList();
  int n=strList.count();
  int i;
  QString path(dir.absPath());
  strList.first();
  for(i=0;i<n;i++) {
    QString file=strList[i];

    if ((file != ".") && (file != "..")) {
      QFileInfo fileInfo;
      QString testDir=path+"/"+file;
      fileInfo.setFile(testDir);
      if (fileInfo.isDir()) {
	QString back=findMainBMP(testDir);
	if (back != "") {
	  return back;
	}
	continue;
      }
      QString cmpString=fileInfo.baseName().upper();
      if (cmpString=="MAIN") {
	return QString(tempdir)+"/";
      }
    }
    
  }
  return "";
}



QString WaSkinOperation::baseName(QString filename) {
  int pos = filename.find('.');
  if ( pos == -1 ) {
    return filename;
  } else {
    return filename.left(pos);
  }

}



void WaSkinOperation::doSystemCall(QString call) {
  system(call.latin1());
  if (errno == 127) {
  }
  if (errno < 0) {
    perror("error is:");
  } 
}

