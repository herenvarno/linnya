# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

PROGMAN_NAME		= linnya
PLUGIN_DIR			= ${prefix}/share/${PROGMAN_NAME}/pix
libdir				= ${PLUGIN_DIR}/opc
datadir				= ${PLUGIN_DIR}/opc

lib_LTLIBRARIES 	= libopc.la
data_DATA			=\
					logo.png\
					playing.svg\
					plugin.xml
libopc_la_SOURCES	=\
					opc.c\
					opc.h\
					opc_left.c\
					opc_left.h\
					opc_right.c\
					opc_right.h\
					opc_config.c\
					opc_config.h
INCLUDES			=\
					@GTK3_CFLAGS@\
					@GSTREAMER_CFLAGS@\
					@SQLITE3_CFLAGS@\
					@CLUTTERGTK_CFLAGS@\
					@MX_CFLAGS@\
					-DLOCALEDIR=\"$(localedir)\"\
					-I../../cox\
					-I../../uix
libopc_la_LIBADD	=\
					@GTK3_LIBS@\
					@GSTREAMER_LIBS@\
					@SQLITE3_LIBS@\
					@CLUTTERGTK_LIBS@\
					@MX_LIBS@\
					-L../../cox -llinnyacox\
					-L../../uix -llinnyauix
