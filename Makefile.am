SUBDIRS = src scripts

# Installation of configuration and SQLite database
install-data-local:
	$(INSTALL) -d -m 755 $(DESTDIR)/etc/scastd
	$(INSTALL_DATA) -m 640 $(srcdir)/scastd.conf $(DESTDIR)/etc/scastd/scastd.conf
	sqlite3 $(DESTDIR)/etc/scastd/scastd.db < $(srcdir)/src/sqlite.sql
	chmod 640 $(DESTDIR)/etc/scastd/scastd.db
