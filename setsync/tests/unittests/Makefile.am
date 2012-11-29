
noinst_HEADERS = \
	MemSetTest.h \
	MemTrieTest.h \
	BloomFilterTest.h \
	FSBloomFilterTest.h \
	DoubleHashingSchemeTest.h \
	SetTest.h \
	DiffHandlerTest.h

unittest_SOURCES = \
	Main.cpp \
	MemSetTest.cpp \
	MemTrieTest.cpp \
	BloomFilterTest.cpp \
	FSBloomFilterTest.cpp \
	DoubleHashingSchemeTest.cpp \
	SetTest.cpp \
	DiffHandlerTest.cpp
	

# what flags you want to pass to the C compiler & linker
AM_CPPFLAGS = @CPPUNIT_CFLAGS@ -Wall
AM_LDFLAGS = @CPPUNIT_LIBS@

if SQLITE
AM_CPPFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
unittest_SOURCES += SQLiteIndexTest.cpp
noinst_HEADERS += SQLiteIndexTest.h
endif

if BERKELEY_DB
unittest_SOURCES += DBTrieTest.cpp DBBloomFilterTest.cpp DbSetTest.cpp DbIndexTest.cpp
noinst_HEADERS += DBTrieTest.h DBBloomFilterTest.h DbSetTest.h DbIndexTest.h
endif

INCLUDES = -I@top_srcdir@ -I@top_srcdir@/setsync/tests/unittests

check_PROGRAMS = unittest
unittest_CXXFLAGS = ${AM_CPPFLAGS} ${CPPUNIT_CFLAGS} -Wall
unittest_LDFLAGS = ${AM_LDFLAGS} ${CPPUNIT_LIBS}
unittest_LDADD = @top_srcdir@/setsync/libsetsync.la

TESTS = unittest
