ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

include_h_sources = KeyValueStorage.h StorageException.h

h_sources = $(include_h_sources)
cpp_sources = KeyValueStorage.cpp StorageException.cpp
AM_CPPFLAGS = 
AM_LDFLAGS =

if LEVELDB
AM_CPPFLAGS += @LEVELDB_CFLAGS@
AM_LDFLAGS += @LEVELDB_LIBS@
h_sources += LevelDbStorage.h 
cpp_sources += LevelDbStorage.cpp
endif

if BERKELEY_DB
h_sources += BdbStorage.h BdbTableUser.h MemStorage.h
cpp_sources += BdbStorage.cpp BdbTableUser.cpp MemStorage.cpp
endif

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/storage
library_include_HEADERS = $(include_h_sources)

noinst_LTLIBRARIES = libstorage.la
libstorage_la_SOURCES= $(h_sources) $(cpp_sources)
