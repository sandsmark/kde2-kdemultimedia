# $Id: Makefile.am 192563 2002-12-07 05:44:23Z staikos $

mimedir = $(kde_mimedir)/text/
picsdir  = $(kde_datadir)/kscd/pics/
appsdir = $(kde_appsdir)/Multimedia
CDDBDIR = $(kde_datadir)/kscd/cddb/

LDFLAGS	= $(LIB_KIO) $(all_libraries) $(KDE_RPATH)
kscd_LDADD   = libwm/libworkman.la $(LIBCDROM) $(LIBCDAUDIO) $(LIBFPE) $(LIB_KSYCOCA) $(LIB_KDEUI) -lkfile
INCLUDES= $(all_includes)
LINK    = $(CXXLINK)

####### Files

noinst_HEADERS = kscd.h bwlednum.h ledlamp.h inexact.h \
	CDDialog.h CDDialogData.h CDDBSetup.h CDDBSetupData.h mgconfdlg.h \
	configdlg.h cddb.h version.h docking.h smtpconfig.h \
	smtp.h

#	struct.h wmcdda.h

kscd_SOURCES = kscd.cpp kscd.skel configdlg.cpp cddb.cpp ledlamp.cpp CDDialog.cpp \
	CDDialogData.cpp CDDBSetup.cpp CDDBSetupData.cpp docking.cpp \
	smtpconfig.cpp smtp.cpp inexact.cpp mgconfdlg.cpp  bwlednum.cpp

#	plat_scor5.c plat_sun.c plat_hpux.c plat_linux.c plat_news.c \
#	plat_bsd386.c plat_ultrix.c cdrom.c scsi.c drv_toshiba.c drv_sony.c \
#	plat_svr4.c plat_freebsd.c plat_osf1.c plat_irix.c


#cddaslave_SOURCES = cddaslave.c
#cddaslave_LDADD = libwm/libworkman.la

#SOURCES3=kscdmagic/bitmap.cpp  kscdmagic/core.cpp kscdmagic/main.cpp \
#	kscdmagic/sound.cpp kscdmagic/xlibwrap.cpp kscdmagic/xlib.c

#OBJECTS3=kscdmagic/bitmap.o  kscdmagic/core.o kscdmagic/main.o \
#	kscdmagic/sound.o kscdmagic/xlibwrap.o kscdmagic/xlib.o

METASOURCES = kscd.moc bwlednum.moc configdlg.moc cddb.moc ledlamp.moc \
	 inexact.moc CDDialog.moc CDDialogData.moc CDDBSetup.moc \
	CDDBSetupData.moc docking.moc mgconfdlg.moc smtpconfig.moc \
	smtp.moc

DISTCLEANFILES = config.h

bin_PROGRAMS = kscd
# Don't Build cddaslave. It doesn't work.
# cddaslave
#TARGET3 = kscdmagic

#config.h: kscd-script
#$(SHELL) $(srcdir)/kscd-script

SUBDIRS = bitmaps libwm

bin_SCRIPTS = workman2cddb.pl

EXTRA_DIST = kscdlogo.xpm xmcd.desktop kscd.desktop kscd-script $(bin_SCRIPTS) config.h.std

KDE_ICON = kscd

apps_DATA = kscd.desktop

mime_DATA = xmcd.desktop

pics_DATA = kscdlogo.xpm cdsmall.xpm

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kscd.pot

cdrom.c:config.h
plat_bsd386.c:config.h
plat_freebsd.c:config.h
plat_hpux.c:config.h
plat_irix.c:config.h
plat_linux.c:config.h
plat_news.c:config.h
plat_osf1.c:config.h
plat_scor5.c:config.h
plat_sun.c:config.h
plat_sun_audio.c:config.h
plat_sun_cdda.c:config.h
plat_svr4.c:config.h
plat_ultrix.c:config.h

