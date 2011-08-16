bin_PROGRAMS = linnya
linnya_SOURCES = global.c common.c configure.c connect.c core.c debug.c equalizer.c gui_dialog.c gui_lrc.c gui_window.c gui_treeview.c library.c main.c metadata.c playlist.c theme.c

INCLUDES = @GTK2_CFLAGS@\
		@GSTREAMER_CFLAGS@\
		-I../intl -DLOCALEDIR=\"$(localedir)\"
linnya_LDADD = @GTK2_LIBS@\
		@GSTREAMER_LIBS@\
		-lgstapp-0.10