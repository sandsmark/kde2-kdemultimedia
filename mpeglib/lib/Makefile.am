#  player - Makefile.am

SUBDIRS		=	frame util input output decoder \
			splay oggvorbis mpegplay mpgplayer tplay yuv

MMXFILES	=	


EXTRA_DIST	=	README


THIS_EXTRALIBS	=	mpgplayer/*.lo \
			mpegplay/*.lo \
			splay/*.lo \
			oggvorbis/*.lo \
			tplay/*.lo \
			decoder/*.lo \
			output/*.lo \
			input/*.lo \
			util/render/*.lo \
			$(DISPLAY_VIDEO_DIR) \
			util/render/dither/*.lo \
			util/render/dither2YUV/*.lo \
			util/audio/*.lo \
			util/file/*.lo \
			frame/*.lo \
			util/*.lo \
			yuv/*.lo \
			util/abstract/*.lo


lib_LTLIBRARIES		=	libmpeg.la

libmpeg_la_SOURCES	=	dummy.cpp


libmpeg_la_LDFLAGS	=	$(all_libraries) \
				-module \
				-release $(THIS_LIB_VERSION) \
				-version-info $(THIS_LIB_MAJOR_VERSION):$(THIS_LIB_MINOR_VERSION):$(THIS_LIB_MICRO_VERSION)


libmpeg_la_LIBADD	=	$(THIS_EXTRALIBS) $(MMXFILES) \
				$(THIS_LIB_LIBS)
