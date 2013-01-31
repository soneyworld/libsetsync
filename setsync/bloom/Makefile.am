ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
AM_CPPFLAGS =
AM_LDFLAGS = 
if OPENSSL
AM_CPPFLAGS += @OPENSSL_CFLAGS@
AM_LDFLAGS += @OPENSSL_LIBS@
endif
include_h_sources = BloomFilter.h \
			CountingBloomFilter.h \
			HashFunction.h \
			FSBloomFilter.h \
			DoubleHashingScheme.h \
			ComparableBloomFilter.h \
			KeyValueCountingBloomFilter.h
cpp_sources = BloomFilter.cpp \
			CountingBloomFilter.cpp \
			HashFunction.cpp \
			FSBloomFilter.cpp \
			DoubleHashingScheme.cpp \
			KeyValueCountingBloomFilter.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/bloom
library_include_HEADERS = $(include_h_sources)

noinst_LTLIBRARIES = libbloom.la
libbloom_la_SOURCES= $(include_h_sources) $(cpp_sources)
