lib_LTLIBRARIES = liblinnyacox.la

liblinnyacox_la_HEADERS = \
	gla.h\
	mbs.h mbs.obj.h\
	log.h log.obj.h\
	reg.h reg.obj.h\
	dbm.h\
	mdh.h\
	lib.h\
	plm.h\
	pqm.h\
	ppl.h\
	eql.h\
	lrc.h\
	aud.h\
	cox.h

liblinnyacox_la_SOURCES = \
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

liblinnyacox_ladir = ${includedir}/linnya/cox

INCLUDES = \
	@GSTREAMER_CFLAGS@\
	@SQLITE3_CFLAGS@\
	-DLOCALEDIR=\"$(localedir)\"

liblinnyacox_la_LIBADD = \
	@GSTREAMER_LIBS@\
	@SQLITE3_LIBS@\
	-lgstinterfaces-0.10
