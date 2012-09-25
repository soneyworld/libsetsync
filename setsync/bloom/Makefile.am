ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
AM_CPPFLAGS =
AM_LDFLAGS = 
if OPENSSL
AM_CPPFLAGS += @OPENSSL_CFLAGS@
AM_LDFLAGS += @OPENSSL_LIBS@
endif
h_sources = BloomFilter.h CountingBloomFilter.h bloom.h HashFunction.h FSBloomFilter.h
cpp_sources = BloomFilter.cpp CountingBloomFilter.cpp bloom.c HashFunction.cpp FSBloomFilter.cpp
if SQLITE
AM_CPPFLAGS += @SQLITE_CFLAGS@
AM_LDFLAGS += @SQLITE_LIBS@
h_sources += 
cpp_sources +=
endif

noinst_LTLIBRARIES = libbloom.la
libbloom_la_SOURCES= $(h_sources) $(cpp_sources)
