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
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <curl/curl.h>
#include <signal.h>
#include "DB.h"
#include "Config.h"


FILE	*filep_log = 0;
char	logfile[2046] = "";
int	paused = 0;
int     exiting = 0;

struct CurlBuffer {
    std::string data;
};

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total = size * nmemb;
    CurlBuffer *mem = static_cast<CurlBuffer *>(userp);
    mem->data.append(static_cast<char *>(contents), total);
    return total;
}

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

main(int argc, char **argv)
{
        xmlDocPtr doc;
        DB      db;
        DB      db2;
        Config  cfg;

        char    buf[1024];
        char    *p1;
        xmlNodePtr cur;
        MYSQL_ROW       row;
        serverData      sData;
        char    query[2046] = "";
        char    serverURL[255] = "";
        char    password[255] = "";
        int     sleeptime = 0;
        int     insert_flag = 0;
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

        curl_global_init(CURL_GLOBAL_DEFAULT);

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
        db.Connect(dbUser.c_str(), dbPass.c_str());
        db2.Connect(dbUser.c_str(), dbPass.c_str());
	sprintf(query, "select sleeptime, logfile from scastd_runtime");
	db.Query(query);
	row = db.Fetch();
	if (!row) {
		fprintf(stderr, "We must have an entry in the scastd_runtime table..exiting.\n");
		exit(-1);
	}
	sleeptime = atoi(row[0]);
	strcpy(logfile, row[1]);
	
	openLog();

	writeToLog("SCASTD starting...\n");

	while (1) {
		if (exiting) {
			writeToLog("Exiting...\n");
			exit(1);
		}
		if (!paused) {
			sprintf(query, "select serverURL, password from scastd_memberinfo where gather_flag = 1");
			db2.Query(query);
			while (row = db2.Fetch()) {
                                memset(serverURL, '\000', sizeof(serverURL));
                                memset(password, '\000', sizeof(password));
                                if (row[0]) {
                                        strcpy(serverURL, row[0]);
                                }
                                if (row[1]) {
                                        strcpy(password, row[1]);
                                }
                                char url[512];
                                snprintf(url, sizeof(url), "%s/admin.cgi?pass=%s&mode=viewxml", serverURL, password);
                                sprintf(buf, "Connecting to server %s\n", url);
                                writeToLog(buf);
                                CurlBuffer chunk;
                                CURL *curl = curl_easy_init();
                                if (curl) {
                                        curl_easy_setopt(curl, CURLOPT_URL, url);
                                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
                                        CURLcode res = curl_easy_perform(curl);
                                        curl_easy_cleanup(curl);
                                        if (res == CURLE_OK) {
                                                if (chunk.data.find("<title>SHOUTcast Administrator</title>") != std::string::npos) {
                                                        sprintf(buf, "Bad password (%s/%s)\n", serverURL, password);
                                                        writeToLog(buf);
                                                } else {
                                                        p1 = strchr(chunk.data.c_str(), '<');
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
										memset(sData.songTitle, '\000', sizeof(sData.songTitle));
										strcpy(sData.songTitle, (char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1));
									}
									cur = cur->next;
								}
							}
							trimRight(sData.songTitle);
							sprintf(query, "select songTitle from scastd_songinfo where serverURL = '%s' order by time desc limit 1", serverURL);
							db.Query(query);
							insert_flag = 0;
							row = db.Fetch();
							if (row) {
								if (!strcmp(sData.songTitle, row[0])) {
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
								db.Query(query);
							}
							sprintf(query, "insert into scastd_serverinfo (serverURL, currentlisteners, peaklisteners, maxlisteners, averagetime, streamhits, time) values('%s', %d, %d, %d, %d, %d, NULL)", serverURL, sData.currentListeners, sData.maxListeners, sData.peakListeners, sData.avgTime, sData.streamHits);
							db.Query(query);
						}
                                                else {
                                                        sprintf(buf, "Bad data from %s\n", serverURL);
                                                        writeToLog(buf);
                                                }
                                        } else {
                                                sprintf(buf, "HTTP request failed for %s: %s\n", url, curl_easy_strerror(res));
                                                writeToLog(buf);
                                        }
                                }
			}
		}
			
                sprintf(buf, "Sleeping for %d seconds\n", sleeptime);
                writeToLog(buf);
                sleep(sleeptime);
        }
        curl_global_cleanup();
        }
}


