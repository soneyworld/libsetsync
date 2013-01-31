ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

include_h_sources = Trie.h KeyValueTrie.h TrieException.h
h_sources = $(include_h_sources)
cpp_sources = Trie.cpp KeyValueTrie.cpp

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

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/trie
library_include_HEADERS = $(include_h_sources)

noinst_LTLIBRARIES = libtrie.la
libtrie_la_SOURCES= $(h_sources) $(cpp_sources)
