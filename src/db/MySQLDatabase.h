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

#ifndef MYSQLDATABASE_H
#define MYSQLDATABASE_H

#include "IDatabase.h"
#include <mysql/mysql.h>

class MySQLDatabase : public IDatabase {
public:
    MySQLDatabase();
    ~MySQLDatabase();

    bool connect(const std::string &username,
                 const std::string &password,
                 const std::string &host,
                 int port,
                 const std::string &dbname,
                 const std::string &sslmode) override;
    bool query(const std::string &query) override;
    Row fetch() override;
    void disconnect() override;
private:
    MYSQL mySQL;
    MYSQL_RES *pResult;
};

#endif // MYSQLDATABASE_H
