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
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <libxml/nanohttp.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <signal.h>
#include "db/IDatabase.h"
#include "db/MySQLDatabase.h"
#include "db/MariaDBDatabase.h"
#include "db/PostgresDatabase.h"
#include "Config.h"

#include "CurlClient.h"
#include "HttpServer.h"
#include "UrlParser.h"

FILE	*filep_log = 0;
char	logfile[2046] = "";
std::string logDir = ".";
size_t	logMaxSize = 1024 * 1024;
int	logRetention = 5;
int	paused = 0;
int     exiting = 0;
volatile sig_atomic_t reloadConfig = 0;

void parseWebdata(xmlNodePtr cur)
{
	
    while (cur != NULL) {
	fprintf(stdout, "Webdata: %s\n", cur->name);
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

static void purgeOldLogs() {
        DIR *dir = opendir(logDir.c_str());
        if (!dir) return;
        std::vector<std::string> files;
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
                std::string name = entry->d_name;
                if (name.find("scastd-") == 0 && name.rfind(".log") == name.size() - 4) {
                        files.push_back(name);
                }
        }
        closedir(dir);
        std::sort(files.begin(), files.end());
        while ((int)files.size() > logRetention) {
                std::string path = logDir + "/" + files.front();
                remove(path.c_str());
                files.erase(files.begin());
        }
}

static void rotateLogsIfNeeded() {
        struct stat st;
        time_t now = time(0);
        bool rotate = false;
        if (stat(logfile, &st) == 0) {
                if ((size_t)st.st_size >= logMaxSize) {
                        rotate = true;
                }
                char fileDate[9];
                char curDate[9];
                strftime(fileDate, sizeof(fileDate), "%Y%m%d", gmtime(&st.st_mtime));
                strftime(curDate, sizeof(curDate), "%Y%m%d", gmtime(&now));
                if (strcmp(fileDate, curDate) != 0) {
                        rotate = true;
                }
        }
        if (rotate) {
                if (filep_log) {
                        fclose(filep_log);
                        filep_log = 0;
                }
                char ts[32];
                strftime(ts, sizeof(ts), "%Y%m%d-%H%M%S", gmtime(&now));
                char rotated[4096];
                snprintf(rotated, sizeof(rotated), "%s/scastd-%s.log", logDir.c_str(), ts);
                rename(logfile, rotated);
                purgeOldLogs();
        }
}

void openLog() {
        rotateLogsIfNeeded();
        if (!filep_log) {
                mkdir(logDir.c_str(), 0755);
                filep_log = fopen(logfile, "a");
                if (!filep_log) {
                        fprintf(stderr, "Cannot open logfile %s\n", logfile);
                }
        }
}
void writeToLog(char *message) {
	time_t date = time(0);

	char	str[255] = "";
	strftime(str, sizeof(str), "%Y-%m-%d %T", gmtime(&date));
      
	openLog();
	if (filep_log) {
		fprintf(filep_log, "%s-%s", str, message);
		fflush(filep_log);
	}
}
void closeLog() {
	if (filep_log) {
		fclose(filep_log);
	}
}

void sigUSR1(int sig)
{
	if (paused) {
		writeToLog("Caught SIGUSR1 - Resuming\n");
		paused = 0;
	}
	else {
		writeToLog("Caught SIGUSR1 - Pausing\n");
		paused = 1;
	}
	
}
void sigUSR2(int sig)
{
        writeToLog("Caught SIGUSR2 - Exiting\n");
        exiting = 1;

}

void sigTERM(int sig)
{
        writeToLog("Caught SIGTERM - Exiting\n");
        exiting = 1;
}

void sigINT(int sig)
{
        writeToLog("Caught SIGINT - Exiting\n");
        exiting = 1;
}

void sigHUP(int sig)
{
	writeToLog("Caught SIGHUP - Reloading config\n");
	reloadConfig = 1;
}


typedef struct tagServerData {
	int	currentListeners;
	int	peakListeners;
	int	maxListeners;
	int	reportedListeners;
	int	avgTime;
	int	webHits;
	int	streamHits;
	char	songTitle[1024];
} serverData;

int main(int argc, char **argv)
{
        scastd::HttpServer httpServer;
        xmlDocPtr doc;
	IDatabase       *db = NULL;
	IDatabase       *db2 = NULL;
        Config  cfg;

	char	buf[1024];
        const char    *p1;
	xmlNodePtr cur;
	IDatabase::Row       row;
	serverData	sData;
	char	query[2046] = "";
	char	serverURL[255] = "";
	char	IP[255] = "";
	int	port = 0;
	char	password[255] = "";
	int	sleeptime = 0;
	int	insert_flag = 0;
        std::string configPath = "scastd.conf";
        if (argc > 1) {
                configPath = argv[1];
        }
        if (!cfg.Load(configPath)) {
                fprintf(stderr, "Cannot load config file %s\n", configPath.c_str());
                exit(1);
        }
        bool httpEnabled = cfg.Get("http_enabled", true);
        int httpPort = cfg.Get("http_port", 8333);
        if (httpEnabled) {
                if (!httpServer.start(httpPort)) {
                        fprintf(stderr, "Failed to start HTTP server on port %d\n", httpPort);
                }
        }
        std::string dbUser = cfg.Get("username", "root");
        std::string dbPass = cfg.Get("password", "");
        std::string dbType = cfg.Get("DatabaseType", "mysql");
        std::string dbHost = cfg.Get("host", "localhost");
        int dbPort = cfg.Get("port", 0);
        std::string dbName = cfg.Get("dbname", "scastd");
        std::string dbSSLMode = cfg.Get("sslmode", "");
        logDir = cfg.Get("log_dir", ".");
        logMaxSize = (size_t)cfg.Get("log_max_size", 1024 * 1024);
        logRetention = cfg.Get("log_retention", 5);
        snprintf(logfile, sizeof(logfile), "%s/scastd.log", logDir.c_str());
        if (dbType == "mysql") {
                db = new MySQLDatabase();
                db2 = new MySQLDatabase();
        } else if (dbType == "mariadb") {
                db = new MariaDBDatabase();
                db2 = new MariaDBDatabase();
        } else if (dbType == "postgres") {
                db = new PostgresDatabase();
                db2 = new PostgresDatabase();
        } else {
                fprintf(stderr, "Unknown DatabaseType '%s'. Supported values are mysql, mariadb, postgres. Falling back to default 'mysql'.\n", dbType.c_str());
                db = new MySQLDatabase();
                db2 = new MySQLDatabase();
                dbType = "mysql";
        }

        fprintf(stdout, "Detaching from console...\n");

	if (fork()) {
		// Parent
		exit(1);
	}
	else {
		// Da child
	
	if (signal(SIGUSR1, sigUSR1) == SIG_ERR) {
		fprintf(stderr, "Cannot install handler for SIGUSR1\n");
		exit(1);
	}
        if (signal(SIGUSR2, sigUSR2) == SIG_ERR) {
                fprintf(stderr, "Cannot install handler for SIGUSR2\n");
                exit(1);
        }
        if (signal(SIGTERM, sigTERM) == SIG_ERR) {
                fprintf(stderr, "Cannot install handler for SIGTERM\n");
                exit(1);
        }
        if (signal(SIGINT, sigINT) == SIG_ERR) {
                fprintf(stderr, "Cannot install handler for SIGINT\n");
                exit(1);
        }
        if (signal(SIGHUP, sigHUP) == SIG_ERR) {
                fprintf(stderr, "Cannot install handler for SIGHUP\n");
                exit(1);
        }
        db->connect(dbUser, dbPass, dbHost, dbPort, dbName, dbSSLMode);
        db2->connect(dbUser, dbPass, dbHost, dbPort, dbName, dbSSLMode);
        sprintf(query, "select sleeptime from scastd_runtime");
        db->query(query);
        row = db->fetch();
        if (row.empty()) {
                fprintf(stderr, "We must have an entry in the scastd_runtime table..exiting.\n");
                exit(-1);
        }
        sleeptime = atoi(row[0].c_str());
	
	openLog();

	writeToLog("SCASTD starting...\n");

	while (1) {
                if (reloadConfig) {
                        reloadConfig = 0;
                        Config newCfg;
                        if (newCfg.Load(configPath)) {
                                cfg = newCfg;
                                std::string newLogDir = cfg.Get("log_dir", ".");
                                size_t newLogMaxSize = (size_t)cfg.Get("log_max_size", 1024 * 1024);
                                int newLogRetention = cfg.Get("log_retention", 5);
                                if (newLogDir != logDir || newLogMaxSize != logMaxSize || newLogRetention != logRetention) {
                                        logDir = newLogDir;
                                        logMaxSize = newLogMaxSize;
                                        logRetention = newLogRetention;
                                        snprintf(logfile, sizeof(logfile), "%s/scastd.log", logDir.c_str());
                                        closeLog();
                                }
                                std::string newDbType = cfg.Get("DatabaseType", dbType);
                                std::string newDbUser = cfg.Get("username", dbUser);
                                std::string newDbPass = cfg.Get("password", dbPass);
                                std::string newDbHost = cfg.Get("host", dbHost);
                                int newDbPort = cfg.Get("port", dbPort);
                                std::string newDbName = cfg.Get("dbname", dbName);
                                std::string newDbSSLMode = cfg.Get("sslmode", dbSSLMode);
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
                                        } else {
                                                writeToLog("Unknown DatabaseType. Falling back to mysql\n");
                                                db = new MySQLDatabase();
                                                db2 = new MySQLDatabase();
                                                newDbType = "mysql";
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
                                writeToLog("Configuration reloaded\n");
                        } else {
                                writeToLog("Failed to reload config\n");
                        }
                }
		if (exiting) {
			writeToLog("Exiting...\n");
			break;
		}
		if (!paused) {
                        sprintf(query, "select serverURL, password from scastd_memberinfo where gather_flag = 1");
                        db2->query(query);
                        while (true) {
				if (exiting) break;
                                row = db2->fetch();
                                if (row.empty()) break;
                                memset(serverURL, '\000', sizeof(serverURL));
                                memset(password, '\000', sizeof(password));
                                memset(IP, '\000', sizeof(IP));
                                port = 0;
                                if (!row[0].empty()) {
                                        strcpy(serverURL, row[0].c_str());
                                        std::string ipStr;
                                        if (parseHostPort(row[0], ipStr, port)) {
                                                strncpy(IP, ipStr.c_str(), sizeof(IP) - 1);
                                                IP[sizeof(IP) - 1] = '\0';
                                        }

                                }
                                if (!row[1].empty()) {
                                        strcpy(password, row[1].c_str());
                                }

                                sprintf(buf, "Connecting to server %s at port %d\n", IP, port);
                                writeToLog(buf);
                                std::string url = std::string("http://") + IP + ":" + std::to_string(port) + "/admin.cgi?pass=" + password + "&mode=viewxml";
                                std::string response;
                                CurlClient curl;
                                if (curl.fetchUrl(url, response)) {
                                        if (response.find("<title>SHOUTcast Administrator</title>") != std::string::npos) {
                                                sprintf(buf, "Bad password (%s/%s)\n", serverURL, password);
                                                writeToLog(buf);
                                        }
                                        else {
                                                p1 = strchr(response.c_str(), '<');
                                                if (p1) {
                                                        doc = xmlParseMemory(p1, strlen(p1));
                                                        if (!doc) {
                                                                writeToLog("Bad parse!");
                                                        }

                                                        cur = xmlDocGetRootElement(doc);
                                                        if (cur == NULL) {
                                                                writeToLog("Empty Document!");
                                                                xmlFreeDoc(doc);
                                                                exiting = 1;
                                                                break;
                                                        }
                                                        else {
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
                                                        }
                                                        trimRight(sData.songTitle);
                                                        sprintf(query, "select songTitle from scastd_songinfo where serverURL = '%s' order by time desc limit 1", serverURL);
                                                        db->query(query);
                                                        insert_flag = 0;
                                                        row = db->fetch();
                                                        if (!row.empty()) {
                                                                if (!strcmp(sData.songTitle, row[0].c_str())) {
                                                                        insert_flag = 0;
                                                                }
                                                                else {
                                                                        insert_flag = 1;
                                                                }
                                                        }
                                                        else {
                                                                insert_flag = 1;
                                                        }
                                                        if (insert_flag) {
                                                                sprintf(query, "insert into scastd_songinfo values('%s', '%s', NULL)", serverURL, sData.songTitle);
                                                                db->query(query);
                                                        }
                                                        sprintf(query, "insert into scastd_serverinfo (serverURL, currentlisteners, peaklisteners, maxlisteners, averagetime, streamhits, time) values('%s', %d, %d, %d, %d, %d, NULL)", serverURL, sData.currentListeners, sData.maxListeners, sData.peakListeners, sData.avgTime, sData.streamHits);
                                                        db->query(query);
                                                }
                                                else {
                                                        sprintf(buf, "Bad data from %s\n", serverURL);
                                                        writeToLog(buf);
                                                }
                                        }
                                } else {
                                        sprintf(buf, "Failed to fetch data from %s\n", serverURL);
                                        writeToLog(buf);
                                }

			}
		}
		if (exiting) break;
			
		sprintf(buf, "Sleeping for %d seconds\n", sleeptime);
		writeToLog(buf);
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
        closeLog();
        return 0;
}
}
