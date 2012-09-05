ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = 
cpp_sources = 

if SQLITE
AM_CFLAGS = @SQLITE_CFLAGS@
AM_LDFLAGS = @SQLITE_LIBS@
h_sources += SQLiteIndex.h SQLiteException.h
cpp_sources += SQLiteIndex.cpp SQLiteException.cpp
endif

noinst_LTLIBRARIES = libindex.la
libindex_la_SOURCES= $(h_sources) $(cpp_sources)
