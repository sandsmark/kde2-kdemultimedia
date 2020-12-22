/*
  reads paket input data
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef __MPEGSYSTEMSTREAM_H
#define __MPEGSYSTEMSTREAM_H


#include <stdlib.h>
#include "../input/inputPlugin.h"
#include "mpegSystemHeader.h"

  

#define FLOAT_0x10000 (double)((unsigned long)1 << 16)
  
  
// MPEG2 PACK HEADER
// SCRbase and SCRext
//                  32 .......................................0 9.........0
// [PACK_START_CODE][01---1--][--------][-----1--][--------][-----1--][-------1

#define ui64 unsigned long
#define ui32 unsigned int
#define i64  long
#define i32  int

#define MPEG1_CLK_REF  (i64)90000
#define MPEG2_CLK_REF  (i64)27000000

#define ZERO                    (i64)0
#define GET_SCRBASE(buf) ( ((ui64)buf[4]&0xF8)>>3   |  \
                            (ui64)buf[3]<<5         |  \
                           ((ui64)buf[2]&0x03)<<13  |  \
                           ((ui64)buf[2]&0xF8)<<12  |  \
                            (ui64)buf[1]<<20        |  \
                           ((ui64)buf[0]&0x03)<<28  |  \
                            (ui64)buf[0]&0x38 <<27 )

#define GET_SCREXT(buf)  ( ((ui64)buf[5]&0xFE)>>1  |  \
                           ((ui64)buf[4]&0x03)<<7 )

// muxrate
//      22 ......................0         stl
// ... [--------][--------][------11][rrrrr---]
#define GET_MPEG2MUXRATE(buf) ( (ui32)buf[6]<<14  |      \
				((ui32)buf[7])<<6 |      \
				((ui32)buf[8]&0x03)>>2)  \
 
#define GET_MPEG2STUFFING(buf) ((buf[9]&0x07))
  
// MPEG1 PACK HEADER
//                      SCR                                            muxrate
//                     32........................................0    22......
//[PACK_START_CODE][0010---1][--------][-------1][--------][-------1][1-------]
#define GET_SCR(buf)   ( ((ui64)buf[4]&0xFE) >>1     | \
                         ((ui64)buf[3])      <<7     | \
                         ((ui64)buf[2]&0xFE) <<14    | \
                         ((ui64)buf[1])      <<22    | \
                         ((ui64)buf[0]&0x0E) <<29 )

#define GET_MPEG1_PTS(x) (GET_SCR(x)) //they follow the same pattern

#define GET_MPEG1_MUXRATE(x) ( ((ui32)x[7]&0xFE) >>1     | \
                               ((ui32)x[6])          <<7     | \
			       ((ui32)x[5]&0x7F) <<15 )

#define GET_MPEG2_PTS_FLAGS(buf) ( ((ui8)buf[3]&0xC0)>>6  )
// MPEG2 PES packet (optional parameters)
//                         PTS                      
//                         32........................................0
// [PACKET_START_CODE][001x---1][--------][-------1][--------][-------1]
#define GET_MPEG2_PTS(buf)   GET_MPEG1_PTS(buf)



 
  

class MpegSystemStream {

  InputStream* input;
  int lHasLength;
  unsigned int syncCode;
  int lState;

 public:
  MpegSystemStream(InputStream* input);
  ~MpegSystemStream();

  // returns true if init successfull
  int firstInitialize(MpegSystemHeader* mpegHeader);
  int nextPacket(MpegSystemHeader* mpegHeader);
  inline int eof() { return input->eof(); }
  void reset();

 private:
  int readSyncCode();
  int getByteDirect();
  int readHeader(unsigned int* startCode);
  int processStartCode(unsigned int startCode,MpegSystemHeader* mpegHeader);

  int processPackHeader(MpegSystemHeader* mpegHeader);
  int processSystemHeader(MpegSystemHeader* mpegHeader);

  int processPacket(unsigned int startCode,MpegSystemHeader* mpegHeader);
  int processPacketHeader(MpegSystemHeader* mpegHeader);
  int processMPEG2PacketHeader(MpegSystemHeader* mpegHeader);
  int processPrivateHeader(MpegSystemHeader* mpegHeader);

  int readStartCode(unsigned int startCode,MpegSystemHeader* mpegHeader);

  void readSTD(unsigned char* inputBuffer,MpegSystemHeader* mpegHeader);
  void readTimeStamp(unsigned char* inputBuffer,unsigned char* hiBit,
		     unsigned long* low4Bytes);

  int makeClockTime(unsigned char hiBit, unsigned long low4Bytes,
		    double * clockTime);
  

};

#endif
