ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
AM_CPPFLAGS =
AM_LDFLAGS = 
if OPENSSL
AM_CPPFLAGS += @OPENSSL_CFLAGS@
AM_LDFLAGS += @OPENSSL_LIBS@
endif
h_sources = BloomFilter.h \
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

if BERKELEY_DB
h_sources += BdbBloomFilter.h
cpp_sources += BdbBloomFilter.cpp
endif

noinst_LTLIBRARIES = libbloom.la
libbloom_la_SOURCES= $(h_sources) $(cpp_sources)
