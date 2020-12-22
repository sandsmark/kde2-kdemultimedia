INCLUDES= $(all_includes) \
	-I$(top_srcdir)/arts/modules -I$(top_builddir)/arts/modules \
	-I$(top_builddir)/arts/midi \
	-I$(top_srcdir)/arts/gui/kde -I$(top_builddir)/arts/gui/common \
	-I$(kde_includes)/arts

####### Files

bin_PROGRAMS	= artscontrol
artscontrol_SOURCES = choosebusdlg.cpp levelmeters.cpp main.cpp \
   audiomanager.cpp midimanagerview.cpp midimanagerdlg.ui \
   midimanagerwidget.ui midiportdlg.cpp midiinstdlg.cpp

artscontrol_LDADD	= $(LIB_KDEUI) $(LIB_KSYCOCA) $(LIB_KFILE) \
     $(kde_libraries)/libqtmcop.la \
     $(kde_libraries)/libartsflow_idl.la \
     $(kde_libraries)/libsoundserver_idl.la \
     $(kde_libraries)/libartskde.la \
	 $(top_builddir)/arts/gui/kde/libartsgui_kde.la \
	 $(top_builddir)/arts/modules/libartsmodules.la \
	 $(top_builddir)/arts/midi/libartsmidi_idl.la

artscontrol_LDFLAGS	= $(all_libraries) $(KDE_RPATH)

rcdir = $(kde_datadir)/artscontrol
rc_DATA = artscontrol.rc artsmidimanagerview.rc

applnk_DATA = artscontrol.desktop
applnkdir = $(kde_appsdir)/Multimedia

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/artscontrol.pot

