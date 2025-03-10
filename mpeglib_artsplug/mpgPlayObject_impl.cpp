/*
  class for mpeg video decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpgPlayObject_impl.h"
#include "debug.h"





MPGPlayObject_impl::MPGPlayObject_impl() {
  arts_debug("MPGPlayObject_impl");
}

MPGPlayObject_impl::~MPGPlayObject_impl() {
  arts_debug("~MPGPlayObject_impl");
}


DecoderPlugin* MPGPlayObject_impl::createPlugin() {
  return new MpgPlugin();
}

void MPGPlayObject_impl::calculateBlock(unsigned long samples) {
  DecoderBaseObject_impl::calculateBlock(samples,left,right);
}



REGISTER_IMPLEMENTATION(MPGPlayObject_impl);

