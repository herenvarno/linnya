lib_LTLIBRARIES=liblinnyacox.la
liblinnyacox_la_SOURCES=	gla.c\
							mbs.c\
							mbs.obj.c\
							log.c\
							log.obj.c\
							reg.c\
							dbm.c\
							mdh.c\
							lib.c\
							plm.c\
							pqm.c\
							ppl.c\
							eql.c\
							lrc.c\
							aud.c\
							cox.c
INCLUDES = @GTK3_CFLAGS@\
		@GSTREAMER_CFLAGS@\
		@SQLITE3_CFLAGS@\
		-DLOCALEDIR=\"$(localedir)\"
liblinnyacox_la_LIBADD = @GTK3_LIBS@\
		@GSTREAMER_LIBS@\
		@SQLITE3_LIBS@
