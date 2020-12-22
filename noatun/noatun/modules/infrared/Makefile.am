
INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatuninfrared.la

libnoatuninfrared_la_SOURCES = infrared.cpp lirc.cpp irprefs.cpp
noinst_HEADERS = infrared.h lirc.h irprefs.h

libnoatuninfrared_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatuninfrared_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

libnoatuninfrared_la_METASOURCES = AUTO


data_DATA = infrared.plugin
datadir = $(kde_datadir)/noatun
