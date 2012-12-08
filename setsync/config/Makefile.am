ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = Configuration.h
cpp_sources = Configuration.cpp

#if IBRCOMMON
#AM_CFLAGS = @IBRCOMMON_CFLAGS@ 
#AM_LDFLAGS = @IBRCOMMON_LIBS@
#endif

noinst_LTLIBRARIES = libconfig.la
libconfig_la_SOURCES= $(h_sources) $(cpp_sources)
