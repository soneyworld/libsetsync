ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

include_h_sources = CryptoHash.h
h_sources = $(include_h_sources)
cpp_sources = CryptoHash.cpp

if OPENSSL
AM_CFLAGS = $(OPENSSL_CFLAGS)
AM_LDFLAGS = $(OPENSSL_LIBS)
else
polarssl_h_sources = md_wrap.h md.h md2.h md4.h md5.h sha1.h sha2.h sha4.h
polarssl_c_sources = md_wrap.c md.c md2.c md4.c md5.c sha1.c sha2.c sha4.c
cpp_sources += $(polarssl_c_sources)
h_sources += $(polarssl_h_sources)
endif

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/crypto
library_include_HEADERS = $(include_h_sources)
noinst_LTLIBRARIES = libcrypto.la
libcrypto_la_SOURCES= $(h_sources) $(cpp_sources)
