#ifndef PLUGIN_DOWNLRC
#define PLUGIN_DOWNLRC

/*
 * HEADERS
 */
#include "core.h"
#include "ui.h"
#include "Lrc.h"
#include "Python.h"

/*
 * FUNCTIONS
 */
const gchar *g_module_check_init(GModule *module);
void g_module_unload(GModule *module);

GtkWidget *ly_plugin_downlrc_config();

gboolean ly_plugin_downlrc_download_cb(gpointer message, gpointer data);
gpointer ly_plugin_downlrc_create_thread_cb(void *arg);
gboolean ly_plugin_downlrc_download_lrc123(char *title, char *artist, char *dest);
gboolean ly_plugin_downlrc_download_qianqian(char *title, char *artist, char *dest);

#endif
