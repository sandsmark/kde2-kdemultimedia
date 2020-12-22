    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

/*===========================================================================*

  Experimental aRts video support stuff:

  This video support stuff will eventually go to kdelibs/arts if it works.
  There are a two things that are important for that reason

    - keep LGPL licensing
    - do *not* use Qt in Videocodecs, VideoFrame class, ...

  Besides this, here the current state of the art. mcopidl supports now, with
  a hack, to define custom packet types. So you can define streams of
  AVideoFrames with some flags (-P AVideoFrame defines a new custom packet
  type for asynchronous streaming and -C "avideoframe.h" tells mcopidl to
  include this header file, so that the classes are defined).

  So what games can we play with this:
   - implement more of the AVideoFrame class
   - implement some of the interfaces specified in the artsvideo.idl file
   - connect stuff together, and use it using connect() and start()
   - try to find an optimal rendering strategy
   - write the marshalling functions and laugh how slooow it is over network ;)

  Issues that need work (but are not even touched yet):
   - threading (people at least told me that they want threads for video)
   - trading (how to find a codec that supports mimetype xyz)
   - tighter communication between native classes and mcop typed stuff
     (i.e. how do I tell my renderer a QWidget to render to)
   - stream format (should the format be more than just "VideoFrame"?
     should it be possible to "see" before connecting which resolution
	 and which colorspace a stream is using)
   - using shared memory for efficient IPC when running locally
   - audio, audio/video synchronization
  
 *===========================================================================*/

#include "asyncstream.h"
#include "datapacket.h"

class AVideoFrame
{
public:
	/* put video data, format, colorspace and similar for one frame in here */
	unsigned int width;
	unsigned int height;
	
	unsigned long length;
	char *data;
};

class AVideoFrameAsyncStream : public Arts::AsyncStream<AVideoFrame>
{
public:
	Arts::DataPacket<AVideoFrame> *allocPacket(int capacity);
	Arts::GenericAsyncStream *createNewStream();
};                                                                              


class AVideoFrameDataPacket : public Arts::DataPacket<AVideoFrame> {
public:
    AVideoFrameDataPacket(int capacity, Arts::GenericDataChannel *channel)
            : Arts::DataPacket<AVideoFrame>(capacity, channel)
    {
        //
    }
    void read(Arts::Buffer& stream);
    void write(Arts::Buffer& stream);
};                                                                              
