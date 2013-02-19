# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

SUBDIRS = style

PROGMAN_NAME	= linnya
PLUGIN_DIR		= ${prefix}/share/${PROGMAN_NAME}/pix
libdir			= ${PLUGIN_DIR}/lrc
datadir			= ${PLUGIN_DIR}/lrc

lib_LTLIBRARIES = liblrc.la
data_DATA		=\
				logo.png\
				plugin.xml
liblrc_la_SOURCES	=\
				lrc.c\
				lrc.h\
				lrc_widget.c\
				lrc_widget.h\
				lrc_desktop.c\
				lrc_desktop.h\
				lrc_config.c\
				lrc_config.h
liblrc_la_CPPFLAGS	=\
				@GTK3_CFLAGS@\
				@GSTREAMER_CFLAGS@\
				@SQLITE3_CFLAGS@\
				-DLOCALEDIR=\"$(localedir)\"\
				-I../../cox\
				-I../../uix
liblrc_la_LIBADD	=\
				@GTK3_LIBS@\
				@GSTREAMER_LIBS@\
				@SQLITE3_LIBS@\
				-L../../cox -llinnyacox\
				-L../../uix -llinnyauix
