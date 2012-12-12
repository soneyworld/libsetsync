
noinst_HEADERS = \
	BloomFilterTest.h \
	FSBloomFilterTest.h \
	DoubleHashingSchemeTest.h \
	SetTest.h \
	KeyValueStorageTest.h \
	DiffHandlerTest.h

unittest_SOURCES = \
	Main.cpp \
	BloomFilterTest.cpp \
	FSBloomFilterTest.cpp \
	DoubleHashingSchemeTest.cpp \
	SetTest.cpp \
	KeyValueStorageTest.cpp \
	DiffHandlerTest.cpp
	

# what flags you want to pass to the C compiler & linker
AM_CPPFLAGS = @CPPUNIT_CFLAGS@ -Wall
AM_LDFLAGS = @CPPUNIT_LIBS@

if IBRCOMMON
AM_CPPFLAGS += $(IBRCOMMON_CFLAGS)
AM_LDFLAGS += $(IBRCOMMON_LIBS)
endif

noinst_HEADERS += KeyValueCountingBloomFilterTest.h KeyValueIndexTest.h KeyValueTrieTest.h
unittest_SOURCES += KeyValueCountingBloomFilterTest.cpp KeyValueIndexTest.cpp KeyValueTrieTest.cpp

INCLUDES = -I@top_srcdir@ -I@top_srcdir@/setsync/tests/unittests

check_PROGRAMS = unittest
unittest_CXXFLAGS = ${AM_CPPFLAGS} ${CPPUNIT_CFLAGS} -Wall
unittest_LDFLAGS = ${AM_LDFLAGS} ${CPPUNIT_LIBS}
unittest_LDADD = @top_srcdir@/setsync/libsetsync.la

TESTS = unittest
