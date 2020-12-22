/*
  reads paket input data
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegSystemStream.h"

#define _RESYNC_STATE_INIT                0
#define _RESYNC_STATE_NEED_PACKET         1
#define _RESYNC_STATE_NEED_START          2




MpegSystemStream::MpegSystemStream(InputStream* input) {
  this->input=input;
  syncCode=0xff;  // invalid
  lState=_RESYNC_STATE_INIT;
}


MpegSystemStream::~MpegSystemStream() {
}


int MpegSystemStream::getByteDirect() {
  unsigned char byte;
  if (input->read((char*)&byte,1) != 1) {
    return -1;
  }
  return (int)byte;
}

int MpegSystemStream::readSyncCode() {
  int byte;
  byte=getByteDirect();
  if (byte==-1) {
    return false;
  } 

  syncCode<<=8;
  syncCode|=byte;
  syncCode&=0xffffffff;
  return true;

}

int MpegSystemStream::firstInitialize(MpegSystemHeader* mpegHeader) {

  if (readSyncCode() == false) {
    return false;
  }
  
  if (syncCode == _SEQ_START_CODE) {
    // If we found a seq_heade_start code we assume
    // that we are a video only stream
    // we must directly insert it.

    // here we set for all future calls fixed
    // parameter
    mpegHeader->setLayer(_PACKET_NO_SYSLAYER);     // no syslayer
    mpegHeader->setPacketID(_PAKET_ID_VIDEO);
    mpegHeader->setPacketLen(8192);
    mpegHeader->resetAvailableLayers();
    return true;
  } else {
    if (syncCode == _PACK_START_CODE) {
      if (processStartCode(syncCode,mpegHeader)==true) {
	mpegHeader->setLayer(_PACKET_SYSLAYER);     // syslayer
	mpegHeader->resetAvailableLayers();
	lState=_RESYNC_STATE_NEED_START;
	return true;
      }

    }
  }
  return false;
}

void MpegSystemStream::reset() {
  lState=_RESYNC_STATE_NEED_PACKET;
}


int MpegSystemStream::nextPacket(MpegSystemHeader* mpegHeader) {
  int layer=mpegHeader->getLayer();
  if (layer == _PACKET_NO_SYSLAYER) {
    return true;
  }
  // seek to packet
  if (readSyncCode() == false) {
    return false;
  }
  if ((syncCode & 0xFFFFFF00) == 0x00000100) {
    if (lState == _RESYNC_STATE_NEED_PACKET) {
      if (syncCode != _PACK_START_CODE) {
	return false;
      }
      lState=_RESYNC_STATE_NEED_START;
    }
    if (processStartCode(syncCode,mpegHeader) == false) {
      reset();      
      return false;
    }
    return true;
  }
  return false;
}




/*
 *-----------------------------------------------------------
 *
 *  readHeader
 *
 *  Parses a start code out of the stream by tossing bytes until it gets one
 *
 *  Results/Side Effects:  Parses bytes of the stream, returns code
 *                         Returns EOF in case of end of file
 *  We search for SEQ_START_CODE, PACKET_START_CODE, _SYSTEM_HEADER_START_CODE
 *-----------------------------------------------------------
 */
int MpegSystemStream::readHeader(unsigned int* startCode) {

  int byte;
  byte=getByteDirect();
  if (byte==-1) {
    return false;
  } 
  if(byte!=0x00) {
    return false;
  }
  byte=getByteDirect();
  if (byte==-1) {
    return false;
  } 
  if (byte != 0x00) {
    return false;
  }  
  byte=getByteDirect();
  if (byte==-1) {
    return false;
  }
  *startCode=byte;
  if (*startCode != 0x01) {
    return false;
  }
  *startCode<<=8;
  byte=getByteDirect();
  if (byte==-1) {
    return false;
  }
  *startCode|=byte;

  return true;
}





/*
 *----------------------------------------------------------
 *
 *      Parse out a packet of the system layer MPEG file.
 *
 *  Results:  Returns true/false
 *
 *  Side Effects:  ReadPacket can change *bs_ptr to be a new buffer
 *                 buf_ptr will remain pointing at *length_ptr (at input)
 *                         into the buffer
 *                 *length_ptr will be changed to the new size
 *                 *max_length can be changed if a new buffer is alloc'd
 *
 *----------------------------------------------------------
 */
