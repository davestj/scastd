-- SQLite schema for scastd

-- Remove legacy member table
DROP TABLE IF EXISTS scastd_memberinfo;

-- Table structure for table 'servers'
DROP TABLE IF EXISTS servers;
CREATE TABLE servers (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  server_host TEXT NOT NULL,
  server_port INTEGER NOT NULL,
  server_username TEXT,
  server_password TEXT
);

-- Dumping data for table 'servers'
INSERT INTO servers (server_host, server_port, server_username, server_password) VALUES
  ('example.com', 8000, NULL, NULL);

-- Table structure for table 'scastd_runtime'
DROP TABLE IF EXISTS scastd_runtime;
CREATE TABLE scastd_runtime (
  sleeptime INTEGER,
  logfile TEXT
);

-- Dumping data for table 'scastd_runtime'
INSERT INTO scastd_runtime (sleeptime, logfile) VALUES (30, './scastd.log');

-- Table structure for table 'scastd_serverinfo'
DROP TABLE IF EXISTS scastd_serverinfo;
CREATE TABLE scastd_serverinfo (
  serverURL TEXT,
  currentlisteners INTEGER,
  peaklisteners INTEGER,
  maxlisteners INTEGER,
  averagetime INTEGER,
  streamhits INTEGER,
  time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Table structure for table 'scastd_songinfo'
DROP TABLE IF EXISTS scastd_songinfo;
CREATE TABLE scastd_songinfo (
  serverURL TEXT,
  songTitle TEXT,
  time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Table structure for table 'scastd_xml'
DROP TABLE IF EXISTS scastd_xml;
CREATE TABLE scastd_xml (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  currentlisteners TEXT NOT NULL DEFAULT '',
  peaklisteners TEXT NOT NULL DEFAULT '',
  maxlisteners TEXT NOT NULL DEFAULT '',
  reportedlisteners TEXT NOT NULL DEFAULT '',
  averagetime TEXT NOT NULL DEFAULT '',
  servergenre TEXT NOT NULL DEFAULT '',
  serverurl TEXT NOT NULL DEFAULT '',
  servertitle TEXT NOT NULL DEFAULT '',
  currentsong TEXT NOT NULL DEFAULT '',
  webhits TEXT NOT NULL DEFAULT '',
  streamhits TEXT NOT NULL DEFAULT '',
  hostname TEXT NOT NULL DEFAULT '',
  useragent TEXT NOT NULL DEFAULT '',
  connecttime TEXT NOT NULL DEFAULT '',
  songtitle TEXT NOT NULL DEFAULT '',
  playedat TEXT NOT NULL DEFAULT '',
  bwidthus TEXT NOT NULL DEFAULT '',
  totbwidthus TEXT NOT NULL DEFAULT '',
  date TEXT NOT NULL DEFAULT '',
  time TEXT NOT NULL DEFAULT ''
);

