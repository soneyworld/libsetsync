
noinst_HEADERS = \
	MemSetTest.h \
	MemTrieTest.h \
	FSTrieTest.h \
	BloomFilterTest.h
	

unittest_SOURCES = \
	Main.cpp \
	MemSetTest.cpp \
	MemTrieTest.cpp \
	FSTrieTest.cpp \
	BloomFilterTest.cpp
	

# what flags you want to pass to the C compiler & linker
AM_CPPFLAGS = @CPPUNIT_CFLAGS@ -Wall
AM_LDFLAGS = @CPPUNIT_LIBS@

if SQLITE
AM_CPPFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
unittest_SOURCES += SQLiteTrieTest.cpp SQLiteIndexTest.cpp
noinst_HEADERS += SQLiteTrieTest.h SQLiteIndexTest.h
endif

INCLUDES = -I@top_srcdir@ -I@top_srcdir@/setsync/tests/unittests

check_PROGRAMS = unittest
unittest_CXXFLAGS = ${AM_CPPFLAGS} ${CPPUNIT_CFLAGS} -Wall
unittest_LDFLAGS = ${AM_LDFLAGS} ${CPPUNIT_LIBS}
unittest_LDADD = @top_srcdir@/setsync/libsetsync.la

TESTS = unittest
