# MariaDB schema for scastd
# Host: localhost    Database: scastd
#--------------------------------------------------------
# Server version        10.x


#
# Table structure for table 'servers'
#

DROP TABLE IF EXISTS servers;
CREATE TABLE servers (
  server_host varchar(255) NOT NULL,
  server_port int NOT NULL,
  server_username varchar(255) DEFAULT '' NOT NULL,
  server_password varchar(255) DEFAULT '' NOT NULL,
  PRIMARY KEY (server_host, server_port)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

#
# Dumping data for table 'servers'
#

INSERT INTO servers VALUES ('example.com', 8000, '', '');

#
# Table structure for table 'scastd_runtime'
#

DROP TABLE IF EXISTS scastd_runtime;
CREATE TABLE scastd_runtime (
  sleeptime INT DEFAULT NULL,
  logfile VARCHAR(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

#
# Dumping data for table 'scastd_runtime'
#

INSERT INTO scastd_runtime VALUES (30,'./scastd.log');

#
# Table structure for table 'scastd_serverinfo'
#

DROP TABLE IF EXISTS scastd_serverinfo;
CREATE TABLE scastd_serverinfo (
  serverURL VARCHAR(255) DEFAULT NULL,
  currentlisteners INT DEFAULT NULL,
  peaklisteners INT DEFAULT NULL,
  maxlisteners INT DEFAULT NULL,
  averagetime INT DEFAULT NULL,
  streamhits INT DEFAULT NULL,
  time TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

#
# Dumping data for table 'scastd_serverinfo'
#


#
# Table structure for table 'scastd_songinfo'
#

DROP TABLE IF EXISTS scastd_songinfo;
CREATE TABLE scastd_songinfo (
  serverURL VARCHAR(255) DEFAULT NULL,
  songTitle VARCHAR(255) DEFAULT NULL,
  time TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

#
# Dumping data for table 'scastd_songinfo'
#


#
# Table structure for table 'scastd_xml'
#

DROP TABLE IF EXISTS scastd_xml;
CREATE TABLE scastd_xml (
  id INT NOT NULL AUTO_INCREMENT,
  currentlisteners VARCHAR(155) DEFAULT '' NOT NULL,
  peaklisteners VARCHAR(155) DEFAULT '' NOT NULL,
  maxlisteners VARCHAR(155) DEFAULT '' NOT NULL,
  reportedlisteners VARCHAR(155) DEFAULT '' NOT NULL,
  averagetime VARCHAR(255) DEFAULT '' NOT NULL,
  servergenre VARCHAR(155) DEFAULT '' NOT NULL,
  serverurl VARCHAR(255) DEFAULT '' NOT NULL,
  servertitle VARCHAR(255) DEFAULT '' NOT NULL,
  currentsong VARCHAR(255) DEFAULT '' NOT NULL,
  webhits VARCHAR(155) DEFAULT '' NOT NULL,
  streamhits VARCHAR(255) DEFAULT '' NOT NULL,
  hostname VARCHAR(255) DEFAULT '' NOT NULL,
  useragent VARCHAR(255) DEFAULT '' NOT NULL,
  connecttime VARCHAR(155) DEFAULT '' NOT NULL,
  songtitle VARCHAR(255) DEFAULT '' NOT NULL,
  playedat VARCHAR(55) DEFAULT '' NOT NULL,
  bwidthus VARCHAR(55) DEFAULT '' NOT NULL,
  totbwidthus VARCHAR(255) DEFAULT '' NOT NULL,
  date DATE DEFAULT '1970-01-01' NOT NULL,
  time TIME DEFAULT '00:00:00' NOT NULL,
  PRIMARY KEY (id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

#
# Dumping data for table 'scastd_xml'
#


