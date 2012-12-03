ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = KeyValueStorage.h 
cpp_sources = KeyValueStorage.cpp
AM_CPPFLAGS = 
AM_LDFLAGS =

if LEVELDB
AM_CPPFLAGS += @LEVELDB_CFLAGS@
AM_LDFLAGS += @LEVELDB_LIBS@
h_sources += LevelDbStorage.h 
cpp_sources += LevelDbStorage.cpp
endif

if BERKELEY_DB
h_sources += BdbStorage.h
cpp_sources += BdbStorage.cpp
endif

noinst_LTLIBRARIES = libstorage.la
libstorage_la_SOURCES= $(h_sources) $(cpp_sources)
