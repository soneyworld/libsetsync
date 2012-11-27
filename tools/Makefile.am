ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
AUTOMAKE_OPTIONS = foreign

# what flags you want to pass to the C compiler & linker
AM_CFLAGS = @CFLAGS@ 
AM_LDFLAGS = @LIBS@ -lsetsync -L@top_srcdir@/setsync/.libs


INCLUDES = -I@top_srcdir@

noinst_PROGRAMS=trie2dot
trie2dot_SOURCES = Trie2Dot.h Trie2Dot.cpp 
