lib_LTLIBRARIES=liblinnyacore.la
liblinnyacore_la_SOURCES=	global.c\
							msg.c\
							conf.c\
							db.c\
							lib.c\
							playlist.c\
							audio.c\
							core.c
INCLUDES = @GTK3_CFLAGS@\
		@GSTREAMER_CFLAGS@\
		@SQLITE3_CFLAGS@\
		-DLOCALEDIR=\"$(localedir)\"
liblinnyacore_la_LIBADD = @GTK3_LIBS@\
		@GSTREAMER_LIBS@\
		@SQLITE3_LIBS@