/* start is either a start code or 0 to indicate continued parsing */
int MpegSystemStream::processStartCode(unsigned int startCode,
				       MpegSystemHeader* mpegHeader) {
  int lok=true;
  mpegHeader->setPacketLen(0);
  mpegHeader->setPacketID(_PAKET_ID_NUKE);

  if (startCode == _PACK_START_CODE) {
    lok=processPackHeader(mpegHeader);
    if (lok == false) {
      printf("Error in reading pack header\n");
      return false;
    }
    return true;
  }
  if (startCode == _SYSTEM_HEADER_START_CODE) {
    lok = processSystemHeader(mpegHeader);
    if (lok == false) {
      fprintf(stderr, "Error in reading system header\n");
      return false;
    }
    return true;
  }
  // handle default
  lok=processPacket(startCode,mpegHeader);
  if (lok == false) {
    return false;
  }
  return true;
}








/*
 *-----------------------------------------------------------------
 *
 *  ReadPackHeader
 *
 *      Parses out the PACK header
 *
 *  Returns: 1 on error, 0 on success
 *
 *-------------------------------------------------------------------
 */
int MpegSystemStream::processPackHeader(MpegSystemHeader* mpegHeader) {
  int back=false;
  int numRead;
  unsigned long scrbase;
  unsigned long scrext;
  unsigned long rate;

  double timeStamp;

  unsigned char inputBuffer[_PACKET_HEADER_SIZE+2];
  numRead = input->read((char*)inputBuffer, _PACKET_HEADER_SIZE);
  if (numRead < _PACKET_HEADER_SIZE) {
    return false;
  }  

  // only set this if we do an initialize
  int layer=mpegHeader->getLayer();
  if (layer == _PACKET_UNKNOWN_LAYER) {
    if ((inputBuffer[0]>>6)==1) {
      mpegHeader->setMPEG2(true);
    }
  }

  if (mpegHeader->getMPEG2()) {
    numRead = input->read((char*)inputBuffer+_PACKET_HEADER_SIZE, 2);
    if (numRead < 2) {
      cout << "cannot read processPackHeader"<<endl;
      return false;
    }    

    scrbase=GET_SCRBASE(inputBuffer);
    scrext=GET_SCREXT(inputBuffer);

    // BUGGY:
    timeStamp = (double)(scrbase*300 + scrext) / (double)MPEG2_CLK_REF;
    
    rate=GET_MPEG2MUXRATE(inputBuffer);
    
    int stuffing = GET_MPEG2STUFFING(inputBuffer);  // always <= 7

    numRead=stuffing;
    if (stuffing) {
      numRead = input->read((char*)inputBuffer, stuffing);
    }
    back= (numRead == stuffing);
  } else {


    scrbase=GET_SCR(inputBuffer);
    timeStamp=(double)scrbase/MPEG1_CLK_REF;
    rate=GET_MPEG1_MUXRATE(inputBuffer);
    rate*=_MUX_RATE_SCALE_FACTOR;
    back=true;
  }
  mpegHeader->setSCRTimeStamp(timeStamp);
  mpegHeader->setRate(rate);


  return back;
}


/*
 *------------------------------------------------------------------
 *
 *   ReadSystemHeader
 *
 *      Parse out the system header, setup out stream IDs for parsing packets
 *
 *   Results:  Returns 1 on error, 0 on success.
 *             Sets gAudioStreamID and gVideoStreamID
 *
 *------------------------------------------------------------------
 */
