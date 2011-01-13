


#ifndef THEME_H
#define THEME_H

//HEADERS
#include "global.h"
#include "debug.h"
#include "common.h"
#include "configure.h"
#include "metadata.h"
#include "library.h"
#include "playlist.h"
//#include "core.h"

//FUNCTIONS
gboolean	th_read_theme();
_gl_theme*	th_get_th_by_name(gchar *name);
GList*		th_get_name_list();

#endif
