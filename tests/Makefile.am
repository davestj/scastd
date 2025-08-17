AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests $(DEPS_CFLAGS) @mariadb_CFLAGS@ -DTEST_SRCDIR=\"$(top_srcdir)\"

check_PROGRAMS = unit_tests
unit_tests_SOURCES = test_main.cpp test_config.cpp test_sql.cpp test_http.cpp test_icecast.cpp test_icecast_dev.cpp test_url_parser.cpp test_db_invalid.cpp test_setupdb.cpp test_missing_config.cpp test_pid.cpp test_logger.cpp test_scastd_cli.cpp \
    ../src/Config.cpp ../src/HttpServer.cpp ../src/icecast2.cpp ../src/UrlParser.cpp \
    ../src/CurlClient.cpp ../src/logger.cpp ../src/StatusLogger.cpp ../src/scastd.cpp \
    ../src/db/MariaDBDatabase.cpp \
    ../src/db/PostgresDatabase.cpp stubs.cpp
unit_tests_LDADD = $(DEPS_LIBS) $(LIBINTL) @mariadb_LIBS@

TESTS = unit_tests
