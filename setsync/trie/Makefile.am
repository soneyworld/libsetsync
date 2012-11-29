ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = Trie.h MemTrie.h
cpp_sources = Trie.cpp MemTrie.cpp

if SQLITE
AM_CFLAGS = @SQLITE_CFLAGS@
AM_LDFLAGS = @SQLITE_LIBS@
endif

if BERKELEY_DB
h_sources += DBTrie.h
cpp_sources += DBTrie.cpp
endif

noinst_LTLIBRARIES = libtrie.la
libtrie_la_SOURCES= $(h_sources) $(cpp_sources)
