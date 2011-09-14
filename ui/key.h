#ifndef UI_KEY_H
#define UI_KEY_H

/*
 * HEADERS
 */
#include "global.h"

/*
 * TYPES
 */
typedef struct lyUiKeyKeybind
{
	gchar key[64];
	gchar mask0[64];
	gchar mask1[64];
}lyUiKeyKeybind;

/*
 * VARIABLES
 */
GHashTable *ly_ui_key_keybinds;
GtkAccelGroup *ly_ui_key_accel;

/*
 * FUNCTIONS
 */
gboolean ly_ui_key_init();
gboolean ly_ui_key_finalize();

gboolean ly_ui_key_read();
gboolean ly_ui_key_write();

gboolean ly_ui_key_set(char *name, char *mask0, char *mask1, char *key);
lyUiKeyKeybind *ly_ui_key_get(gchar *name);
gboolean ly_ui_key_set_default_if_not_exist(gchar *name);

gboolean ly_ui_key_bind_signal(gchar *name, GtkWidget* widget, gchar *signal);
gboolean ly_ui_key_bind_callback(gchar *name, gpointer f, gpointer data);

void ly_ui_key_start_cb(	GMarkupParseContext *context,
							const gchar *element_name,
							const gchar **attribute_names,
							const gchar **attribute_values,
							gpointer data,
							GError **error);
void ly_ui_key_write_each_cb(gpointer key, gpointer value, gpointer data);
guint ly_ui_key_get_mask(gchar *mask);
#endif
