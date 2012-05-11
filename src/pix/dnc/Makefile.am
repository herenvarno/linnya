# USE SEARCH & REPLACE FUNCTION (eg. type /<PLUGIN_NAME> in command mode of VIM)
# TO REPLACE THE PLUGIN NAME.

PROGMAN_NAME	= linnya
PLUGIN_DIR		= ${prefix}/share/${PROGMAN_NAME}/pix
libdir			= ${PLUGIN_DIR}/dnc
datadir			= ${PLUGIN_DIR}/dnc

lib_LTLIBRARIES = libdnc.la
data_DATA		=\
				logo.png\
				plugin.xml
libdnc_la_SOURCES	=\
				dnc.c\
				dnc.h\
				dnc_cov.c\
				dnc_cov.h\
				dnc_cov_lastfm.c\
				dnc_cov_lastfm.h\
				dnc_cov_top100.c\
				dnc_cov_top100.h\
				dnc_lrc.c\
				dnc_lrc.h\
				dnc_lrc_lrc123.c\
				dnc_lrc_lrc123.h\
				dnc_lrc_qianqian.c\
				dnc_lrc_qianqian.h\
				dnc_config.c\
				dnc_config.h
INCLUDES		=\
				@GTK3_CFLAGS@\
				@GSTREAMER_CFLAGS@\
				@SQLITE3_CFLAGS@\
				@CURL_CFLAGS@\
				-DLOCALEDIR=\"$(localedir)\"\
				-I../../cox\
				-I../../uix
libdnc_la_LIBADD	=\
				@GTK3_LIBS@\
				@GSTREAMER_LIBS@\
				@SQLITE3_LIBS@\
				@CURL_LIBS@\
				-L../../cox -llinnyacox\
				-L../../uix -llinnyauix
