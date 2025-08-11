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
    db/MySQLDatabase.cpp \
    db/MariaDBDatabase.cpp \
    db/PostgresDatabase.cpp \
    db/SQLiteDatabase.cpp

scastd_LDADD = $(SYSLOG_LIBS) $(LIBINTL)
