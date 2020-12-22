SUBDIRS = mcopclass

lib_LTLIBRARIES = libartsgui_kde.la

check_PROGRAMS = artstestgui

INCLUDES= -I$(top_builddir)/arts/runtime -I$(top_builddir)/arts/gui/common -I$(arts_includes) $(all_includes)

libartsgui_kde_la_SOURCES = kwidget_impl.cpp kbutton_impl.cpp \
  kpoti_impl.cpp kpoti.cpp kwidgetrepo.cpp kartswidget.cpp \
  klineedit_impl.cpp khbox_impl.cpp kvbox_impl.cpp kspinbox_impl.cpp

libartsgui_kde_la_LIBADD = $(LIB_KFILE) -lkdecore -lkdeui \
  $(top_builddir)/arts/gui/common/libartsgui_idl.la
libartsgui_kde_la_LDFLAGS = $(all_libraries) -no-undefined -module

artstestgui_SOURCES = artstestgui.cpp
artstestgui_LDADD = -lqtmcop -lkdecore -lartsflow \
  $(top_builddir)/arts/gui/common/libartsgui_idl.la
artstestgui_LDFLAGS	= $(all_libraries) $(KDE_RPATH)

libartsgui_kde_la_METASOURCES = AUTO

####### install header files

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = kartswidget.h
