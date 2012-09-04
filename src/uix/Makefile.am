lib_LTLIBRARIES=liblinnyauix.la

liblinnyauix_la_HEADERS = \
	glb.h\
	dbg.h\
	key.h\
	thm.h\
	win.h\
	pli.h pli.obj.h\
	sss.h\
	cfg.h cfg.obj.h\
	uix.h

liblinnyauix_la_SOURCES = \
	glb.c\
	dbg.c\
	key.c\
	thm.c\
	win.c\
	pli.c pli.obj.c\
	sss.c\
	cfg.c cfg.obj.c\
	uix.c

liblinnyauix_ladir = ${includedir}/linnya/uix
	
INCLUDES = \
	@GTK3_CFLAGS@\
	@GSTREAMER_CFLAGS@\
	@SQLITE3_CFLAGS@\
	-DLOCALEDIR=\"$(localedir)\"\
	-I.
	
liblinnyauix_la_LIBADD = \
	@GTK3_LIBS@\
	@GSTREAMER_LIBS@\
	@SQLITE3_LIBS@\
	-L../cox -llinnyacox
