lib_LTLIBRARIES=liblinnyauix.la
liblinnyauix_la_SOURCES=	\
						glb.c\
						dbg.c\
						key.c\
						thm.c\
						win.c\
						pli.c\
						sss.c\
						cfg.c\
						uix.c
INCLUDES = @GTK3_CFLAGS@\
		@GSTREAMER_CFLAGS@\
		@SQLITE3_CFLAGS@\
		-DLOCALEDIR=\"$(localedir)\"
liblinnyauix_la_LIBADD = @GTK3_LIBS@\
		@GSTREAMER_LIBS@\
		@SQLITE3_LIBS@
