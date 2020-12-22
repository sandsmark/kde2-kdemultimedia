/*
  testprogramm for arts streaming
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "soundserver.h"
#include "kmedia2.h"
#include <connect.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace Arts;

#if defined(HAVE_GETOPT_H) 
#include <getopt.h>
#endif

//
// KDE extension
//

#include <kurl.h>
#include <arts/kplayobjectfactory.h>

void usage() {
  cout << "libsplayartsplay command line tool for arts playobjects2"<<endl;
  cout << "Usage : libsplayartsplay [url]"<<endl;
  cout << endl;
  cout << "-h : help"<<endl;
  cout << endl;
  cout << "THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! " \
       << "USE AT YOUR OWN RISK!"<<endl;
  cout << endl;
  exit(0);
}


int main(int argc, char **argv) {
  int c;

  while(1) { 
    c = getopt (argc, argv, "h");
    if (c == -1) break;
    switch(c) {
    case 'h': {    
      usage();
      exit(0);
      break;
    }
    default:
      printf ("?? getopt returned character code 0%o ??\n", c);
      usage();
      exit(-1);
    }
  }
  if (optind == argc ) {
    usage();
    exit(0);
  }

  cout << "file:"<<argv[optind]<<endl;
  string file(argv[optind]);

  Dispatcher* d=new Dispatcher();
  Arts::SoundServerV2 server;
  server = Arts::Reference("global:Arts_SoundServerV2");
  cout << "4"<<endl;
  
  if(server.isNull()) {
    cerr << "Can't connect to sound server2" << endl;
    exit(0);
  }
  
  //PlayObject play=server.createPlayObjectForURL(file,"audio/x-mp3",true);

  FileInputStream fileInputStream;

  if(!fileInputStream.open(file)) {
    printf("can't open file %s\n",file.c_str());
    exit(1);
  }

  PlayObject play=server.createPlayObjectForStream(fileInputStream,
						   "audio/x-mp3",true);

  if(play.isNull()) {
    cerr << "Can't create PlayObject" << endl;
    exit(0);
  }
  connect(fileInputStream,"outdata", play,"indata");
  fileInputStream.start();
  play.play();
  while(!fileInputStream.eof()) {
    d->ioManager()->processOneEvent(false);
  }



  /*



  KURL file(argv[optind]);

  cout << "1"<<endl;
  Arts::Dispatcher *dispatcher = new Arts::Dispatcher();
  cout << "2"<<endl;
 
  Arts::SoundServerV2 server;
  cout << "3"<<endl;
  server = Arts::Reference("global:Arts_SoundServerV2");
  cout << "4"<<endl;
  
  if(server.isNull()) {
    cerr << "Can't connect to sound server2" << endl;
    exit(0);
  }
  cout << "5"<<endl;

  KPlayObject *playobj = new KPlayObject();
  cout << "6"<<endl;
  playobj->setObject(Arts::PlayObject::null());
  cout << "7"<<endl;
  
  KPlayObjectFactory factory(server);
  cout << "8"<<endl;
  playobj = factory.createPlayObject(file, true);
  cout << "9"<<endl;
  
  Arts::PlayObject po=playobj->object();

  if(po.isNull()) {
    cerr << "Cannot create playobject" << endl;
    exit(0);
  }
  playobj->play();
  */
}

