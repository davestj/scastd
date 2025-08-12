<!--
 * README.md - Scast Daemon Project Documentation
 * Original Authors: oddsock, dstjohn (David St. John)
 * Current Author: dstjohn (David St. John)
 * Created: Original project inception on March 2003
 * Last Modified: August 12, 2025
 * Purpose: Professional documentation for scastd - Icecast2 streaming statistics daemon
 * File Path: /README.md (root of scastd repository)
 * License: GNU General Public License v2.0
 * 
 * This README.md serves as the primary documentation for scastd, a lightweight daemon
 * I created for collecting and publishing Icecast2 stream statistics. The documentation
 * follows professional DevSecOps standards with comprehensive build instructions,
 * configuration examples, and usage guidelines.
 * 
 * Change Log:
 * - v1.0.0: Initial daemon implementation with basic HTTP API
 * - v1.1.0: Added MariaDB/MySQL and PostgreSQL support
 * - v1.2.0: Implemented comprehensive logging system with rotation
 * - v1.3.0: Added HTTPS/TLS support with certificate management
 * - v1.4.0: Enhanced configuration management with environment variables
 * - v1.5.0: Integrated CI/CD pipeline with GitHub Actions
 * - v2.0.0 (Current): Complete documentation overhaul with professional presentation
-->

<!-- Project Header with Dynamic Elements -->
<div align="center">
  <img src="https://readme-typing-svg.herokuapp.com?font=Fira+Code&size=32&duration=2800&pause=2000&color=00D9FF&center=true&vCenter=true&width=700&lines=Scast+Daemon+(scastd);Icecast2+Statistics+Collection;HTTP+API+%7C+Real-time+Monitoring;Multi-Database+Support" alt="Scastd Typing Banner" />
</div>

<!-- Professional Banner -->
<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=gradient&customColorList=0,2,2,5,30&height=180&section=header&text=Streaming%20Statistics%20%7C%20HTTP%20API%20%7C%20Real-time%20Monitoring&fontSize=20&fontColor=fff&animation=twinkling&fontAlignY=35" alt="Header Banner" />
</div>

<!-- Build Status and Project Badges -->
<div align="center">

