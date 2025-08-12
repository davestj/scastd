dnl Process this file with autoconf to produce a configure script.
AC_INIT([scastd],[1.0])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([src/scastd.cpp])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
AC_CANONICAL_HOST
LT_INIT

case $host_os in
  darwin*)
    AC_MSG_NOTICE([macOS detected - using Homebrew paths])
    CPPFLAGS="$CPPFLAGS -I/opt/homebrew/include"
    LDFLAGS="$LDFLAGS -L/opt/homebrew/lib"
    PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:/opt/homebrew/share/pkgconfig:${PKG_CONFIG_PATH}"
    export PKG_CONFIG_PATH
    ;;
esac

dnl Use a modern C++ compiler with Linux specific options
CXXFLAGS="$CXXFLAGS -std=c++17 -Wall -D_GNU_SOURCE"

dnl Checks for required libraries
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0], [], [AC_MSG_ERROR([libxml2 library not found; install libxml2-dev.])])
CXXFLAGS="$CXXFLAGS $LIBXML2_CFLAGS"
LIBS="$LIBS $LIBXML2_LIBS"

PKG_CHECK_MODULES([LIBCURL], [libcurl], [], [AC_MSG_ERROR([libcurl library not found; install libcurl development files.])])
CXXFLAGS="$CXXFLAGS $LIBCURL_CFLAGS"
LIBS="$LIBS $LIBCURL_LIBS"

PKG_CHECK_MODULES([mysqlclient], [mysqlclient], [], [
  AC_CHECK_HEADERS([mysql/mysql.h], [], [AC_MSG_ERROR([MySQL headers not found])])
  AC_CHECK_LIB([mysqlclient], [mysql_init], [], [AC_MSG_ERROR([MySQL library not found])])
])

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

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.21])
AC_SUBST([LIBINTL])


dnl Make substitutions
AC_SUBST(CXXFLAGS)
AC_SUBST(LIBS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

AC_CONFIG_FILES([Makefile src/Makefile scripts/Makefile])
AC_OUTPUT

