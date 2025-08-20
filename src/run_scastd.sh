#!/bin/bash
# Helper script to configure the MariaDB database for scastd
PATH=/usr/bin:/usr/sbin:/sbin:/bin

echo "type ./install help"

if [ $1 = "help" ]; then
echo "Please enter a username and password for MariaDB i.e. ./install.sh root password"
fi
if [ $1 != "" ]; then
echo "Creating and importing the default scastd database and schema"
mariadb-admin --user=$1 --password=$2 create scastd
mariadb --user=$1 --password=$2 scastd < mariadb.sql
fi

printf "Would you like to start scastd now? (y/n):"
read answer
printf "\n"

if [ $answer = "y" ]; then
./scastd
fi
if [ $answer = "n" ]; then
echo "Done installing scastd db and schema"
echo "status of MariaDB"
sleep 3
mariadb-admin --user=$1 --password=$2 status
fi
