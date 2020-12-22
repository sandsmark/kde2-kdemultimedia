/*
  stores info about system stream and sends data to audio/video
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __MPEGSYSTEMHEADER_H
#define __MPEGSYSTEMHEADER_H



#define _PACKET_SYSLAYER            1
#define _PACKET_NO_SYSLAYER         0
#define _PACKET_UNKNOWN_LAYER      -1
#define _PACKET_HEADER_SIZE 8


// note: packetid 1 & 2 are normalized to _PAKET_ID_AUDIO
// see packet.cpp

#define _PAKET_ID_AUDIO_1    ((unsigned char) 0xc0)
#define _PAKET_ID_AUDIO_2    ((unsigned char) 0xd0)
#define _PAKET_ID_VIDEO      ((unsigned char) 0xe0)
#define _PAKET_ID_NUKE       ((unsigned char) 0x0)
  
#define _STD_SYSTEM_CLOCK_FREQ (unsigned long)90000
#define _MUX_RATE_SCALE_FACTOR 50
#define _MAX_STREAMS 8
#define _NOT_PACKET_ID       ((unsigned char) 0xff)
#define _KILL_BUFFER         ((unsigned char) 0xfe)


  
#define _STD_AUDIO_STREAM_ID          ((unsigned char) 0xb8)
#define _STD_VIDEO_STREAM_ID          ((unsigned char) 0xb9)
#define _MIN_STREAM_ID_ID             ((unsigned char) 0xbc)
#define _RESERVED_STREAM_ID           ((unsigned char) 0xbc)
#define _PRIVATE_STREAM_1_ID          ((unsigned char) 0xbd)
#define _PADDING_STREAM_ID            ((unsigned char) 0xbe)
#define _PRIVATE_STREAM_2_ID          ((unsigned char) 0xbf)
#define _ECM_STREAM_ID                ((unsigned char) 0xf0)
#define _EMM_STREAM_ID                ((unsigned char) 0xf1)
#define _PROGRAM_STREAM_DIRECTORY_ID  ((unsigned char) 0xff)
#define _DSMCC_STREAM_ID              ((unsigned char) 0xf2)
#define _ITUTRECH222TYPEE_STREAM_ID   ((unsigned char) 0xf8)
#define _SUBSTREAM_AC3_ID             ((unsigned char) 0x80)
#define _SUBSTREAM_LPCM_ID            ((unsigned char) 0xA0)
#define _SUBSTREAM_SUBPIC_ID          ((unsigned char) 0x20)


/* Silly Constants.... */
#define _PACK_START_CODE             ((unsigned int)0x000001ba)
#define _SYSTEM_HEADER_START_CODE    ((unsigned int)0x000001bb)
#define _PACKET_START_CODE_MASK      ((unsigned int)0xffffff00)
#define _PACKET_START_CODE_PREFIX    ((unsigned int)0x00000100)
#define _ISO_11172_END_CODE          ((unsigned int)0x000001b9)

#define _SEQ_START_CODE 0x000001b3
 
  
#include <iostream.h>


  
class MpegSystemHeader {

  int lPTSFlag;
  double scrTimeStamp;
  double ptsTimeStamp;
  double dtsTimeStamp;

  int layer;

  int lmpeg2;

  int packetID;
  int packetLen;
  unsigned long rate;
  int stdBufferScale;
  unsigned long stdBufferSize;

  /* This are bitmaps, if bit x is a 1 the stream number x is
     available */
  unsigned long availableAudioLayers;
  unsigned long availableVideoLayers;

  int audioLayerSelect;
  int videoLayerSelect;
  
  //
  // MPEG2 Header Info [START]
  //
  int lOriginal;
  int lCopyRight;
  int lDataAlignmentIndicator;
  int lPesPriority;
  int lEncrypted;
  int startCodePrefix;



  int lPTSDTSFlag;
  int lESCR;
  int lESRateFlag;
  int lDMSTRICKFLAG;
  int lADDITIONAL_COPY_FLAG;
  int lPES_CRC_FLAG;
  int lPES_EXT_FLAG;
  int nPES_HEADER_DATA_LENGTH;

  // EXT FILED [START]

