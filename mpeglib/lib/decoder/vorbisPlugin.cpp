/*
  vorbis player plugin
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "vorbisPlugin.h"

#ifdef OGG_VORBIS

size_t fread_func(void *ptr, size_t size, size_t nmemb, void *stream) {
  InputStream* input=(InputStream*) stream;
  return input->read((char*)ptr,size*nmemb);
}


int fseek_func(void *stream, ogg_int64_t offset, int whence) {
  int ret;
  InputStream* input=(InputStream*) stream;

  if (whence==SEEK_SET) {
    ret=input->seek(offset);
    return ret;
  }
  if (whence==SEEK_CUR) {
    ret=input->seek(input->getBytePosition()+offset);
    return ret;
  }  
  if (whence==SEEK_END) {
    ret=input->seek(input->getByteLength());
    return ret;
  }   
  cout << "hm, strange call"<<endl;
  return -1;
}


int fclose_func (void *) {
  //InputStream* input=(InputStream*) stream;

  // its handled different in kmpg
  // we close the stream if the decoder signals eof.
  return true;

}


long ftell_func  (void *stream) {
  InputStream* input=(InputStream*) stream;
  return input->getBytePosition();
}


VorbisPlugin::VorbisPlugin() {
  

  timeDummy=new TimeStamp();
  pcmout=new char[4096];
  lnoLength=false;
  lshutdown=true;

}


VorbisPlugin::~VorbisPlugin() {
  delete timeDummy;
  delete pcmout;
}


// here we can config our decoder with special flags
void VorbisPlugin::config(const char* key,const char* value,void* user_data) {

  if (strcmp(key,"-c")==0) {
    lnoLength=true;
  }
  DecoderPlugin::config(key,value,user_data);
}


int VorbisPlugin::init() {
  ov_callbacks callbacks;

  callbacks.read_func = fread_func;
  callbacks.seek_func = fseek_func;
  callbacks.close_func = fclose_func;
  callbacks.tell_func = ftell_func;

  // here is the hack to pass the pointer to
  // our streaming interface.
  
  if(ov_open_callbacks(input, &vf, NULL, 0, callbacks) < 0) {
    return false;
  }

  return true;
}


// called by decoder thread
int VorbisPlugin::processVorbis(vorbis_info* vi,vorbis_comment* comment) {

  // decode
  int ret;
  int current_section=-1; /* A vorbis physical bitstream may
			     consist of many logical sections
			     (information for each of which may be
			     fetched from the vf structure).  This
			     value is filled in by ov_read to alert
			     us what section we're currently
			     decoding in case we need to change
			     playback settings at a section
			     boundary */
  ret=ov_read(&vf,pcmout,4096,0,2,1,&current_section);
  switch(ret){
  case 0:
    /* EOF */
    lDecoderLoop=false;
    break;
  case -1:
    /* error in the stream.  Not a problem, just reporting it in
       case we (the app) cares.  In this case, we don't. */
    break;  
  default:
    if(current_section!=last_section){
      vi=ov_info(&vf,-1); /* The info struct is different in each
			     section.  vf holds them all for the
			     given bitstream.  This requests the
			     current one */
      
      double timeoffset=ov_time_tell(&vf);
      
      comment = ov_comment(&vf, -1);
      if(comment) {
	cout << "we have a comment:"<<timeoffset<<endl;
      }
    }  
      last_section=current_section;
      output->audioPlay(timeDummy,timeDummy,pcmout,ret);
      break;
    }
  return true;
}


void VorbisPlugin::decoder_loop() {
  vorbis_info *vi=NULL;
  vorbis_comment *comment=NULL;
  last_section=0;
  current_section=0;
       


  if (input == NULL) {
    cout << "VorbisPlugin::decoder_loop input is NULL"<<endl;
    exit(0);
  }
  if (output == NULL) {
    cout << "VorbisPlugin::decoder_loop output is NULL"<<endl;
    exit(0);
  }
  // init audio stream
  output->audioInit();

  /********** Decode setup ************/
  // start decoding
  lshutdown=false;
  while(runCheck()) {

    switch(streamState) {
    case _STREAM_STATE_FIRST_INIT :
      if (init()== false) {
	// total failure. exit decoding
	lDecoderLoop=false;
	break;
      }	
      // now init stream
      vi=ov_info(&vf,-1);
      if (lnoLength==false) {
	pluginInfo->setLength(getTotalLength());
	output->writeInfo(pluginInfo);
      }
      output->audioOpen();
      output->audioSetup(vi->rate,vi->channels-1,1,0,16);
  

      lhasLength=true;
      setStreamState(_STREAM_STATE_PLAY);
      break;
    case _STREAM_STATE_INIT :
    case _STREAM_STATE_PLAY :
      processVorbis(vi,comment);
      break;
    case _STREAM_STATE_WAIT_FOR_END:
      // exit while loop
      lDecoderLoop=false;
      break;
    default:
      cout << "unknown stream state vorbis decoder:"<<streamState<<endl;
    }
  }
  lshutdown=true;
  ov_clear(&vf); /* ov_clear closes the stream if its open.  Safe to
		    call on an uninitialized structure as long as
		    we've zeroed it */
  

  output->audioFlush();
}

// vorbis can seek in streams
int VorbisPlugin::seek_impl(int second) {
  ov_time_seek(&vf,(double) second);
  return true;
}



int VorbisPlugin::getTotalLength() {
  int back=0;
  int byteLen=input->getByteLength();
  if (byteLen == 0) {
    return 0;
  }
  /* Retrieve the length in second*/
  shutdownLock();
  if (lshutdown==false) {
      back = (int) ov_time_total(&vf, -1);
  }
  shutdownUnlock();
  
  return back;
}




#endif
//OGG_VORBIS


