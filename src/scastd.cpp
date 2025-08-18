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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <signal.h>
#include "db/IDatabase.h"
#include "db/MariaDBDatabase.h"
#include "db/PostgresDatabase.h"
#include "db/SQLiteDatabase.h"
#include "Config.h"

#include "CurlClient.h"
#include "HttpServer.h"
#include "icecast2.h"
#include "i18n.h"
#include "scastd.h"
#include "logger.h"
#include "StatusLogger.h"

#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <utility>
#include <tuple>
#include <map>
#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iostream>

namespace scastd {

Logger logger(false);
StatusLogger statusLogger("logs/status.json");
int     paused = 0;
int     exiting = 0;
volatile sig_atomic_t reloadConfig = 0;

std::mutex dbMutex;



static std::string shellEscape(const std::string &in) {
        std::string out = "'";
        for (char c : in) {
                if (c == '\'') out += "'\\''";
                else out += c;
        }
        out += "'";
        return out;
}

static std::string trim(const std::string &s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
}

bool setupDatabase(const std::string &dbType, IDatabase *db) {
        std::string type = dbType;
        if (type == "sqlite3") type = "sqlite";
        std::string script = "/etc/scastd/" + type + ".sql";
        std::ifstream sqlFile(script);
        if (!sqlFile) {
                script = "src/" + type + ".sql";
                sqlFile.open(script);
        }
        if (!sqlFile) {
                logger.logError(std::string("Cannot open SQL script ") + script);
                return false;
        }
        std::stringstream buffer; buffer << sqlFile.rdbuf();
        std::string content = buffer.str();
        if (type == "sqlite") {
                return db->query(content);
        }
        std::string statement;
        bool inString = false; char quote = 0;
        for (size_t i = 0; i < content.size(); ++i) {
                char c = content[i];
                if (inString) {
                        if (c == quote) inString = false;
                        if (c == '\\' && i + 1 < content.size()) { statement += c; statement += content[++i]; continue; }
                        statement += c;
                        continue;
                }
                if (c == '\'' || c == '"') { inString = true; quote = c; statement += c; continue; }
                if (c == '-' && i + 1 < content.size() && content[i+1] == '-') {
                        while (i < content.size() && content[i] != '\n') i++;
                        continue;
                }
                if (c == '#') {
                        while (i < content.size() && content[i] != '\n') i++;
                        continue;
                }
                if (c == '/' && i + 1 < content.size() && content[i+1] == '*') {
                        i += 2;
                        while (i + 1 < content.size() && !(content[i] == '*' && content[i+1] == '/')) i++;
                        i++;
                        continue;
                }
                if (c == ';') {
                        std::string stmt = trim(statement);
                        if (!stmt.empty() && !db->query(stmt)) {
                                return false;
                        }
                        statement.clear();
                        continue;
                }
                statement += c;
        }
        std::string stmt = trim(statement);
        if (!stmt.empty() && !db->query(stmt)) return false;
        return true;
}

int dumpDatabase(const std::string &configPath,
                 const std::map<std::string, std::string> &overrides,
                 const std::string &dumpDir,
                 bool defaultConsoleLog) {
        init_i18n();
        Config cfg;
        if (!cfg.Load(configPath)) {
                std::string msg = std::string(_("Cannot load config file ")) + configPath;
                logger.logError(msg);
		std::cerr << msg << std::endl;
                return 1;
        }
        for (const auto &kv : overrides) {
                cfg.Set(kv.first, kv.second);
        }
        std::string logDir = cfg.Get("log_dir", "/var/log/scastd");
        bool loggingEnabled = true;
        logger.setLogDir(logDir);
        logger.setLogFiles(cfg.AccessLog(), cfg.ErrorLog(), cfg.DebugLog());
        logger.setConsoleOutput(cfg.Get("log_console", defaultConsoleLog));
        logger.setDebugLevel(cfg.DebugLevel());
        logger.setRotation(cfg.LogMaxSize(), cfg.LogRetention());
        statusLogger.setPath(logDir + "/status.json");
        statusLogger.setRotation(cfg.LogMaxSize(), cfg.LogRetention());
        if (cfg.SyslogEnabled()) {
                Logger::SyslogProto proto = cfg.SyslogProtocol() == "tcp" ?
                        Logger::SyslogProto::TCP : Logger::SyslogProto::UDP;
                logger.setSyslog(cfg.SyslogHost(), cfg.SyslogPort(), proto);
        }
        logger.setEnabled(loggingEnabled);

        std::string dbUser = cfg.Get("username", "");
        std::string dbPass = cfg.Get("password", "");
        std::string dbType = cfg.Get("DatabaseType", "");
        std::string dbHost = cfg.Get("host", "");
        int dbPort = cfg.Get("port", 0);
        std::string dbName = cfg.Get("dbname", "");
        std::string sqlitePath = cfg.Get("sqlite_path", "/etc/scastd/scastd.db");
        bool useSqlite = dbType == "sqlite" || dbUser.empty() || dbHost.empty() || dbName.empty();
        if (useSqlite) {
                dbType = "sqlite";
                dbName = sqlitePath;
        }

        mkdir(dumpDir.c_str(), 0755);
        std::time_t now = std::time(nullptr);
        std::tm tm{};
        localtime_r(&now, &tm);
        char ts[32];
        std::strftime(ts, sizeof(ts), "%Y%m%d-%H%M%S", &tm);
        std::string dumpFile = dumpDir + "/scastd-" + ts + ".sql";

        std::string cmd;
        if (dbType == "postgres") {
                cmd = "PGPASSWORD=" + shellEscape(dbPass) + " pg_dump";
                if (!dbHost.empty()) cmd += " -h " + shellEscape(dbHost);
                if (dbPort > 0) cmd += " -p " + std::to_string(dbPort);
                if (!dbUser.empty()) cmd += " -U " + shellEscape(dbUser);
                cmd += " " + shellEscape(dbName) + " > " + shellEscape(dumpFile);
        } else if (dbType == "sqlite") {
                cmd = "sqlite3 " + shellEscape(dbName) + " .dump > " + shellEscape(dumpFile);
        } else {
               cmd = "MARIADB_PWD=" + shellEscape(dbPass) + " mariadb-dump";
                if (!dbHost.empty()) cmd += " -h " + shellEscape(dbHost);
                if (dbPort > 0) cmd += " -P " + std::to_string(dbPort);
                if (!dbUser.empty()) cmd += " -u " + shellEscape(dbUser);
                cmd += " " + shellEscape(dbName) + " > " + shellEscape(dumpFile);
        }

        int rc = system(cmd.c_str());
        if (rc == 0) {
                std::string msg = std::string("Database dump successful: ") + dumpFile;
                logger.logAccess(msg);
        } else {
                std::string msg = std::string("Database dump failed: ") + dumpFile;
                logger.logError(msg);
        }
        return rc == 0 ? 0 : 1;
}

static std::vector<Icecast2::StreamInfo>
processServer(const std::string &host,
              int port,
              const std::string &user,
              const std::string &pass) {
        CurlClient curl;
        Icecast2 server(host, port, user, pass, curl);
        std::vector<Icecast2::StreamInfo> stats;
        std::string error;
        if (!server.fetchStats(stats, error)) {
                char buf[1024];
                snprintf(buf, sizeof(buf), _("Failed to fetch data from %s:%d - %s"), host.c_str(), port, error.c_str());
                logger.logError(buf);
                statusLogger.log("poll", "error", host + ":" + std::to_string(port));
                return {};
        }
        statusLogger.log("poll", "ok", host + ":" + std::to_string(port));
        return stats;
}

void sigUSR1(int sig)
{
        if (paused) {
                logger.logAccess(_("Caught SIGUSR1 - Resuming\n"));
                paused = 0;
        }
        else {
                logger.logAccess(_("Caught SIGUSR1 - Pausing\n"));
                paused = 1;
        }
	
}
void sigUSR2(int sig)
{
        logger.logAccess(_("Caught SIGUSR2 - Exiting\n"));
        exiting = 1;

}

void sigTERM(int sig)
{
        logger.logAccess(_("Caught SIGTERM - Exiting\n"));
        exiting = 1;
}

void sigINT(int sig)
{
        logger.logAccess(_("Caught SIGINT - Exiting\n"));
        exiting = 1;
}

void sigHUP(int sig)
{
        logger.logAccess(_("Caught SIGHUP - Reloading config\n"));
	reloadConfig = 1;
}


int run(const std::string &configPath,
        const std::map<std::string, std::string> &overrides,
        bool defaultConsoleLog)
{
        init_i18n();
        scastd::HttpServer httpServer;
	IDatabase       *db = NULL;
	IDatabase       *db2 = NULL;
        Config  cfg;

	char	buf[1024];
	IDatabase::Row       row;
	char	query[2046] = "";
	logger.setConsoleOutput(defaultConsoleLog);
	logger.setEnabled(true);
        if (!cfg.Load(configPath)) {
                std::string msg = std::string(_("Cannot load config file ")) + configPath;
		logger.logError(msg);
		std::cerr << msg << std::endl;
		return 1;
	}
        for (const auto &kv : overrides) {
                cfg.Set(kv.first, kv.second);
        }
	int poll_ms = cfg.GetDuration("poll_interval", 60000);
        std::string accessLog = cfg.AccessLog();
        std::string errorLog = cfg.ErrorLog();
        std::string debugLog = cfg.DebugLog();
        bool consoleFlag = cfg.Get("log_console", defaultConsoleLog);
        int debugLevel = cfg.DebugLevel();
        std::string logDir = cfg.Get("log_dir", "/var/log/scastd");
        bool loggingEnabled = true;
        logger.setLogDir(logDir);
        logger.setLogFiles(accessLog, errorLog, debugLog);
        logger.setConsoleOutput(consoleFlag);
        logger.setDebugLevel(debugLevel);
        logger.setRotation(cfg.LogMaxSize(), cfg.LogRetention());
        statusLogger.setPath(logDir + "/status.json");
        statusLogger.setRotation(cfg.LogMaxSize(), cfg.LogRetention());
        if (cfg.SyslogEnabled()) {
                Logger::SyslogProto proto = cfg.SyslogProtocol() == "tcp" ?
                        Logger::SyslogProto::TCP : Logger::SyslogProto::UDP;
                logger.setSyslog(cfg.SyslogHost(), cfg.SyslogPort(), proto);
        }
        logger.setEnabled(loggingEnabled);
        logger.logDebug("scastd starting up");

        int threadCount = cfg.Get("thread_count", static_cast<int>(std::thread::hardware_concurrency()));
        int cpuCores = cfg.Get("cpu_cores", 0);
        int memoryLimit = cfg.Get("memory_limit", 0);
#if defined(__linux__)
        if (cpuCores > 0) {
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                for (int i = 0; i < cpuCores; ++i) {
                        CPU_SET(i, &cpuset);
                }
                sched_setaffinity(0, sizeof(cpuset), &cpuset);
        }
#endif
        if (memoryLimit > 0) {
                struct rlimit rl;
                rl.rlim_cur = rl.rlim_max = static_cast<rlim_t>(memoryLimit) * 1024 * 1024;
                setrlimit(RLIMIT_AS, &rl);
        }
        bool httpEnabled = cfg.Get("http_enabled", true);
        int httpPort = cfg.Get("http_port", 8333);
        std::string httpIp = cfg.Get("ip", "");
        std::string httpUser = cfg.Get("http_username", "");
        std::string httpPass = cfg.Get("http_password", "");
        bool sslEnabled = cfg.Get("ssl_enabled", false);
        std::string sslCert = cfg.Get("ssl_cert", "");
        std::string sslKey = cfg.Get("ssl_key", "");
        if (httpEnabled) {
                if (!httpServer.start(httpPort, httpIp, httpUser, httpPass,
                                       threadCount,
                                       sslEnabled, sslCert, sslKey)) {
                        char lbuf[256];
                        snprintf(lbuf, sizeof(lbuf), _("Failed to start HTTP%s server on %s:%d"),
                                sslEnabled ? "S" : "",
                                httpIp.empty() ? "*" : httpIp.c_str(), httpPort);
                        logger.logError(lbuf);
                }
        }
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
                dbUser.clear();
                dbPass.clear();
                dbHost.clear();
                dbPort = 0;
                dbSSLMode.clear();
        }
        if (dbType == "postgres") {
                db = new PostgresDatabase();
                db2 = new PostgresDatabase();
        } else if (dbType == "sqlite") {
                db = new SQLiteDatabase();
                db2 = new SQLiteDatabase();
        } else {
                db = new MariaDBDatabase();
                db2 = new MariaDBDatabase();
                dbType = "mariadb";
        }
        bool needInit = false;
        if (dbType == "sqlite") {
                std::string dir = dbName.substr(0, dbName.find_last_of('/'));
                if (!dir.empty()) {
                        mkdir(dir.c_str(), 0755);
                }
                if (access(dbName.c_str(), F_OK) != 0) {
                        needInit = true;
                }
        }

