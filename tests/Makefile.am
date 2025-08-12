AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests $(DEPS_CFLAGS) @mysqlclient_CFLAGS@ -DTEST_SRCDIR=\"$(top_srcdir)\"

check_PROGRAMS = unit_tests
unit_tests_SOURCES = test_main.cpp test_config.cpp test_sql.cpp test_http.cpp test_icecast.cpp test_url_parser.cpp test_db_invalid.cpp test_setupdb.cpp \
    ../src/Config.cpp ../src/HttpServer.cpp ../src/icecast2.cpp ../src/UrlParser.cpp \
    ../src/CurlClient.cpp ../src/logger.cpp ../src/StatusLogger.cpp \
    ../src/db/MySQLDatabase.cpp ../src/db/MariaDBDatabase.cpp \
    ../src/db/PostgresDatabase.cpp
unit_tests_LDADD = $(DEPS_LIBS) $(LIBINTL) @mysqlclient_LIBS@

TESTS = unit_tests
