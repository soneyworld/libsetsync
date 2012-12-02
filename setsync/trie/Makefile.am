ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = Trie.h MemTrie.h KeyValueTrie.h
cpp_sources = Trie.cpp MemTrie.cpp KeyValueTrie.cpp

AM_CFLAGS = 
AM_LDFLAGS =

if SQLITE
AM_CFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
endif

if BERKELEY_DB
h_sources += BdbTrie.h
cpp_sources += BdbTrie.cpp
endif

if LEVELDB
AM_CFLAGS += @LEVELDB_CFLAGS@
AM_LDFLAGS += @LEVELDB_LIBS@
h_sources += LevelDbTrie.h
cpp_sources += LevelDbTrie.cpp
endif

noinst_LTLIBRARIES = libtrie.la
libtrie_la_SOURCES= $(h_sources) $(cpp_sources)
