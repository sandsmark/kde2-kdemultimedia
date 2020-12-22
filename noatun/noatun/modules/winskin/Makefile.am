INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libwinskin.la

libwinskin_la_SOURCES = guiPlugin.cpp \
guiSpectrumAnalyser.cpp \
spectrumImage.cpp \
waBPS.cpp \
waBalanceSlider.cpp \
waButton.cpp \
waDigit.cpp \
waFreq.cpp \
waInfo.cpp \
waJumpSlider.cpp \
waMono.cpp \
waSkin.cpp \
waSkinDesc.cpp \
waSkinModell.cpp \
waSkinOperation.cpp \
waSlider.cpp \
waStatus.cpp \
waStereo.cpp \
waTitleBar.cpp \
waVolumeSlider.cpp \
winskin.cpp \
winskinui.cpp \
winSkinConfig.cpp \
l33tFileInfo.cpp

libwinskin_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined 
libwinskin_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

libwinskin_la_METASOURCES = AUTO

noinst_HEADERS = 

data_DATA = winskin.plugin winskinui.rc
datadir = $(kde_datadir)/noatun

SUBDIRS = skins
