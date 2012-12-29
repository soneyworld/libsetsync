ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = Synchronization.h Progress.h
cpp_sources = Synchronization.cpp Progress.cpp

AM_CFLAGS = 
AM_LDFLAGS =

if SQLITE
AM_CFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
endif

if LEVELDB
AM_CFLAGS += @LEVELDB_CFLAGS@
AM_LDFLAGS += @LEVELDB_LIBS@
endif

if IBRCOMMON
AM_CPPFLAGS = @IBRCOMMON_CFLAGS@
AM_LDFLAGS += @IBRCOMMON_LIBS@
endif

noinst_LTLIBRARIES = libsync.la
libsync_la_SOURCES= $(h_sources) $(cpp_sources)