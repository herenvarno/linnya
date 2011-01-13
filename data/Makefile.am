SUBDIRS = icons conf scripts

EXTRA_DIST = linnya_trayicon.png\
		linnya.png\
		linnya.desktop\
		linnya-uninstall

trayicondir = ${datadir}/icons/hicolor/64x64/apps
trayicon_DATA = linnya_trayicon.png

linnyaicondir = ${datadir}/icons/hicolor/64x64/apps
linnyaicon_DATA = linnya.png

# Rule to update the icon cache after installing the icon
gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

menudir = ${datadir}/applications
menu_DATA = linnya.desktop

linnya_uninstalldir = ${datadir}/linnya
linnya_uninstall_DATA = linnya-uninstall