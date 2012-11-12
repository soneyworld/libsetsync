ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

h_sources = bittest.h OutputFunctions.h CryptoHash.h FileSystem.h BerkeleyDB.h
cpp_sources = OutputFunctions.cpp CryptoHash.cpp FileSystem.cpp BerkeleyDB.cpp

if OPENSSL
AM_CFLAGS = $(OPENSSL_CFLAGS)
AM_LDFLAGS = $(OPENSSL_LIBS)
else
polarssl_h_sources = md_wrap.h md.h md2.h md4.h md5.h sha1.h sha2.h sha4.h
polarssl_c_sources = md_wrap.c md.c md2.c md4.c md5.c sha1.c sha2.c sha4.c
cpp_sources += $(polarssl_c_sources)
h_sources += $(polarssl_h_sources)
endif

noinst_LTLIBRARIES = libutils.la
libutils_la_SOURCES= $(h_sources) $(cpp_sources)
