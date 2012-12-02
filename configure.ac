dnl --------------------------------
dnl Package name and version number
dnl --------------------------------

# include version information
m4_include([version.m4])

dnl --------------------------------
dnl Initialization macros.
dnl --------------------------------
ACLOCAL_AMFLAGS = -I m4 --install

AC_PREREQ([2.68])
AC_INIT([setsync], [PKG_FULL_VERSION], [lorentze@ibr.cs.tu-bs.de])
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([setsync/set.h])
AC_CONFIG_HEADERS([config.h])
LT_INIT


dnl --------------------------------
dnl library version number
dnl --------------------------------

#API version
GENERIC_API_VERSION=PKG_MAJOR_VERSION.PKG_MINOR_VERSION

#shared library versioning
GENERIC_LIBRARY_VERSION=PKG_LIB_VERSION
#                       | | |
#                +------+ | +---+
#                |        |     |
#             current:revision:age
#                |        |     |
#                |        |     +- increment if interfaces have been added
#                |        |        set to zero if interfaces have been removed
#                                  or changed
#                |        +- increment if source code has changed
#                |           set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed

# define the library name
GENERIC_LIBRARY_NAME=$PACKAGE_NAME
GENERIC_VERSION=$PACKAGE_VERSION
GENERIC_RELEASE=PKG_MAJOR_VERSION.PKG_MINOR_VERSION

AC_SUBST(GENERIC_API_VERSION)
AC_SUBST(GENERIC_LIBRARY_VERSION)
AC_SUBST(GENERIC_LIBRARY_NAME)
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)


dnl -----------------------------------------------
dnl Checks for programs.
dnl -----------------------------------------------

AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

AC_LANG_CPLUSPLUS

# Checks for header files.
AC_CHECK_HEADERS([math.h])
AC_CHECK_HEADERS([limits.h])

AC_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([munmap])
AC_CHECK_FUNCS([pow])
AC_CHECK_FUNCS([strcasecmp])
AC_CHECK_FUNCS([rmdir])

AC_OPENMP

AC_CHECK_LIB([m], [log])

# Check for inline support
AC_C_INLINE


# BerkeleyDB c++
AM_CONDITIONAL(BERKELEY_DB, true)
AC_ARG_WITH(bdb,
	AS_HELP_STRING([--with-bdb],[build in BerkeleyDB backend support (default is yes)]),
    [
    	use_bdb = $withval
    ],[])
if test "x$use_bdb" != "xno"; then
	AX_PATH_BDB([4],[
		LIBS="$BDB_LIBS $LIBS"
		LDFLAGS="$BDB_LDFLAGS $LDFLAGS"
		CPPFLAGS="$CPPFLAGS $BDB_CPPFLAGS"
	])
	AC_CHECK_LIB(db_cxx, [open], [],[
		AC_MSG_WARN([You should install db_cxx; please install libraries and development files])
		AM_CONDITIONAL(BERKELEY_DB, false)
	])
	AC_CHECK_HEADERS([db_cxx.h],,[
    	AC_MSG_WARN([db_cxx.h is missing; please add db_cxx development files])
		AM_CONDITIONAL(BERKELEY_DB, false)
	])
fi


# For CppUnit testing
AC_ARG_ENABLE(cppunit,
  [AS_HELP_STRING([--enable-cppunit], [enable unit testing with cppunit (default: no)])],
  [], [])
#AC_MSG_RESULT($enable_cppunit)
AM_CONDITIONAL(CPPUNIT, test "$enable_cppunit" = yes)
if test "$enable_cppunit" = yes; then
  AM_PATH_CPPUNIT(1.10.2,
          [AM_CONDITIONAL(CPPUNIT, true)],
          [AM_CONDITIONAL(CPPUNIT, false)])
fi

AC_ARG_WITH([openssl],AS_HELP_STRING([--with-openssl], [build with openssl support instead of own implementation]),[
# check for openssl
	PKG_CHECK_MODULES(OPENSSL, [openssl],[
		use_openssl="yes"
    	AC_SUBST(OPENSSL_CFLAGS)
    	AC_SUBST(OPENSSL_LIBS)
    	AC_DEFINE(HAVE_OPENSSL, [1], ["openssl library is available"])
    	AC_CHECK_HEADERS([openssl/sha.h],[AC_DEFINE(HAVE_OPENSSL_SHA, [1], ["sha.h is available"])], [AC_MSG_ERROR([openssl extension requested, but no header files found. ssl extensions are disabled.])]) 
    	], [
    	use_openssl="no"
    	AC_MSG_WARN([openssl extension requested, but no openssl library is found. ssl extensions are disabled.])
	])
],[
	use_openssl="no"
])

AC_ARG_WITH(sqlite,
	AS_HELP_STRING([--with-sqlite], [Build with sqlite support
		@<:@default=no@:>@]),
	[
	if test "x$with_sqlite" = "xyes"; then
		PKG_CHECK_MODULES(SQLITE, sqlite3 >= 3.6, [
			use_sqlite="yes"
			AC_SUBST(SQLITE_CFLAGS)
			AC_SUBST(SQLITE_LIBS)
			AC_DEFINE(HAVE_SQLITE, [1], ["sqlite library is available"])
		], [
			use_sqlite="no"
			AC_MSG_WARN([sqlite extension requested, but no sqlite library is found.])
		])
	else
		use_sqlite="no"
	fi
	],[
	use_sqlite="no"
])

AM_CONDITIONAL(OPENSSL, test x$use_openssl = xyes)
AM_CONDITIONAL(SQLITE, test x$use_sqlite = xyes)
AC_ARG_ENABLE(evaluation,
  [AS_HELP_STRING([--enable-evaluation], [enable evaluation (default: no)])],
  [
  	AC_CHECK_FUNCS([mkdir])
  	AC_CHECK_FUNCS([clock_gettime])
  ], [])
#AC_MSG_RESULT($enable_evaluation)

AM_CONDITIONAL(EVALUATION, test x$enable_evaluation = xyes)

AC_ARG_ENABLE(tools,
  [AS_HELP_STRING([--enable-tools], [enable tools (default: no)])],
  [], [])
#AC_MSG_RESULT($enable_tools)

AM_CONDITIONAL(TOOLS, test x$enable_tools = xyes)

AC_ARG_WITH([leveldb],
            [AS_HELP_STRING([--with-leveldb],
              [enable evaluation support for leveldb])],
            [],
            [with_leveldb=no])
AS_IF([test "x$with_leveldb" != xno],
  		AC_CHECK_HEADERS([leveldb/db.h snappy.h],[
			AC_DEFINE(HAVE_LEVELDB, [1], ["leveldb library is available"])
			AC_SUBST(LEVELDB_CFLAGS,[])
			AC_SUBST(LEVELDB_LIBS,["-lleveldb -lsnappy"])
		],[
			with_leveldb=no
		])
	)

AM_CONDITIONAL(LEVELDB, test x$with_leveldb = xyes)


AC_CONFIG_FILES([Makefile \
	setsync/Makefile \
	setsync/bloom/Makefile \
	setsync/trie/Makefile \
	setsync/index/Makefile \
	setsync/utils/Makefile \
	setsync/tests/Makefile \
	setsync/tests/unittests/Makefile \
	setsync/tests/evaluation/Makefile \
	tools/Makefile \
	setsync.pc])
AC_OUTPUT
