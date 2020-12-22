INCLUDES = -I$(kde_includes)/kio -I$(top_srcdir)/noatun/noatun/library -I$(kde_includes)/arts $(all_includes)

SUBDIRS = pics library modules  .

KDE_ICON = AUTO

lib_LTLIBRARIES = libnoatunmain.la
libnoatunmain_la_SOURCES = main.cpp joinhandler.cpp joinhandler.skel
libnoatunmain_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -module -avoid-version
libnoatunmain_la_LIBADD = -lqtmcop -lkmedia2_idl -lsoundserver_idl $(top_builddir)/noatun/noatun/library/libnoatun.la

bin_PROGRAMS = noatun
noatun_SOURCES = dummy.cpp
noatun_LDFLAGS = $(all_libraries) $(KDE_RPATH)
noatun_LDADD = $(LIB_KFILE) libnoatunmain.la

METASOURCES = AUTO

CLEANFILES = dummy.cpp

dummy.cpp:
	echo > dummy.cpp

applnk_DATA = noatun.desktop
applnkdir = $(kde_appsdir)/Multimedia

presetsdir = $(kde_datadir)/noatun/eq.preset
presets_DATA = preset.dance preset.jazz preset.metal preset.trance preset.zero
