# $Id: Makefile.am 90474 2001-04-04 23:19:06Z garbanzo $

EXTRA_DIST = README decode_i386.c decode_i586.s dct64_i386.c decode.c \
			dct64.c audio.h

COMMON_CFLAGS = -I$(top_builddir) -I$(top_srcdir)/include \
	@ARCH_CFLAGS@ -D_REENTRANT -DNOXFERMEM -DNO_EQUALIZER \
	-DNO_DECODE_AUDIO -DNO_DECODE_FILE -DNO_DECODE_WAV

COMMON_SRC = common.c decode_2to1.c decode_4to1.c decode_ntom.c \
	layer1.c layer2.c layer3.c readers.c \
	httpget.c getbits.c tabinit.c xfermem.c vbrhead.c buffer.c audio.c

noinst_HEADERS = huffman.h mpg123.h l2tables.h audio.h common.h \
		genre.h getbits.h xfermem.h buffer.h

#486, 586, 686
if ARCH_486
ARCH_CFLAGS = -DROT_I386 -DI386_ASSEM -DI486_OPT -DLINUX -DREAL_IS_FLOAT
libmpg123_la_SOURCES = $(COMMON_SRC) decode_i386.c dct64_i386.c decode_i486.c dct64_i486.c
endif
if ARCH_586
ARCH_CFLAGS = -DROT_I386 -DI386_ASSEM -DPENTIUM_OPT -DLINUX -DREAL_IS_FLOAT
libmpg123_la_SOURCES = $(COMMON_SRC) decode_i386.c dct64_i386.c decode_i586.s
endif
if ARCH_586_MMX
ARCH_CFLAGS = -DROT_I386 -DI386_ASSEM -DPENTIUM_OPT -DLINUX -DREAL_IS_FLOAT
libmpg123_la_SOURCES = $(COMMON_SRC) decode_i386.c dct64_MMX.s decode_MMX.s tabinit_MMX.s
endif
if ARCH_686
ARCH_CFLAGS = -DROT_I386 -DI386_ASSEM -DPENTIUM_OPT -DLINUX -DREAL_IS_FLOAT -march=pentiumpro
libmpg123_la_SOURCES = $(COMMON_SRC) decode_i386.c dct64_i386.c decode_i586.s
endif
if ARCH_686_MMX
ARCH_CFLAGS = -DROT_I386 -DI386_ASSEM -DPENTIUM_OPT -DUSE_MMX -DLINUX -DREAL_IS_FLOAT -march=pentiumpro
libmpg123_la_SOURCES = $(COMMON_SRC) decode_i386.c dct64_MMX.s decode_MMX.s tabinit_MMX.s
endif

#PPC
if ARCH_PPC
ARCH_CFLAGS = -mcpu=ppc
libmpg123_la_SOURCES = $(COMMON_SRC) decode.c dct64.c
endif

if ARCH_PLAIN
ARCH_CFLAGS=
libmpg123_la_SOURCES = $(COMMON_SRC) decode.c dct64.c
endif

CFLAGS = $(COMMON_CFLAGS) $(ARCH_CFLAGS)
INCLUDES = -I$(ROOT_DIR)/app

lib_LTLIBRARIES = libmpg123.la
libmpg123_la_LDFLAGS = -module -avoid-version
