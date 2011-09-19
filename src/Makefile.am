SUBDIRS = core ui plugin

bin_PROGRAMS = linnya
linnya_SOURCES = main.c

INCLUDES = @GTK3_CFLAGS@\
		@GSTREAMER_CFLAGS@\
		@SQLITE3_CFLAGS@\
		-DLOCALEDIR=\"$(localedir)\"
linnya_LDADD = @GTK3_LIBS@\
		@GSTREAMER_LIBS@\
		@SQLITE3_LIBS@\
		-Lcore -llinnyacore\
		-Lui -llinnyaui
