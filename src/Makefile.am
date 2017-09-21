## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir). \
	$(ALLEGRO_FONT_CFLAGS)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(MACH_TEST_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = mach_test

mach_test_SOURCES = \
	main.cc \
	environment.cc \
	environment.h \
	cfg.h \
	insect.cc \
	insect.h \
	track.cc \
	track.h \
	to-do

mach_test_LDFLAGS = 

mach_test_LDADD = $(MACH_TEST_LIBS) \
	$(ALLEGRO_FONT_LIBS)


