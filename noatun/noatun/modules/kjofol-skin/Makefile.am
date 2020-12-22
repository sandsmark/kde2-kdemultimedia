INCLUDES= -I$(top_srcdir)/noatun/noatun/library -I$(kde_includes)/kio $(all_includes)
lib_LTLIBRARIES = libkjofolui.la

SUBDIRS= . skins

libkjofolui_la_SOURCES = kjloader.cpp noatunui.cpp parser.cpp kjprefs.cpp

libkjofolui_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkjofolui_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la \
                             -lkfile -lkio -lm

libkjofolui_la_METASOURCES = AUTO

noinst_HEADERS = kjprefs.h kjloader.h parser.h

script_DATA = noatun_kjofol_fixskin.sh
scriptdir = $(kde_bindir)

data_DATA = kjofolui.plugin
datadir = $(kde_datadir)/noatun

