/*
  base class for all mpeglib decoders
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __DECODERBASEOBJECT_IMPL_H
#define __DECODERBASEOBJECT_IMPL_H


#include <math.h>
#include "splayPlayObject.h"
#include "stdsynthmodule.h"
#include "convert.h"
#include <X11/Xlib.h>
#include <audiosubsys.h>
#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <arts/kmedia2.h>
#include <stdlib.h>
#include <connect.h>

class SplayDecoder;
class MpegAudioFrame;
class AudioFrameQueue;
class FrameQueue;



using namespace std;
using namespace Arts;

class SplayPlayObject_impl :
  virtual public Arts::StdSynthModule,
  virtual public SplayPlayObject_skel {


  poState _state;

  // decoder
  SplayDecoder* splay;
  MpegAudioFrame* framer;
  AudioFrameQueue* frameQueue;
  FrameQueue* packetQueue;

  // local input
  FILE* file;

  double flpos;
  int lStreaming;

  float* resampleBuffer;
  int resampleBufferSize;

  unsigned char* inputbuffer;
  DataPacket<mcopbyte> *currentPacket;
  int currentPos;


public:

  SplayPlayObject_impl();
  virtual ~SplayPlayObject_impl();


  bool loadMedia(const string &filename);

  bool streamMedia(InputStream instream);
  void process_indata(DataPacket<mcopbyte> *inpacket);

  string description();
  void description(const string &);

  poTime currentTime();
  poTime overallTime();

  poCapabilities capabilities();
  string mediaName();
  poState state();
  void play();
  void halt();

  void seek(const class poTime &);
  void pause();
  void streamInit();
  void streamStart();
  void calculateBlock(unsigned long samples);
  void streamEnd();

 private:
  Arts::InputStream currentStream;   
 
  void checkResampleBuffer(int size);
  void checkPacketBuffer(int size);
  void getMoreSamples(int needLen);
  void processQueue();



};



#endif
