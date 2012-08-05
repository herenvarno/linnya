# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

PROGMAN_NAME		= linnya
PLUGIN_DIR		= ${prefix}/share/${PROGMAN_NAME}/pix
libdir			= ${PLUGIN_DIR}/shr
datadir			= ${PLUGIN_DIR}/shr

lib_LTLIBRARIES 	= libshr.la
data_DATA		=\
				logo.png\
				plugin.xml
libshr_la_SOURCES	=\
				shr.c\
				shr.h\
				shr_widget.c\
				shr_widget.h\
				shr_config.c\
				shr_config.h
INCLUDES		=\
				@GTK3_CFLAGS@\
				@GSTREAMER_CFLAGS@\
				@SQLITE3_CFLAGS@\
				@CLUTTERGTK_CFLAGS@\
				@MX_CFLAGS@\
				-DLOCALEDIR=\"$(localedir)\"\
				-I../../cox\
				-I../../uix
libshr_la_LIBADD	=\
				@GTK3_LIBS@\
				@GSTREAMER_LIBS@\
				@SQLITE3_LIBS@\
				@CLUTTERGTK_LIBS@\
				@MX_LIBS@\
				-L../../cox -llinnyacox\
				-L../../uix -llinnyauix
