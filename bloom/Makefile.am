ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = BloomFilter.h CountingBloomFilter.h 
cpp_sources = BloomFilter.cpp CountingBloomFilter.cpp

noinst_LTLIBRARIES = libbloom.la
libbloom_la_SOURCES= $(h_sources) $(cpp_sources)
libbloom_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)