  int lPrivateDataFlag;
  int lPackHeaderFieldFlag;
  int lSequenceCounterFlag;
  int lSTDBufferFlag;
  int lPES_EXT_FLAG_2;
  int nPES_EXT_FIELD_LENGTH;
  // EXT FILED [END]

  int subStreamID;
  //
  // MPEG2 Header Info [END]
  //

 public:
  MpegSystemHeader();
  ~MpegSystemHeader();
  
  int getPTSFlag();
  void setPTSFlag(int lPTSFlag);
    
  double getSCRTimeStamp();
  double getPTSTimeStamp();
  double getDTSTimeStamp();
  

  void setSCRTimeStamp(double timeStamp);
  void setPTSTimeStamp(double ptsTimeStamp);
  void setDTSTimeStamp(double dtsTimeStamp);

  // returns 1 if it is a syslayer 0 if non syslayer -1 if unknown
  int getLayer();

  void setLayer(int layer);

  //
  //  MPEG2 Stuff [START]
  //

  int getMPEG2();
  void setMPEG2(int lmpeg2);

  // 1 Byte [Start]
  int getOriginalOrCopy();
  void setOriginalOrCopy(int lOriginal);

  int getCopyRight();
  void setCopyRight(int lCopyRight);

  int getDataAlignmentIndicator();
  void setDataAlignmentIndicator(int lDataAlignmentIndicator);

  int getPesPriority();
  void setPesPriority(int lPesPriority);

  int getEncrypted();
  void setEncrypted(int lEncrypted);

  int getStartCodePrefix();
  void setStartCodePrefix(int startCodePrefix);

  // 1. Byte [End]

  int getPTSDTSFlag();
  void setPTSDTSFlag(int lPTSDTSFlag);

  int getESCRFlag();
  void setESCRFlag(int lESCR);
 
  int getES_RATE_Flag();
  void setES_RATE_Flag(int lESRateFlag);
  

  int getDMSTRICKFLAG();
  void setDMSTRICKFLAG(int lDMSTRICKFLAG);

  int getADDITIONAL_COPY_FLAG();
  void setADDITIONAL_COPY_FLAG(int lADDITIONAL_COPY_FLAG);
  
  int getPES_CRC_FLAG();
  void setPES_CRC_FLAG(int lPES_CRC_FLAG);
    
  int getPES_EXT_FLAG();
  void setPES_EXT_FLAG(int lPES_EXT_FLAG);

  //
  // PES EXTENSION [START]
  //

  int getPrivateDataFlag();
  void setPrivateDataFlag(int lPrivateDataFlag);

  int getPackHeaderFieldFlag();
  void setPackHeaderFieldFlag(int lPackHeaderFieldFlag);
 
  int getSequenceCounterFlag();
  void setSequenceCounterFlag(int lSequenceCounterFlag);

  int getSTDBufferFlag();
  void setSTDBufferFlag(int lSTDBufferFlag);
  
  int getPES_EXT_FLAG_2();
  void setPES_EXT_FLAG_2(int lPES_EXT_FLAG_2);

  int getPES_EXT_FIELD_LENGTH();
  void setPES_EXT_FIELD_LENGTH(int nPES_EXT_FIELD_LENGTH);



  //
  // PES EXTENSION [END]
  //

  int getPES_HEADER_DATA_LENGTH();
  void setPES_HEADER_DATA_LENGTH(int nPES_HEADER_DATA_LENGTH);


  int getSubStreamID();
  void setSubStreamID(int subStreamID);


  //
  // MPEG2 Stuff [END]
  //
   
  void addAvailableLayer(int streamID);
  void resetAvailableLayers();
  
  int getAudioLayerSelect();
  void setAudioLayerSelect(int layer);

  int getVideoLayerSelect();
  void setVideoLayerSelect(int layer);


  int getPacketID();
  void setPacketID(int packetID);

  int getPacketLen();
  void setPacketLen(int packetLen);

  int getRate();
  void setRate(int rate);

  int getStdBufferScale();
  void setStdBufferScale(int stdBufferScale);
  
  unsigned long getStdBufferSize();
  void setStdBufferSize(unsigned long stdBufferSize);

  void print();
};

#endif



