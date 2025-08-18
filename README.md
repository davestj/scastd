<!--
 * README.md - SCASTD Project Documentation
 * Authors: oddsock, dstjohn (David St. John)
 * Created: Original project inception
 * Last Modified: August 12, 2025
 * Purpose: Primary documentation for SCASTD - Statistical Casting Daemon
 * File Path: /README.md (root of scastd repository)
 * License: GNU General Public License v2.0
 * 
 * We maintain this README.md as the central hub for all project documentation.
 * This file serves as both an educational guide and professional project showcase
 * for developers, system administrators, and streaming infrastructure professionals.
 * 
 * Change Log:
 * - v1.0.0: Initial daemon implementation with basic HTTP API
* - v1.1.0: Added multi-database backend support (MariaDB, PostgreSQL)
 * - v1.2.0: Implemented comprehensive logging system with rotation
 * - v1.3.0: Added HTTPS/TLS support with certificate management
 * - v1.4.0: Enhanced configuration management with environment variables
 * - v1.5.0: Integrated CI/CD pipeline with GitHub Actions across three branches
 * - v2.0.0 (Current): Complete documentation overhaul with professional presentation
 *
 * Git Commit: git commit -m "docs: Create comprehensive main README with full project navigation"
 * Next Development: Implement core statistics collection engine and ICY2 protocol support
-->

<!-- Project Header with Dynamic Elements -->
<div align="center">
  <img src="https://readme-typing-svg.herokuapp.com?font=Fira+Code&size=32&duration=2800&pause=2000&color=00D9FF&center=true&vCenter=true&width=800&lines=SCASTD+-+Statistical+Casting+Daemon;Real-time+Streaming+Statistics+Collection;Multi-Protocol+Monitoring+%26+Analytics;Enterprise-Grade+Reporting+Engine" alt="SCASTD Typing Banner" />
</div>

<!-- Professional Banner -->
<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=gradient&customColorList=0,2,2,5,30&height=180&section=header&text=Statistical%20Collection%20%7C%20Real-time%20Analytics%20%7C%20Multi-Protocol%20Support&fontSize=20&fontColor=fff&animation=twinkling&fontAlignY=35" alt="Header Banner" />
</div>

<!-- Build Status and Project Badges -->
<div align="center">

