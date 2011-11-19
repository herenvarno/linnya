#ifndef UI_THEME_H
#define UI_THEME_H

/*
 * HEADERS
 */
#include "glb.h"
#include "dbg.h"

/*
 * TYPES
 */
typedef struct _LyThmItem
{
	gint pos[2];
	gint size[2];
	gint border;
	gint decorated;
	gchar bg[512];
}LyThmItem;

/*
 * VARIABLES
 */
GHashTable *ly_thm_table;

/*
 * FUNCTIONS
 */
gboolean ly_thm_init();
gboolean ly_thm_finalize();

GList* ly_thm_get_list();
gboolean ly_thm_table_new();
gboolean ly_thm_table_destroy();

LyThmItem* ly_thm_get(char *name);
gboolean ly_thm_set(char *name, LyThmItem *th);

void ly_thm_start_cb(	GMarkupParseContext *context,
							const gchar *element_name,
							const gchar **attribute_names,
							const gchar **attribute_values,
							gpointer data,
							GError **error);
#endif
