/*
/////////////////////////////////////////////////
// Scast Daemon
// Authors: oddsock, dstjohn
/////////////////////////////////////////////////

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "scastd.h"
#include "Config.h"
#include "logger.h"
#include "db/IDatabase.h"
#include "db/MariaDBDatabase.h"
#include "db/PostgresDatabase.h"
#include "db/SQLiteDatabase.h"
#include <getopt.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <filesystem>
#include <system_error>
#include <cerrno>
#include <cstring>
#include <unistd.h>

namespace scastd { extern Logger logger; }

static void print_usage(const char *prog) {
    std::cout << "Usage: " << prog << " [options]\n"
              << "  -h, --help           Show this help and exit\n"
              << "  -c, --config PATH    Configuration file\n"
              << "  -D, --daemon         Run as a daemon\n"
              << "      --pid-file PATH  PID file path (used with --daemon)\n"
              << "      --ip ADDRESS     Bind IP address\n"
              << "      --port PORT      HTTP server port\n"
              << "      --debug LEVEL    Debug level\n"
              << "      --poll INTERVAL  Poll interval (e.g., 60s, 5m)\n"
              << "      --test-mode      Validate configuration and exit\n"
              << "      --db-type TYPE   Database type\n"
              << "      --db-host HOST   Database host\n"
              << "      --db-port PORT   Database port\n"
              << "      --db-name NAME   Database name\n"
              << "      --db-user USER   Database user\n"
              << "      --db-pass PASS   Database password\n"
              << "      --sqlite-db PATH SQLite database file\n"
              << "      --setupdb TYPE   Initialize database of specified type\n"
              << "      --ssl-cert PATH  SSL certificate file\n"
              << "      --ssl-key PATH   SSL key file\n"
              << "      --ssl-enable     Enable SSL\n"
              << "      --dump           Dump database and exit\n"
              << "      --dump-dir DIR   Directory for database dump\n";
}

static bool validateConfig(const Config &cfg) {
    std::string dbUser = cfg.Get("username", "");
    std::string dbPass = cfg.Get("password", "");
    std::string dbType = cfg.Get("DatabaseType", "");
    std::string dbHost = cfg.Get("host", "");
    int dbPort = cfg.Get("port", 0);
    std::string dbName = cfg.Get("dbname", "");
    std::string dbSSLMode = cfg.Get("sslmode", "");
    std::string sqlitePath = cfg.Get("sqlite_path", "/etc/scastd/scastd.db");
    bool useSqlite = dbType == "sqlite" || dbUser.empty() || dbHost.empty() || dbName.empty();
    if (useSqlite) {
        dbType = "sqlite";
        dbName = sqlitePath;
    }
    IDatabase *db = nullptr;
    if (dbType == "postgres") db = new PostgresDatabase();
    else if (dbType == "sqlite") db = new SQLiteDatabase();
    else db = new MariaDBDatabase();

    bool ok = db->connect(dbUser, dbPass, dbHost, dbPort, dbName, dbSSLMode);
    if (ok) {
        db->disconnect();
        scastd::logger.logDebug("Test-mode: database connection succeeded");
    } else {
        scastd::logger.logError("Test-mode: database connection failed");
    }
    delete db;
    return ok;
}

int main(int argc, char **argv) {
    std::string configPath = "scastd.conf";
    std::string dumpDir = "/tmp";
    bool doDump = false;
    bool testMode = false;
    bool daemonMode = false;
    std::string pidPath = "/var/run/scastd.pid";
    std::string setupdbType;
    std::map<std::string, std::string> overrides;

    enum {
        OPT_DB_TYPE = 1000,
        OPT_DB_HOST,
        OPT_DB_PORT,
        OPT_DB_NAME,
        OPT_DB_USER,
        OPT_DB_PASS,
        OPT_SQLITE_DB,
        OPT_SETUPDB,
        OPT_SSL_CERT,
        OPT_SSL_KEY,
        OPT_SSL_ENABLE,
        OPT_DUMP,
        OPT_DUMP_DIR,
        OPT_POLL_INTERVAL,
        OPT_PID_FILE
    };

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"config", required_argument, 0, 'c'},
        {"ip", required_argument, 0, 'i'},
        {"port", required_argument, 0, 'p'},
        {"debug", required_argument, 0, 'd'},
        {"test-mode", no_argument, 0, 't'},
        {"daemon", no_argument, 0, 'D'},
        {"db-type", required_argument, 0, OPT_DB_TYPE},
        {"db-host", required_argument, 0, OPT_DB_HOST},
        {"db-port", required_argument, 0, OPT_DB_PORT},
        {"db-name", required_argument, 0, OPT_DB_NAME},
        {"db-user", required_argument, 0, OPT_DB_USER},
        {"db-pass", required_argument, 0, OPT_DB_PASS},
        {"sqlite-db", required_argument, 0, OPT_SQLITE_DB},
        {"setupdb", required_argument, 0, OPT_SETUPDB},
        {"ssl-cert", required_argument, 0, OPT_SSL_CERT},
        {"ssl-key", required_argument, 0, OPT_SSL_KEY},
        {"ssl-enable", no_argument, 0, OPT_SSL_ENABLE},
        {"dump", no_argument, 0, OPT_DUMP},
        {"dump-dir", required_argument, 0, OPT_DUMP_DIR},
        {"poll", required_argument, 0, OPT_POLL_INTERVAL},
        {"pid-file", required_argument, 0, OPT_PID_FILE},
        {0, 0, 0, 0}
    };

    int opt;
    int long_index = 0;
    while ((opt = getopt_long(argc, argv, "hc:i:p:d:tD", long_options, &long_index)) != -1) {
        switch (opt) {
        case 'h':
            print_usage(argv[0]);
            return 0;
        case 'c':
            configPath = optarg;
            break;
        case 'i':
            overrides["ip"] = optarg;
            break;
        case 'p':
            overrides["http_port"] = optarg;
            break;
        case 'd':
            overrides["debug_level"] = optarg;
            break;
        case 't':
            testMode = true;
            break;
        case 'D':
            daemonMode = true;
            break;
        case OPT_DB_TYPE:
            overrides["DatabaseType"] = optarg;
            break;
        case OPT_DB_HOST:
            overrides["host"] = optarg;
            break;
        case OPT_DB_PORT:
            overrides["port"] = optarg;
            break;
        case OPT_DB_NAME:
            overrides["dbname"] = optarg;
            break;
        case OPT_DB_USER:
            overrides["username"] = optarg;
            break;
        case OPT_DB_PASS:
            overrides["password"] = optarg;
            break;
        case OPT_SQLITE_DB:
            overrides["sqlite_path"] = optarg;
            break;
        case OPT_SETUPDB:
            setupdbType = optarg;
            break;
        case OPT_SSL_CERT:
            overrides["ssl_cert"] = optarg;
            break;
        case OPT_SSL_KEY:
            overrides["ssl_key"] = optarg;
            break;
        case OPT_SSL_ENABLE:
            overrides["ssl_enabled"] = "true";
            break;
        case OPT_DUMP:
            doDump = true;
            break;
        case OPT_DUMP_DIR:
            dumpDir = optarg;
            break;
        case OPT_POLL_INTERVAL:
            overrides["poll_interval"] = optarg;
            break;
        case OPT_PID_FILE:
            overrides["pid_file"] = optarg;
            pidPath = optarg;
            break;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }

    scastd::logger.setConsoleOutput(!daemonMode);

    Config cfg;
    if (cfg.Load(configPath)) {
        for (const auto &kv : overrides) {
            cfg.Set(kv.first, kv.second);
        }
        pidPath = cfg.PIDFile();
        scastd::logger.setLogFiles(cfg.AccessLog(), cfg.ErrorLog(), cfg.DebugLog());
        scastd::logger.setDebugLevel(cfg.DebugLevel());
        if (cfg.SyslogEnabled()) {
            Logger::SyslogProto proto = cfg.SyslogProtocol() == "tcp" ?
                                        Logger::SyslogProto::TCP : Logger::SyslogProto::UDP;
            scastd::logger.setSyslog(cfg.SyslogHost(), cfg.SyslogPort(), proto);
        }
    } else {
        scastd::logger.logError(std::string("Cannot load config file ") + configPath);
    }

    if (!setupdbType.empty()) {
        std::string dbType = setupdbType;
        std::string dbUser = cfg.Get("username", "");
        std::string dbPass = cfg.Get("password", "");
        std::string dbHost = cfg.Get("host", "");
        int dbPort = cfg.Get("port", 0);
        std::string dbName = cfg.Get("dbname", "");
        std::string dbSSLMode = cfg.Get("sslmode", "");
        std::string sqlitePath = cfg.Get("sqlite_path", "/etc/scastd/scastd.db");
        if (dbType == "sqlite3" || dbType == "sqlite") {
            dbType = "sqlite";
            dbName = sqlitePath;
        }
        IDatabase *db = nullptr;
        if (dbType == "postgres") db = new PostgresDatabase();
        else if (dbType == "sqlite") db = new SQLiteDatabase();
        else db = new MariaDBDatabase();
        bool ok = db->connect(dbUser, dbPass, dbHost, dbPort, dbName, dbSSLMode);
        if (ok) {
            ok = scastd::setupDatabase(setupdbType, db);
            db->disconnect();
        }
        delete db;
        return ok ? 0 : 1;
    }

    if (testMode) {
        return validateConfig(cfg) ? 0 : 1;
    }

    if (doDump) {
        return scastd::dumpDatabase(configPath, overrides, dumpDir, !daemonMode);
    }

    if (daemonMode) {
        if (daemon(0, 0) != 0) {
            scastd::logger.logError("Failed to daemonize");
            return 1;
        }
        std::ofstream pidFile(pidPath);
        if (pidFile.is_open()) {
            pidFile << getpid();
            pidFile.close();
        } else {
            scastd::logger.logError(std::string("Cannot write PID file ") + pidPath + ": " + std::strerror(errno));
        }
    }

    int rc = scastd::run(configPath, overrides, !daemonMode);
    if (daemonMode) {
        std::error_code ec;
        std::filesystem::remove(pidPath, ec);
    }
    return rc;
}
