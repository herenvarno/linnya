# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

PROGMAN_NAME		= linnya
PLUGIN_DIR			= ${prefix}/share/${PROGMAN_NAME}/plugin
libdir				= ${PLUGIN_DIR}/rdo
datadir				= ${PLUGIN_DIR}/rdo

lib_LTLIBRARIES 	= librdo.la
data_DATA			=\
					logo.svg\
					plugin.xml\
					playing.svg
librdo_la_SOURCES	=\
					rdo.c\
					rdo.h\
					rdo_config.c\
					rdo_config.h
INCLUDES			=\
					@GTK3_CFLAGS@\
					@GSTREAMER_CFLAGS@\
					@SQLITE3_CFLAGS@\
					-DLOCALEDIR=\"$(localedir)\"\
					-I../../cox\
					-I../../uix
librdo_la_LIBADD	=\
					@GTK3_LIBS@\
					@GSTREAMER_LIBS@\
					@SQLITE3_LIBS@\
					-L../../cox -llinnyacox\
					-L../../uix -llinnyauix