### 🚦 Project Status & Compatibility
[![Build Status](https://img.shields.io/github/actions/workflow/status/davestj/scastd/ci.yml?branch=master&style=for-the-badge&logo=github-actions&logoColor=white&label=Build%20Status)](https://github.com/davestj/scastd/actions)
[![License](https://img.shields.io/badge/License-GPL%20v2-blue?style=for-the-badge&logo=gnu&logoColor=white)](https://www.gnu.org/licenses/gpl-2.0)
[![Version](https://img.shields.io/github/v/release/davestj/scastd?style=for-the-badge&logo=github&logoColor=white)](https://github.com/davestj/scastd/releases)
[![Last Commit](https://img.shields.io/github/last-commit/davestj/scastd?style=for-the-badge&logo=git&logoColor=white)](https://github.com/davestj/scastd/commits/master)

### 🎵 Streaming Protocol Support
[![Icecast2](https://img.shields.io/badge/Icecast2-Supported-brightgreen?style=for-the-badge&logo=music&logoColor=white)](https://icecast.org/)
[![Shoutcast v1](https://img.shields.io/badge/Shoutcast%20v1-Compatible-orange?style=for-the-badge&logo=radio&logoColor=white)](https://en.wikipedia.org/wiki/SHOUTcast)
[![Shoutcast v2](https://img.shields.io/badge/Shoutcast%20v2-Compatible-orange?style=for-the-badge&logo=radio&logoColor=white)](https://en.wikipedia.org/wiki/SHOUTcast)
[![ICY 2.1](https://img.shields.io/badge/ICY%202.1-Protocol%20Ready-purple?style=for-the-badge&logo=wave&logoColor=white)](https://en.wikipedia.org/wiki/Icecast)

### 💾 Database Support
[![MySQL](https://img.shields.io/badge/MySQL-4479A1?style=for-the-badge&logo=mysql&logoColor=white)](https://www.mysql.com/)
[![MariaDB](https://img.shields.io/badge/MariaDB-003545?style=for-the-badge&logo=mariadb&logoColor=white)](https://mariadb.org/)
[![PostgreSQL](https://img.shields.io/badge/PostgreSQL-316192?style=for-the-badge&logo=postgresql&logoColor=white)](https://www.postgresql.org/)

### 🏗️ Platform Support
[![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)](https://github.com/davestj/scastd)
[![Ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)](https://github.com/davestj/scastd)
[![Debian](https://img.shields.io/badge/Debian-A81D33?style=for-the-badge&logo=debian&logoColor=white)](https://github.com/davestj/scastd)
[![macOS](https://img.shields.io/badge/macOS-000000?style=for-the-badge&logo=apple&logoColor=white)](https://github.com/davestj/scastd)

</div>

---

## 🎯 What is scastd?

**scastd** is a lightweight, high-performance daemon I developed for collecting Icecast2 stream statistics and publishing them through a RESTful HTTP API. As a DevSecOps professional with over 20 years of experience in streaming media and infrastructure automation, I designed this tool to bridge the gap between streaming servers and modern monitoring systems.

### Why I Built This

Working with streaming media platforms since 2000, I consistently encountered the need for real-time statistics collection from Icecast2 servers. Traditional polling methods were inefficient and didn't integrate well with modern infrastructure monitoring. That's why I created scastd - to provide:

```c
/*
 * scastd_core.h - Core daemon functionality
 * Author: David St. John (dstjohn)
 * Created: Project inception
 * Purpose: I designed this daemon to efficiently poll Icecast2 servers and provide
 *          real-time statistics through a lightweight HTTP API built with libmicrohttpd
 * 
 * Key Design Principles I Implemented:
 * - Minimal resource footprint for high-performance streaming environments
 * - RESTful API design following modern web service standards
 * - Database abstraction layer supporting multiple backends
 * - Comprehensive logging with rotation and remote syslog support
 * - Security-first approach with TLS/HTTPS support
 */

// I structured the daemon with these core capabilities:
typedef struct {
    char* icecast_host;     // I connect to Icecast2 server here
    int polling_interval;   // I configure how often to collect stats
    int http_port;         // I serve the API on port 8333 by default
    database_config_t db;  // I support MySQL, MariaDB, and PostgreSQL
    logging_config_t log;  // I provide comprehensive logging capabilities
    ssl_config_t ssl;      // I enable HTTPS for secure API access
} scastd_config_t;
```

---

## ✨ Key Features & Capabilities

<div align="center">

### 🔧 Core Functionality
![Real-time Stats](https://img.shields.io/badge/Real--time%20Statistics-Polling-success?style=for-the-badge&logo=chart-line&logoColor=white)
![HTTP API](https://img.shields.io/badge/HTTP%20API-RESTful-blue?style=for-the-badge&logo=api&logoColor=white)
![JSON/XML](https://img.shields.io/badge/Output-JSON%20%7C%20XML-yellow?style=for-the-badge&logo=code&logoColor=black)
![Port 8333](https://img.shields.io/badge/Default%20Port-8333-red?style=for-the-badge&logo=network-wired&logoColor=white)

### 🔒 Security Features
![HTTPS/TLS](https://img.shields.io/badge/HTTPS%2FTLS-Supported-green?style=for-the-badge&logo=lock&logoColor=white)
![Certificate Management](https://img.shields.io/badge/Certificate-Management-purple?style=for-the-badge&logo=certificate&logoColor=white)
![Secure Config](https://img.shields.io/badge/Secure-Configuration-orange?style=for-the-badge&logo=shield&logoColor=white)

### 📊 Monitoring & Logging
![Log Rotation](https://img.shields.io/badge/Log-Rotation-blue?style=for-the-badge&logo=rotate&logoColor=white)
![Syslog Support](https://img.shields.io/badge/Remote-Syslog-green?style=for-the-badge&logo=server&logoColor=white)
![Debug Levels](https://img.shields.io/badge/Debug-Levels%201--4-yellow?style=for-the-badge&logo=bug&logoColor=black)

</div>

### What Makes scastd Special

Based on my decades of experience with streaming infrastructure, I engineered scastd with these distinguishing characteristics:

**Lightweight Architecture**: I built this daemon using libmicrohttpd, ensuring minimal resource consumption even under high-load streaming environments. The entire application maintains a small memory footprint while delivering enterprise-grade performance.

**Multi-Protocol Compatibility**: Having worked with various streaming protocols since the early 2000s, I ensured scastd seamlessly integrates with Icecast2, Shoutcast v1/v2, and ICY 2.1 protocol implementations.

**Enterprise-Ready Logging**: I implemented a comprehensive logging system with configurable rotation, retention policies, and remote syslog support - essential features I've found critical in production streaming environments.

**Database Flexibility**: I designed the database abstraction layer to support MySQL, MariaDB, and PostgreSQL, allowing you to integrate with your existing infrastructure without forcing technology choices.

---

## 🚀 Quick Start Guide

### Prerequisites I Recommend

Before building scastd, I suggest ensuring your system has these development packages installed. Based on my experience across different platforms, here's what you'll need:

<details>
<summary><b>📦 Debian/Ubuntu Installation</b></summary>

```bash
# install_dependencies_debian.sh - Debian/Ubuntu dependency installation
# Author: David St. John
# Purpose: I created this script to streamline the build process on Debian-based systems

#!/bin/bash
# I always start with system updates to ensure package compatibility
sudo apt-get update && sudo apt-get install -y \
    build-essential \
    autoconf \
    automake \
    libtool \
    pkg-config \
    libxml2-dev \
    libcurl4-openssl-dev \
    libmysqlclient-dev \
    libpq-dev \
    libmicrohttpd-dev

echo "✅ All dependencies installed successfully"
echo "🚀 Ready to build scastd"
```

</details>

<details>
<summary><b>🍎 macOS arm64 Installation</b></summary>

```bash
# install_dependencies_macos.sh - macOS dependency installation  
# Author: David St. John
# Purpose: I optimized this for Apple Silicon Macs based on my development experience

#!/bin/bash
# I use Homebrew for consistent package management on macOS
brew update
brew install autoconf automake libtool pkg-config libxml2 curl mysql-client libpq libmicrohttpd

# I set PKG_CONFIG_PATH so the build system can locate all libraries
export PKG_CONFIG_PATH="/opt/homebrew/opt/libpq/lib/pkgconfig:/opt/homebrew/opt/mysql-client/lib/pkgconfig:/opt/homebrew/opt/libxml2/lib/pkgconfig:$PKG_CONFIG_PATH"

echo "✅ macOS dependencies installed"
echo "📝 PKG_CONFIG_PATH configured for build"
```

</details>

### Building scastd

I designed the build process to be straightforward, following standard autotools conventions:

```bash
# build_scastd.sh - Complete build script
# Author: David St. John  
# Purpose: I created this comprehensive build script for reliable compilation

#!/bin/bash
set -e  # I enable strict error handling

echo "🔧 Generating build system files..."
# I generate the configure script and Makefiles
./autogen.sh

echo "🔍 Configuring build with optimizations..."
# I configure with C++17 support and optimization flags
./configure CXXFLAGS='-std=c++17 -g -O2'

echo "⚡ Building scastd..."
# I use parallel compilation to speed up the build process
make -j$(nproc)

echo "🧪 Running test suite..."
# I always run tests to ensure build quality
make check

echo "✅ Build completed successfully!"
echo "📍 Binary location: ./src/scastd"
```

---

## ⚙️ Configuration Management

I designed scastd's configuration system to be both simple and powerful. The daemon reads settings from a configuration file that supports environment variable overrides - a pattern I've found essential in containerized environments.

### Basic Configuration

Here's a complete configuration example I recommend for production use:

```ini
# scastd.conf - Production configuration template
# Author: David St. John
# Created: Based on 20+ years of streaming infrastructure experience
# Purpose: I provide this template to demonstrate all available configuration options

# =============================================================================
# DATABASE CONFIGURATION
# I support MySQL, MariaDB, and PostgreSQL backends
# =============================================================================
hostname localhost
port 3306
database scastd_stats
username scastd_user
password your_secure_password_here

# Alternative: I allow loading credentials from files for enhanced security
# username_file /etc/scastd/db_username
# password_file /etc/scastd/db_password

# =============================================================================
# ICECAST2 SERVER CONFIGURATION  
# I poll this server for streaming statistics
# =============================================================================
icecast_host localhost
icecast_port 8000
icecast_admin_path /admin/stats.xml
polling_interval 30

# =============================================================================
# HTTP API CONFIGURATION
# I serve the REST API on this port with optional HTTPS
# =============================================================================
http_port 8333
bind_address 0.0.0.0

# =============================================================================
# HTTPS/TLS CONFIGURATION (Optional but recommended)
# I provide secure API access when these options are enabled
# =============================================================================
ssl_enabled true
ssl_cert /etc/scastd/ssl/server.crt
ssl_key /etc/scastd/ssl/server.key
ssl_require_client_cert false

# =============================================================================
# LOGGING CONFIGURATION
# I provide comprehensive logging with rotation and remote syslog support
# =============================================================================
log_dir /var/log/scastd
log_max_size 10485760  # 10MB - I rotate logs at this size
log_retention 7        # I keep 7 rotated log files
access_log access.log
error_log error.log  
debug_log debug.log
debug_level 2          # I use level 1-4 (1=minimal, 4=verbose)

# =============================================================================
# REMOTE LOGGING CONFIGURATION
# I can forward logs to a central syslog server
# =============================================================================
syslog_enabled false
syslog_host log-server.example.com
syslog_port 514
syslog_protocol udp    # I support both 'udp' and 'tcp'
syslog_facility local0
```

### Environment Variable Overrides

I implemented environment variable support for containerized deployments:

```bash
# docker_environment.sh - Container environment setup
# Author: David St. John
# Purpose: I demonstrate how to use environment variables for secure configuration

#!/bin/bash
# I prefer environment variables for sensitive data in containers

export SCASTD_USERNAME="scastd_user"
export SCASTD_PASSWORD="$(cat /run/secrets/db_password)"
export SCASTD_HOSTNAME="mysql-server.internal"
export SCASTD_DATABASE="streaming_stats"

# I can also point to secret files mounted by orchestration systems
export SCASTD_USERNAME_FILE="/run/secrets/db_username"
export SCASTD_PASSWORD_FILE="/run/secrets/db_password"

echo "🔒 Environment variables configured for secure operation"
```

### Security Best Practices I Recommend

Based on my DevSecOps experience, I strongly advise implementing these security measures:

```bash
# secure_scastd.sh - Security hardening script
# Author: David St. John
# Purpose: I implement security best practices for production deployments

#!/bin/bash
set -e

echo "🔒 Implementing security hardening..."

# I set restrictive permissions on configuration files
chmod 600 scastd.conf
chmod 600 /etc/scastd/ssl/server.key

# I create a dedicated user for the daemon
sudo useradd --system --no-create-home --shell /bin/false scastd

# I set proper ownership
sudo chown scastd:scastd /var/log/scastd
sudo chown scastd:scastd /etc/scastd

# I configure systemd service with security restrictions
sudo tee /etc/systemd/system/scastd.service > /dev/null << 'EOF'
[Unit]
Description=Scast Daemon - Streaming Statistics Collector
After=network.target mysql.service

[Service]
Type=simple
User=scastd
Group=scastd
ExecStart=/usr/local/bin/scastd /etc/scastd/scastd.conf
Restart=always
RestartSec=10

# Security hardening I implement
NoNewPrivileges=yes
PrivateTmp=yes
ProtectSystem=strict
ProtectHome=yes
ReadWritePaths=/var/log/scastd

[Install]
WantedBy=multi-user.target
EOF

echo "✅ Security hardening completed"
```

---

## 🔌 API Reference & Usage

I designed the HTTP API following RESTful principles with both JSON and XML output formats. The API serves statistics on port 8333 by default, with versioned endpoints for future compatibility.

### API Endpoints I Provide

```http
# api_endpoints.http - Complete API reference
# Author: David St. John
# Purpose: I document all available API endpoints with examples

# =============================================================================
# PRIMARY ENDPOINTS (Versioned - I recommend using these)
# =============================================================================

### Get JSON statistics
GET http://localhost:8333/v1/status.json
Accept: application/json

### Get XML statistics  
GET http://localhost:8333/v1/status.xml
Accept: application/xml

# =============================================================================
# LEGACY ENDPOINTS (Unversioned - I maintain for backward compatibility)
# =============================================================================

### Legacy JSON endpoint
GET http://localhost:8333/status.json

### Legacy XML endpoint
GET http://localhost:8333/status.xml

# =============================================================================
# HEALTH CHECK ENDPOINT
# =============================================================================

### Service health check
GET http://localhost:8333/health
```

### JSON Response Format

I structure the JSON response to provide comprehensive streaming statistics:

```json
{
  "response_format": "JSON API Response Example",
  "author": "David St. John",
  "purpose": "I provide this example to show the complete data structure",
  "timestamp": "2025-08-12T10:30:00Z",
  "version": "v1",
  "status": "success",
  "data": {
    "server": {
      "hostname": "stream.example.com",
      "port": 8000,
      "version": "Icecast 2.4.4",
      "uptime": 86400,
      "total_listeners": 1547
    },
    "mounts": [
      {
        "mount": "/live.mp3",
        "stream_title": "Live Music Stream",
        "current_listeners": 234,
        "peak_listeners": 456,
        "bitrate": 128,
        "format": "MP3",
        "sample_rate": 44100,
        "channels": 2,
        "source_ip": "192.168.1.100",
        "connected_since": "2025-08-12T08:00:00Z",
        "metadata": {
          "artist": "Sample Artist",
          "title": "Current Track",
          "album": "Live Performance"
        }
      }
    ],
    "statistics": {
      "total_connections": 15047,
      "total_bytes_sent": 1234567890,
      "average_bitrate": 128.5,
      "connection_rate": 12.3
    }
  },
  "collection": {
    "timestamp": "2025-08-12T10:30:00Z",
    "duration_ms": 45,
    "next_poll": "2025-08-12T10:30:30Z"
  }
}
```

### Integration Examples I Provide

Here are practical integration examples I've developed for common monitoring systems:

<details>
<summary><b>📊 Prometheus Integration</b></summary>

```python
#!/usr/bin/env python3
# prometheus_exporter.py - Prometheus metrics exporter for scastd
# Author: David St. John
# Purpose: I created this exporter to integrate scastd with Prometheus monitoring

import requests
import time
from prometheus_client import start_http_server, Gauge, Counter, Info

class ScastdPrometheusExporter:
    """I export scastd metrics in Prometheus format"""
    
    def __init__(self, scastd_url='http://localhost:8333'):
        self.scastd_url = scastd_url
        
        # I define Prometheus metrics for comprehensive monitoring
        self.total_listeners = Gauge('scastd_total_listeners', 'Total listeners across all mounts')
        self.mount_listeners = Gauge('scastd_mount_listeners', 'Listeners per mount', ['mount', 'title'])
        self.connection_rate = Gauge('scastd_connection_rate', 'New connections per second')
        self.bytes_sent = Counter('scastd_bytes_sent_total', 'Total bytes sent')
        self.server_info = Info('scastd_server', 'Server information')
        
    def collect_metrics(self):
        """I collect and update metrics from scastd API"""
        try:
            response = requests.get(f'{self.scastd_url}/v1/status.json', timeout=5)
            data = response.json()
            
            # I update server-level metrics
            self.total_listeners.set(data['data']['server']['total_listeners'])
            self.connection_rate.set(data['data']['statistics']['connection_rate'])
            
            # I update mount-specific metrics
            for mount in data['data']['mounts']:
                self.mount_listeners.labels(
                    mount=mount['mount'],
                    title=mount['stream_title']
                ).set(mount['current_listeners'])
            
            print(f"✅ Metrics updated at {time.strftime('%Y-%m-%d %H:%M:%S')}")
            
        except Exception as e:
            print(f"❌ Error collecting metrics: {e}")

def main():
    """I run the Prometheus exporter service"""
    exporter = ScastdPrometheusExporter()
    
    # I start the Prometheus HTTP server on port 9090
    start_http_server(9090)
    print("🚀 Prometheus exporter started on port 9090")
    
    # I collect metrics every 30 seconds
    while True:
        exporter.collect_metrics()
        time.sleep(30)

if __name__ == '__main__':
    main()
```

</details>

<details>
<summary><b>📈 Grafana Dashboard</b></summary>

```json
{
  "dashboard_config": "Grafana Dashboard JSON",
  "author": "David St. John", 
  "purpose": "I provide this dashboard configuration for comprehensive scastd monitoring",
  "dashboard": {
    "id": null,
    "title": "Scastd Streaming Statistics",
    "description": "I created this dashboard to visualize streaming server performance",
    "panels": [
      {
        "id": 1,
        "title": "Total Listeners",
        "type": "singlestat",
        "targets": [
          {
            "expr": "scastd_total_listeners",
            "legendFormat": "Current Listeners"
          }
        ]
      },
      {
        "id": 2,
        "title": "Listener Growth",
        "type": "graph", 
        "targets": [
          {
            "expr": "rate(scastd_total_listeners[5m])",
            "legendFormat": "Listener Change Rate"
          }
        ]
      }
    ]
  }
}
```

</details>

---

## 🧪 Testing & Quality Assurance

I implemented comprehensive testing to ensure scastd's reliability in production environments. The test suite covers unit tests, integration tests, and performance benchmarks.

### Running the Test Suite

```bash
# test_scastd.sh - Comprehensive testing script
# Author: David St. John
# Purpose: I created this script to validate all aspects of scastd functionality

#!/bin/bash
set -e

echo "🧪 Starting scastd test suite..."

# I run unit tests first
echo "📋 Running unit tests..."
make check

# I test API endpoints
echo "🔌 Testing API endpoints..."
./tests/api_test.sh

# I run performance benchmarks
echo "⚡ Running performance tests..."
./tests/performance_test.sh

# I validate configuration parsing
echo "⚙️ Testing configuration management..."
./tests/config_test.sh

echo "✅ All tests passed successfully!"
```

---

## 📊 Project Analytics & Community

<!-- GitHub Project Statistics -->
<div align="center">
  
  ### 📈 Project Statistics
  ![GitHub Stars](https://img.shields.io/github/stars/davestj/scastd?style=for-the-badge&logo=star&logoColor=white)
  ![GitHub Forks](https://img.shields.io/github/forks/davestj/scastd?style=for-the-badge&logo=fork&logoColor=white)
  ![GitHub Issues](https://img.shields.io/github/issues/davestj/scastd?style=for-the-badge&logo=issue&logoColor=white)
  ![GitHub Pull Requests](https://img.shields.io/github/issues-pr/davestj/scastd?style=for-the-badge&logo=pull-request&logoColor=white)
  
  ### 📊 Contribution Activity
  <img src="https://github-readme-activity-graph.vercel.app/graph?username=davestj&repo=scastd&theme=tokyo-night&hide_border=true&area=true&custom_title=Scastd%20Development%20Activity" alt="Development Activity"/>
  
  ### 🏆 Release Information
  ![Latest Release](https://img.shields.io/github/v/release/davestj/scastd?style=for-the-badge&logo=github&logoColor=white)
  ![Release Date](https://img.shields.io/github/release-date/davestj/scastd?style=for-the-badge&logo=calendar&logoColor=white)
  ![Downloads](https://img.shields.io/github/downloads/davestj/scastd/total?style=for-the-badge&logo=download&logoColor=white)
  
</div>

---

## 🤝 Contributing to scastd

I welcome contributions from the streaming media and DevOps communities. Having maintained open source projects for over two decades, I believe in fostering collaborative development while maintaining high code quality standards.

### How I Prefer Contributions

```markdown
# CONTRIBUTING.md - Contribution Guidelines
# Author: David St. John
# Purpose: I outline the contribution process based on DevSecOps best practices

## Development Workflow I Recommend

1. **Fork & Clone**: I suggest forking the repository and working in feature branches
2. **Code Standards**: I require adherence to our coding standards (see .clang-format)
3. **Testing**: I expect all contributions to include appropriate tests
4. **Documentation**: I require documentation updates for new features
5. **Security**: I mandate security review for all code changes

## Areas Where I Need Help

- 🔧 **Protocol Support**: Expanding compatibility with additional streaming protocols
- 🔍 **Monitoring Integrations**: New exporters for monitoring systems
- 📊 **Database Backends**: Additional database driver implementations  
- 🧪 **Testing**: Expanding test coverage and performance benchmarks
- 📚 **Documentation**: User guides, tutorials, and deployment examples
```

### Coding Standards I Enforce

```c
/*
 * coding_standards.h - Code formatting and style guidelines
 * Author: David St. John
 * Purpose: I maintain consistent code quality across all contributions
 */

// I follow these naming conventions consistently:
typedef struct {
    char* server_hostname;     // I use snake_case for variables
    int connection_timeout;    // I use descriptive names
    database_handle_t db;      // I use typed handles for clarity
} scastd_connection_t;        // I use _t suffix for types

// I structure functions with clear documentation:
/**
 * I collect statistics from the specified Icecast2 server
 * @param config: Server configuration parameters I need
 * @param stats: Output structure where I store collected data  
 * @return: Status code indicating success or specific error condition
 */
int scastd_collect_stats(const scastd_config_t* config, 
                        stream_stats_t* stats);
```

---

## 🔧 Deployment & Operations

### Docker Deployment I Recommend

```dockerfile
# Dockerfile - Production-ready container configuration
# Author: David St. John  
# Purpose: I provide this optimized container setup for production deployments

FROM debian:12-slim

LABEL maintainer="davestj@gmail.com"
LABEL description="Scastd - Icecast2 Statistics Daemon"
LABEL version="2.0.0"

# I install runtime dependencies only
RUN apt-get update && apt-get install -y \
    libxml2 \
    libcurl4 \
    libmysqlclient21 \
    libpq5 \
    libmicrohttpd12 \
    && rm -rf /var/lib/apt/lists/*

# I create a non-root user for security
RUN useradd --system --no-create-home --shell /bin/false scastd

# I copy the compiled binary and configuration
COPY --from=builder /app/src/scastd /usr/local/bin/
COPY docker/scastd.conf /etc/scastd/

# I set up logging directory
RUN mkdir -p /var/log/scastd && chown scastd:scastd /var/log/scastd

# I expose the API port
EXPOSE 8333

# I run as non-root user
USER scastd

# I provide health check capability
HEALTHCHECK --interval=30s --timeout=5s --start-period=10s --retries=3 \
    CMD curl -f http://localhost:8333/health || exit 1

# I start the daemon with proper configuration
ENTRYPOINT ["/usr/local/bin/scastd", "/etc/scastd/scastd.conf"]
```

### Kubernetes Deployment

```yaml
# k8s-deployment.yaml - Kubernetes deployment manifest
# Author: David St. John
# Purpose: I provide cloud-native deployment configuration for scastd

apiVersion: apps/v1
kind: Deployment
metadata:
  name: scastd
  labels:
    app: scastd
    author: davestj
spec:
  replicas: 2
  selector:
    matchLabels:
      app: scastd
  template:
    metadata:
      labels:
        app: scastd
    spec:
      containers:
      - name: scastd
        image: davestj/scastd:2.0.0
        ports:
        - containerPort: 8333
          name: http-api
        env:
        - name: SCASTD_HOSTNAME
          value: "mysql-service"
        - name: SCASTD_USERNAME
          valueFrom:
            secretKeyRef:
              name: scastd-db-credentials
              key: username
        - name: SCASTD_PASSWORD
          valueFrom:
            secretKeyRef:
              name: scastd-db-credentials  
              key: password
        resources:
          requests:
            memory: "64Mi"
            cpu: "50m"
          limits:
            memory: "128Mi" 
            cpu: "100m"
        livenessProbe:
          httpGet:
            path: /health
            port: 8333
          initialDelaySeconds: 10
          periodSeconds: 30
        readinessProbe:
          httpGet:
            path: /v1/status.json
            port: 8333
          initialDelaySeconds: 5
          periodSeconds: 10
---
apiVersion: v1
kind: Service
metadata:
  name: scastd-service
spec:
  selector:
    app: scastd
  ports:
  - port: 8333
    targetPort: 8333
    name: http-api
  type: ClusterIP
```

---

## 📞 Support & Contact

<div align="center">

### 🛠️ Technical Support
I provide support through multiple channels based on your needs:

[![GitHub Issues](https://img.shields.io/badge/GitHub-Issues-green?style=for-the-badge&logo=github&logoColor=white)](https://github.com/davestj/scastd/issues)
[![Email Support](https://img.shields.io/badge/Email-davestj%40gmail.com-blue?style=for-the-badge&logo=gmail&logoColor=white)](mailto:davestj@gmail.com)
[![Documentation](https://img.shields.io/badge/Documentation-Wiki-purple?style=for-the-badge&logo=gitbook&logoColor=white)](https://github.com/davestj/scastd/wiki)

### 💼 Professional Services
![Consulting](https://img.shields.io/badge/DevSecOps-Consulting-orange?style=for-the-badge&logo=handshake&logoColor=white)
![Architecture](https://img.shields.io/badge/Streaming-Architecture-red?style=for-the-badge&logo=stream&logoColor=white)
![Training](https://img.shields.io/badge/Team-Training-yellow?style=for-the-badge&logo=graduation-cap&logoColor=black)

</div>

---

## 📚 Additional Resources

### Documentation I Maintain

- 📖 **[Installation Guide](docs/INSTALL.md)** - Comprehensive setup instructions
- 🔧 **[Configuration Reference](docs/CONFIG.md)** - Complete configuration options
- 🔌 **[API Documentation](docs/API.md)** - Detailed endpoint specifications  
- 🐳 **[Container Guide](docs/DOCKER.md)** - Docker and Kubernetes deployment
- 🔒 **[Security Guide](docs/SECURITY.md)** - Production security recommendations
- 🧪 **[Testing Guide](docs/TESTING.md)** - Development and testing procedures

### Community Resources

- 💬 **[Discussions](https://github.com/davestj/scastd/discussions)** - Community Q&A and feature requests
- 🎯 **[Project Board](https://github.com/davestj/scastd/projects)** - Development roadmap and progress
- 📋 **[Wiki](https://github.com/davestj/scastd/wiki)** - Extended documentation and tutorials
- 🏷️ **[Releases](https://github.com/davestj/scastd/releases)** - Version history and changelog

---

## 📝 License & Legal

This project is licensed under the **GNU General Public License v2.0**. I chose this license to ensure scastd remains open source and benefits the entire streaming media community.

```
Copyright (C) 2025 oddsock, David St. John (dstjohn)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
```

---

<!-- Footer -->
<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=gradient&customColorList=0,2,2,5,30&height=100&section=footer&text=Built%20with%20❤️%20for%20the%20Streaming%20Community&fontSize=18&fontColor=fff&animation=twinkling&fontAlignY=70" alt="Footer" />
  
  <sub>Created by David St. John | DevSecOps Professional | 20+ Years Streaming Infrastructure Experience</sub>
</div>

<!-- 
  Documentation Maintenance Notes (August 12, 2025):
  ==================================================
  
  I maintain this README.md as the primary project documentation following
  professional DevSecOps standards. This document combines technical depth
  with visual appeal to serve both developers and end users effectively.
  
  Key Features I Implemented:
  - Comprehensive badge system showing protocol compatibility
  - Professional visual design with consistent theming
  - Complete code examples with inline documentation
  - Security-focused configuration examples
  - Production-ready deployment configurations
  - Community engagement features
  
  Maintenance Schedule:
  - Update build status badges with each release
  - Refresh compatibility information quarterly  
  - Review and update code examples monthly
  - Validate all links and references weekly
  
  Version History:
  - v1.0: Basic technical documentation
  - v2.0: Enhanced with professional presentation (Current)
  
  For questions about this documentation, contact: davestj@gmail.com
-->
