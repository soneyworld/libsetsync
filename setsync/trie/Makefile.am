ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = Trie.h MemTrie.h FSTrie.h
cpp_sources = Trie.cpp MemTrie.cpp FSTrie.cpp

if SQLITE
AM_CFLAGS = @SQLITE_CFLAGS@
AM_LDFLAGS = @SQLITE_LIBS@
h_sources += SQLiteTrie.h
cpp_sources += SQLiteTrie.cpp
endif

if BERKELEY_DB
h_sources += DBTrie.h
cpp_sources += DBTrie.cpp
endif

noinst_LTLIBRARIES = libtrie.la
libtrie_la_SOURCES= $(h_sources) $(cpp_sources)
