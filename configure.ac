dnl Process this file with autoconf to produce a configure script.
AC_INIT([scastd],[1.0])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/scastd.cpp])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
AC_CANONICAL_HOST
AM_PROG_LIBTOOL

dnl Use a modern C++ compiler with Linux specific options
CXXFLAGS="$CXXFLAGS -std=c++17 -Wall -D_GNU_SOURCE"

dnl Checks for required libraries
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0], [], [AC_MSG_ERROR([libxml2 library not found; install libxml2-dev.])])
CXXFLAGS="$CXXFLAGS $LIBXML2_CFLAGS"
LIBS="$LIBS $LIBXML2_LIBS"

PKG_CHECK_MODULES([LIBCURL], [libcurl], [], [AC_MSG_ERROR([libcurl library not found; install libcurl development files.])])
CXXFLAGS="$CXXFLAGS $LIBCURL_CFLAGS"
LIBS="$LIBS $LIBCURL_LIBS"

PKG_CHECK_MODULES([MYSQLCLIENT], [mysqlclient],
  [CXXFLAGS="$CXXFLAGS $MYSQLCLIENT_CFLAGS"
   LIBS="$LIBS $MYSQLCLIENT_LIBS"],
  [PKG_CHECK_MODULES([MYSQLCLIENT], [libmariadb],
     [CXXFLAGS="$CXXFLAGS $MYSQLCLIENT_CFLAGS"
      LIBS="$LIBS $MYSQLCLIENT_LIBS"],
     [AC_MSG_ERROR([MySQL client library not found; install libmysqlclient-dev or libmariadb-dev.])])])

PKG_CHECK_MODULES([LIBPQ], [libpq], [], [AC_MSG_ERROR([PostgreSQL client library (libpq) not found; install libpq-dev.])])
CXXFLAGS="$CXXFLAGS $LIBPQ_CFLAGS"
LIBS="$LIBS $LIBPQ_LIBS"

PKG_CHECK_MODULES([LIBMICROHTTPD], [libmicrohttpd], [], [AC_MSG_ERROR([libmicrohttpd library not found; install libmicrohttpd-dev.])])
CXXFLAGS="$CXXFLAGS $LIBMICROHTTPD_CFLAGS"
LIBS="$LIBS $LIBMICROHTTPD_LIBS"

PKG_CHECK_MODULES([SQLITE3], [sqlite3], [], [AC_MSG_ERROR([sqlite3 library not found; install libsqlite3-dev.])])
CXXFLAGS="$CXXFLAGS $SQLITE3_CFLAGS"
LIBS="$LIBS $SQLITE3_LIBS"

PKG_CHECK_MODULES([OPENSSL], [openssl], [], [AC_MSG_ERROR([OpenSSL library not found; install libssl-dev.])])
CXXFLAGS="$CXXFLAGS $OPENSSL_CFLAGS"
LIBS="$LIBS $OPENSSL_LIBS"

AC_CHECK_LIB([syslog], [openlog], [SYSLOG_LIBS="-lsyslog"], [SYSLOG_LIBS=""])
AC_SUBST([SYSLOG_LIBS])


dnl Make substitutions
AC_SUBST(CXXFLAGS)
AC_SUBST(LIBS)

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile scripts/Makefile])
AC_OUTPUT