int MpegSystemStream::processSystemHeader(MpegSystemHeader* mpegHeader) { 
  unsigned char *inputBuffer = NULL;
  int numRead;
  unsigned short headerSize;

  
  numRead = input->read((char *)&headerSize, 2); 
  headerSize = ntohs(headerSize);
  if (numRead < 2) {
    return false;
  }
  inputBuffer = (unsigned char*) malloc (sizeof(unsigned char)*(headerSize+1));
  inputBuffer[headerSize]=0;
  numRead = input->read((char*)inputBuffer, headerSize); 

  if (numRead < headerSize) {
    return false;
  }  
  mpegHeader->resetAvailableLayers();
  int i = 6;
  while (i<headerSize) {
    if (inputBuffer[i] & 0x80) {
      mpegHeader->addAvailableLayer(inputBuffer[i]);
    }
    i += 3;
  }
  free(inputBuffer);
  return true;
}

 
/*
 *-----------------------------------------------------------------
 *
 *  ReadPacket
 *
 *      Reads a single packet out of the stream, and puts it in the
 *      buffer if it is video.
 *
 *  Results:
 *      Changes the value of *length_ptr to be the new length (plus old)
 *      If the buffer is too small, can change *bs_ptr, *max_length, and 
 *      buf_ptr to be correct for a newly allocated buffer.
 *
 *  State:  
 *      The buffer is in ints, but the packets can be an arbitrary number
 *      of bytes, so leftover bytes are kept in the VideoDecoder variable and
 *      are added on the next call.
 *
 *-----------------------------------------------------------------
 */   
/* Returns:
   0 - no error, but not video packet we want
   -1 - error
   >0 - length of packet
*/
int MpegSystemStream::processPacket(unsigned int startCode,
				    MpegSystemHeader* mpegHeader) {

  int ioBytes;
  unsigned short packetLength;
  unsigned int packetDataLength;

  /* Leftovers from previous video packets */

  int packetID=startCode & 0xff;
  mpegHeader->setPacketID(packetID);
  int lPacket=startCode & _PACKET_START_CODE_MASK &_PACKET_START_CODE_PREFIX;
  if ((lPacket == false) || (packetID < 0xbc)) {
    //printf("unknown startcode,packet or packetID:%8x\n",startCode);
    return false;
  }
  
  if (packetID == _NOT_PACKET_ID) {
    cout << "(vid_stream->mpegVideoStream)->makeEnd()"<<endl;
  } else if (packetID==_KILL_BUFFER) {
    printf("packetID==_KILL_BUFFER\n");
  }
  
  ioBytes = input->read((char*)&packetLength, 2);
  packetLength = htons(packetLength);
  if (ioBytes < 2) {
    return false;
  }
  mpegHeader->setPTSFlag(false);
  mpegHeader->setPacketID(packetID);
  mpegHeader->setPacketLen(packetLength);
  switch (packetID>>4) {
  case _PAKET_ID_AUDIO_1>>4:
  case _PAKET_ID_AUDIO_2>>4:
  case _PAKET_ID_VIDEO>>4:
    break;
  default:
    switch(packetID) {
    case _PRIVATE_STREAM_1_ID:
      break;
    default:
      switch (packetID) {
      case _PRIVATE_STREAM_2_ID:
      case _PADDING_STREAM_ID:
      case _RESERVED_STREAM_ID:
      case _ECM_STREAM_ID:
      case _EMM_STREAM_ID:
      case _PROGRAM_STREAM_DIRECTORY_ID:
      case _DSMCC_STREAM_ID:
      case _ITUTRECH222TYPEE_STREAM_ID:
	return true;
      }
      printf("\nUnknown packet type. (%x) at %ld\n",
	     packetID,input->getBytePosition());
      return true;
    }
  }
  // this is only processed if audio or video found
  
  if (mpegHeader->getMPEG2()==false) {
    packetDataLength = packetLength-processPacketHeader(mpegHeader);
  } else {
    int len=processMPEG2PacketHeader(mpegHeader);
    if (len < 0) {
      return false;
    }
    packetDataLength = packetLength-len;

    // now check in private stream for AC3
    if ( packetID == _PRIVATE_STREAM_1_ID ) {
      packetDataLength = packetDataLength-processPrivateHeader(mpegHeader);
    }
  }

  mpegHeader->setPacketLen(packetDataLength);
  return true;

}


