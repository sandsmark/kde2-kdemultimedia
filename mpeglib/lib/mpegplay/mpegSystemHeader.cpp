/*
  stores info about system stream and sends data to audio/video
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegSystemHeader.h"


MpegSystemHeader::MpegSystemHeader() {
   audioLayerSelect=0;
   videoLayerSelect=0;
   lmpeg2=false;
   lPTSFlag=false;

   layer=_PACKET_UNKNOWN_LAYER;
   //
   //  MPEG2 Stuff [START]
   //

   lOriginal=false;
   lCopyRight=false;
   lDataAlignmentIndicator=false;
   lPesPriority=false;
   lEncrypted=false;
   startCodePrefix=0;

   lPTSDTSFlag=false;
   lESCR=false;
   lESRateFlag=false;
   lDMSTRICKFLAG=false;
   lADDITIONAL_COPY_FLAG=false;
   lPES_CRC_FLAG=false;
   lPES_EXT_FLAG=false;
   nPES_HEADER_DATA_LENGTH=0;

   // EXT FILED [START]

   lPrivateDataFlag=false;
   lPackHeaderFieldFlag=false;
   lSequenceCounterFlag=false;
   lSTDBufferFlag=false;
   lPES_EXT_FLAG_2=false;
   nPES_EXT_FIELD_LENGTH=0;
   // EXT FILED [END]

   subStreamID=0;
   //
   // MPEG2 Stuff [END]
   //
 
}


MpegSystemHeader::~MpegSystemHeader() {

}

double MpegSystemHeader::getSCRTimeStamp() {
  return scrTimeStamp;
}


double MpegSystemHeader::getPTSTimeStamp() {
  return ptsTimeStamp;
}


int MpegSystemHeader::getPTSFlag() {
  return lPTSFlag;
}


void MpegSystemHeader::setPTSFlag(int lPTSFlag) {
  this->lPTSFlag=lPTSFlag;
}


double MpegSystemHeader::getDTSTimeStamp() {
  return dtsTimeStamp;
}


void MpegSystemHeader::setSCRTimeStamp(double scrTimeStamp) {
  this->scrTimeStamp=scrTimeStamp;
}


void MpegSystemHeader::setPTSTimeStamp(double ptsTimeStamp) {
  this->ptsTimeStamp=ptsTimeStamp;
}


void MpegSystemHeader::setDTSTimeStamp(double dtsTimeStamp) {
  this->dtsTimeStamp=dtsTimeStamp;
}



int MpegSystemHeader::getLayer() {
  return layer;
}


void MpegSystemHeader::setLayer(int layer) {
  this->layer=layer;
}

void MpegSystemHeader::addAvailableLayer(int streamID) {
  switch (streamID>>4){
  case _PAKET_ID_AUDIO_1>>4:
  case _PAKET_ID_AUDIO_2>>4:
    availableAudioLayers |= 1<<(streamID - 0xc0);
    break;
  case _PAKET_ID_VIDEO>>4:
    availableVideoLayers |= 1<<(streamID - 0xe0);
    break;
  case _SUBSTREAM_AC3_ID>>4:
    availableAudioLayers |= 1<<(streamID - 0x80);
    break;
  default:
    cout << "unknown streamID MpegSystemHeader::addAvailableLayer"<<endl;
  }


}
void MpegSystemHeader::resetAvailableLayers() {
  availableAudioLayers = 0;
  availableVideoLayers = 0;
}

int MpegSystemHeader::getAudioLayerSelect() {
  if (availableAudioLayers & (1<<audioLayerSelect))
    return audioLayerSelect;
  else
    return 0;
}

void MpegSystemHeader::setAudioLayerSelect(int layer) {
  audioLayerSelect = layer;
}

int MpegSystemHeader::getVideoLayerSelect() {
  if (availableVideoLayers & (1<<videoLayerSelect))
    return videoLayerSelect;
  else
    return 0;
}

void MpegSystemHeader::setVideoLayerSelect(int layer) {
  videoLayerSelect = layer;
}
 




int MpegSystemHeader::getPacketID() {
  return packetID;
}


void MpegSystemHeader::setPacketID(int packetID) {
  this->packetID=packetID;
}

int MpegSystemHeader::getPacketLen() {
  return packetLen;
}


void MpegSystemHeader::setPacketLen(int packetLen) {
  this->packetLen=packetLen;
}
 
int MpegSystemHeader::getRate() {
  return rate;
}


void MpegSystemHeader::setRate(int rate) {
  this->rate=rate;
}


int MpegSystemHeader::getStdBufferScale() {
  return stdBufferScale;
}


void MpegSystemHeader::setStdBufferScale(int stdBufferScale) {
  this->stdBufferScale=stdBufferScale;
}

unsigned long MpegSystemHeader::getStdBufferSize() {
  return stdBufferSize;
}


void MpegSystemHeader::setStdBufferSize(unsigned long stdBufferSize) {
  this->stdBufferSize=stdBufferSize;
}
 

int MpegSystemHeader::getMPEG2() {
  return lmpeg2;
}


//
//  MPEG2 Stuff [START]
//


int MpegSystemHeader::getOriginalOrCopy() {
  return lOriginal;
}


void MpegSystemHeader::setOriginalOrCopy(int lOriginal) {
  this->lOriginal=lOriginal;
}


int MpegSystemHeader::getCopyRight() {
  return lCopyRight;
}


void MpegSystemHeader::setCopyRight(int lCopyRight) {
  this->lCopyRight=lCopyRight;
}



int MpegSystemHeader::getDataAlignmentIndicator() {
  return lDataAlignmentIndicator;
}


void MpegSystemHeader::setDataAlignmentIndicator(int lDataAlignmentIndicator) {
  this->lDataAlignmentIndicator=lDataAlignmentIndicator;
}


int MpegSystemHeader::getPesPriority() {
  return lPesPriority;
}


void MpegSystemHeader::setPesPriority(int lPesPriority) {
  this->lPesPriority=lPesPriority;
}



int MpegSystemHeader::getEncrypted() {
  return lEncrypted;
}


void MpegSystemHeader::setEncrypted(int lEncrypted) {
  this->lEncrypted=lEncrypted;
}


int MpegSystemHeader::getStartCodePrefix() {
  return startCodePrefix;
}


void MpegSystemHeader::setStartCodePrefix(int startCodePrefix) {
  this->startCodePrefix=startCodePrefix;
}


int MpegSystemHeader::getPTSDTSFlag(){
  return lPTSDTSFlag;
}


void MpegSystemHeader::setPTSDTSFlag(int lPTSDTSFlag){
  this->lPTSDTSFlag=lPTSDTSFlag;
}


int MpegSystemHeader::getESCRFlag() {
  return lESCR;
}


void MpegSystemHeader::setESCRFlag(int lESCR) {
  this->lESCR=lESCR;
}


int MpegSystemHeader::getES_RATE_Flag() {
  return lESRateFlag;
}


void MpegSystemHeader::setES_RATE_Flag(int lESRateFlag) {
  this->lESRateFlag=lESRateFlag;
}



int MpegSystemHeader::getDMSTRICKFLAG(){
  return lDMSTRICKFLAG;
}


void MpegSystemHeader::setDMSTRICKFLAG(int lDMSTRICKFLAG) {
  this->lDMSTRICKFLAG=lDMSTRICKFLAG;
}


int MpegSystemHeader::getADDITIONAL_COPY_FLAG() {
  return lADDITIONAL_COPY_FLAG;
}


void MpegSystemHeader::setADDITIONAL_COPY_FLAG(int lADDITIONAL_COPY_FLAG) {
  this->lADDITIONAL_COPY_FLAG=lADDITIONAL_COPY_FLAG;
}



int MpegSystemHeader::getPES_CRC_FLAG() {
  return lPES_CRC_FLAG;
}


void MpegSystemHeader::setPES_CRC_FLAG(int lPES_CRC_FLAG) {
  this->lPES_CRC_FLAG=lPES_CRC_FLAG;
}


int MpegSystemHeader::getPES_EXT_FLAG() {
  return lPES_EXT_FLAG;
}


void MpegSystemHeader::setPES_EXT_FLAG(int lPES_EXT_FLAG) {
  this->lPES_EXT_FLAG=lPES_EXT_FLAG;
}


  //
  // PES EXTENSION [START]
  //

int MpegSystemHeader::getPrivateDataFlag() {
  return lPrivateDataFlag;
}


void MpegSystemHeader::setPrivateDataFlag(int lPrivateDataFlag) {
  this->lPrivateDataFlag=lPrivateDataFlag;
}


int MpegSystemHeader::getPackHeaderFieldFlag() {
  return lPackHeaderFieldFlag;
}


void MpegSystemHeader::setPackHeaderFieldFlag(int lPackHeaderFieldFlag) {
  this->lPackHeaderFieldFlag=lPackHeaderFieldFlag;
}


int MpegSystemHeader::getSequenceCounterFlag() {
  return lSequenceCounterFlag;
}


void MpegSystemHeader::setSequenceCounterFlag(int lSequenceCounterFlag) {
  this->lSequenceCounterFlag=lSequenceCounterFlag;
}



int MpegSystemHeader::getSTDBufferFlag() {
  return lSTDBufferFlag;
}


void MpegSystemHeader::setSTDBufferFlag(int lSTDBufferFlag) {
  this->lSTDBufferFlag=lSTDBufferFlag;
}



int MpegSystemHeader::getPES_EXT_FLAG_2() {
  return lPES_EXT_FLAG_2;
}


void MpegSystemHeader::setPES_EXT_FLAG_2(int lPES_EXT_FLAG_2) {
  this->lPES_EXT_FLAG_2=lPES_EXT_FLAG_2;
}


int MpegSystemHeader::getPES_EXT_FIELD_LENGTH() {
  return nPES_EXT_FIELD_LENGTH;
}


void MpegSystemHeader::setPES_EXT_FIELD_LENGTH(int nPES_EXT_FIELD_LENGTH) {
  this->nPES_EXT_FIELD_LENGTH=nPES_EXT_FIELD_LENGTH;
}



  //
  // PES EXTENSION [END]
  //



int MpegSystemHeader::getPES_HEADER_DATA_LENGTH() {
  return nPES_HEADER_DATA_LENGTH;
}


void MpegSystemHeader::setPES_HEADER_DATA_LENGTH(int nPES_HEADER_DATA_LENGTH){
  this->nPES_HEADER_DATA_LENGTH=nPES_HEADER_DATA_LENGTH;
}


int MpegSystemHeader::getSubStreamID() {
  return subStreamID;
}


void MpegSystemHeader::setSubStreamID(int subStreamID) {
  this->subStreamID=subStreamID;
}

//
// MPEG2 Stuff [END]
//



void MpegSystemHeader::setMPEG2(int lmpeg2) {
  this->lmpeg2=lmpeg2;
}


void MpegSystemHeader::print() {
  cout << "MpegSystemHeader [START]"<<endl;
  cout << "layer:"<<getLayer()<<endl;
  cout << "MpegSystemHeader [END]"<<endl;
  
}
