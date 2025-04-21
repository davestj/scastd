# MySQL dump 8.8
#
# Host: localhost    Database: scastd
#--------------------------------------------------------
# Server version	3.23.22-beta

#
# Table structure for table 'scastd_memberinfo'
#

DROP TABLE IF EXISTS scastd_memberinfo;
CREATE TABLE scastd_memberinfo (
  serverURL varchar(255) DEFAULT '0' NOT NULL,
  password varchar(155) DEFAULT '' NOT NULL,
  gather_flag varchar(155) DEFAULT '' NOT NULL,
  PRIMARY KEY (serverURL)
);

#
# Dumping data for table 'scastd_memberinfo'
#

INSERT INTO scastd_memberinfo VALUES ('http://boa.mediacast1.com:9908','party7324','1');

#
# Table structure for table 'scastd_runtime'
#

DROP TABLE IF EXISTS scastd_runtime;
CREATE TABLE scastd_runtime (
  sleeptime int(11),
  logfile varchar(255)
);

#
# Dumping data for table 'scastd_runtime'
#

INSERT INTO scastd_runtime VALUES (30,'./scastd.log');

#
# Table structure for table 'scastd_serverinfo'
#

DROP TABLE IF EXISTS scastd_serverinfo;
CREATE TABLE scastd_serverinfo (
  serverURL varchar(255),
  currentlisteners int(11),
  peaklisteners int(11),
  maxlisteners int(11),
  averagetime int(11),
  streamhits int(11),
  time timestamp(14)
);

#
# Dumping data for table 'scastd_serverinfo'
#


#
# Table structure for table 'scastd_songinfo'
#

DROP TABLE IF EXISTS scastd_songinfo;
CREATE TABLE scastd_songinfo (
  serverURL varchar(255),
  songTitle varchar(255),
  time timestamp(14)
);

#
# Dumping data for table 'scastd_songinfo'
#


#
# Table structure for table 'scastd_xml'
#

DROP TABLE IF EXISTS scastd_xml;
CREATE TABLE scastd_xml (
  id int(10) NOT NULL auto_increment,
  currentlisteners varchar(155) DEFAULT '' NOT NULL,
  peaklisteners varchar(155) DEFAULT '' NOT NULL,
  maxlisteners varchar(155) DEFAULT '' NOT NULL,
  reportedlisteners varchar(155) DEFAULT '' NOT NULL,
  averagetime varchar(255) DEFAULT '' NOT NULL,
  servergenre varchar(155) DEFAULT '' NOT NULL,
  serverurl varchar(255) DEFAULT '' NOT NULL,
  servertitle varchar(255) DEFAULT '' NOT NULL,
  currentsong varchar(255) DEFAULT '' NOT NULL,
  webhits varchar(155) DEFAULT '' NOT NULL,
  streamhits varchar(255) DEFAULT '' NOT NULL,
  hostname varchar(255) DEFAULT '' NOT NULL,
  useragent varchar(255) DEFAULT '' NOT NULL,
  connecttime varchar(155) DEFAULT '' NOT NULL,
  songtitle varchar(255) DEFAULT '' NOT NULL,
  playedat varchar(55) DEFAULT '' NOT NULL,
  bwidthus varchar(55) DEFAULT '' NOT NULL,
  totbwidthus varchar(255) DEFAULT '' NOT NULL,
  date date DEFAULT '0000-00-00' NOT NULL,
  time time DEFAULT '00:00:00' NOT NULL,
  PRIMARY KEY (id)
);

#
# Dumping data for table 'scastd_xml'
#


