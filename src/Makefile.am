SUBDIRS = cox uix pix


bin_PROGRAMS = linnya

linnya_HEADERS = linnya.h

linnya_SOURCES = linnya.c

linnyadir = ${includedir}/linnya

INCLUDES = \
	@GTK3_CFLAGS@\
	@GSTREAMER_CFLAGS@\
	@SQLITE3_CFLAGS@\
	-DLOCALEDIR=\"$(localedir)\"\
	-Icox\
	-Iuix

linnya_LDADD = \
	@GTK3_LIBS@\
	@GSTREAMER_LIBS@\
	@SQLITE3_LIBS@\
	-Lcox -llinnyacox\
	-Luix -llinnyauix