### 🚦 Build Status & Development Pipeline
[![Production Build](https://img.shields.io/github/actions/workflow/status/davestj/scastd/production.yml?branch=master&style=for-the-badge&logo=github-actions&logoColor=white&label=Production%20Build)](https://github.com/davestj/scastd/actions/workflows/production.yml)
[![Development Build](https://img.shields.io/github/actions/workflow/status/davestj/scastd/development.yml?branch=dev&style=for-the-badge&logo=github-actions&logoColor=white&label=Development%20Build)](https://github.com/davestj/scastd/actions/workflows/development.yml)
[![Future Build](https://img.shields.io/github/actions/workflow/status/davestj/scastd/future.yml?branch=future&style=for-the-badge&logo=github-actions&logoColor=white&label=Future%20Build)](https://github.com/davestj/scastd/actions/workflows/future.yml)

### 📊 Project Status & Compatibility
[![License](https://img.shields.io/badge/License-GPL%20v2-blue?style=for-the-badge&logo=gnu&logoColor=white)](COPYING.md)
[![Version](https://img.shields.io/github/v/release/davestj/scastd?style=for-the-badge&logo=github&logoColor=white)](https://github.com/davestj/scastd/releases)
[![Last Commit](https://img.shields.io/github/last-commit/davestj/scastd?style=for-the-badge&logo=git&logoColor=white)](https://github.com/davestj/scastd/commits/master)
[![Contributors](https://img.shields.io/github/contributors/davestj/scastd?style=for-the-badge&logo=github&logoColor=white)](AUTHORS.md)

### 🎵 Streaming Protocol Support
[![SHOUTcast v1](https://img.shields.io/badge/SHOUTcast%20v1-Full%20Support-brightgreen?style=for-the-badge&logo=radio&logoColor=white)](docs/Icecast2.md)
[![SHOUTcast v2](https://img.shields.io/badge/SHOUTcast%20v2-Enhanced%20Support-brightgreen?style=for-the-badge&logo=radio&logoColor=white)](docs/Icecast2.md)
[![Icecast2](https://img.shields.io/badge/Icecast2-Mount--Point%20Aware-success?style=for-the-badge&logo=music&logoColor=white)](docs/Icecast2.md)
[![ICY Protocol](https://img.shields.io/badge/ICY%20Protocol-Legacy%20Compatible-orange?style=for-the-badge&logo=wave&logoColor=white)](docs/Icecast2.md)

### 💾 Database Support
[![MariaDB](https://img.shields.io/badge/MariaDB-003545?style=for-the-badge&logo=mariadb&logoColor=white)](docs/DatabaseMigration.md)
[![PostgreSQL](https://img.shields.io/badge/PostgreSQL-316192?style=for-the-badge&logo=postgresql&logoColor=white)](docs/PostgreSQL.md)
[![SQLite](https://img.shields.io/badge/SQLite-07405E?style=for-the-badge&logo=sqlite&logoColor=white)](docs/DatabaseMigration.md)

### 🏗️ Platform Support
[![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)](INSTALL.md)
[![Ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)](INSTALL.md)
[![Debian](https://img.shields.io/badge/Debian-A81D33?style=for-the-badge&logo=debian&logoColor=white)](INSTALL.md)
[![macOS](https://img.shields.io/badge/macOS-000000?style=for-the-badge&logo=apple&logoColor=white)](INSTALL.md)

</div>

## 🔐 Verify Downloads

Releases ship Debian `.deb` packages, a macOS `.pkg` installer, and a Homebrew formula (`scastd.rb`). SHA256 checksums for all artifacts are provided in `CHECKSUMS.txt`. Verify package integrity before installation:

```bash
sha256sum -c CHECKSUMS.txt
```

macOS users can run:

```bash
shasum -a 256 -c CHECKSUMS.txt
```

## 🧪 Required Secrets for CI and Local Testing

| Secret | Purpose |
| ------ | ------- |
| `MARIADB_TEST_HOST` | Hostname of the MariaDB instance used for tests |
| `MARIADB_TEST_USER` | User for the test MariaDB database |
| `MARIADB_TEST_PASS` | Password for the test MariaDB user |
| `ICECAST_DEV_HOST` | Hostname of the development Icecast server |
| `ICECAST_DEV_PORT` | Port of the development Icecast server |
| `ICEADMINUSER` | Icecast administrator username used in tests |
| `ICEUSERPASS` | Icecast user password used in tests |

---

## 🎯 What is SCASTD?

**SCASTD** (Statistical Casting Daemon) is a lightweight, high-performance statistics collection and analytics engine we designed specifically for streaming media infrastructure. Unlike traditional streaming servers, we built SCASTD to be the definitive monitoring solution that bridges legacy and modern streaming protocols, providing unified analytics across your entire streaming ecosystem.

### Why We Built This

As DevSecOps professionals with over 20 years of combined experience in streaming media infrastructure, we consistently encountered the challenge of collecting unified statistics from diverse streaming server implementations. Traditional monitoring tools either focused on single protocols or provided basic metrics without the depth needed for professional streaming operations. That's why we created SCASTD - to provide:

```c
/*
 * scastd_core.h - Statistical Collection Engine Architecture
 * Authors: oddsock, dstjohn (David St. John)
 * Created: Project inception
 * Purpose: We designed this daemon to efficiently collect statistics from multiple
 *          streaming protocols and provide unified analytics through modern APIs
 * 
 * Key Design Principles We Implemented:
 * - Protocol-agnostic statistics collection
 * - Real-time data aggregation and analysis
 * - Modern RESTful API with JSON/XML output
 * - Multi-database backend support for scalability
 * - Enterprise-grade logging and monitoring
 * - Security-first approach with authentication
 */

// We structured the daemon with these core capabilities:
typedef struct {
    char* target_servers[];      // We monitor multiple server types simultaneously
    int polling_intervals[];     // We configure collection frequency per server
    database_config_t backends; // We support multiple database backends
    api_config_t rest_api;      // We serve statistics via modern HTTP APIs
    logging_config_t logging;   // We provide comprehensive operational logging
    security_config_t auth;     // We implement token-based authentication
} scastd_engine_t;
```

---

## ✨ Core Features & Capabilities

<div align="center">

### 🔧 Statistics Collection Engine
![Real-time Collection](https://img.shields.io/badge/Real--time-Statistics%20Collection-success?style=for-the-badge&logo=chart-line&logoColor=white)
![Multi-Protocol](https://img.shields.io/badge/Multi--Protocol-Support-blue?style=for-the-badge&logo=network-wired&logoColor=white)
![Historical Analysis](https://img.shields.io/badge/Historical-Analysis-purple?style=for-the-badge&logo=history&logoColor=white)
![Performance Metrics](https://img.shields.io/badge/Performance-Metrics-orange?style=for-the-badge&logo=speedometer&logoColor=white)

### 🌐 API & Integration
![RESTful API](https://img.shields.io/badge/RESTful-API-green?style=for-the-badge&logo=api&logoColor=white)
![JSON/XML Output](https://img.shields.io/badge/JSON%2FXML-Output-yellow?style=for-the-badge&logo=code&logoColor=black)
![WebSocket Support](https://img.shields.io/badge/WebSocket-Real--time-red?style=for-the-badge&logo=websocket&logoColor=white)
![Prometheus Compatible](https://img.shields.io/badge/Prometheus-Compatible-E6522C?style=for-the-badge&logo=prometheus&logoColor=white)

### 🔒 Security & Operations
![JWT Authentication](https://img.shields.io/badge/JWT-Authentication-4A90E2?style=for-the-badge&logo=jsonwebtokens&logoColor=white)
![HTTPS/TLS](https://img.shields.io/badge/HTTPS%2FTLS-Secured-green?style=for-the-badge&logo=lock&logoColor=white)
![Audit Logging](https://img.shields.io/badge/Audit-Logging-purple?style=for-the-badge&logo=file-text&logoColor=white)
![Rate Limiting](https://img.shields.io/badge/Rate-Limiting-orange?style=for-the-badge&logo=shield&logoColor=white)

</div>

### What Makes SCASTD Unique

Based on our decades of experience with streaming infrastructure, we engineered SCASTD with these distinguishing characteristics that set it apart from traditional monitoring solutions:

**Protocol Unification**: We implement native support for SHOUTcast v1/v2, Icecast2, and legacy ICY protocols, providing a single interface to monitor heterogeneous streaming environments. This eliminates the need for multiple monitoring tools and provides consistent metrics across different server types.

**Real-time Analytics Engine**: We built a sophisticated statistics aggregation system that not only collects current metrics but also performs historical analysis, trend detection, and predictive analytics. This gives streaming operators the insights they need to optimize performance and plan capacity.

**Enterprise Integration Ready**: We designed SCASTD with modern DevOps practices in mind, providing native integration with Prometheus, Grafana, ELK stack, and other monitoring platforms. The RESTful API design ensures seamless integration with existing infrastructure monitoring systems.

**Scalable Architecture**: We implemented a multi-threaded collection engine with configurable polling intervals and database connection pooling, allowing SCASTD to monitor hundreds of streaming servers while maintaining low resource consumption.

---

## 🚀 Quick Start Guide

### Understanding Our Development Workflow

Before diving into installation, it's important to understand how we structure our development process. We maintain three distinct branches that serve different purposes in our engineering pipeline:

- **`master` Branch**: Our production-ready code that undergoes our ProductionBuildDeployment pipeline
- **`dev` Branch**: Our active development branch running the DevelopmentBuildPipeline for integration testing
- **`future` Branch**: Our experimental features branch using the FutureBuildPipeline for cutting-edge development

This three-tier approach allows us to maintain stability in production while continuously innovating and testing new features.

### Prerequisites We Recommend

Before building SCASTD, we suggest ensuring your system has the necessary development tools and libraries. Based on our experience across different platforms, here's what you'll need:

<details>
<summary><b>📦 Debian/Ubuntu Installation</b></summary>

```bash
# install_dependencies_debian.sh - Debian/Ubuntu dependency installation
# Authors: SCASTD Development Team
# Purpose: We streamline the build process on Debian-based systems

#!/bin/bash
# We always start with system updates to ensure package compatibility
sudo apt-get update && sudo apt-get install -y \
    build-essential \
    autoconf \
    automake \
    libtool \
    pkg-config \
    libxml2-dev \
    libcurl4-openssl-dev \
    libmariadb-dev \
    libpq-dev \
    libmicrohttpd-dev \
    libssl-dev \
    libyaml-dev

echo "✅ All dependencies installed successfully"
echo "🚀 Ready to build SCASTD"
```

</details>

<details>
<summary><b>🍎 macOS arm64 Installation</b></summary>

```bash
# install_dependencies_macos.sh - macOS dependency installation  
# Authors: SCASTD Development Team
# Purpose: We optimize this for Apple Silicon Macs based on our development experience

#!/bin/bash
# We use Homebrew for consistent package management on macOS
brew update
brew install autoconf automake libtool pkg-config libxml2 curl mariadb-connector-c libpq libmicrohttpd libyaml

# We set PKG_CONFIG_PATH so the build system can locate all libraries
export PKG_CONFIG_PATH="/opt/homebrew/opt/libpq/lib/pkgconfig:/opt/homebrew/opt/mariadb-connector-c/lib/pkgconfig:/opt/homebrew/opt/libxml2/lib/pkgconfig:$PKG_CONFIG_PATH"

echo "✅ macOS dependencies installed"
echo "📝 PKG_CONFIG_PATH configured for build"
```

</details>

### Building SCASTD

We designed our build process to be straightforward while maintaining professional standards. Here's how to compile SCASTD from source:

```bash
# build_scastd.sh - Complete build script
# Authors: SCASTD Development Team
# Purpose: We provide this comprehensive build script for reliable compilation

#!/bin/bash
set -e  # We enable strict error handling

echo "🔧 Generating build system files..."
# We generate the configure script and Makefiles using autotools
./autogen.sh

echo "🔍 Configuring build with optimizations..."
# We configure with C++17 support and optimization flags
./configure CXXFLAGS='-std=c++17 -g -O2 -Wall -Werror'

echo "⚡ Building SCASTD..."
# We use parallel compilation to speed up the build process
make -j$(nproc)

echo "🧪 Running test suite..."
# We always run tests to ensure build quality
make check

echo "✅ Build completed successfully!"
echo "📍 Binary location: ./src/scastd"
```

### 📥 Installing Pre-built Packages

#### Debian/Ubuntu (.deb)
```bash
sudo dpkg -i scastd_<version>_amd64.deb
sudo apt-get install -f
```

#### macOS (.pkg installer)
```bash
sudo installer -pkg scastd-<version>.pkg -target /
```

#### macOS (Homebrew)
Release archives include a `scastd.rb` Homebrew formula. To test it locally:

```bash
brew install --formula ./scastd.rb
brew services start scastd
```

Once verified, install from the official tap:

```bash
brew tap davestj/scastd
brew install scastd
```

To update the formula later:

```bash
brew upgrade scastd
```

To remove it:

```bash
brew uninstall scastd
```

For details on creating the macOS package or Homebrew formula yourself, see
[packaging/macos/README.md](packaging/macos/README.md).

### 🚀 Post-Installation Steps

#### Edit `scastd.conf`
- **Debian/Ubuntu**: `sudo nano /etc/scastd/scastd.conf`
- **macOS (.pkg)**: `sudo nano /usr/local/etc/scastd.conf`
- **macOS (Homebrew)**: `nano /opt/homebrew/etc/scastd.conf`

Update absolute paths for databases and certificates as needed.

#### Start and verify the service
- **Debian/Ubuntu**
```bash
sudo systemctl enable scastd
sudo systemctl start scastd
```
- **macOS (Homebrew)**
```bash
brew services start scastd
```
- **macOS (.pkg)**
```bash
sudo launchctl load -w /Library/LaunchDaemons/com.scastd.plist
```

Confirm the daemon responds:
```bash
curl http://localhost:8000/v1/status.json
curl http://localhost:8000/v1/status.xml
```
Expected responses:
```json
{"status":"ok"}
```
```xml
<status>ok</status>
```
Troubleshooting tips if unreachable:
- Check service state: `sudo systemctl status scastd` or `brew services list`
- Ensure the configured port matches the `curl` command
- Verify no firewall or other service blocks the port
- Review logs in `/var/log/scastd` or with `journalctl -u scastd`

#### Explore command-line options
```bash
scastd --help
```
```text
Usage: scastd [options]
  -h, --help           Show this help and exit
  -c, --config PATH    Configuration file
  -D, --daemon         Run as a daemon
      --pid-file PATH  PID file path (used with --daemon)
      --ip ADDRESS     Bind IP address
      --port PORT      HTTP server port
      --debug LEVEL    Debug level
      --poll INTERVAL  Poll interval (e.g., 60s, 5m)
      --test-mode      Validate configuration and exit
      --db-type TYPE   Database type
      --db-host HOST   Database host
      --db-port PORT   Database port
      --db-name NAME   Database name
      --db-user USER   Database user
      --db-pass PASS   Database password
      --sqlite-db PATH SQLite database file
      --setupdb TYPE   Initialize database of specified type
      --ssl-cert PATH  SSL certificate file
      --ssl-key PATH   SSL key file
      --ssl-enable     Enable SSL
      --dump           Dump database and exit
      --dump-dir DIR   Directory for database dump
```

#### Enable HTTPS mode
Run with certificate options:
```bash
scastd --ssl-enable \
       --ssl-cert /etc/letsencrypt/live/example.com/fullchain.pem \
       --ssl-key /etc/letsencrypt/live/example.com/privkey.pem
```
Or configure `scastd.conf`:
```conf
ssl_enable     true
ssl_cert       /etc/letsencrypt/live/example.com/fullchain.pem
ssl_key        /etc/letsencrypt/live/example.com/privkey.pem
```
On macOS the Homebrew prefix contains the certificates:
```conf
ssl_enable     true
ssl_cert       /usr/local/etc/letsencrypt/live/example.com/fullchain.pem
ssl_key        /usr/local/etc/letsencrypt/live/example.com/privkey.pem
# Use /opt/homebrew on Apple Silicon
```

#### Test HTTP/HTTPS endpoints
```bash
curl http://localhost:8000/v1/status.json
curl http://localhost:8000/v1/status.xml
curl http://localhost:8000/v1/uptime
curl -k https://localhost:8000/v1/status.json
curl https://example.com:8000/v1/status.json
```

#### Obtain TLS certificates
*Debian/Ubuntu*
```bash
sudo certbot certonly --standalone -d example.com
```
*macOS (Homebrew)*
```bash
brew install certbot
sudo certbot certonly --standalone -d example.com
```
Certificates are placed under the Let's Encrypt directory for your
platform:

| Platform | Certificate path |
| --- | --- |
| Debian/Ubuntu | `/etc/letsencrypt/live/example.com/` |
| macOS (Intel) | `/usr/local/etc/letsencrypt/live/example.com/` |
| macOS (Apple Silicon) | `/opt/homebrew/etc/letsencrypt/live/example.com/` |

Reference `fullchain.pem` and `privkey.pem` from that directory in
`scastd.conf` and restart the daemon:

```bash
sudo systemctl restart scastd      # or reload
brew services restart scastd       # macOS
```

---

## ⚙️ Configuration Management

We designed SCASTD's configuration system to be both powerful and intuitive. The daemon supports multiple configuration methods to accommodate different deployment scenarios and security requirements.

### Configuration Files We Support

SCASTD reads settings from configuration files that support environment variable overrides - a pattern we've found essential in containerized and cloud-native environments:

- **[scastd.conf](scastd.conf)** - Main configuration file with comprehensive settings
- **[scastd_pg.conf](scastd_pg.conf)** - PostgreSQL-specific configuration example

Console logging is enabled by default when SCASTD runs in the foreground. Running with `--daemon` disables console output unless `log_console true` is set in the configuration. Set `log_console false` to silence console logs even in foreground mode.

### Environment Variable Integration

We implement robust environment variable support for sensitive data and containerized deployments:

```bash
# Environment variables we recognize for secure configuration
export SCASTD_USERNAME="statistics_user"
export SCASTD_PASSWORD_FILE="/run/secrets/db_password"
export SCASTD_MARIADB_HOST="mariadb-cluster.internal"
export SCASTD_API_TOKEN_SECRET="your-jwt-secret-here"
export ICEADMINUSER="admin"
export ICEUSERPASS="hackme"
```

These credentials configure access to the Icecast administrative interface. If
`ICEADMINUSER` or `ICEUSERPASS` are not provided, the daemon falls back to the
legacy `SCASTD_ADMINUSER` and `SCASTD_USERPASS` variables.

For detailed configuration instructions, please refer to our comprehensive [Installation Guide](INSTALL.md).

---

## 🔌 API Reference & Integration

We designed SCASTD's HTTP API following RESTful principles with comprehensive authentication and rate limiting. The API serves statistics on port 8333 by default, with versioned endpoints to ensure backward compatibility.

### Core API Endpoints We Provide

Our API offers several categories of endpoints designed for different use cases:

**Statistics Collection Endpoints**: Access real-time and historical streaming server metrics
**Health Monitoring Endpoints**: Check daemon status and perform health verification
**Configuration Endpoints**: Manage monitored servers and collection settings
**Export Endpoints**: Generate reports in various formats (JSON, XML, CSV)

### Integration Examples We Support

We provide ready-to-use integration examples for popular monitoring platforms:

- **Prometheus Integration**: Native metrics export for Prometheus scraping
- **Grafana Dashboards**: Pre-built dashboards for streaming analytics
- **ELK Stack Integration**: Structured logging for Elasticsearch ingestion
- **Custom API Clients**: Example implementations in Python, PHP, and Node.js

For complete API documentation and integration examples, see our [API Documentation](docs/CI.md).

---

## 🧪 Testing & Quality Assurance

We implement comprehensive testing to ensure SCASTD's reliability across different environments and use cases. Our testing strategy covers multiple layers from unit tests to full integration scenarios.

### Test Categories We Maintain

**Unit Tests**: We test individual components and functions in isolation
**Integration Tests**: We verify correct interaction between system components  
**Protocol Tests**: We validate compatibility with different streaming server types
**Performance Tests**: We benchmark performance under various load conditions
**Security Tests**: We verify authentication, authorization, and data protection

### Running Our Test Suite

```bash
# We provide a comprehensive testing framework
make check                    # Run all unit tests
./tests/integration_tests.sh  # Run integration test suite
./tests/performance_tests.sh  # Run performance benchmarks
./tests/security_tests.sh     # Run security validation tests
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
  
  ### 📊 Development Activity
  <img src="https://github-readme-activity-graph.vercel.app/graph?username=davestj&repo=scastd&theme=tokyo-night&hide_border=true&area=true&custom_title=SCASTD%20Development%20Activity" alt="Development Activity"/>
  
  ### 🏆 Release Information
  ![Latest Release](https://img.shields.io/github/v/release/davestj/scastd?style=for-the-badge&logo=github&logoColor=white)
  ![Release Date](https://img.shields.io/github/release-date/davestj/scastd?style=for-the-badge&logo=calendar&logoColor=white)
  ![Downloads](https://img.shields.io/github/downloads/davestj/scastd/total?style=for-the-badge&logo=download&logoColor=white)
  
</div>

---

## 📚 Documentation & Resources

We maintain comprehensive documentation to support different user needs and experience levels. Our documentation is organized into practical guides that build understanding progressively.

### 📖 Core Documentation

| Document | Purpose | Audience |
|----------|---------|----------|
| **[Installation Guide](INSTALL.md)** | Step-by-step setup instructions for all platforms | System Administrators |
| **[Contributing Guide](CONTRIBUTING.md)** | Development workflow and coding standards | Developers |
| **[Change Log](ChangeLog.md)** | Version history and feature updates | All Users |
| **[License Information](LICENSE.md)** | Legal terms and GPL v2.0 details | All Users |
| **[Authors & Credits](AUTHORS.md)** | Project contributors and acknowledgments | Community |
| **[Release Notes](NEWS.md)** | Latest updates and announcements | All Users |

### 🔧 Technical Documentation

| Document | Purpose | Audience |
|----------|---------|----------|
| **[Continuous Integration](docs/CI.md)** | Build pipeline and automation details | DevOps Engineers |
| **[CI Secrets Management](docs/CISecrets.md)** | Secure credential handling in pipelines | DevOps Engineers |
| **[Database Migration](docs/DatabaseMigration.md)** | Database setup and migration procedures | Database Administrators |
| **[PostgreSQL Configuration](docs/PostgreSQL.md)** | PostgreSQL-specific setup and tuning | Database Administrators |
| **[HTTPS Configuration](docs/HTTPS.md)** | SSL/TLS setup and certificate management | System Administrators |
| **[Logging Configuration](docs/Logging.md)** | Log management and analysis setup | Operations Teams |
| **[Resource Limits](docs/ResourceLimits.md)** | Performance tuning and resource management | System Administrators |
| **[Packaging Guide](docs/Packaging.md)** | Distribution and deployment packaging | Release Engineers |
| **[Icecast2 Integration](docs/Icecast2.md)** | Specific Icecast2 monitoring setup | Streaming Engineers |

### 🔨 Configuration & Deployment

| File | Purpose | Environment |
|------|---------|-------------|
| **[scastd.conf](scastd.conf)** | Main configuration template | Production/Development |
| **[scastd_pg.conf](scastd_pg.conf)** | PostgreSQL-specific configuration | PostgreSQL Environments |
| **[scastd.service](scastd.service)** | Systemd service configuration | Linux Systems |
| **[Agent Configuration](AGENTS.md)** | Build agent and automation setup | CI/CD Systems |

---

## 🤝 Contributing to SCASTD

We welcome contributions from the streaming media and DevOps communities. Having maintained open source projects for over two decades, we believe in fostering collaborative development while maintaining high code quality standards.

### How We Prefer Contributions

Our contribution process follows established DevSecOps best practices:

1. **Fork & Branch**: We suggest forking the repository and working in feature branches based on our three-tier system
2. **Code Standards**: We require adherence to our coding standards outlined in [CONTRIBUTING.md](CONTRIBUTING.md)
3. **Testing Requirements**: We expect all contributions to include appropriate tests and documentation
4. **Security Review**: We mandate security review for all code changes affecting authentication or data handling
5. **Documentation Updates**: We require documentation updates for new features and API changes

### Areas Where We Need Help

We actively seek contributions in these key areas:

- **Protocol Extensions**: Expanding compatibility with additional streaming protocols and metadata formats
- **Monitoring Integrations**: New exporters for monitoring systems like DataDog, New Relic, and CloudWatch
- **Database Backends**: Additional database driver implementations and optimization
- **Performance Optimization**: Improving collection efficiency and reducing resource consumption
- **Documentation**: User guides, tutorials, and deployment examples for different environments
- **Testing**: Expanding test coverage and adding performance benchmarks

---

## 🔧 Deployment & Operations

### Production Deployment Options

We provide several deployment options to accommodate different infrastructure requirements:

**Containerized Deployment**: Docker and Kubernetes configurations for cloud-native environments
**Traditional Installation**: System packages and service configurations for bare-metal deployments
**Cloud Integration**: Native integration examples for AWS, Azure, and Google Cloud Platform

### Monitoring SCASTD Itself

As a monitoring tool, SCASTD includes comprehensive self-monitoring capabilities:

- **Health Endpoints**: Built-in health checks for load balancers and monitoring systems
- **Metrics Export**: Prometheus-compatible metrics about SCASTD's own performance
- **Audit Logging**: Detailed logging of all API access and configuration changes
- **Performance Monitoring**: Real-time visibility into collection performance and resource usage

For deployment guides and operational procedures, see our [Installation Guide](INSTALL.md) and [Logging Documentation](docs/Logging.md).

---

## 📞 Support & Community

<div align="center">

### 🛠️ Technical Support
We provide support through multiple channels based on your needs:

[![GitHub Issues](https://img.shields.io/badge/GitHub-Issues-green?style=for-the-badge&logo=github&logoColor=white)](https://github.com/davestj/scastd/issues)
[![Email Support](https://img.shields.io/badge/Email-davestj%40gmail.com-blue?style=for-the-badge&logo=gmail&logoColor=white)](mailto:davestj@gmail.com)
[![Project Website](https://img.shields.io/badge/Website-mcaster1.com-purple?style=for-the-badge&logo=globe&logoColor=white)](https://mcaster1.com)

### 💼 Professional Services
![DevSecOps Consulting](https://img.shields.io/badge/DevSecOps-Consulting-orange?style=for-the-badge&logo=handshake&logoColor=white)
![Streaming Architecture](https://img.shields.io/badge/Streaming-Architecture-red?style=for-the-badge&logo=stream&logoColor=white)
![Team Training](https://img.shields.io/badge/Team-Training-yellow?style=for-the-badge&logo=graduation-cap&logoColor=black)

</div>

### Community Resources

We maintain several community resources to support SCASTD users and contributors:

- **GitHub Discussions**: Community Q&A, feature requests, and general discussion
- **Project Wiki**: Extended documentation, tutorials, and community-contributed guides
- **Issue Tracking**: Bug reports, feature requests, and development coordination
- **Release Notifications**: Automated notifications for new releases and security updates

---

## 📝 License & Legal

This project is licensed under the **GNU General Public License v2.0**. We chose this license to ensure SCASTD remains open source and benefits the entire streaming media community while protecting contributors' rights and maintaining compatibility with enterprise environments.

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

For complete license terms, see [LICENSE.md](LICENSE.md) and [COPYING.md](COPYING.md).

---

## 🔄 Development Roadmap

### Current Release (v2.0.0)

We focus on core functionality and stability in our current release:

- ✅ Multi-protocol statistics collection (SHOUTcast v1/v2, Icecast2, ICY)
- ✅ RESTful API with JSON/XML output formats
- ✅ Multi-database backend support (MariaDB, PostgreSQL, SQLite)
- ✅ Comprehensive logging and monitoring capabilities
- ✅ JWT-based authentication and security features

### Next Release (v2.1.0)

Our next release will focus on enhanced analytics and integration:

- 🔧 Real-time WebSocket streaming of statistics
- 🔧 Advanced analytics engine with trend analysis
- 🔧 Native Prometheus metrics export
- 🔧 Enhanced ICY2 protocol support
- 🔧 Grafana dashboard templates

### Future Releases (v3.0.0+)

Our long-term roadmap includes advanced features for enterprise deployments:

- 📋 Machine learning-based anomaly detection
- 📋 Multi-tenant support for service providers
- 📋 Advanced reporting and alerting capabilities
- 📋 Cloud-native scaling and orchestration
- 📋 Enhanced security features and compliance tools

---

<!-- Footer -->
<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=gradient&customColorList=0,2,2,5,30&height=100&section=footer&text=Built%20with%20❤️%20for%20the%20Streaming%20Community&fontSize=18&fontColor=fff&animation=twinkling&fontAlignY=70" alt="Footer" />
  
  <sub>Created by oddsock & David St. John | DevSecOps Professionals | 20+ Years Streaming Infrastructure Experience</sub>
</div>

<!-- 
  Documentation Maintenance Notes (August 12, 2025):
  ==================================================
  
  We maintain this README.md as the primary project documentation following
  professional DevSecOps standards. This document serves as both an educational
  guide and comprehensive project overview for developers, system administrators,
  and streaming infrastructure professionals.
  
  Key Features We Implemented:
  - Three-branch build status monitoring (master/dev/future)
  - Comprehensive documentation linking to all project files
  - Educational progression from basic concepts to advanced topics
  - Professional visual design with consistent theming
  - Complete file organization and navigation structure
  
  Maintenance Schedule:
  - Update build status badges with each branch deployment
  - Review and update documentation links monthly
  - Validate all external references weekly
  - Refresh project statistics and metrics quarterly
  
  Branch Strategy:
  - master: Production releases with ProductionBuildDeployment pipeline
  - dev: Development integration with DevelopmentBuildPipeline
  - future: Experimental features with FutureBuildPipeline
  
  For questions about this documentation, contact: davestj@gmail.com
-->