        if (defaultConsoleLog) {
                logger.logDebug(_("Detaching from console..."));
                if (fork()) {
                        exit(0);
                }
        }

        if (signal(SIGUSR1, sigUSR1) == SIG_ERR) {
                logger.logError(_("Cannot install handler for SIGUSR1"));
                exit(1);
        }
        if (signal(SIGUSR2, sigUSR2) == SIG_ERR) {
                logger.logError(_("Cannot install handler for SIGUSR2"));
                exit(1);
        }
        if (signal(SIGTERM, sigTERM) == SIG_ERR) {
                logger.logError(_("Cannot install handler for SIGTERM"));
                exit(1);
        }
        if (signal(SIGINT, sigINT) == SIG_ERR) {
                logger.logError(_("Cannot install handler for SIGINT"));
                exit(1);
        }
        if (signal(SIGHUP, sigHUP) == SIG_ERR) {
                logger.logError(_("Cannot install handler for SIGHUP"));
                exit(1);
        }
        db->connect(dbUser, dbPass, dbHost, dbPort, dbName, dbSSLMode);
        db2->connect(dbUser, dbPass, dbHost, dbPort, dbName, dbSSLMode);
        if (dbType == "sqlite" && needInit) {
                setupDatabase("sqlite", db);
        }
	

