ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
AUTOMAKE_OPTIONS = foreign
SUBDIRS = bloom trie tests index

# what flags you want to pass to the C compiler & linker
AM_CFLAGS = @CFLAGS@ 
AM_LDFLAGS = @LIBS@

include_h_sources = set.h Set.hpp
h_sources = $(include_h_sources) MemSet.h Config.h
c_sources = set.c
cpp_sources = Set.cpp MemSet.cpp Config.cpp

if OPENSSL
AM_CFLAGS += $(OPENSSL_CFLAGS)
AM_LDFLAGS += $(OPENSSL_LIBS)
else
h_sources += sha1.h
c_sources += sha1.c
endif

if SQLITE
AM_CFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
endif

INCLUDES = -I@top_srcdir@/setsync

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(include_h_sources)

lib_LTLIBRARIES = libsetsync.la
libsetsync_la_SOURCES = $(h_sources) $(c_sources) $(cpp_sources)
libsetsync_la_LDFLAGS = -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
libsetsync_la_LIBADD = bloom/libbloom.la trie/libtrie.la index/libindex.la

noinst_PROGRAMS=set
set_SOURCES = main.c
set_LDADD = libsetsync.la 
