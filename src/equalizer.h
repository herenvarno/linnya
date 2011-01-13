

#ifndef EQUALIZER_H
#define EQUALIZER_H

//HEADERS
#include "global.h"
#include "debug.h"
#include "common.h"
#include "configure.h"
#include "metadata.h"
#include "library.h"
#include "playlist.h"

//FUNCTIONS
gboolean	eq_read_equalizer();
GList*		eq_find_eq_by_name(GList *list, gchar *name);
gboolean	eq_set_equalizer();
gboolean	eq_write_equalizer();
GList*		eq_find_best(GList *list, gchar *name);

#endif
