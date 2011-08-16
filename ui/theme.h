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
typedef struct lyUiThemeTheme
{
	gint pos[2];
	gint size[2];
	gint border;
	gint decorated;
	gchar background[512];
	gchar font[128];
	gdouble fg_color0[4];
	gdouble fg_color1[4];
	gdouble bg_color0[4];
	gdouble bg_color1[4];
}lyUiThemeTheme;

/*
 * VARIABLES
 */
GHashTable *ly_ui_theme_themes;

/*
 * FUNCTIONS
 */
gboolean ly_ui_theme_init();
gboolean ly_ui_theme_finalize();
lyUiThemeTheme* ly_ui_theme_get(gchar *name);
gboolean ly_ui_theme_set(gchar *name, lyUiThemeTheme *theme);

gboolean ly_ui_theme_read();
gboolean ly_ui_theme_write();
void ly_ui_theme_write_each_cb(gpointer key, gpointer value, gpointer data);

void ly_ui_theme_start_cb(	GMarkupParseContext *context,
							const gchar *element_name,
							const gchar **attribute_names,
							const gchar **attribute_values,
							gpointer data,
							GError **error);

#endif
