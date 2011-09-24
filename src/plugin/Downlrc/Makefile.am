# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

PROGMAN_NAME	= linnya
PLUGIN_DIR		= ${prefix}/share/${PROGMAN_NAME}/plugin
libdir			= ${PLUGIN_DIR}/Downlrc
datadir			= ${PLUGIN_DIR}/Downlrc

lib_LTLIBRARIES = libDownlrc.la
data_DATA		=\
				logo.png\
				plugin.xml\
				Downlrc.py
libDownlrc_la_SOURCES	=\
				Downlrc.c\
				Downlrc.h
INCLUDES		=\
				@GTK3_CFLAGS@\
				@GSTREAMER_CFLAGS@\
				@SQLITE3_CFLAGS@\
				-DLOCALEDIR=\"$(localedir)\"\
				-I../../core\
				-I../../ui\
				-I../Python\
				-I../Lrc
libDownlrc_la_LIBADD	=\
				@GTK3_LIBS@\
				@GSTREAMER_LIBS@\
				@SQLITE3_LIBS@\
				-L../../core -llinnyacore\
				-L../../ui -llinnyaui
