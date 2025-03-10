/*

    Copyright (C) 1999 Michael Haardt <michael@moria.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    nas_a.c

    Functions to play sound on the Network Audio System

*/

#ifdef AU_NAS

#ifdef ORIG_TIMPP
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#endif

#include <audio/audiolib.h>
#include <audio/soundlib.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NO_STRING_H /* for memmove */
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef ORIG_TIMPP
#include "timidity.h"
#else
#include "config.h"
#endif
#include "common.h"
#include "output.h"
#include "controls.h"
#ifdef ORIG_TIMPP
#include "timer.h"
#endif
#include "instrum.h"
#include "playmidi.h"
#ifdef ORIG_TIMPP
#include "miditrace.h"
#endif

#define BUFFERED_SECS 3
#define LOW_WATER_PERCENT 40

#ifdef LITTLE_ENDIAN
#define LINEAR16_FORMAT AuFormatLinearSigned16LSB
#else
#define LINEAR16_FORMAT AuFormatLinearSigned16MSB
#endif


#define dpm nas_play_mode

static int opendevaudio(void);
static void closedevaudio(void);
#ifdef ORIG_TIMPP
static int writedevaudio(char *buf, int32 len);
static int ioctldevaudio(int request, void *arg);
static int acntl(int request, void *arg);
#else
static void output_data(int32 *buf, uint32 count);
static int driver_output_data(unsigned char *buf, uint32 count);
static void flush_output(void);
static void purge_output(void);
static int output_count(uint32 ct);
#endif

#ifdef ORIG_TIMPP
PlayMode dpm=
{
  DEFAULT_RATE,
  PE_16BIT|PE_SIGNED,
  PF_PCM_STREAM|PF_CAN_TRACE,
  -1,
  {0},
  "Network Audio Server", 'n',
  (char*)0,
  opendevaudio,
  closedevaudio,
  writedevaudio,
  ioctldevaudio
};
#else
PlayMode dpm=
{
  DEFAULT_RATE,
  PE_16BIT|PE_SIGNED,
  -1,
  {0},
  "Network Audio Server", 'n',
  (char*)0,
  opendevaudio,
  closedevaudio,
  output_data,
  driver_output_data,
  flush_output,
  purge_output,
  output_count
};
#endif

struct DevAudio
{
  AuServer *aud;
  AuFlowID flow;
  char *data;
  AuUint32 used;
  AuUint32 capacity;
  AuBool synced;
  AuBool finished;
};

static struct DevAudio fd;
static int32 output_counter, play_counter, reset_samples;
static double play_start_time;

static void reset_sample_counters()
{
    output_counter = play_counter = reset_samples = 0;
}

#ifndef ORIG_TIMPP
static int output_count(uint32 ct)
{
    return play_counter;
}
#endif

static int32 current_samples(void)
{
#ifdef ORIG_TIMPP
    double realtime, es;

    realtime = get_current_calender_time();
    if(play_counter == 0)
    {
	play_start_time = realtime;
	return reset_samples;
    }
    es = dpm.rate * (realtime - play_start_time);
    if(es >= play_counter)
    {
	/* out of play counter */
	reset_samples += play_counter;
	play_counter = 0;
	play_start_time = realtime;
	return reset_samples;
    }
    if(es < 0)
	return 0; /* for safety */
    return (int32)es + reset_samples;
#else
    return 0;
#endif
}

static void play(struct DevAudio *fd, AuUint32 len)
{
  int count;

  count = len;
  if(!(dpm.encoding & PE_MONO))
      count /= 2;
  if(dpm.encoding & PE_16BIT)
      count /= 2;

  if (len<fd->used) 
  {
    AuWriteElement(fd->aud,fd->flow,0,len,fd->data,AuFalse,(AuStatus*)0);
    memmove(fd->data,fd->data+len,fd->used-=len);
  }
  else 
  {
    AuWriteElement(fd->aud,fd->flow,0,fd->used,fd->data,len!=fd->used,(AuStatus*)0);
    fd->used=0;
  }
  fd->synced=AuTrue;

  current_samples();	/* Update sample counter */
  play_counter += count;
}

static AuBool nas_eventHandler(AuServer *aud, AuEvent *ev, AuEventHandlerRec *handler)
{
  struct DevAudio *fd=(struct DevAudio*)handler->data;

  switch (ev->type)
  {
    case AuEventTypeMonitorNotify:
    {
      fd->finished=AuTrue;
      break;
    }
    case AuEventTypeElementNotify:
    {
      AuElementNotifyEvent *event=(AuElementNotifyEvent*)ev;

      switch (event->kind)
      {
        case AuElementNotifyKindLowWater:
        {
          play(fd,event->num_bytes);
          break;
        }
        case AuElementNotifyKindState: switch (event->cur_state)
        {
          case AuStatePause:
          {
            if (event->reason!=AuReasonUser) play(fd,event->num_bytes);
            break;
          }
          case AuStateStop:
          {
            fd->finished=AuTrue;
            break;
          }
        }
      }
    }
  }
  return AuTrue;
}

static void syncdevaudio(void)
{
  AuEvent ev;

  while ((!fd.synced) && (!fd.finished)) 
  {
    AuNextEvent(fd.aud,AuTrue,&ev);
    AuDispatchEvent(fd.aud,&ev);
  }
  fd.synced=AuFalse;
}

