ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

include_h_sources = AbstractSetIndex.h KeyValueIndex.h
cpp_sources = AbstractSetIndex.cpp KeyValueIndex.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/index
library_include_HEADERS = $(include_h_sources)

noinst_LTLIBRARIES = libindex.la
libindex_la_SOURCES= $(h_sources) $(cpp_sources)
