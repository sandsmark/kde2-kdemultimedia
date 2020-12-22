INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libsplitplaylist.la

libsplitplaylist_la_SOURCES = splitplaylist.cpp playlist.cpp view.cpp find.cpp

libsplitplaylist_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libsplitplaylist_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la \
                             -lkfile -lkio

libsplitplaylist_la_METASOURCES = AUTO

noinst_HEADERS = playlist.h view.h find.h

data_DATA = splitplaylist.plugin
datadir = $(kde_datadir)/noatun
