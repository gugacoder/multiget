@INTLTOOL_DESKTOP_RULE@

SUBDIRS =

desktop_in_in_files = multiget.desktop.in.in
desktop_in_files = $(desktop_in_in_files:.desktop.in.in=.desktop.in)

desktopdir = $(datadir)/applications
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	if test -z "$(DESTDIR)"; then \
	   echo "Updating Gtk icon cache."; \
	   $(gtk_update_icon_cache); \
	else \
	   echo "*** Icon cache not updated.  After (un)install, run this:"; \
	   echo "***   $(gtk_update_icon_cache)"; \
	fi

EXTRA_DIST = \
	$(desktop_in_in_files)  \
	$(desktop_in_files) \
	$(desktop_DATA)     

DISTCLEANFILES = \
	$(desktop_in_files) \
	$(desktop_DATA)

