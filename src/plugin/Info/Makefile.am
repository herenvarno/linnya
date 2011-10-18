# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

PROGMAN_NAME	= linnya
PLUGIN_DIR		= ${prefix}/share/${PROGMAN_NAME}/plugin
libdir			= ${PLUGIN_DIR}/Info
datadir			= ${PLUGIN_DIR}/Info

lib_LTLIBRARIES = libInfo.la
data_DATA		=\
				logo.svg\
				plugin.xml\
				InfoCoverDown.py
libInfo_la_SOURCES	=\
				Info.c\
				Info.h\
				InfoCover.c\
				InfoCover.h
INCLUDES		=\
				@GTK3_CFLAGS@\
				@GSTREAMER_CFLAGS@\
				@SQLITE3_CFLAGS@\
				@CURL_CFLAGS@\
				-DLOCALEDIR=\"$(localedir)\"\
				-I../../core\
				-I../../ui\
				-I../Python
libInfo_la_LIBADD	=\
				@GTK3_LIBS@\
				@GSTREAMER_LIBS@\
				@SQLITE3_LIBS@\
				@CURL_LIBS@\
				-L../../core -llinnyacore\
				-L../../ui -llinnyaui
