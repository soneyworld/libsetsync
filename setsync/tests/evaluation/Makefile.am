
evaluation_SOURCES = \
	Main.cpp \
	SHA1Generator.h	\
	SHA1Generator.cpp \
	BFTest.h \
	BFTest.cpp \
	DBTest.h \
	DBTest.cpp \
	BDBInsertRemoveTest.h \
	BDBInsertRemoveTest.cpp \
	BDBTransactionsTest.h \
	BDBTransactionsTest.cpp \
	StopWatch.h \
	StopWatch.cpp \
	SetTest.h \
	SetTest.cpp \
	SetSync.h \
	SetSync.cpp \
	Test.h

# what flags you want to pass to the C compiler & linker
AM_CPPFLAGS = -fopenmp
AM_LDFLAGS = -lrt

if OPENSSL
AM_CPPFLAGS += @OPENSSL_CFLAGS@
AM_LDFLAGS += @OPENSSL_LIBS@
endif

if SQLITE
AM_CPPFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
evaluation_SOURCES += SQLiteTest.h SQLiteDatabase.h SQLiteException.h
evaluation_SOURCES += SQLiteTest.cpp SQLiteDatabase.cpp SQLiteException.cpp
endif

if LEVELDB
AM_CPPFLAGS += @LEVELDB_CFLAGS@
AM_LDFLAGS += @LEVELDB_LIBS@
evaluation_SOURCES += LevelDbTest.h LevelDbTest.cpp
endif

if IBRCOMMON
AM_CPPFLAGS += @IBRCOMMON_CFLAGS@
AM_LDFLAGS += @IBRCOMMON_LIBS@
endif


noinst_PROGRAMS=evaluation
evaluation_CXXFLAGS = ${AM_CPPFLAGS}
evaluation_LDFLAGS = ${AM_LDFLAGS}
evaluation_LDADD = @top_srcdir@/setsync/libsetsync.la
INCLUDES = -I@top_srcdir@
