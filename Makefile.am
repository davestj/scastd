SUBDIRS = src scripts tests

# Installation of configuration and SQLite database
install-data-local:
	$(INSTALL) -d -m 755 $(DESTDIR)/etc/scastd
	$(INSTALL_DATA) -m 640 $(srcdir)/scastd.conf $(DESTDIR)/etc/scastd/scastd.conf
	sqlite3 $(DESTDIR)/etc/scastd/scastd.db < $(srcdir)/src/sqlite.sql
	chmod 640 $(DESTDIR)/etc/scastd/scastd.db
	$(MAKE) permissions_great_again

# Ensure ownership and modes for installed directories and files
permissions_great_again:
	$(INSTALL) -d -m 750 $(DESTDIR)/var/log/scastd
	chown root:root $(DESTDIR)/etc/scastd \
		$(DESTDIR)/etc/scastd/scastd.conf \
		$(DESTDIR)/etc/scastd/scastd.db \
		$(DESTDIR)/var/log/scastd
	chmod 755 $(DESTDIR)/etc/scastd
	chmod 640 $(DESTDIR)/etc/scastd/scastd.conf $(DESTDIR)/etc/scastd/scastd.db
	chmod 750 $(DESTDIR)/var/log/scastd
