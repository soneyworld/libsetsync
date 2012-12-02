
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

if BERKELEY_DB
unittest_SOURCES += BdbTrieTest.cpp BdbBloomFilterTest.cpp BdbSetTest.cpp BdbIndexTest.cpp
noinst_HEADERS += BdbTrieTest.h BdbBloomFilterTest.h BdbSetTest.h BdbIndexTest.h
endif

if LEVELDB
noinst_HEADERS += LevelDbStorageTest.h
unittest_SOURCES += LevelDbStorageTest.cpp
endif

INCLUDES = -I@top_srcdir@ -I@top_srcdir@/setsync/tests/unittests

check_PROGRAMS = unittest
unittest_CXXFLAGS = ${AM_CPPFLAGS} ${CPPUNIT_CFLAGS} -Wall
unittest_LDFLAGS = ${AM_LDFLAGS} ${CPPUNIT_LIBS}
unittest_LDADD = @top_srcdir@/setsync/libsetsync.la

TESTS = unittest
