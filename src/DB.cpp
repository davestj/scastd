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
#ifdef WIN32
#include <winsock2.h>
#endif
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include "DB.h"
#include <string.h>
#ifdef WIN32
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

double  DBGetCurrentTime() {
        double  thetime;
        struct timeval  tv;
        gettimeofday(&tv, NULL);
        thetime = (double)tv.tv_sec + (double)((double)tv.tv_usec / (double)1000000);

        return thetime;
}

DB::DB() {
        pResult = 0;
        sqlTimings = 0;
        memset(tempQuery, '\000', sizeof(tempQuery));
}
DB::~DB() {
        if (pResult) {
                mysql_free_result(pResult);
        }
}
int DB::Connect(const char *username, const char *password) {
        char    u[255] = "";
        char    p[255] = "";
        mysql_init(&mySQL);


        mysql_options(&mySQL,MYSQL_READ_DEFAULT_GROUP,"scastd");

        if (username && *username) {
                strcpy(u, username);
        }
        else {
                strcpy(u, "root");
        }
        if (password && *password) {
                strcpy(p, password);
        }
        else {
                strcpy(p, "");
        }

        if (!mysql_real_connect(&mySQL,"localhost",u,p,"scastd",0,NULL,0)) {
                fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mySQL));
                return(0);
        }
        return 1;
}
int DB::Query(char *query) {
        // double       startTime = 0.0;
        // double       endTime = 0.0;

        numFetches = 0;
        strcpy(tempQuery, query);
        if (sqlTimings) {
                startTime = DBGetCurrentTime();
        }
        if (pResult) {
                mysql_free_result(pResult);
        }
        if (mysql_query(&mySQL, query) != 0) {
                fprintf(stderr, "Misformed query (%s)\n", query);
                fprintf(stderr, "Error: %s\n", mysql_error(&mySQL));
                return(0);
        }

        pResult = mysql_store_result(&mySQL);
        if (pResult) {
                numFields = mysql_num_fields(pResult);
                numRows = mysql_num_rows(pResult);
        }
        if (sqlTimings) {
                endTime = DBGetCurrentTime();
        }

        if (sqlTimings) {
                fprintf(stdout, "Query %f secs - SQL: %s\n", endTime - startTime, query);
        }
        return(1);

}
MYSQL_ROW DB::Fetch() {
        numFetches++;
        return mysql_fetch_row(pResult);
}

void DB::EndQuery() {
        if (sqlTimings) {
                endTime = DBGetCurrentTime();
        }
        if (sqlTimings) {
                fprintf(stdout, "Total Time Query (%d fetches) %f secs - SQL: %s\n", numFetches, endTime - startTime, tempQuery);
        }
}
void DB::Disconnect() {
        mysql_close(&mySQL);
        return;
}
