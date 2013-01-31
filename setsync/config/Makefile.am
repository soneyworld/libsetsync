ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

include_h_sources = Configuration.h
cpp_sources = Configuration.cpp

AM_CPPFLAGS = 
AM_LDFLAGS = 

if IBRCOMMON
AM_CPPFLAGS += $(IBRCOMMON_CFLAGS)
AM_LDFLAGS += $(IBRCOMMON_LIBS)
endif

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/config
library_include_HEADERS = $(include_h_sources)

noinst_LTLIBRARIES = libconfig.la
libconfig_la_SOURCES= $(include_h_sources) $(cpp_sources)
