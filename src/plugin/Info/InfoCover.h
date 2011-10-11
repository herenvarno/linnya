#ifndef LY_PLUGIN_INFO_COVER_H
#define LY_PLUGIN_INFO_COVER_H

/*
 * HEADERS
 */
#include "core.h"
#include "ui.h"
#include <curl/curl.h>
#include "Python.h"

/*
 * MACRO
 */
#define LY_PLUGIN_INFO_COVERDIR ly_plugin_info_coverdir

/*
 * VARIABLES
 */
GtkWidget		*ly_plugin_info_cover_image;
const char		*ly_plugin_info_coverdir;
PyObject		*pModule;

/*
 * FUNCTIONS
 */
gboolean	ly_plugin_info_cover_init		();
gboolean	ly_plugin_info_cover_finalize	();
gboolean	ly_plugin_info_cover_set		(char *artist, char *album);
char*		ly_plugin_info_cover_get		(char *artist, char *album);
char*		ly_plugin_info_cover_down		(char *artist, char *album);

#endif

