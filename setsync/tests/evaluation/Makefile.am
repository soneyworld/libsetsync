
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
	Test.h

# what flags you want to pass to the C compiler & linker
AM_CPPFLAGS = -fopenmp
AM_LDFLAGS = 

if OPENSSL
AM_CPPFLAGS += @OPENSSL_CFLAGS@
AM_LDFLAGS += @OPENSSL_LIBS@
endif

if SQLITE
AM_CPPFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
evaluation_SOURCES += SQLiteTest.h
evaluation_SOURCES += SQLiteTest.cpp
endif


noinst_PROGRAMS=evaluation
evaluation_CXXFLAGS = ${AM_CPPFLAGS}
evaluation_LDFLAGS = ${AM_LDFLAGS}
evaluation_LDADD = @top_srcdir@/setsync/libsetsync.la
INCLUDES = -I@top_srcdir@