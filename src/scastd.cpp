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
#include "Config.h"

#include "CurlWrapper.h"

FILE	*filep_log = 0;
char	logfile[2046] = "";
int	paused = 0;
int	exiting = 0;

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

void openLog() {
	if (!filep_log) {
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
	char	*p2;
	char	*p3;
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
        std::string dbUser = cfg.Get("username", "root");
        std::string dbPass = cfg.Get("password", "");
        std::string dbType = cfg.Get("DatabaseType", "mysql");
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
                fprintf(stderr, "Unknown DatabaseType %s\n", dbType.c_str());
                exit(1);
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
        db->connect(dbUser.c_str(), dbPass.c_str());
        db2->connect(dbUser.c_str(), dbPass.c_str());
        sprintf(query, "select sleeptime, logfile from scastd_runtime");
        db->query(query);
        row = db->fetch();
        if (row.empty()) {
                fprintf(stderr, "We must have an entry in the scastd_runtime table..exiting.\n");
                exit(-1);
        }
        sleeptime = atoi(row[0].c_str());
        strcpy(logfile, row[1].c_str());
	
	openLog();

	writeToLog("SCASTD starting...\n");

	while (1) {
		if (exiting) {
			writeToLog("Exiting...\n");
			exit(1);
		}
		if (!paused) {
                        sprintf(query, "select serverURL, password from scastd_memberinfo where gather_flag = 1");
                        db2->query(query);
                        while (true) {
                                row = db2->fetch();
                                if (row.empty()) break;
                                memset(serverURL, '\000', sizeof(serverURL));
                                memset(password, '\000', sizeof(password));
                                memset(IP, '\000', sizeof(IP));
                                port = 0;
                                if (!row[0].empty()) {
                                        strcpy(serverURL, row[0].c_str());
                                        p2 = strstr(serverURL, "http://");
                                        if (p2) {
                                                p2 = p2 + strlen("http://");
                                                p3 = strchr(p2, ':');
                                                if (p3) {
                                                        strncpy(IP, p2, p3-p2);
                                                        p3++;
                                                        port = atoi(p3);
                                                }
                                        }

                                }
                                if (!row[1].empty()) {
                                        strcpy(password, row[1].c_str());
                                }

                                sprintf(buf, "Connecting to server %s at port %d\n", IP, port);
                                writeToLog(buf);
                                std::string url = std::string("http://") + IP + ":" + std::to_string(port) + "/admin.cgi?pass=" + password + "&mode=viewxml";
                                std::string response;
                                if (fetchUrl(url, response)) {
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
                                                                exit(1);
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
			
		sprintf(buf, "Sleeping for %d seconds\n", sleeptime);
		writeToLog(buf);
		sleep(sleeptime);
	}
	}
	return 0;
}
