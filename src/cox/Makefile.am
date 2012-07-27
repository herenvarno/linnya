lib_LTLIBRARIES=liblinnyacox.la
liblinnyacox_la_SOURCES=	\
gla.c\
mbs.c mbs.obj.c\
log.c log.obj.c\
reg.c reg.obj.c\
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

INCLUDES = \
		@GSTREAMER_CFLAGS@\
		@GSTINTERFACES_CFLAGS@\
		@SQLITE3_CFLAGS@\
		-DLOCALEDIR=\"$(localedir)\"
liblinnyacox_la_LIBADD = \
		@GSTREAMER_LIBS@\
		@GSTINTERFACES_LIBS@\
		@SQLITE3_LIBS@
