# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

PROGMAN_NAME	= linnya
PLUGIN_DIR		= ${prefix}/share/${PROGMAN_NAME}/pix
libdir			= ${PLUGIN_DIR}/inf
datadir			= ${PLUGIN_DIR}/inf

lib_LTLIBRARIES = libinf.la
data_DATA		=\
				logo.png\
				plugin.xml
libinf_la_SOURCES	=\
				inf.c\
				inf.h\
				inf_cover.c\
				inf_cover.h\
				inf_config.c\
				inf_config.h
libinf_la_CPPFLAGS	=\
				@GTK3_CFLAGS@\
				@GSTREAMER_CFLAGS@\
				@SQLITE3_CFLAGS@\
				-DLOCALEDIR=\"$(localedir)\"\
				-I../../cox\
				-I../../uix
libinf_la_LIBADD	=\
				@GTK3_LIBS@\
				@GSTREAMER_LIBS@\
				@SQLITE3_LIBS@\
				-L../../cox -llinnyacox\
				-L../../uix -llinnyauix
