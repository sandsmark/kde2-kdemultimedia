INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatunsystray.la

libnoatunsystray_la_SOURCES = systray.cpp noatunui.cpp kitsystemtray.cpp cmodule.cpp

libnoatunsystray_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunsystray_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

libnoatunsystray_la_METASOURCES = AUTO

noinst_HEADERS = systray.h kitsystemtray.h cmodule.h

data_DATA = systray.plugin systrayui.rc
datadir = $(kde_datadir)/noatun

noatuniconsdir = $(kde_datadir)/noatun/icons
noatunicons_ICON = AUTO

