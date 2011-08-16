#ifndef UI_PLUGIN_H
#define UI_PLUGIN_H

/*
 * HEADERS
 */
#include "global.h"
#include "key.h"
#include "theme.h"
#include "win.h"

/*
 * TYPES
 */
typedef struct lyUiPlPlugin
{
	gchar name[256];
	gchar version[256];
	gchar author[256];
	gchar license[2048];
	gchar logo[256];
	gchar comment[2048];
	gchar website[1024];
	
	GModule *module;
	gpointer widget;
	gint deamon;
	
	gchar create_symbol[256];
	gchar refresh_symbol[256];
	gchar destroy_symbol[256];
	gchar config_symbol[256];
}lyUiPlPlugin;

typedef struct lyUiPlDepend
{
	gchar name[256];
	gchar depend[256];
}lyUiPlDepend;

/*
 * VARIABLES
 */
GHashTable *ly_ui_pl_plugins;
GList *ly_ui_pl_depends;
gchar *ly_ui_pl_element_name;	/* PRIVATE */

/*
 * FUNCTIONS
 */
gboolean ly_ui_pl_init();
gboolean ly_ui_pl_finalize();

lyUiPlPlugin *ly_ui_pl_new(gchar *filename);
void ly_ui_pl_new_start_cb(GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer data, GError **error);
void ly_ui_pl_new_text_cb(GMarkupParseContext * context, const gchar * text, gsize text_len, gpointer data, GError **error);
void ly_ui_pl_new_end_cb(GMarkupParseContext *context, const gchar *element_name, gpointer data, GError **error);

gboolean ly_ui_pl_set(lyUiPlPlugin* plugin);
lyUiPlPlugin* ly_ui_pl_get(gchar *name);
gboolean ly_ui_pl_delete(gchar *name);

GList *ly_ui_pl_get_depends(gchar *name);
gboolean ly_ui_pl_set_depend(gchar *name, gchar *depend);
#endif