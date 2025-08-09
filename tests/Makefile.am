AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests $(DEPS_CFLAGS) -DTEST_SRCDIR=\"$(top_srcdir)\"

check_PROGRAMS = unit_tests
unit_tests_SOURCES = test_main.cpp test_config.cpp test_sql.cpp test_http.cpp test_icecast.cpp \
    ../src/Config.cpp ../src/HttpServer.cpp ../src/icecast2.cpp
unit_tests_LDADD = $(DEPS_LIBS)

TESTS = unit_tests
