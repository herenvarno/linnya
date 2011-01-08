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
#include <gst/app/gstappsrc.h>

/*
 * MACROS
 */
#define LY_GL_VERSION 2.0
#define _(String) gettext(String)
#define LY_GL_LOCALEDIR LOCALEDIR
#define LY_GL_PACKAGE "musicme"

/*
 * VARIABLES	[PUBLIC]
 */
GMainLoop *ly_gl_mainloop;

/*
 * FUNCTIONS	[PUBLIC]
 */
gboolean ly_gl_init();

#endif
