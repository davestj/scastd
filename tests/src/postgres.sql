-- PostgreSQL schema for scastd

-- Table structure for table 'scastd_memberinfo'
DROP TABLE IF EXISTS scastd_memberinfo;
CREATE TABLE scastd_memberinfo (
  serverURL TEXT PRIMARY KEY,
  password TEXT NOT NULL DEFAULT '',
  gather_flag TEXT NOT NULL DEFAULT ''
);

-- Dumping data for table 'scastd_memberinfo'
INSERT INTO scastd_memberinfo (serverURL, password, gather_flag) VALUES
  ('http://boa.mediacast1.com:9908', 'party7324', '1');

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
  id SERIAL PRIMARY KEY,
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
  date DATE NOT NULL DEFAULT CURRENT_DATE,
  time TIME NOT NULL DEFAULT CURRENT_TIME
);
