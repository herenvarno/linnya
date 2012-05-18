/*
 * gla.h
 * This file is part of linnya
 *
 * Copyright (C) 2010-2012 - Edward
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * linnya is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION: gla
 * @short_description: the common module.
 * @stability: stable
 * @include: cox.h
 *
 * This module is a common module for the upper module in cox. It defines the
 * most useful macros, variables as well as API functions.
 */

#ifndef LY_GLA_H
#define LY_GLA_H

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
#include <gio/gio.h>
#include <glib/gprintf.h>
#include <sqlite3.h>
#include <gst/gst.h>

/*
 * MACROS
 */
// APPLICATION INFORMATION DEFINATION
#define	 LY_GLA_APPLICATION _("linnya")
#define LY_GLA_AUTHOR	 "Edward<herenvarno@workmail.com>"
#define LY_GLA_WEBSITE "http://www.linnya.org"
#define LY_GLA_COMMENTS _("A free music player on Linux, with unlimited feathers from plug-ins.")
#define LY_GLA_COPYRIGHT _("Copyright (C) 2010-2012 Edward Yang")
#define LY_GLA_VERSION_STR "2.0.0"
#define LY_GLA_VERSION_NUM 2.00
#define	 LY_GLA_LICENSE "GPLv3"
#define	 LY_GLA_LICENSE_FULL _("\
Linnya is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
Linnya is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with Linnya. If not, see <http://www.gnu.org/licenses/>.")

// BUILD DEFINATION
#define UNIX_BUILD

// GETTEXT DEFINATION
#define _(String) gettext(String)
#define LY_GLA_PACKAGE "linnya"
#define LY_GLA_LOCALEDIR LOCALEDIR

// GLOBAL DIRECTORY DEFINATION
#define LY_GLA_HOMEDIR ly_gla_homedir
#define LY_GLA_PROGDIR ly_gla_progdir
#define LY_GLA_USERDIR ly_gla_userdir
#define LY_GLA_TEMPDIR ly_gla_tempdir

// OTHERS
#define LY_GLA_FIRST_FLAG	ly_gla_first_flag
#define LY_GLA_CALLBACK(f)	gboolean (*f) (gpointer arg, gpointer data);

/*
 * VARIABLES
 */
const char	*ly_gla_homedir;
const char	*ly_gla_progdir;
const char	*ly_gla_userdir;
const char	*ly_gla_tempdir;

int			*ly_gla_argc;
char		***ly_gla_argv;

gboolean	ly_gla_first_flag;

/*
 * FUNCTIONS
 */
// INIT & FINAL
void		ly_gla_init				(int *argc, char ***argv);
void		ly_gla_fina				();

// API
char*		ly_gla_uri_get_prefix		(char *uri);
char*		ly_gla_uri_get_suffix		(char *uri);
char*		ly_gla_uri_get_filename	(char *uri);
char*		ly_gla_uri_get_dir		(char *uri);
char*		ly_gla_uri_get_path		(char *uri);
GList*		ly_gla_traverse_dir		(const char *path, int depth, gboolean showhide);
GList*		ly_gla_get_subdirs		(const char *path, gboolean showhide);

#endif //LY_GLA_H
