# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

SUBDIRS	= style

PROGMAN_NAME		= linnya
PLUGIN_DIR		= ${prefix}/share/${PROGMAN_NAME}/pix
libdir			= ${PLUGIN_DIR}/vdo
datadir			= ${PLUGIN_DIR}/vdo

lib_LTLIBRARIES 	= libvdo.la
data_DATA		=\
				logo.png\
				plugin.xml
libvdo_la_SOURCES	=\
				vdo.c\
				vdo.h
libvdo_la_CPPFLAGS	=\
				@GTK3_CFLAGS@\
				@GSTREAMER_CFLAGS@\
				@SQLITE3_CFLAGS@\
				-DLOCALEDIR=\"$(localedir)\"\
				-I../../cox\
				-I../../uix
libvdo_la_LIBADD	=\
				@GTK3_LIBS@\
				@GSTREAMER_LIBS@\
				@SQLITE3_LIBS@\
				-L../../cox -llinnyacox\
				-L../../uix -llinnyauix
