#ifndef UI_SESSION_H
#define UI_SESSION_H

/*
 * HEADERS
 */
#include "global.h"
#include "key.h"
#include "theme.h"
#include "win.h"
#include "pl.h"

/*
 * VARIABLES
 */
GtkListStore *ly_ui_session_store;
GtkWidget *ly_ui_session_tab_add_label;

/*
 * FUNCTIONS
 */
gboolean ly_ui_session_init();
gboolean ly_ui_session_finalize();

gboolean ly_ui_session_create(gchar *name, GtkWidget *widget);
gboolean ly_ui_session_refresh();
gboolean ly_ui_session_destroy(GtkWidget *widget);

gboolean ly_ui_session_tab_add_init();
gboolean ly_ui_session_tab_add_refresh();
gboolean ly_ui_session_tab_add_finalize();
gboolean ly_ui_session_tab_add_create();
gboolean ly_ui_session_tab_add_destroy();

void ly_ui_session_tab_add_refresh_cb(gpointer key, gpointer value, gpointer data);

gboolean ly_ui_session_active_cb(GtkIconView *iconview,GtkTreePath *path,gpointer data);
gboolean ly_ui_session_close_cb(GtkWidget *widget, gpointer data);
#endif