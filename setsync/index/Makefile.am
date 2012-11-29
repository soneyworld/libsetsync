ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = AbstractSetIndex.h MemIndex.h DbIndex.h
cpp_sources = AbstractSetIndex.cpp MemIndex.cpp DbIndex.cpp

if SQLITE
AM_CFLAGS = @SQLITE_CFLAGS@
AM_LDFLAGS = @SQLITE_LIBS@
h_sources += SQLiteIndex.h SQLiteException.h SQLiteDatabase.h
cpp_sources += SQLiteIndex.cpp SQLiteException.cpp SQLiteDatabase.cpp
endif

noinst_LTLIBRARIES = libindex.la
libindex_la_SOURCES= $(h_sources) $(cpp_sources)
