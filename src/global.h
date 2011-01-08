#ifndef GLOBAL_H
#define GLOBAL_H

/*
 * HEADERS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <dirent.h>
#include <locale.h>
#include <libintl.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gst/gst.h>

/*
 * MACROS
 */
#define LY_GLOBAL_VERSION 2.0

#define _(String) gettext(String)
#define LY_GLOBAL_PACKAGE "linnya"
#define LY_GLOBAL_LOCALEDIR LOCALEDIR

#define LY_GLOBAL_HOMEDIR ly_global_homedir
#define LY_GLOBAL_PROGDIR ly_global_progdir
#define LY_GLOBAL_USERDIR ly_global_userdir
#define LY_GLOBAL_TEMPDIR ly_global_tempdir

/*
 * VARIABLES	[PRIVATE]
 */
GMainLoop *ly_global_mainloop;	/*[PUBLIC]*/

gchar *ly_global_homedir;
gchar *ly_global_progdir;
gchar *ly_global_userdir;
gchar *ly_global_tempdir;

/*
 * FUNCTIONS	[PUBLIC]
 */
gboolean ly_global_init();
gboolean ly_global_run();
gboolean ly_global_finalize();

#endif
