# We don't want INT32 since Xmd.h defines it
AM_CPPFLAGS = -DQT_CLEAN_NAMESPACE

SUBDIRS = pics 

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = aktion

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# since the "real" aktion is a library, we start constructing the
# stuff for it
lib_LTLIBRARIES = libaktion.la

libaktion_la_SOURCES = main.cpp aktionConf.cpp capture.cpp  \
                       aktionVm.cpp kxanim.cpp principal.cpp \
                       aktion_part.cpp
libaktion_la_LDFLAGS = $(all_libraries)  -version-info 1:0:0 -module
libaktion_la_LIBADD  = $(LIB_KFILE) $(LIBVM) -lkparts

aktion_LDFLAGS = $(KDE_RPATH)
aktion_SOURCES = main.cpp
aktion_LDADD   = libaktion.la

# these are the headers for your project
noinst_HEADERS = principal.h aktionConf.h kxanim.h capture.h aktionVm.h \
                 aktion_part.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/aktion.pot

KDE_ICON = aktion

# this is where the menu and toolbar description file goes
partdir   = $(kde_datadir)/aktion
part_DATA = aktion_part.rc

# this is where the desktop file will go 
lnkdir   = $(kde_appsdir)/Multimedia
lnk_DATA = aktion.desktop

rcdir   = $(kde_confdir)
rc_DATA = aktionrc

