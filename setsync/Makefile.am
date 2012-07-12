ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

include_h_sources = set.h
h_sources = $(include_h_sources) 
c_sources = set.c

AM_CFLAGS = $(OPENSSL_CFLAGS)
AM_LDFLAGS = $(OPENSSL_LIBS)

if !OPENSSL
h_sources += sha1.h
c_sources += sha1.c
endif

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(include_h_sources)

lib_LTLIBRARIES = libsetsync.la
libsetsync_la_SOURCES = $(h_sources) $(c_sources)
libsetsync_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
