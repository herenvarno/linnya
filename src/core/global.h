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
#include <sqlite3.h>
#include <gst/gst.h>

/*
 * MACROS
 */
#define LY_GLOBAL_VERSION_STR "2.0.0 alpha (1.9.1)"
#define LY_GLOBAL_VERSION_NUM 1.91

#define _(String) gettext(String)
#define LY_GLOBAL_PACKAGE "linnya"
#define LY_GLOBAL_LOCALEDIR	LOCALEDIR

#define LY_GLOBAL_HOMEDIR ly_global_homedir
#define LY_GLOBAL_PROGDIR ly_global_progdir
#define LY_GLOBAL_USERDIR ly_global_userdir
#define LY_GLOBAL_TEMPDIR ly_global_tempdir
#define LY_GLOBAL_UIDIR ly_global_uidir
#define LY_GLOBAL_PLUGINDIR ly_global_plugindir

#define LY_GLOBAL_FIRST_FLAG ly_global_first_flag

#define LY_GLOBAL_CALLBACK(f) gboolean (*f)(gpointer object, gpointer data)

/*
 * VARIABLES	[PUBLIC]
 */
GMainLoop	*ly_global_mainloop;

const char	*ly_global_homedir;
const char	*ly_global_progdir;
const char	*ly_global_userdir;
const char	*ly_global_tempdir;
const char	*ly_global_uidir;
const char	*ly_global_plugindir;

int			*ly_global_argc;
char		***ly_global_argv;

gboolean	ly_global_first_flag;

/*
 * FUNCTIONS	[PUBLIC]
 */
gboolean	ly_global_init			(int *argc, char ***argv);
gboolean	ly_global_finalize		(void);

GList*		ly_global_traverse_dir	(gchar *path, gint depth, gboolean showhide);
GList*		ly_global_get_subdirs	(gchar *path, gboolean showhide);
void		ly_global_replace_str	(gchar *string, gint length, gchar *sour, char *dest);

char		*ly_global_get_suffix	(char *uri);
char		*ly_global_get_filename	(char *uri);
char		*ly_global_get_dir		(char *uri);
char		*ly_global_get_path		(char *uri);

void		ly_global_debug			(gchar *type, const gchar *format, ...);

#endif
