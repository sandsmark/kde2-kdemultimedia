INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatuntron.la

libnoatuntron_la_SOURCES = cmodule.cpp \
	ksaver.cpp \
	plugininterface.cpp \
	mcp.cpp \
	tron.cpp 

libnoatuntron_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatuntron_la_LIBADD = $(LIB_KFILE) \
	$(top_builddir)/noatun/noatun/library/libnoatun.la \
	-lqtmcop -lkmedia2_idl -lsoundserver_idl

libnoatuntron_la_METASOURCES = AUTO

noinst_HEADERS = cmodule.h \
	ksaver.h \
	mcp.h \
	tron.h

data_DATA = tron.plugin tronui.rc
datadir = $(kde_datadir)/noatun

noatuniconsdir = $(kde_datadir)/noatun/icons
noatunicons_ICON = AUTO
