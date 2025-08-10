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
#include <libxml/nanohttp.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <signal.h>
#include "db/IDatabase.h"
#include "db/MySQLDatabase.h"
#include "db/MariaDBDatabase.h"
#include "db/PostgresDatabase.h"
#include "db/SQLiteDatabase.h"
#include "Config.h"

#include "CurlClient.h"
#include "HttpServer.h"
#include "UrlParser.h"
#include "i18n.h"
#include "scastd.h"
#include "logger.h"

#include <thread>
#include <mutex>
#include <vector>
#include <utility>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <ctime>

namespace scastd {

Logger logger(false);
int     paused = 0;
int     exiting = 0;
volatile sig_atomic_t reloadConfig = 0;

std::mutex dbMutex;

typedef struct tagServerData {
        int     currentListeners;
        int     peakListeners;
        int     maxListeners;
        int     reportedListeners;
        int     avgTime;
        int     webHits;
        int     streamHits;
        char    songTitle[1024];
} serverData;

void parseWebdata(xmlNodePtr cur)
{
    while (cur != NULL) {
        char buf[256];
        sprintf(buf, _("Webdata: %s"), cur->name);
        logger.logDebug(buf);
        cur = cur->next;
    }
}
void trimRight(char *buf) {
        for (char *p1 = buf + strlen(buf)-1;p1 > buf; p1--) {
                if (*p1 != ' ') {
                        break;
                }
                else {
                        *p1 = '\000';
                }
        }
}

static std::string shellEscape(const std::string &in) {
        std::string out = "'";
        for (char c : in) {
                if (c == '\'') out += "'\\''";
                else out += c;
        }
        out += "'";
        return out;
}

int dumpDatabase(const std::string &configPath, const std::string &dumpDir) {
        init_i18n();
        Config cfg;
        if (!cfg.Load(configPath)) {
                std::string msg = std::string(_("Cannot load config file ")) + configPath;
                logger.logError(msg);
                return 1;
        }
        logger.setLogFiles(cfg.AccessLog(), cfg.ErrorLog(), cfg.DebugLog());
        logger.setConsoleOutput(cfg.Get("log_console", false));
        logger.setDebugLevel(cfg.DebugLevel());
        if (cfg.SyslogEnabled()) {
                Logger::SyslogProto proto = cfg.SyslogProtocol() == "tcp" ?
                        Logger::SyslogProto::TCP : Logger::SyslogProto::UDP;
                logger.setSyslog(cfg.SyslogHost(), cfg.SyslogPort(), proto);
        }
        logger.setEnabled(true);

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
        if (dbType == "mysql" || dbType == "mariadb") {
                cmd = "MYSQL_PWD=" + shellEscape(dbPass) + " mysqldump";
                if (!dbHost.empty()) cmd += " -h " + shellEscape(dbHost);
                if (dbPort > 0) cmd += " -P " + std::to_string(dbPort);
                if (!dbUser.empty()) cmd += " -u " + shellEscape(dbUser);
                cmd += " " + shellEscape(dbName) + " > " + shellEscape(dumpFile);
        } else if (dbType == "postgres") {
                cmd = "PGPASSWORD=" + shellEscape(dbPass) + " pg_dump";
                if (!dbHost.empty()) cmd += " -h " + shellEscape(dbHost);
                if (dbPort > 0) cmd += " -p " + std::to_string(dbPort);
                if (!dbUser.empty()) cmd += " -U " + shellEscape(dbUser);
                cmd += " " + shellEscape(dbName) + " > " + shellEscape(dumpFile);
        } else {
                cmd = "sqlite3 " + shellEscape(dbName) + " .dump > " + shellEscape(dumpFile);
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

static void processServer(const std::string &serverURL,
                          const std::string &password,
                          IDatabase *db) {
        char IP[255] = "";
        int port = 0;
        std::string ipStr;
        if (parseHostPort(serverURL, ipStr, port)) {
                strncpy(IP, ipStr.c_str(), sizeof(IP) - 1);
                IP[sizeof(IP) - 1] = '\0';
        }
        char buf[1024];
        sprintf(buf, _("Connecting to server %s at port %d\n"), IP, port);
        logger.logDebug(buf);

        std::string urlV1 = std::string("http://") + IP + ":" + std::to_string(port) +
                            "/admin.cgi?pass=" + password + "&mode=viewxml";
        std::string urlV2 = std::string("http://") + IP + ":" + std::to_string(port) +
                            "/admin.cgi?mode=viewxml&sid=1&pass=" + password;
        std::string response;
        CurlClient curl;
        bool fetched = curl.fetchUrl(urlV1, response);
        if (!fetched || response.find("<CURRENTLISTENERS>") == std::string::npos) {
                fetched = curl.fetchUrl(urlV2, response);
        }
        if (!fetched) {
                sprintf(buf, _("Failed to fetch data from %s\n"), serverURL.c_str());
                logger.logError(buf);
                return;
        }
        if (response.find("<title>SHOUTcast Administrator</title>") != std::string::npos) {
                sprintf(buf, _("Bad password (%s/%s)\n"), serverURL.c_str(), password.c_str());
                logger.logError(buf);
                return;
        }

        const char *p1 = strchr(response.c_str(), '<');
        if (!p1) {
                logger.logError(_("Bad parse!"));
                return;
        }
        xmlDocPtr doc = xmlParseMemory(p1, strlen(p1));
        if (!doc) {
                logger.logError(_("Bad parse!"));
                return;
        }
        xmlNodePtr cur = xmlDocGetRootElement(doc);
        if (cur == NULL) {
                logger.logError(_("Empty Document!"));
                xmlFreeDoc(doc);
                return;
        }
        serverData sData{};
        cur = cur->xmlChildrenNode;
        while (cur != NULL) {
                if (!xmlStrcmp(cur->name, (const xmlChar *) "CURRENTLISTENERS")) {
                        sData.currentListeners = atoi((char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1));
                }
                if (!xmlStrcmp(cur->name, (const xmlChar *) "PEAKLISTENERS")) {
                        sData.peakListeners = atoi((char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1));
                }
                if (!xmlStrcmp(cur->name, (const xmlChar *) "MAXLISTENERS")) {
                        sData.maxListeners = atoi((char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1));
                }
                if (!xmlStrcmp(cur->name, (const xmlChar *) "REPORTEDLISTENERS")) {
                        sData.reportedListeners = atoi((char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1));
                }
                if (!xmlStrcmp(cur->name, (const xmlChar *) "AVERAGETIME")) {
                        sData.avgTime = atoi((char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1));
                }
                if (!xmlStrcmp(cur->name, (const xmlChar *) "WEBHITS")) {
                        sData.webHits = atoi((char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1));
                }
                if (!xmlStrcmp(cur->name, (const xmlChar *) "STREAMHITS")) {
                        sData.streamHits = atoi((char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1));
                }
                if (!xmlStrcmp(cur->name, (const xmlChar *) "SONGTITLE")) {
                        memset(sData.songTitle, '\0', sizeof(sData.songTitle));
                        strcpy(sData.songTitle, (char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1));
                }
                cur = cur->next;
        }
        xmlFreeDoc(doc);
        trimRight(sData.songTitle);

        char query[2046];
        std::lock_guard<std::mutex> lock(dbMutex);
        sprintf(query, "select songTitle from scastd_songinfo where serverURL = '%s' order by time desc limit 1", serverURL.c_str());
        db->query(query);
        IDatabase::Row row = db->fetch();
        int insert_flag = 1;
        if (!row.empty() && !strcmp(sData.songTitle, row[0].c_str())) {
                insert_flag = 0;
        }
        if (insert_flag) {
                sprintf(query, "insert into scastd_songinfo values('%s', '%s', NULL)", serverURL.c_str(), sData.songTitle);
                db->query(query);
        }
        sprintf(query,
                "insert into scastd_serverinfo (serverURL, currentlisteners, peaklisteners, maxlisteners, averagetime, streamhits, time) "
                "values('%s', %d, %d, %d, %d, %d, NULL)",
                serverURL.c_str(), sData.currentListeners, sData.peakListeners,
                sData.maxListeners, sData.avgTime, sData.streamHits);
        db->query(query);
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


int run(const std::string &configPath)
{
        init_i18n();
        scastd::HttpServer httpServer;
	IDatabase       *db = NULL;
	IDatabase       *db2 = NULL;
        Config  cfg;

	char	buf[1024];
	IDatabase::Row       row;
	char	query[2046] = "";
	int	sleeptime = 0;
        if (!cfg.Load(configPath)) {
                std::string msg = std::string(_("Cannot load config file ")) + configPath;
                logger.logError(msg);
                return 1;
        }
        std::string accessLog = cfg.AccessLog();
        std::string errorLog = cfg.ErrorLog();
        std::string debugLog = cfg.DebugLog();
        bool consoleFlag = cfg.Get("log_console", false);
        int debugLevel = cfg.DebugLevel();
        logger.setLogFiles(accessLog, errorLog, debugLog);
        logger.setConsoleOutput(consoleFlag);
        logger.setDebugLevel(debugLevel);
        if (cfg.SyslogEnabled()) {
                Logger::SyslogProto proto = cfg.SyslogProtocol() == "tcp" ?
                        Logger::SyslogProto::TCP : Logger::SyslogProto::UDP;
                logger.setSyslog(cfg.SyslogHost(), cfg.SyslogPort(), proto);
        }
        logger.setEnabled(true);
        bool httpEnabled = cfg.Get("http_enabled", true);
        int httpPort = cfg.Get("http_port", 8333);
        std::string httpUser = cfg.Get("http_username", "");
        std::string httpPass = cfg.Get("http_password", "");
        bool sslEnabled = cfg.Get("ssl_enabled", false);
        std::string sslCert = cfg.Get("ssl_cert", "");
        std::string sslKey = cfg.Get("ssl_key", "");
        if (httpEnabled) {
                if (!httpServer.start(httpPort, httpUser, httpPass,
                                       std::thread::hardware_concurrency(),
                                       sslEnabled, sslCert, sslKey)) {
                        char lbuf[256];
                        sprintf(lbuf, _("Failed to start HTTP%s server on port %d"),
                                sslEnabled ? "S" : "", httpPort);
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
        if (dbType == "mysql") {
                db = new MySQLDatabase();
                db2 = new MySQLDatabase();
        } else if (dbType == "mariadb") {
                db = new MariaDBDatabase();
                db2 = new MariaDBDatabase();
        } else if (dbType == "postgres") {
                db = new PostgresDatabase();
                db2 = new PostgresDatabase();
        } else if (dbType == "sqlite") {
                db = new SQLiteDatabase();
                db2 = new SQLiteDatabase();
        } else {
                char lbuf[256];
                sprintf(lbuf, _("Unknown DatabaseType '%s'. Falling back to sqlite."), dbType.c_str());
                logger.logError(lbuf);
                db = new SQLiteDatabase();
                db2 = new SQLiteDatabase();
                dbType = "sqlite";
                dbName = sqlitePath;
                dbUser.clear();
                dbPass.clear();
                dbHost.clear();
                dbPort = 0;
                dbSSLMode.clear();
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

        logger.logDebug(_("Detaching from console..."));

	if (fork()) {
		// Parent
		exit(1);
	}
	else {
		// Da child
	
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
                std::ifstream sqlFile("src/sqlite.sql");
                if (sqlFile) {
                        std::stringstream buffer; buffer << sqlFile.rdbuf();
                        db->query(buffer.str());
                }
        }
        sprintf(query, "select sleeptime from scastd_runtime");
        db->query(query);
        row = db->fetch();
        if (row.empty()) {
                logger.logError(_("We must have an entry in the scastd_runtime table..exiting."));
                exit(-1);
        }
        sleeptime = atoi(row[0].c_str());
	

        logger.logAccess(_("SCASTD starting...\n"));

	while (1) {
                if (reloadConfig) {
                        reloadConfig = 0;
                        Config newCfg;
                        if (newCfg.Load(configPath)) {
                                cfg = newCfg;
                                std::string newAccess = cfg.AccessLog();
                                std::string newError = cfg.ErrorLog();
                                std::string newDebug = cfg.DebugLog();
                                bool newConsole = cfg.Get("log_console", false);
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
                                        if (newDbType == "mysql") {
                                                db = new MySQLDatabase();
                                                db2 = new MySQLDatabase();
                                        } else if (newDbType == "mariadb") {
                                                db = new MariaDBDatabase();
                                                db2 = new MariaDBDatabase();
                                        } else if (newDbType == "postgres") {
                                                db = new PostgresDatabase();
                                                db2 = new PostgresDatabase();
                                        } else if (newDbType == "sqlite") {
                                                db = new SQLiteDatabase();
                                                db2 = new SQLiteDatabase();
                                        } else {
                                                logger.logError(_("Unknown DatabaseType. Falling back to sqlite\n"));
                                                db = new SQLiteDatabase();
                                                db2 = new SQLiteDatabase();
                                                newDbType = "sqlite";
                                                newDbName = newSqlitePath;
                                                newDbUser.clear();
                                                newDbPass.clear();
                                                newDbHost.clear();
                                                newDbPort = 0;
                                                newDbSSLMode.clear();
                                                if (access(newDbName.c_str(), F_OK) != 0) {
                                                        newNeedInit = true;
                                                }
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
                                        std::ifstream sqlFile("src/sqlite.sql");
                                        if (sqlFile) {
                                                std::stringstream buffer; buffer << sqlFile.rdbuf();
                                                db->query(buffer.str());
                                        }
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
                        sprintf(query, "select serverURL, password from scastd_memberinfo where gather_flag = 1");
                        db2->query(query);
                        std::vector<std::pair<std::string, std::string>> servers;
                        while (true) {
                                if (exiting) break;
                                row = db2->fetch();
                                if (row.empty()) break;
                                servers.emplace_back(row[0], row.size() > 1 ? row[1] : "");
                        }
                        std::vector<std::thread> workers;
                        for (const auto &srv : servers) {
                                workers.emplace_back([&, srv] {
                                        processServer(srv.first, srv.second, db);
                                });
                        }
                        for (auto &t : workers) {
                                if (t.joinable()) t.join();
                        }
		}
		if (exiting) break;
			
                sprintf(buf, _("Sleeping for %d seconds\n"), sleeptime);
                logger.logDebug(buf);
		sleep(sleeptime);
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
}

} // namespace scastd
