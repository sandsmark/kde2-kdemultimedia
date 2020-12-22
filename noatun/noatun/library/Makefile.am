INCLUDES = -I$(top_srcdir)/noatun/noatun/library \
	-I$(top_srcdir)/arts/gui/kde -I$(top_builddir)/arts/gui/kde \
	-I$(top_srcdir)/arts/gui/common -I$(top_builddir)/arts/gui/common \
	-I$(kde_includes)/kio -I$(kde_includes)/arts \
	-I$(top_srcdir)/noatun/noatun/library/noatunarts \
	-I$(top_builddir)/noatun/noatun/library/noatunarts \
	 $(all_includes)

SUBDIRS=noatunarts .

lib_LTLIBRARIES = libnoatun.la libnoatuncontrols.la

libnoatun_la_SOURCES = pluginmodule.cpp cmodule.cpp downloader.cpp engine.cpp \
	noatunplaylist.cpp noatunpref.cpp \
	player.cpp playlistsaver.cpp noatunapp.cpp \
	pluginloader.cpp plugin.cpp \
	noatunstdaction.cpp conversion.cpp\
	noatunui.cpp effectview.cpp \
	equalizerview.cpp \
	equalizer.cpp \
	effects.cpp mimetypetree.cpp stereobuttonaction.cpp

noatuninclude_HEADERS= noatunplaylist.h player.h noatunapp.h \
	noatunpref.h plugin.h pluginloader.h effects.h \
	downloader.h engine.h controls.h \
	equalizer.h noatunstdaction.h conversion.h

noatunincludedir = $(includedir)/noatun

include_HEADERS  = playlistsaver.h cmodule.h plugin_deps.h equalizerview.h effectview.h mimetypetree.h

libnoatun_la_METASOURCES = AUTO
libnoatun_la_LDFLAGS     = -L$(top_builddir)/noatun/noatun/library/noatunarts \
$(KDE_RPATH) $(all_libraries) -L$(top_builddir)/arts/gui/common \
-L$(top_builddir)/arts/gui/kde
	
libnoatun_la_LIBADD      = $(top_builddir)/arts/gui/common/libartsgui.la	\
	$(top_builddir)/arts/gui/kde/libartsgui_kde.la			\
	-lkio -lqtmcop -lkmedia2_idl -lnoatunarts			\
	-lartsflow -lsoundserver_idl -lartskde $(LIBDL)

libnoatuncontrols_la_SOURCES = controls.cpp
libnoatuncontrols_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
libnoatuncontrols_la_LIBADD  = $(LIBDL)

