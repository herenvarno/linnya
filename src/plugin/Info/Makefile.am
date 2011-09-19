# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

PROGMAN_NAME	= linnya
PLUGIN_DIR		= ${prefix}/share/${PROGMAN_NAME}/plugin
libdir			= ${PLUGIN_DIR}/Info
datadir			= ${PLUGIN_DIR}/Info

lib_LTLIBRARIES = libInfo.la
data_DATA		=\
				logo.png\
				plugin.xml
libInfo_la_SOURCES	=\
				Info.c\
				Info.h
INCLUDES		=\
				@GTK3_CFLAGS@\
				@GSTREAMER_CFLAGS@\
				@SQLITE3_CFLAGS@\
				-DLOCALEDIR=\"$(localedir)\"\
				-I../../core\
				-I../../ui
libInfo_la_LIBADD	=\
				@GTK3_LIBS@\
				@GSTREAMER_LIBS@\
				@SQLITE3_LIBS@\
				-L../../core -llinnyacore\
				-L../../ui -llinnyaui
