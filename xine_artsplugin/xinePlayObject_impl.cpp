/*
   This file is part of KDE/aRts (Noatun) - xine integration
   Copyright (C) 2002-2003 Ewald Snel <ewald@rambo.its.tudelft.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <sys/time.h>
#include <audiosubsys.h>
#include <convert.h>
#include <debug.h>

#include "xinePlayObject_impl.h"

#ifndef HAVE_XSHMGETEVENTBASE
extern "C" {
extern int XShmGetEventBase( Display* );
};
#endif

#define TIMEOUT		15	// 15 seconds

#ifndef HAVE_XINE_1
#define xine_open_video_driver(xine, id, type, visual) \
	xine_load_video_output_plugin(config, (char *)id, type, visual)
#define xine_gui_send_vo_data(stream, type, data) \
	vo_port->gui_data_exchange(vo_port, type, data)
#define xine_event_create_listener_thread(queue, evtcb, obj) \
	xine_register_event_listener(queue, evtcb, obj)
#define xine_event_new_queue(stream) \
	stream
#define xine_event_dispose_queue(queue) \
	xine_remove_event_listener(queue, xine_handle_event)

#define XINE_STATUS_PLAY		XINE_PLAY
#define XINE_EVENT_UI_PLAYBACK_FINISHED	XINE_EVENT_PLAYBACK_FINISHED
#define XINE_VISUAL_TYPE_X11		VISUAL_TYPE_X11
#define XINE_GUI_SEND_COMPLETION_EVENT	GUI_DATA_EX_COMPLETION_EVENT
#define XINE_GUI_SEND_DRAWABLE_CHANGED	GUI_DATA_EX_DRAWABLE_CHANGED
#define XINE_GUI_SEND_EXPOSE_EVENT	GUI_DATA_EX_EXPOSE_EVENT
#endif

using namespace Arts;


#ifdef HAVE_XINE_1
// Global xine pointer
static xine_t		*xine_shared	 = NULL;
#endif
static pthread_mutex_t	 xine_mutex	 = PTHREAD_MUTEX_INITIALIZER;
#ifdef HAVE_XINE_1
static pthread_cond_t	 xine_cond	 = PTHREAD_COND_INITIALIZER;
static int		 xineRefCount	 = 0;
#endif
static bool		 xineForceXShm	 = false;

#ifdef HAVE_XINE_1
static void xine_init_routine()
{
    const char *id;
    char cfgFileName[272];

    xine_shared = (xine_t *)xine_new();

    snprintf( cfgFileName, 272, "%s/.xine/config", getenv( "HOME" ) );

    xine_config_load( xine_shared, (const char *)cfgFileName );

    // Check default video output driver
    id = xine_config_register_string (xine_shared, "video.driver",
				      "auto", "video driver to use",
				      NULL, 10, NULL, NULL);

    xineForceXShm = (id && !strcasecmp( id, "XShm" ));

    xine_init( xine_shared );
}

static void *xine_timeout_routine( void * )
{
    pthread_mutex_lock( &xine_mutex );

    while (xine_shared != 0)
    {
	if (xineRefCount == 0)
	{
	    struct timespec ts;
	    struct timeval tv;

	    gettimeofday( &tv, 0 );

	    ts.tv_sec	= tv.tv_sec;
	    ts.tv_nsec	= tv.tv_usec * 1000;
	    ts.tv_sec  += TIMEOUT;

	    if (pthread_cond_timedwait( &xine_cond, &xine_mutex, &ts ) != 0 &&
		xineRefCount == 0)
	    {
		xine_exit( xine_shared );
		xine_shared = NULL;
		break;
	    }
	}
	else
	{
	    pthread_cond_wait( &xine_cond, &xine_mutex );
	}
    }
    pthread_mutex_unlock( &xine_mutex );

    return NULL;
}

static xine_t *xine_shared_init()
{
    pthread_mutex_lock( &xine_mutex );

    ++xineRefCount;

    if (xine_shared == 0)
    {
	pthread_t thread;

	xine_init_routine();

	if (pthread_create( &thread, NULL, xine_timeout_routine, NULL ) == 0)
	{
	    pthread_detach( thread );
	}
    }
    else
    {
	pthread_cond_signal( &xine_cond );
    }
    pthread_mutex_unlock( &xine_mutex );

    return xine_shared;
}

static void xine_shared_exit( xine_t * )
{
    pthread_mutex_lock( &xine_mutex );

    if (--xineRefCount == 0)
    {
	pthread_cond_signal( &xine_cond );
    }
    pthread_mutex_unlock( &xine_mutex );
}
#endif

int ao_fifo_arts_delay()
{
    return (int)(1000 * Arts::AudioSubSystem::the()->outputDelay());
}

xinePlayObject_impl::xinePlayObject_impl()
    : mrl( "" ), xine( 0 ), stream( 0 ), queue( 0 ), ao_port( 0 ), vo_port( 0 )
{
    XInitThreads();

    if (!(display = XOpenDisplay( NULL )))
    {
	arts_fatal( "could not open X11 display" );
    }

    XFlush( display );

    // Create a special window for uninterrupted X11 communication
    xcomWindow = XCreateSimpleWindow( display, DefaultRootWindow( display ),
				       0, 0, 1, 1, 0, 0, 0 );

    XSelectInput( display, xcomWindow, ExposureMask );

    pthread_mutex_init( &mutex, 0 );

    // Initialize X11 properties
    xcomAtomQuit	   = XInternAtom( display, "VPO_INTERNAL_EVENT", False );
    xcomAtomResize	   = XInternAtom( display, "VPO_RESIZE_NOTIFY", False );
    screen		   = DefaultScreen( display );
    shmCompletionType	   = (XShmQueryExtension( display ) == True)
			     ? XShmGetEventBase( display ) + ShmCompletion : -1;

    width		   = 0;
    height		   = 0;
    dscbTimeOut		   = 0;

    // Initialize xine visual structure
    visual.display	   = display;
    visual.screen	   = screen;
    visual.d		   = xcomWindow;
    visual.dest_size_cb	   = &dest_size_cb;
    visual.frame_output_cb = &frame_output_cb;
    visual.user_data	   = this;
#ifndef HAVE_XINE_1
    visual.display_ratio   = 1.0;

    const char *default_driver;
    string config_file = string( getenv( "HOME" )).append( "/.xine/config" );

    config = xine_config_file_init( (char *)config_file.c_str() );

    // Disable OSD
    config->register_bool( config, (char *)"misc.osd_display", 0,
			   (char *)"disabled for KDE", 0, 0, 0 );
    config->update_num( config, (char *)"misc.osd_display", 0 );

    // Disable xine logo
    config->register_string( config, (char *)"misc.logo_mrl", (char *)"",
			     (char *)"disabled for KDE", 0, 0, 0 );
    config->update_string( config, (char *)"misc.logo_mrl", (char *)"" );

    // Get default video output driver
    default_driver = config->register_string( config,
					      (char *)"video.driver",
					      (char *)"auto",
					      (char *)"video driver to use",
					      NULL, NULL, NULL );

    pthread_mutex_lock( &xine_mutex );

    xineForceXShm = (default_driver && !strcasecmp( default_driver, "XShm" ));

    pthread_mutex_unlock( &xine_mutex );
#endif

    // Initialize audio and video details
    Arts::SoundServerV2 server = Arts::Reference( "global:Arts_SoundServerV2" );
    audio.sample_rate	   = 0;
    audio.num_channels	   = 0;
    audio.bits_per_sample  = 0;

    flpos		   = 0.0;

    if (pthread_create( &thread, 0, pthread_start_routine, this ))
    {
	arts_fatal( "could not create thread" );
    }
}

xinePlayObject_impl::~xinePlayObject_impl()
{
    XEvent event;

    halt();

    // Send stop event to thread (X11 client message)
    memset( &event, 0, sizeof(event) );

    event.type			= ClientMessage;
    event.xclient.window	= xcomWindow;
    event.xclient.message_type	= xcomAtomQuit;
    event.xclient.format	= 32;

    XSendEvent( display, xcomWindow, True, 0, &event );

    XFlush( display );

    // Wait for the thread to die
    pthread_join( thread, 0 );

    // Destroy stream, xine and related resources
    if (stream != 0)
    {
	halt();

	xine_event_dispose_queue( queue );
#ifdef HAVE_XINE_1
	xine_dispose( stream );
	xine_close_audio_driver( xine, ao_port );
	xine_close_video_driver( xine, vo_port );
#endif
    }
    if (xine != 0)
    {
#ifdef HAVE_XINE_1
	xine_shared_exit( xine );
#else
	xine_exit( xine );
#endif
    }
#ifndef HAVE_XINE_1
    config->dispose( config );
#endif

    pthread_mutex_destroy( &mutex );

    XSync( display, False );
    XDestroyWindow( display, xcomWindow );
    XCloseDisplay( display );
}

bool xinePlayObject_impl::loadMedia( const string &url )
{
    bool result = false;

    pthread_mutex_lock( &mutex );

    mrl = "";

    if (stream == 0)
    {
#ifdef HAVE_XINE_1
	if (xine == 0)
	{
	    xine = xine_shared_init();
	}
#endif

	ao_port = init_audio_out_plugin( xine, &audio, &ao_driver );

	if (xineForceXShm)
	{
	    vo_port = xine_open_video_driver( xine, "XShm",
					      XINE_VISUAL_TYPE_X11,
					      (void *)&visual );
	}
	if (vo_port == 0)
	{
	    vo_port = xine_open_video_driver( xine, "Xv",
					      XINE_VISUAL_TYPE_X11,
					      (void *)&visual );
	}
	if (vo_port == 0)
	{
	    vo_port = xine_open_video_driver( xine, "XShm",
					      XINE_VISUAL_TYPE_X11,
					      (void *)&visual );
	}
	if (vo_port == 0)
	{
	    vo_port = xine_open_video_driver( xine, "OpenGL",
					      XINE_VISUAL_TYPE_X11,
					      (void *)&visual );
	}
	if (ao_port != 0 && vo_port != 0)
	{
#ifdef HAVE_XINE_1
	    stream = xine_stream_new( xine, ao_port, vo_port );
#else
	    stream = xine = xine_init( vo_port, ao_port, config );
#endif

	    if (stream != 0)
	    {
#ifdef HAVE_XINE_1
		xine_set_param( stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, 0 );
		xine_set_param( stream, XINE_PARAM_SPU_CHANNEL, -1 );
#else
		xine_select_audio_channel( xine, 0 );
		xine_select_spu_channel( xine, -1 );
#endif

		queue = xine_event_new_queue( stream );
		xine_event_create_listener_thread( queue, xine_handle_event, this );
	    }
	}
#ifdef HAVE_XINE_1
	if (stream == 0)
	{
	    if (ao_port != 0)
	    {
		xine_close_audio_driver( xine, ao_port );
		ao_port = 0;
	    }
	    if (vo_port != 0)
	    {
		xine_close_video_driver( xine, vo_port );
		vo_port = 0;
	    }
	}
#endif
    }

    if (stream != 0)
    {
	if (xine_get_status( stream ) == XINE_STATUS_PLAY)
	{
	    ao_fifo_clear( ao_driver, 2 );

	    xine_stop( stream );

	    clearWindow();
	}
#ifdef HAVE_XINE_1
	if ((result = xine_open( stream, url.c_str() )))
#endif
	{
	    mrl = url;
	}

	streamLength = 0;
	streamPosition = 0;

	width = 0;
	height = 0;
#ifndef HAVE_XINE_1
	result = true;
#endif
    }

    pthread_mutex_unlock( &mutex );

    return result;
}

string xinePlayObject_impl::description()
{
    return "xine aRts plugin";
}

poTime xinePlayObject_impl::currentTime()
{
    poTime time;
    int pos_time;

    pthread_mutex_lock( &mutex );

    if (stream != 0 && mrl != "")
    {
#ifdef HAVE_XINE_1
	if (xine_get_pos_length( stream, 0, &pos_time, 0 ))
	{
	    streamPosition = pos_time;
	}
	else
	{
	    pos_time = streamPosition;
	}
#else
	pos_time = (1000 * xine_get_current_time( xine ));
#endif

	time.seconds = pos_time / 1000;
	time.ms = pos_time % 1000;
    }
    else
    {
	time.seconds = 0;
	time.ms = 0;
    }
    pthread_mutex_unlock( &mutex );

    return time;
}

poTime xinePlayObject_impl::overallTime()
{
    poTime time;
    int length_time;

    pthread_mutex_lock( &mutex );

    if (stream != 0 && mrl != "")
    {
#ifdef HAVE_XINE_1
	if (xine_get_pos_length( stream, 0, 0, &length_time ))
	{
	    streamLength = length_time;
	}
	else
	{
	    length_time = streamLength;
	}
#else
	length_time = (1000 * xine_get_stream_length( xine ));
#endif

	if (length_time <= 0)
	{
	    length_time = 1;
	}

	time.seconds = length_time / 1000;
	time.ms = length_time % 1000;
    }
    else
    {
	time.seconds = 0;
	time.ms = 1;
    }
    pthread_mutex_unlock( &mutex );

    return time;
}

poCapabilities xinePlayObject_impl::capabilities()
{
    int n;

    pthread_mutex_lock( &mutex );

#ifdef HAVE_XINE_1
    n = (stream == 0) ? 0 : xine_get_stream_info( stream, XINE_STREAM_INFO_SEEKABLE );
#else
    n = 1;
#endif

    pthread_mutex_unlock( &mutex );

    return static_cast<poCapabilities>( capPause | ((n == 0) ? 0 : capSeek) );
}

string xinePlayObject_impl::mediaName()
{
    return mrl;
}

poState xinePlayObject_impl::state()
{
    poState state;

    pthread_mutex_lock( &mutex );

    if (stream == 0 || xine_get_status( stream ) != XINE_STATUS_PLAY)
	state = posIdle;
#ifdef HAVE_XINE_1
    else if (xine_get_param( stream, XINE_PARAM_SPEED ) == XINE_SPEED_PAUSE)
#else
    else if (xine_get_speed( xine ) == SPEED_PAUSE)
#endif
	state = posPaused;
    else
	state = posPlaying;

    pthread_mutex_unlock( &mutex );

    return state;
}

long xinePlayObject_impl::x11Snapshot()
{
    long pixmap = -1;

    pthread_mutex_lock( &mutex );

    if (stream != 0 && xine_get_status( stream ) == XINE_STATUS_PLAY)
    {
	// FIXME: snapshot...
	pixmap = (long)-1;
    }
    pthread_mutex_unlock( &mutex );

    return pixmap;
}

void xinePlayObject_impl::x11WindowId( long window )
{
    pthread_mutex_lock( &mutex );

    if (window == -1)
    {
	window = xcomWindow;
    }

    if ((Window)window != visual.d)
    {
	XLockDisplay( display );

	// Change window and set event mask of new window
	visual.d = window;

	XSelectInput( display, window, ExposureMask );

	if (stream != 0)
	{
	    resizeNotify();

	    xine_gui_send_vo_data( stream,
				   XINE_GUI_SEND_DRAWABLE_CHANGED,
				   (void *)window );
	}

	XUnlockDisplay( display );
    }
    pthread_mutex_unlock( &mutex );
}

long xinePlayObject_impl::x11WindowId()
{
    return (visual.d == xcomWindow) ? (long)-1 : visual.d;
}

void xinePlayObject_impl::play()
{
    pthread_mutex_lock( &mutex );

    if (stream != 0)
    {
	if (xine_get_status( stream ) == XINE_STATUS_PLAY)
	{
#ifdef HAVE_XINE_1
	    if (xine_get_param( stream, XINE_PARAM_SPEED ) == XINE_SPEED_PAUSE)
#else
	    if (xine_get_speed( xine ) == SPEED_PAUSE)
#endif
	    {
#ifdef HAVE_XINE_1
	        xine_set_param( stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL );
#else
		xine_set_speed( xine, SPEED_NORMAL );
#endif
	    }
	}
	else if (mrl != "")
	{
#ifdef HAVE_XINE_1
	    xine_play( stream, 0, 0 );
#else
	    xine_play( xine, (char *)mrl.c_str(), 0, 0 );
#endif
	}
    }
    pthread_mutex_unlock( &mutex );
}

void xinePlayObject_impl::halt()
{
    pthread_mutex_lock( &mutex );

    if (stream != 0 && xine_get_status( stream ) == XINE_STATUS_PLAY)
    {
	ao_fifo_clear( ao_driver, 2 );

	xine_stop( stream );

	clearWindow();

	streamLength = 0;
	streamPosition = 0;
    }
    pthread_mutex_unlock( &mutex );
}

void xinePlayObject_impl::seek( const class poTime &t )
{
    pthread_mutex_lock( &mutex );

    if (stream != 0 && xine_get_status( stream ) == XINE_STATUS_PLAY)
    {
	int seekPosition = (1000 * t.seconds) + t.ms;
#ifdef HAVE_XINE_1
	int paused = (xine_get_param( stream, XINE_PARAM_SPEED ) == XINE_SPEED_PAUSE);
#else
	int paused = (xine_get_speed( xine ) == SPEED_PAUSE);
#endif

	ao_fifo_clear( ao_driver, 1 );

#ifdef HAVE_XINE_1
	if (xine_play( stream, 0, seekPosition ))
#else
	if (xine_play( xine, (char *)mrl.c_str(), 0, (seekPosition / 1000) ))
#endif
	{
	    if (seekPosition >= 0 && seekPosition <= streamLength)
	    {
		streamPosition = seekPosition;
	    }
	}

	if (paused)
	{
#ifdef HAVE_XINE_1
	    xine_set_param( stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );
#else
	    xine_set_speed( xine, SPEED_PAUSE );
#endif
	}

	ao_fifo_clear( ao_driver, 0 );
    }
    pthread_mutex_unlock( &mutex );
}

void xinePlayObject_impl::pause()
{
    pthread_mutex_lock( &mutex );

    if (stream != 0 && xine_get_status( stream ) == XINE_STATUS_PLAY)
    {
	ao_fifo_clear( ao_driver, 1 );

#ifdef HAVE_XINE_1
	xine_set_param( stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );
#else
	xine_set_speed( xine, SPEED_PAUSE );
#endif
    }
    pthread_mutex_unlock( &mutex );
}

void xinePlayObject_impl::calculateBlock( unsigned long samples )
{
    unsigned int skip, received = 0, converted = 0, xSamples = 0;
    unsigned char *buffer;
    double speed = 1.0;

    pthread_mutex_lock( &mutex );

    if (stream != 0)
    {
	// Calculate resampling parameters
	speed = (double)audio.sample_rate / samplingRateFloat;
	xSamples = (unsigned int)((double)samples * speed + 8.0);
	received = ao_fifo_read( ao_driver, &buffer, xSamples );
    }

    pthread_mutex_unlock( &mutex );

    // Convert samples and fill gaps with zeroes
    if (received)
    {
	converted = uni_convert_stereo_2float( samples, buffer, received,
					       audio.num_channels,
					       audio.bits_per_sample,
					       left, right, speed, flpos );
	flpos += (double)converted * speed;
	skip   = (int)floor( flpos );
	skip   = (received < (xSamples - 8)) ? (xSamples - 8) : skip;
	flpos  = flpos - floor( flpos );
	ao_fifo_flush( ao_driver, skip );
    }
    for (unsigned long i=converted; i < samples; i++)
    {
	left[i] = 0;
	right[i] = 0;
    }
}

void xinePlayObject_impl::xineEvent( const xine_event_t &event )
{
    if (event.type == XINE_EVENT_UI_PLAYBACK_FINISHED)
    {
	clearWindow();
    }
}

void xinePlayObject_impl::clearWindow()
{
    Window root;
    unsigned int u, w, h;
    int x, y, screen;

    XLockDisplay( display );

    screen = DefaultScreen( display );

    XGetGeometry( display, visual.d, &root, &x, &y, &w, &h, &u, &u );

    XSetForeground( display, DefaultGC( display, screen ),
		    BlackPixel( display, screen ) );
    XFillRectangle( display, visual.d,
		    DefaultGC( display, screen ), x, y, w, h );

    XUnlockDisplay( display );
}

void xinePlayObject_impl::frameOutput( int &x, int &y,
				       int &width, int &height, double &ratio,
				       int displayWidth, int displayHeight,
				       double displayPixelAspect, bool dscb )
{
    Window child, root;
    unsigned int u;
    int n;

    XLockDisplay( display );

    XGetGeometry( display, visual.d, &root, &n, &n,
		  (unsigned int *)&width, (unsigned int *)&height, &u, &u );

    if (!dscb)
    {
	XTranslateCoordinates( display, visual.d, root, 0, 0, &x, &y, &child );
    }

    // Most displays use (nearly) square pixels
    ratio = 1.0;

    // Correct for display pixel aspect
    if (displayPixelAspect < 1.0)
    {
	displayHeight = (int)((displayHeight / displayPixelAspect) + .5);
    }
    else
    {
	displayWidth  = (int)((displayWidth  * displayPixelAspect) + .5);
    }

    if (dscb || dscbTimeOut == 0 || --dscbTimeOut == 0)
    {
	// Notify client of new display size
	if (displayWidth != this->width || displayHeight != this->height)
	{
	    this->width  = displayWidth;
	    this->height = displayHeight;

	    resizeNotify();
	}

	// Reset 'seen dest_size_cb' time out
	if (dscb)
	{
	    dscbTimeOut = 25;
	}
    }
    XUnlockDisplay( display );
}

void xinePlayObject_impl::resizeNotify()
{
    XEvent event;

    // Resize notify signal for front-ends
    memset( &event, 0, sizeof(event) );

    event.type                  = ClientMessage;
    event.xclient.window        = visual.d;
    event.xclient.message_type  = xcomAtomResize;
    event.xclient.format        = 32;
    event.xclient.data.l[0]     = width;
    event.xclient.data.l[1]     = height;

    XSendEvent( display, visual.d, True, 0, &event );

    XFlush( display );
}

void xinePlayObject_impl::eventLoop()
{
    XEvent event;

    do
    {
	XNextEvent( display, &event );

	if (event.type == Expose && event.xexpose.count == 0 &&
	    event.xexpose.window == visual.d)
	{
	    if (stream != 0)
	    {
		xine_gui_send_vo_data( stream,
				       XINE_GUI_SEND_EXPOSE_EVENT,
				       &event );
	    }
	    else
	    {
		clearWindow();
	    }
	}
	else if (event.type == shmCompletionType && stream != 0)
	{
	    xine_gui_send_vo_data( stream,
				   XINE_GUI_SEND_COMPLETION_EVENT,
				   &event );
	}
    }
    while (event.type != ClientMessage ||
	   event.xclient.message_type != xcomAtomQuit ||
	   event.xclient.window != xcomWindow);
}

REGISTER_IMPLEMENTATION(xinePlayObject_impl);
