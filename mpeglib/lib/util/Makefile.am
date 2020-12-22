#  player - Makefile.am

SUBDIRS			=	abstract audio file render


INCLUDES                =       $(all_includes)

noinst_LTLIBRARIES      =	libutil.la

libdir                  =       ${exec_prefix}/lib/$(THIS_LIB_NAME)/util

noinst_HEADERS		=	mmx.h syncClockMPEG.h
			
kmpgincludedir          =       $(includedir)/$(THIS_LIB_NAME)/util

kmpginclude_HEADERS     =       timeStamp.h dynBuffer.h \
				timeStampArray.h syncClock.h timeWrapper.h



libutil_la_SOURCES	=	mmx.c timeStamp.cpp \
				timeStampArray.cpp \
				dynBuffer.cpp syncClock.cpp \
				syncClockMPEG.cpp timeWrapper.cpp \
				mmxflags_asm.S





























