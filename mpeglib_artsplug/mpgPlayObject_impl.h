/*
  class for mpeg video decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __MPGPLAYOBJECT_IMPL_H
#define __MPGPLAYOBJECT_IMPL_H


#include "decoderBaseObject_impl.h"
#include "../mpeglib/lib/decoder/mpgPlugin.h"



class MPGPlayObject_impl: 
 virtual public DecoderBaseObject_impl,
 virtual public MPGPlayObject_skel {


public:

  MPGPlayObject_impl();
  virtual ~MPGPlayObject_impl();


  DecoderPlugin* createPlugin();



  void calculateBlock(unsigned long samples);

};


#endif
