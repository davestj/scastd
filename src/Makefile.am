bin_PROGRAMS = scastd
scastd_SOURCES = \
    main.cpp \
    scastd.cpp \
    Config.cpp \
    CurlClient.cpp \
    HttpServer.cpp \
    UrlParser.cpp \
    i18n.cpp \
    icecast2.cpp \
    logger.cpp \
    StatusLogger.cpp \
    db/MariaDBDatabase.cpp \
    db/PostgresDatabase.cpp \
    db/SQLiteDatabase.cpp

scastd_CPPFLAGS = @mysqlclient_CFLAGS@
scastd_LDADD = $(SYSLOG_LIBS) $(LIBINTL) @mysqlclient_LIBS@
