#ifndef UI_THEME_H
#define UI_THEME_H

/*
 * HEADERS
 */
#include "global.h"
#include "key.h"

/*
 * TYPES
 */
typedef struct _lyUiThemeItem
{
	gint pos[2];
	gint size[2];
	gint border;
	gint decorated;
	gchar bg[512];
}lyUiThemeItem;

/*
 * VARIABLES
 */
GHashTable *ly_ui_theme_table;

/*
 * FUNCTIONS
 */
gboolean ly_ui_theme_init();
gboolean ly_ui_theme_finalize();

GList* ly_ui_theme_get_list();
gboolean ly_ui_theme_table_new();
gboolean ly_ui_theme_table_destroy();

lyUiThemeItem* ly_ui_theme_get(char *name);
gboolean ly_ui_theme_set(char *name, lyUiThemeItem *th);

void ly_ui_theme_start_cb(	GMarkupParseContext *context,
							const gchar *element_name,
							const gchar **attribute_names,
							const gchar **attribute_values,
							gpointer data,
							GError **error);
#endif
