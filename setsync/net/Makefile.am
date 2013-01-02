ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = PacketParser.h Packets.h
cpp_sources = PacketParser.cpp Packets.cpp

AM_CFLAGS = 
AM_LDFLAGS =

if SQLITE
AM_CFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
endif

if LEVELDB
AM_CFLAGS += @LEVELDB_CFLAGS@
AM_LDFLAGS += @LEVELDB_LIBS@
endif

if IBRCOMMON
AM_CPPFLAGS = @IBRCOMMON_CFLAGS@
AM_LDFLAGS += @IBRCOMMON_LIBS@
endif

noinst_LTLIBRARIES = libnet.la
libnet_la_SOURCES= $(h_sources) $(cpp_sources)