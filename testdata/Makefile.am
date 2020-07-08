noinst_PROGRAMS = enumerate lib
enumerate_SOURCES = enumerate.cc
enumerate_LDADD = ../buffy/libbuffy.la
enumerate_LDFLAGS = $(XMLPP_LIBS) $(LIBWIBBLE_LIBS)

lib_SOURCES = lib.cc
lib_LDADD = ../buffy/libbuffy.la
lib_LDFLAGS = $(XMLPP_LIBS) $(LIBWIBBLE_LIBS)

AM_CPPFLAGS =-I.. $(XMLPP_CFLAGS) $(LIBWIBBLE_CFLAGS)

CLEANFILES = test-config-saved.txt

EXTRA_DIST = test-config.txt maildir mbox mboxgz make-links
