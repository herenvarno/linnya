# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

PROGMAN_NAME	= linnya
PLUGIN_DIR		= ${prefix}/share/${PROGMAN_NAME}/plugin
libdir			= ${PLUGIN_DIR}/Python
datadir			= ${PLUGIN_DIR}/Python

lib_LTLIBRARIES = libPython.la
data_DATA		=\
				logo.png\
				plugin.xml
libPython_la_SOURCES	=\
				Python.c\
				Python.h
INCLUDES		=\
				@GTK3_CFLAGS@\
				@GSTREAMER_CFLAGS@\
				@SQLITE3_CFLAGS@\
				`pkg-config --cflags python3`\
				-DLOCALEDIR=\"$(localedir)\"\
				-I../../core\
				-I../../ui
libPython_la_LDFLAGS=\
				`pkg-config --libs python3`\
libPython_la_LIBADD	=\
				@GTK3_LIBS@\
				@GSTREAMER_LIBS@\
				@SQLITE3_LIBS@\
				-L../../core -llinnyacore\
				-L../../ui -llinnyaui
