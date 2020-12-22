KDE_CXXFLAGS = -UQT_NO_ASCII_CAST

SUBDIRS = pics 
# just set the variable
APPSDIR = $(kde_appsdir)/Multimedia
# set the include path for X, qt, KDE, mico and the synthesizer class defs
INCLUDES= -I$(top_srcdir)/arts/runtime -I$(top_builddir)/arts/runtime  -I$(kde_includes)/arts  $(all_includes)

applnk_DATA = artsbuilder.desktop
applnkdir = $(kde_appsdir)/Multimedia

kdemime_DATA = x-artsbuilder.desktop
kdemimedir = $(kde_mimedir)/application

####### This part is very artsbuilder specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	artsbuilder

# Which sources should be compiled for artsbuilder.
artsbuilder_SOURCES = autorouter.cpp main.cpp \
  module.cpp portpropdlg.cpp structure.cpp namedlg.cpp \
  retrievedlg.cpp createtool.cpp structureport.cpp \
  drawutils.cpp scomponent.cpp portposdlg.cpp \
  menumaker.cpp session.cpp dirmanager.cpp \
  portablekde.cpp execdlg.cpp

METASOURCES = AUTO

# the library search path. 
artsbuilder_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
artsbuilder_LDADD   = $(LIB_KFILE) -lqtmcop $(LIB_X11) $(LIBPTHREAD) $(top_builddir)/arts/runtime/libartsbuilder.la -lsoundserver_idl

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages:
	$(XGETTEXT) *.cpp -o $(podir)/artsbuilder.pot

