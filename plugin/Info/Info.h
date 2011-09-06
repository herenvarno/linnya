#ifndef PLUGIN_INFO
#define PLUGIN_INFO

/*
 * HEADERS
 */
#include "core.h"
#include "ui.h"

GtkWidget *label_title, *label_artist, *label_album, *label_duration;

/*
 * FUNCTIONS
 */
const gchar *g_module_check_init(GModule *module);
void g_module_unload(GModule *module);

GtkWidget *ly_plugin_info_create();
void ly_plugin_info_destroy();
gboolean ly_plugin_info_config(GtkWidget *widget, gpointer data);

gboolean ly_plugin_info_on_meta_changed(gpointer message, gpointer data);
#endif