ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = Trie.h KeyValueTrie.h TrieException.h
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

noinst_LTLIBRARIES = libtrie.la
libtrie_la_SOURCES= $(h_sources) $(cpp_sources)
