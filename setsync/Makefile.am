ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
AUTOMAKE_OPTIONS = foreign
SUBDIRS = utils config bloom trie index storage . tests

# what flags you want to pass to the C compiler & linker
AM_CFLAGS = @CFLAGS@ 
AM_LDFLAGS = @LIBS@

include_h_sources = set.h Set.hpp
h_sources = $(include_h_sources) DiffHandler.h Synchronization.h
c_sources = 
cpp_sources = Set.cpp DiffHandler.cpp Synchronization.cpp

if BERKELEY_DB

endif

if OPENSSL
AM_CFLAGS += $(OPENSSL_CFLAGS)
AM_LDFLAGS += $(OPENSSL_LIBS)
endif

if SQLITE
AM_CFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
endif

INCLUDES = -I@top_srcdir@/setsync

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(include_h_sources)

lib_LTLIBRARIES = libsetsync.la
libsetsync_la_SOURCES = $(h_sources) $(cpp_sources) $(c_sources)
libsetsync_la_LDFLAGS = -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
libsetsync_la_LIBADD = bloom/libbloom.la trie/libtrie.la index/libindex.la utils/libutils.la storage/libstorage.la config/libconfig.la

noinst_PROGRAMS=set
set_SOURCES = main.c
set_LDADD = libsetsync.la 
