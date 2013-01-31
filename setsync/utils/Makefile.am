ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

include_h_sources = FileSystem.h

h_sources = $(include_h_sources) bitset.h OutputFunctions.h  BerkeleyDB.h
cpp_sources = OutputFunctions.cpp FileSystem.cpp BerkeleyDB.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/utils
library_include_HEADERS = $(include_h_sources)

noinst_LTLIBRARIES = libutils.la
libutils_la_SOURCES= $(h_sources) $(cpp_sources)
