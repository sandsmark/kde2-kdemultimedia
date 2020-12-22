#  player - Makefile.am
	
SUBDIRS				=	dither dither2YUV x11 sdl

INCLUDES                	=       $(all_includes)


noinst_LTLIBRARIES		=	libutilrender.la

libdir                  	=       ${exec_prefix}/lib/$(THIS_LIB_NAME)/util

noinst_HEADERS			=	imageBase.h \
					renderMachine.h surface.h 

			
kmpgincludedir                  =       $(includedir)/$(THIS_LIB_NAME)/util/render

kmpginclude_HEADERS             =       yuvPicture.h pictureArray.h



libutilrender_la_SOURCES	=	yuvPicture.cpp imageBase.cpp \
					renderMachine.cpp surface.cpp \
					pictureArray.cpp

