int MpegSystemStream::processPrivateHeader(MpegSystemHeader* mpegHeader) {
  char nukeBuffer[30];
  int pos=0;
  int one=getByteDirect();  
  pos++;
  mpegHeader->setSubStreamID(one);
  switch(one>>4) {
  case _SUBSTREAM_AC3_ID>>4:
    input->read(nukeBuffer,3);
    mpegHeader->addAvailableLayer(one);
    cout << "addAvailableLayer:"<<one<<endl;
    pos+=3;
    break;
  case _SUBSTREAM_LPCM_ID>>4:
    input->read(nukeBuffer,6);
    pos+=6;
    break;
  case _SUBSTREAM_SUBPIC_ID>>4:
    input->read(nukeBuffer,3);
    pos+=3;
    break;
  default:
    printf("unknown sub id :%8x\n",one);
  }
  return pos;

}


int MpegSystemStream::processMPEG2PacketHeader(MpegSystemHeader* mpegHeader){

  int stdCnt=0;
  int pos=0;

  // 1. Byte
  /*
    FROM FLASK:
    int getbits(2);
    encrypted = getbits(2);   // PES_scrambling_control
    getbits(4);
    //LIVID
    u_char original_or_copy                 : 1;
    u_char copyright                        : 1;
    u_char data_alignment_indicator         : 1;
    u_char pes_priority                     : 1;
    u_char pes_scrambling_control           : 2;
    u_char start_code_prefix                : 2;    // 0x02 

  */
  int first=getByteDirect();
  stdCnt++;
  mpegHeader->setOriginalOrCopy(first&(128)>>7);
  mpegHeader->setCopyRight(first&(64)>>6);
  mpegHeader->setDataAlignmentIndicator(first&(32)>>5);
  mpegHeader->setPesPriority(first&(16)>>4);
  mpegHeader->setEncrypted((first&(8+4))>>2);
  mpegHeader->setStartCodePrefix(first&(1+2));


  // 2. Byte
  /*
   PTS_DTS_flags = getbits(2);
   ESCR_flag = get1bit();
   ES_rate_flag = get1bit();
   DSM_trick_mode_flag = get1bit();
   additional_copy_info_flag = get1bit();
   PES_CRC_flag = get1bit();
   PES_extension_flag = get1bit();
  */
  int second=getByteDirect();
  stdCnt++;

  mpegHeader->setPTSDTSFlag((second&(128+64))>>6);
  mpegHeader->setESCRFlag((second&(32))>>5);
  mpegHeader->setES_RATE_Flag((second%(16))>>4);
  mpegHeader->setDMSTRICKFLAG((second&(8))>>3);
  mpegHeader->setADDITIONAL_COPY_FLAG((second&(4))>>2);
  mpegHeader->setPES_CRC_FLAG((second&(2))>>1);
  mpegHeader->setPES_EXT_FLAG(second&(1));


  // 3. Byte
  /*
    PES_header_data_length = getbits(8);
  */
  int third=getByteDirect();
  stdCnt++;
  mpegHeader->setPES_HEADER_DATA_LENGTH(third);

  
  //
  // PARSING MPEG 2 HEADER FLAGS [START]
  //  
  unsigned char nukeBuffer[300];

  int PTS_DTS_flags=mpegHeader->getPTSDTSFlag();
  if (PTS_DTS_flags == 0) {
    mpegHeader->setPTSFlag(false);
  } else {
    mpegHeader->setPTSFlag(true);
  }
    
  if (PTS_DTS_flags > 0x1) {
    input->read((char*)nukeBuffer,5);
    double pts=GET_MPEG2_PTS(nukeBuffer);
    pts=(pts*300.0)/(double)MPEG2_CLK_REF;
    mpegHeader->setPTSTimeStamp(pts);
    pos+=5;
  }
  if (PTS_DTS_flags > 0x2) {
    input->read((char*)nukeBuffer,5);
    double dts=GET_MPEG2_PTS(nukeBuffer);
    mpegHeader->setDTSTimeStamp((dts*300.0)/(double)MPEG2_CLK_REF);
    pos+=5;
  }

  int ESCRFlag=mpegHeader->getESCRFlag();
  if (ESCRFlag == 1){
    cout << "ESCRFlag == 1"<<endl;
    input->read((char*)nukeBuffer,6);
    pos+=6;
  }
  
  int ES_rate_flag=mpegHeader->getES_RATE_Flag();
  if (ES_rate_flag == 1){
    cout << "ES_rate_flag == 1"<<endl;
    input->read((char*)nukeBuffer,3);
    pos+=3;
  }

  int DSM_trick_mode_flag=mpegHeader->getDMSTRICKFLAG();
  if (DSM_trick_mode_flag == 1){
    cout << "DSM_trick_mode_flag == 1"<<endl;
    input->read((char*)nukeBuffer,1);
    pos++;
  }

  int additional_copy_info_flag=mpegHeader->getADDITIONAL_COPY_FLAG();
  if (additional_copy_info_flag  == 1) {
    cout << "additional_copy_info_flag  == 1"<<endl;
    input->read((char*)nukeBuffer,1);
    pos++;
  }

  int PES_CRC_flag=mpegHeader->getPES_CRC_FLAG();
  if (PES_CRC_flag == 1) {
    cout << "PES_CRC_flag == 1"<<endl;
    input->read((char*)nukeBuffer,2);
    pos+=2;
  }

  //
  // PES Extension [START]
  //

  int PES_extension_flag=mpegHeader->getPES_EXT_FLAG();
  if (PES_extension_flag == 1) {
    /*
      FLASK:
      PES_private_data_flag = get1bit();
      pack_header_field_flag = get1bit();
      program_packet_sequence_counter_flag = get1bit();
      PSTD_buffer_flag = get1bit();
      getbits(3);
      PES_extension_flag_2 = get1bit();
    */
    int extensionByte=getByteDirect();

    pos++;
    mpegHeader->setPrivateDataFlag((extensionByte&(128))>>7);
    mpegHeader->setPackHeaderFieldFlag((extensionByte&(64))>>6);
    mpegHeader->setSequenceCounterFlag((extensionByte&(32))>>5);
    mpegHeader->setSTDBufferFlag((extensionByte&(16))>>4);
    mpegHeader->setPES_EXT_FLAG_2(extensionByte&(1));

    int PES_private_data_flag=mpegHeader->getPrivateDataFlag();
    if (PES_private_data_flag == 1) {
      input->read((char*)nukeBuffer,128);
      pos+=128;
    }

    int pack_header_field_flag=mpegHeader->getPackHeaderFieldFlag();
    if (pack_header_field_flag == 1) {
      printf("pack header field flag value not allowed in program streams\n");
      exit(1);
    }

    int sequence_counter_flag=mpegHeader->getSequenceCounterFlag();
    if (sequence_counter_flag==1) {
      cout<<"sequence_counter_flag ==1"<<endl;
      input->read((char*)nukeBuffer,2);
      pos+=2;
    }

    int PSTD_buffer_flag=mpegHeader->getSTDBufferFlag();
    if (PSTD_buffer_flag==1) {
      input->read((char*)nukeBuffer,2);
      pos+=2;
    }
    
    int PES_extension_flag_2=mpegHeader->getPES_EXT_FLAG_2();
    if (PES_extension_flag_2 == 1) {
      int extension2_byte=getByteDirect();
      pos++;
      mpegHeader->setPES_EXT_FIELD_LENGTH(extension2_byte&(254));


      int PES_field_length=mpegHeader->getPES_EXT_FIELD_LENGTH();
      int j;
      for (j=0;j<PES_field_length ; j++) {
	cout << "PES_field_length (nuke)"<<endl;
	getByteDirect();
	pos++;
      }
    }
  }
  //
  // PES Extension [END]
  //

  // now nuke remaining bytes from PES DATA Length
  int PES_HEADER_DATA_LENGTH=mpegHeader->getPES_HEADER_DATA_LENGTH();
  int tmp=PES_HEADER_DATA_LENGTH-pos;
  if (tmp>0) {
    input->read((char*)nukeBuffer,tmp);
    pos+=tmp;
  }



  

  //
  // PARSING MPEG 2 HEADER FLAGS [START]
  //  

  int parsed=stdCnt+pos;
  return parsed;
  
}
   

