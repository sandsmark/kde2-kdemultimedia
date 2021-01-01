## $Id: Makefile.am 249376 2003-09-08 11:29:43Z lukas $
## Makefile.am of kdemultimedia/tools/thumbnail

INCLUDES = -I$(kde_includes)/arts $(all_includes)

AM_LDFLAGS = $(all_libraries) $(KDE_RPATH)
METASOURCES = AUTO

kde_module_LTLIBRARIES = videothumbnail.la

videothumbnail_la_SOURCES = videocreator.cpp
videothumbnail_la_LIBADD = $(LIB_KDECORE) -lqtmcop -lkmedia2_idl -lsoundserver_idl -lartskde
videothumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

noinst_HEADERS = videocreator.h

data_DATA = sprocket-small.png sprocket-medium.png sprocket-large.png

datadir = $(kde_datadir)/videothumbnail

services_DATA = videothumbnail.desktop

servicesdir = $(kde_servicesdir)