        logger.logAccess(_("SCASTD starting...\n"));

	while (1) {
                if (reloadConfig) {
                        reloadConfig = 0;
                        Config newCfg;
                        if (newCfg.Load(configPath)) {
                                for (const auto &kv : overrides) {
                                        newCfg.Set(kv.first, kv.second);
                                }
                                cfg = newCfg;
                                std::string newAccess = cfg.AccessLog();
                                std::string newError = cfg.ErrorLog();
                                std::string newDebug = cfg.DebugLog();
                                bool newConsole = cfg.Get("log_console", consoleFlag);
                                int newDebugLevel = cfg.DebugLevel();
                                if (newAccess != accessLog || newError != errorLog || newDebug != debugLog) {
                                        accessLog = newAccess;
                                        errorLog = newError;
                                        debugLog = newDebug;
                                        logger.setLogFiles(accessLog, errorLog, debugLog);
                                }
                                if (newConsole != consoleFlag) {
                                        consoleFlag = newConsole;
                                        logger.setConsoleOutput(consoleFlag);
                                }
                                if (newDebugLevel != debugLevel) {
                                        debugLevel = newDebugLevel;
                                        logger.setDebugLevel(debugLevel);
                                }
                                if (cfg.SyslogEnabled()) {
                                        Logger::SyslogProto proto = cfg.SyslogProtocol() == "tcp" ?
                                                Logger::SyslogProto::TCP : Logger::SyslogProto::UDP;
                                        logger.setSyslog(cfg.SyslogHost(), cfg.SyslogPort(), proto);
                                } else {
                                        logger.setSyslog("", 0, Logger::SyslogProto::UDP);
                                }
                                poll_ms = cfg.GetDuration("poll_interval", poll_ms);
                                std::string newDbType = cfg.Get("DatabaseType", dbType);
                                std::string newDbUser = cfg.Get("username", dbUser);
                                std::string newDbPass = cfg.Get("password", dbPass);
                                std::string newDbHost = cfg.Get("host", dbHost);
                                int newDbPort = cfg.Get("port", dbPort);
                                std::string newDbName = cfg.Get("dbname", dbName);
                                std::string newDbSSLMode = cfg.Get("sslmode", dbSSLMode);
                                std::string newSqlitePath = cfg.Get("sqlite_path", dbName);
                                bool newUseSqlite = newDbType == "sqlite" || newDbUser.empty() || newDbHost.empty() || newDbName.empty();
                                if (newUseSqlite) {
                                        newDbType = "sqlite";
                                        newDbName = newSqlitePath;
                                        newDbUser.clear();
                                        newDbPass.clear();
                                        newDbHost.clear();
                                        newDbPort = 0;
                                        newDbSSLMode.clear();
                                }
                                bool newNeedInit = false;
                                if (newDbType == "sqlite") {
                                        if (access(newDbName.c_str(), F_OK) != 0) {
                                                newNeedInit = true;
                                        }
                                }
                                if (newDbType != dbType) {
                                        if (db) { db->disconnect(); delete db; }
                                        if (db2) { db2->disconnect(); delete db2; }
                                        if (newDbType == "postgres") {
                                                db = new PostgresDatabase();
                                                db2 = new PostgresDatabase();
                                        } else if (newDbType == "sqlite") {
                                                db = new SQLiteDatabase();
                                                db2 = new SQLiteDatabase();
                                        } else {
                                                db = new MariaDBDatabase();
                                                db2 = new MariaDBDatabase();
                                                newDbType = "mariadb";
                                        }
                                        dbType = newDbType;
                                } else {
                                        if (db) db->disconnect();
                                        if (db2) db2->disconnect();
                                }
                                dbUser = newDbUser;
                                dbPass = newDbPass;
                                dbHost = newDbHost;
                                dbPort = newDbPort;
                                dbName = newDbName;
                                dbSSLMode = newDbSSLMode;
                                db->connect(dbUser, dbPass, dbHost, dbPort, dbName, dbSSLMode);
                                db2->connect(dbUser, dbPass, dbHost, dbPort, dbName, dbSSLMode);
                                if (newNeedInit && dbType == "sqlite") {
                                        setupDatabase("sqlite", db);
                                }
                                logger.logDebug(_("Configuration reloaded\n"));
                        } else {
                                logger.logError(_("Failed to reload config\n"));
                        }
                }
		if (exiting) {
                        logger.logAccess(_("Exiting...\n"));
			break;
		}
		if (!paused) {
                        snprintf(query, sizeof(query), "SELECT server_host, server_port, server_username, server_password FROM servers;");
                        db2->query(query);
                        std::vector<std::tuple<std::string, int, std::string, std::string>> servers;
                        while (true) {
                                if (exiting) break;
                                row = db2->fetch();
                                if (row.empty()) break;
                                int port = row.size() > 1 ? atoi(row[1].c_str()) : 0;
                                std::string user = row.size() > 2 ? row[2] : "";
                                std::string pass = row.size() > 3 ? row[3] : "";
                                servers.emplace_back(row[0], port, user, pass);
                        }
                        std::vector<std::thread> workers;
                        size_t maxThreads = threadCount > 0 ? static_cast<size_t>(threadCount) : 1;
                        for (const auto &srv : servers) {
                                workers.emplace_back([&, srv] {
                                        const std::string &host = std::get<0>(srv);
                                        int port = std::get<1>(srv);
                                        const std::string &user = std::get<2>(srv);
                                        const std::string &pass = std::get<3>(srv);
                                        auto stats = processServer(host, port, user, pass);
                                        if (stats.empty()) return;
                                        std::lock_guard<std::mutex> lock(dbMutex);
                                        for (const auto &info : stats) {
                                                std::string serverURL = std::string("http://") + host + ":" + std::to_string(port) + info.mount;
                                                char q[2046];
                                                snprintf(q, sizeof(q), "select songTitle from scastd_songinfo where serverURL = '%s' order by time desc limit 1", serverURL.c_str());
                                                db->query(q);
                                                IDatabase::Row r = db->fetch();
                                                int insert_flag = 1;
                                                if (!r.empty() && r[0] == info.title) {
                                                        insert_flag = 0;
                                                }
                                                if (insert_flag) {
                                                        snprintf(q, sizeof(q), "insert into scastd_songinfo values('%s', '%s', NULL)", serverURL.c_str(), info.title.c_str());
                                                        db->query(q);
                                                }
                                                snprintf(q, sizeof(q),
                                                         "insert into scastd_serverinfo (serverURL, currentlisteners, peaklisteners, maxlisteners, averagetime, streamhits, time) values('%s', %d, %d, %d, %d, %d, NULL)",
                                                         serverURL.c_str(), info.listeners, 0, 0, 0, 0);
                                                db->query(q);
                                        }
                                });
                                if (workers.size() >= maxThreads) {
                                        for (auto &t : workers) {
                                                if (t.joinable()) t.join();
                                        }
                                        workers.clear();
                                }
                        }
                        for (auto &t : workers) {
                                if (t.joinable()) t.join();
                        }
                }
		if (exiting) break;
			
		snprintf(buf, sizeof(buf), _("Sleeping for %d ms\n"), poll_ms);
		logger.logDebug(buf);
		std::this_thread::sleep_for(std::chrono::milliseconds(poll_ms));
	}
        httpServer.stop();
        if (db) {
                db->disconnect();
                delete db;
        }
        if (db2) {
                db2->disconnect();
                delete db2;
        }
        return 0;
}

} // namespace scastd