int MpegSystemStream::processPacketHeader(MpegSystemHeader* mpegHeader) {
  unsigned char nextByte;
  int pos;
  int val;
  unsigned char scratch[10];


  nextByte=getByteDirect();

  mpegHeader->setPTSFlag(false);

  pos = 1;
  while (nextByte & 0x80) {
    ++pos;
    val=getByteDirect();
    if (val == -1) return false;
    scratch[0]=val;
    
    nextByte=scratch[0];
  }
  if ((nextByte >> 6) == 0x01) {
    pos += 2;
    scratch[1]=getByteDirect();
    scratch[2]=getByteDirect();
    nextByte=scratch[2];
  } 
  if ((nextByte >> 4) == 0x02) {
    scratch[0] = nextByte;                     
    input->read((char*)&scratch[1],4);
    /* presentation time stamps */
    unsigned char hiBit;
    unsigned long low4Bytes;
    double ptsTimeStamp;
    double dtsTimeStamp=0.0;
    readTimeStamp((unsigned char*)scratch,&hiBit,&low4Bytes);
    makeClockTime(hiBit,low4Bytes,&ptsTimeStamp);
    mpegHeader->setPTSFlag(true);
    mpegHeader->setPTSTimeStamp(ptsTimeStamp);
    mpegHeader->setDTSTimeStamp(dtsTimeStamp);
    
    pos += 4;
  }
  else if ((nextByte >> 4) == 0x03) {
    scratch[0] = nextByte;                      
    input->read((char*)&scratch[1],9);
    /* presentation and decoding time stamps */
    unsigned char hiBit;
    unsigned long low4Bytes;
    double ptsTimeStamp;
    double dtsTimeStamp;
    readTimeStamp((unsigned char*)scratch,&hiBit,&low4Bytes);
    makeClockTime(hiBit,low4Bytes,&ptsTimeStamp);

    readTimeStamp((unsigned char*)&(scratch[5]),&hiBit,&low4Bytes);
    makeClockTime(hiBit,low4Bytes,&dtsTimeStamp);
    mpegHeader->setPTSFlag(true);
    mpegHeader->setPTSTimeStamp(ptsTimeStamp);
    mpegHeader->setDTSTimeStamp(dtsTimeStamp);
    
    pos += 9;
  } 
  return pos;
}



