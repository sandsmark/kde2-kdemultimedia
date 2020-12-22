####### Various modules for artsmodules

SUBDIRS = mcopclass freeverb
INCLUDES= -I$(top_builddir)/arts/runtime -I$(top_builddir)/arts/midi -I$(top_srcdir)/arts/midi -I$(top_srcdir)/arts/gui/common -I$(top_builddir)/arts/gui/common -I$(srcdir)/freeverb -I$(arts_includes) $(all_includes) 

lib_LTLIBRARIES = libartsmodules.la

libartsmodules_la_SOURCES = artsmodules.cc synth_wave_tri_impl.cc \
	synth_envelope_adsr_impl.cc synth_shelve_cutoff_impl.cc \
	synth_xfade_impl.cc synth_noise_impl.cc synth_midi_test_impl.cc \
	c_filter_stuff.c objectcache_impl.cc synth_wave_square_impl.cc \
	synth_capture_wav_impl.cc synth_freeverb_impl.cc \
	synth_nil_impl.cc synth_debug_impl.cc synth_data_impl.cc \
   	synth_atan_saturate_impl.cc synth_brickwall_limiter_impl.cc \
   	synth_autopanner_impl.cc synth_tremolo_impl.cc \
   	synth_wave_softsaw_impl.cc synth_wave_pulse_impl.cc \
   	synth_cdelay_impl.cc synth_delay_impl.cc \
   	synth_fm_source_impl.cc synth_fx_cflanger_impl.cc \
   	synth_std_equalizer_impl.cc synth_rc_impl.cc \
   	synth_moog_vcf_impl.cc synth_pscale_impl.cc \
   	synth_sequence_impl.cc synth_pitch_shift_impl.cc \
	synth_midi_debug_impl.cc freeverbguifactory_impl.cc

libartsmodules_la_LIBADD = \
   $(top_builddir)/arts/runtime/libartsbuilder.la \
   $(top_builddir)/arts/midi/libartsmidi_idl.la \
   $(top_builddir)/arts/gui/common/libartsgui_idl.la \
   $(top_builddir)/arts/modules/freeverb/libfreeverb.la \
   -lartsflow -lmcop $(LIBDL)
libartsmodules_la_LDFLAGS = $(all_libraries) -no-undefined -module

artsmodules.cc: artsmodules.h

artsmodules.h: $(srcdir)/artsmodules.idl $(MCOPIDL)
	$(MCOPIDL) -t $(INCLUDES) $(srcdir)/artsmodules.idl

####### install idl files

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = artsmodules.h artsmodules.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artsmodules.mcoptype artsmodules.mcopclass
