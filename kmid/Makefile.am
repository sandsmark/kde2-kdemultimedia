INCLUDES = $(all_includes)
lib_LTLIBRARIES = libkmidpart.la

libkmidpart_la_LDFLAGS = $(all_libraries)  -version-info 1:0:0 -module 
libkmidpart_la_LIBADD = -lkmid $(LIB_KIO) $(LIB_KFILE) -lkparts

libkmidpart_la_SOURCES = kmidframe.cpp kmidclient.cpp kdisptext.cpp \
	qslidertime.cpp klcdnumber.cpp ktrianglebutton.cpp \
	midicfgdlg.cpp collectdlg.cpp kaskdlg.cpp randomlist.cpp \
	channelview.cpp channel.cpp channel3d.cpp channel4d.cpp \
	channelcfgdlg.cpp instrname.cpp rhythmview.cpp \
	songlist.cpp slman.cpp kmid_part.cpp main.cpp kmidIface.skel

noinst_HEADERS = kmidframe.h kmidclient.h kdisptext.h \
        qslidertime.h klcdnumber.h ktrianglebutton.h \
        midicfgdlg.h collectdlg.h kaskdlg.h randomlist.h \
        channelview.h channel.h channel3d.h channel4d.h \
        channelcfgdlg.h instrname.h rhythmview.h \
        songlist.h slman.h kmid_part.h

kmid_LDFLAGS = $(KDE_RPATH)
kmid_SOURCES = main.cpp
kmid_LDADD   = libkmidpart.la

DISTCLEANFILES = $(SRCMETA) ./.deps/* ./.deps/.P

bin_PROGRAMS = kmid
METASOURCES = AUTO

servicetype_DATA = audiomidi.desktop
servicetypedir = $(kde_servicetypesdir)

SUBDIRS = maps examples pics

applnk_DATA = kmid.desktop
applnkdir = $(kde_appsdir)/Multimedia

mime_DATA = x-karaoke.desktop
mimedir = $(kde_mimedir)/audio

rc_DATA = kmidui.rc
rcdir = $(kde_datadir)/kmid

KDE_ICON = kmid

fmpatches_DATA = std.sb drums.sb std.o3 drums.o3
fmpatchesdir = $(kde_datadir)/kmid/fm

partdir   = $(kde_datadir)/kmid
part_DATA = kmid_partui.rc

channelview_DATA = keyboard.xpm button1.xpm button2.xpm
channelviewdir = $(kde_datadir)/kmid/icons

EXTRA_DIST = $(applnk_DATA) $(mime_DATA) $(fmpatches_DATA) $(part_DATA)

instrname.i18n: instrname.cpp
	echo "/* This is an automatically generated file" > instrname.i18n
	echo " * DO NOT EDIT !!!" >> instrname.i18n
	echo " * To create this file run : ""make instrname.i18n"" " >> instrname.i18n
	echo " */" >> instrname.i18n
	echo "" >> instrname.i18n
	awk -f instrfilter.awk instrname.cpp >> instrname.i18n


messages: instrname.i18n rc.cpp
	$(XGETTEXT) *.cpp instrname.i18n -o $(podir)/kmid.pot

install-data-local:
	@echo "********************************************"
	@echo "Thanks for installing kmid 2.0 !"
	@echo "I have included some example songs that have been installed in"
	@echo $(kde_datadir)/kmid
	@echo ""
	@echo "If you find any bug, or you have any new idea to implement in"
	@echo "kmid, just send me an email."
	@echo ""
	@echo "I hope that you have fun with kmid !"
	@echo "Antonio Larrosa Jimenez (larrosa@kde.org)"
	@echo "*********************************************"