void MpegSystemStream::readTimeStamp(unsigned char* inputBuffer,
				     unsigned char* hiBit,
				     unsigned long* low4Bytes) {
  *hiBit = ((unsigned long)inputBuffer[0] >> 3) & 0x01;
  *low4Bytes = (((unsigned long)inputBuffer[0] >> 1) & 0x03) << 30; 
  *low4Bytes |= (unsigned long)inputBuffer[1] << 22; 
  *low4Bytes |= ((unsigned long)inputBuffer[2] >> 1) << 15; 
  *low4Bytes |= (unsigned long)inputBuffer[3] << 7; 
  *low4Bytes |= ((unsigned long)inputBuffer[4]) >> 1; 
}


void MpegSystemStream::readSTD(unsigned char* inputBuffer,
			       MpegSystemHeader* mpegHeader) {
  int stdBufferScale;
  unsigned long stdBufferSize;
  stdBufferScale = ((int)(inputBuffer[0] & 0x20) >> 5); 
  stdBufferSize = ((unsigned long)inputBuffer[0] & 0x1f) << 8;
  stdBufferSize |= (unsigned long)inputBuffer[1];
  mpegHeader->setStdBufferScale(stdBufferScale);
  mpegHeader->setStdBufferSize(stdBufferSize);
}


int MpegSystemStream::makeClockTime(unsigned char hiBit, 
				    unsigned long low4Bytes,
				    double * clockTime) {
  if (hiBit != 0 && hiBit != 1) {
    *clockTime = 0.0;
    return 1;
  }
  *clockTime = (double)hiBit*FLOAT_0x10000*FLOAT_0x10000 + (double)low4Bytes;
  *clockTime /= (double)_STD_SYSTEM_CLOCK_FREQ;
  return 0;
}




