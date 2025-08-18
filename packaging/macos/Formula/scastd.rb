class Scastd < Formula
  desc "Statistical Casting Daemon - streaming media statistics collection"
  homepage "https://github.com/davestj/scastd"
  url "https://github.com/davestj/scastd/archive/v1.0.tar.gz"
  sha256 "PLACEHOLDER_SHA256"
  license "GPL-2.0"
  
  head "https://github.com/davestj/scastd.git", branch: "master"

  depends_on "autoconf" => :build
  depends_on "automake" => :build
  depends_on "libtool" => :build
  depends_on "pkg-config" => :build
  depends_on "gettext" => :build
  
  depends_on "libxml2"
  depends_on "curl"
  depends_on "mariadb"
  depends_on "postgresql@14"
  depends_on "libmicrohttpd"
  depends_on "sqlite"
  depends_on "openssl@3"

  def install
    # Set environment for ARM64 macOS builds
    ENV.prepend_path "PKG_CONFIG_PATH", "#{Formula["postgresql@14"].opt_lib}/pkgconfig"
    ENV.prepend_path "PKG_CONFIG_PATH", "#{Formula["mariadb"].opt_lib}/pkgconfig"
    ENV.prepend_path "PKG_CONFIG_PATH", "#{Formula["libxml2"].opt_lib}/pkgconfig"
    
    # Generate build system files
    system "./autogen.sh"
    
    # Configure with Homebrew paths and C++17 standard
    system "./configure",
           "--prefix=#{prefix}",
           "--sysconfdir=#{etc}",
           "--localstatedir=#{var}",
           "CXXFLAGS=-std=c++17 -g -O2 -Wall"
    
    # Build and install
    system "make"
    system "make", "install"
    
    # Install configuration files
    (etc/"scastd").mkpath
    (var/"log/scastd").mkpath
    
    # Copy default configuration
    cp "scastd.conf", etc/"scastd/scastd.conf.example"
    
    # Initialize SQLite database
    system "sqlite3", "#{etc}/scastd/scastd.db", *["-init", "src/sqlite.sql", ".quit"]
  end

  def post_install
    # Create log directory with proper permissions
    (var/"log/scastd").mkpath
    
    # Set secure permissions on configuration and database
    chmod 0640, etc/"scastd/scastd.conf.example" if (etc/"scastd/scastd.conf.example").exist?
    chmod 0640, etc/"scastd/scastd.db" if (etc/"scastd/scastd.db").exist?
    chmod 0750, var/"log/scastd"
  end

  service do
    run [opt_bin/"scastd", "--config", etc/"scastd/scastd.conf"]
    keep_alive true
    log_path var/"log/scastd/scastd.log"
    error_log_path var/"log/scastd/scastd_error.log"
    working_dir var/"log/scastd"
  end

  test do
    # Test that the binary exists and shows version/help
    assert_match "scastd", shell_output("#{bin}/scastd --help", 1)
    
    # Test configuration file syntax
    (testpath/"test.conf").write <<~EOS
      log_file #{testpath}/test.log
      log_level debug
      daemon false
    EOS
    
    # Test that scastd can validate configuration
    system bin/"scastd", "--config", testpath/"test.conf", "--test-config"
  end
end