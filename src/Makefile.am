SUBDIRS = cox uix pix

bin_PROGRAMS = linnya
linnya_SOURCES = app.c

INCLUDES = @GTK3_CFLAGS@\
		@GSTREAMER_CFLAGS@\
		@SQLITE3_CFLAGS@\
		-DLOCALEDIR=\"$(localedir)\"
linnya_LDADD = @GTK3_LIBS@\
		@GSTREAMER_LIBS@\
		@SQLITE3_LIBS@\
		-Lcox -llinnyacox\
		-Luix -llinnyauix