static int opendevaudio(void)
{
  AuDeviceID device=AuNone;
  AuElement elements[2];
  AuUint32 buf_samples;
  int i;

  /* Only supported PE_16BIT|PE_SIGNED yet */
#ifdef ORIG_TIMPP
  dpm.encoding = validate_encoding(dpm.encoding,
				   PE_16BIT|PE_SIGNED,
				   PE_BYTESWAP|PE_ULAW|PE_ALAW);
#else
  dpm.encoding = PE_16BIT|PE_SIGNED;
#endif
 
  if (!(fd.aud = AuOpenServer((char*)0, 0, NULL, 0, NULL, NULL))) return -1;
  fd.capacity = 0;

  for (i=0; i<AuServerNumDevices(fd.aud); ++i)
  {
    if
    (
      AuDeviceKind(AuServerDevice(fd.aud,i))==AuComponentKindPhysicalOutput
      && AuDeviceNumTracks(AuServerDevice(fd.aud,i))==((dpm.encoding&PE_MONO)?1:2)
    )
    {
      device=AuDeviceIdentifier(AuServerDevice(fd.aud,i));
      break;
    }
  }
  if (device==AuNone) return -1;
  if (!(fd.flow=AuCreateFlow(fd.aud,NULL))) return -1;
  buf_samples = dpm.rate*BUFFERED_SECS;
  AuMakeElementImportClient(&elements[0],dpm.rate,LINEAR16_FORMAT,(dpm.encoding&PE_MONO)?1:2,AuTrue,buf_samples,(AuUint32) (buf_samples/100*LOW_WATER_PERCENT),0,NULL);
  AuMakeElementExportDevice(&elements[1],0,device,dpm.rate,AuUnlimitedSamples,0,NULL);
  AuSetElements(fd.aud,fd.flow,AuTrue,2,elements,NULL);
  AuRegisterEventHandler(fd.aud,AuEventHandlerIDMask,0,fd.flow,nas_eventHandler,(AuPointer)&fd);
  fd.capacity = buf_samples*((dpm.encoding&PE_MONO)?1:2)*AuSizeofFormat(LINEAR16_FORMAT);
  if ((fd.data=(char*)malloc(fd.capacity))==(char*)0) return -1;
  fd.used = 0;
  fd.synced = AuFalse;
  fd.finished = AuFalse;
  AuStartFlow(fd.aud,fd.flow,NULL);
  dpm.fd = 0;
  reset_sample_counters();
  return 0;
}

static void closedevaudio(void)
{
  if(dpm.fd != -1)
  {
    AuStopFlow(fd.aud, fd.flow, NULL);
    while (!fd.finished) syncdevaudio();
    AuCloseServer(fd.aud);
    free(fd.data);
    dpm.fd = 0;
  }
}

#ifdef ORIG_TIMPP
static int writedevaudio(char *buf, int32 len)
#else
static int driver_output_data(unsigned char *buf, uint32 count)
#endif
{
  int done = 0;
#ifndef ORIG_TIMPP
  int len = (int)count;
#endif

/* fprintf(stderr,"DOD: write count=%d\n", count); */
  while ((fd.used+(len-done))>fd.capacity)
  {
    int stillFits=fd.capacity-fd.used;

    memcpy(fd.data+fd.used,buf+done,stillFits);
    done+=stillFits;
    fd.used+=stillFits;
/* fprintf(stderr,"DOD: cap=%d stillFits=%d done=%d\n", fd.capacity, stillFits, done); */
    syncdevaudio();
  }

  memcpy(fd.data+fd.used,buf+done,len-done);
  fd.used+=(len-done);
  output_counter += len;
/* fprintf(stderr,"DOD: return len=%d\n", len); */
  return len;
}

#ifndef ORIG_TIMPP
static void output_data(int32 *buf, uint32 count)
{
  int ocount;

  if (!(dpm.encoding & PE_MONO)) count*=2; /* Stereo samples */
  ocount = (int)count;

  if (ocount) {
    if (dpm.encoding & PE_16BIT)
      {
        /* Convert data to signed 16-bit PCM */
        s32tos16(buf, count);
        ocount *= 2;
      }
    else
      {
        /* Convert to 8-bit unsigned and write out. */
        s32tou8(buf, count);
      }
  }

  b_out(dpm.id_character, dpm.fd, (int *)buf, ocount);
}

static void flush_output(void)
{
  output_data(0, 0);
  reset_sample_counters();
}

static void purge_output(void)
{
  b_out(dpm.id_character, dpm.fd, 0, -1);
  AuStopFlow(fd.aud,fd.flow,NULL);
}
#endif


#ifdef ORIG_TIMPP
static int ioctldevaudio(int request, void *arg)
{
  switch (request)
  {
    case PM_REQ_DISCARD:
      if(!output_counter)
	  return 0;
      AuStopFlow(fd.aud,fd.flow,NULL);

      /* Restart playing is not work correctly.
       * I don't know what I should do, so I re-open audio server.
       */
#if 0
      AuStartFlow(fd.aud,fd.flow,NULL);
      reset_sample_counters();
      return 0;
#else
      AuCloseServer(fd.aud);
      free(fd.data);
      return opendevaudio();
#endif

    case PM_REQ_FLUSH:
      reset_sample_counters();
      return 0;

    case PM_REQ_GETSAMPLES:
      if(play_counter)
	  *(int *)arg = current_samples();
      else
	  *(int *)arg = reset_samples;
      return 0;

    case PM_REQ_PLAY_START:
      reset_sample_counters();
      return 0;
  }
  return -1;
}
#endif

#endif /* AU_NAS */
