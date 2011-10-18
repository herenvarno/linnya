lib_LTLIBRARIES=liblinnyaui.la
liblinnyaui_la_SOURCES=	global.c\
						guide.c\
							debug.c\
							key.c\
							theme.c\
							win.c\
							pl.c\
							session.c\
							config.c\
							ui.c
INCLUDES = @GTK3_CFLAGS@\
		@GSTREAMER_CFLAGS@\
		@SQLITE3_CFLAGS@\
		-DLOCALEDIR=\"$(localedir)\"
liblinnyaui_la_LIBADD = @GTK3_LIBS@\
		@GSTREAMER_LIBS@\
		@SQLITE3_LIBS@
