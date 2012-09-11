
generator_SOURCES = \
	Main.cpp \
	SHA1Generator.h	\
	SHA1Generator.cpp

# what flags you want to pass to the C compiler & linker
AM_CPPFLAGS = 
AM_LDFLAGS = 

if OPENSSL
AM_CPPFLAGS += @OPENSSL_CFLAGS@
AM_LDFLAGS += @OPENSSL_LIBS@
endif

if SQLITE
AM_CPPFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
generator_SOURCES += SQLiteDatabase.h
generator_SOURCES += SQLiteDatabase.cpp
endif

noinst_PROGRAMS=generator
generator_CXXFLAGS = ${AM_CPPFLAGS}
generator_LDFLAGS = ${AM_LDFLAGS}
INCLUDES = -I@top_srcdir@/setsync