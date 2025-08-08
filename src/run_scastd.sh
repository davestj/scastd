#!/bin/bash
PATH=/usr/bin:/usr/sbin:/sbin:/bin

echo "type ./install help"

if [ $1 = "help" ]; then
echo"Please enter a username and password for mysql i.e. ./install.sh root password"
fi
if [ $1 != "" ]; then
echo "Creating and importing the default scastd database and schema"
mysqladmin --user=$1 --password=$2 create scastd
mysql --user=$1 --password=$2 scastd < scastd.sql
fi

printf "Would you like to start scastd now? (y/n):"
read answer
printf "\n"

if [ $answer = "y" ]; then
./scastd
fi
if [ $answer = "n" ]; then
echo "Done installing scastd db and schema"
echo "status of mysql db"
sleep 3
mysqladmin --user=$1 --password=$2 status
fi
