INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libliszt.la

libliszt_la_SOURCES =  \
Static.cpp \
Track.cpp \
CharlatanItem.cpp \
ArtistItem.cpp \
AlbumItem.cpp \
TrackItem.cpp \
PlayLiszt.cpp \
EditWidget.cpp \
PreviewItem.cpp \
PreviewWidget.cpp \
ScoreWidget.cpp \
Liszt.cpp

libliszt_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libliszt_la_LIBADD = \
  $(top_builddir)/noatun/noatun/library/libnoatun.la \
  $(LIB_KFILE) -lqtmcop -lkmedia2_idl -lsoundserver_idl

libliszt_la_METASOURCES = AUTO

noinst_HEADERS = \
AlbumItem.h \
ArtistItem.h \
CharlatanItem.h \
PlayLiszt.h \
EditWidget.h \
PreviewItem.h \
PreviewWidget.h \
ScoreWidget.h \
Static.h \
Track.h \
TrackItem.h \
pixmaps.h \
Liszt.h

data_DATA = liszt.plugin
datadir = $(kde_datadir)/noatun
