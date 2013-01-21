# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

PROGMAN_NAME		= linnya
PLUGIN_DIR		= ${prefix}/share/${PROGMAN_NAME}/pix
libdir			= ${PLUGIN_DIR}/gud
datadir			= ${PLUGIN_DIR}/gud

lib_LTLIBRARIES 	= libgud.la
data_DATA		=\
				logo.png\
				plugin.xml
libgud_la_SOURCES	=\
				gud.c\
				gud.h
libgud_la_CPPFLAGS	=\
				@GTK3_CFLAGS@\
				@GSTREAMER_CFLAGS@\
				@SQLITE3_CFLAGS@\
				-DLOCALEDIR=\"$(localedir)\"\
				-I../../cox\
				-I../../uix
libgud_la_LIBADD	=\
				@GTK3_LIBS@\
				@GSTREAMER_LIBS@\
				@SQLITE3_LIBS@\
				-L../../cox -llinnyacox\
				-L../../uix -llinnyauix
