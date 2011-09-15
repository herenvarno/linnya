#ifndef UI_KEY_H
#define UI_KEY_H

/*
 * HEADERS
 */
#include "global.h"

/*
 * TYPES
 */
enum{
	KEY_BIND_UNDEFINED,
	KEY_BIND_SIGNAL,
	KEY_BIND_CALLBACK
};

typedef struct _lyUiKeyKeybind
{
	char mask0[64];
	char mask1[64];
	char key[64];
	
	int type;
	gpointer arg0;
	gpointer arg1;
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

gboolean ly_ui_key_set(gchar *name, gchar *mask0, gchar *mask1, gchar *key, int type, gpointer arg0, gpointer arg1);
gboolean ly_ui_key_set_keys(gchar *name, gchar *mask0, gchar *mask1, gchar *key);
gboolean ly_ui_key_set_args(char *name, int type, gpointer arg0, gpointer arg1);
lyUiKeyKeybind *ly_ui_key_get(gchar *name);
gboolean ly_ui_key_set_default_if_not_exist(gchar *name);

gboolean ly_ui_key_bind(gchar *name);
gboolean ly_ui_key_unbind(gchar *name);

void ly_ui_key_start_cb(	GMarkupParseContext *context,
							const gchar *element_name,
							const gchar **attribute_names,
							const gchar **attribute_values,
							gpointer data,
							GError **error);
void ly_ui_key_write_each_cb(gpointer key, gpointer value, gpointer data);
guint ly_ui_key_get_mask(gchar *mask);
char *ly_ui_key_get_conflict(char *except_name, char *mask0, char *mask1, char *key);
#endif
