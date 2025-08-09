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
PKG_CHECK_MODULES([DEPS], [libxml-2.0 libcurl mysqlclient libpq libmicrohttpd])
CXXFLAGS="$CXXFLAGS $DEPS_CFLAGS"
LIBS="$LIBS $DEPS_LIBS"

dnl Make substitutions
AC_SUBST(CXXFLAGS)
AC_SUBST(LIBS)

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT

