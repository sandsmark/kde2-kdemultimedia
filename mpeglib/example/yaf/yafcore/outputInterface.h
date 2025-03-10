/*
  This class sends an output to the outputstream
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */






#ifndef __OUTPUTINTERFACE_H
#define __OUTPUTINTERFACE_H

#include <pthread.h>
#include <iostream.h>
#include <fstream.h>
#include "buffer.h"

// uncomment this for debugging the output stream
// (written to file outstream.dbg)
//#define _DEBUG_OUTPUT


class OutputInterface {

  int protocolSyntax;
  int nr;
  Buffer* outBuffer;
  ostream* out;
  pthread_mutex_t writeOutMut;


 public:

  OutputInterface(ostream* out);
  ~OutputInterface();


  void flushBuffer();
  void setProtocolSyntax(int proto);
  void setNr(int nr);
  void clearBuffer();
  void appendBuffer(const char* msg);

  // make this interface threadsafe
  void lock();
  void unlock();

};



#endif
