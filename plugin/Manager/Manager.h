#ifndef PLUGIN_MANAGER
#define PLUGIN_MANAGER

/*
 * HEADERS
 */
#include "core.h"
#include "ui.h"

/*
 * VARABLES
 */
GtkWidget *widget;
GtkWidget *hpaned;
GtkWidget *scrolled_window;
GtkWidget *treeview_left;
GtkWidget *treeview_right;

GtkTreeStore *store_left;
GtkTreeStore *store_right;
GtkTreeSelection *selection_left;
GtkTreeSelection *selection_right;

gint left_id;
gint right_id;

gchar info_title[512];
gchar info_artist[512];
gchar info_album[512];
gchar info_genre[512];
gchar info_track[512];
gchar info_codec[512];
gchar info_start[512];
gchar info_duration[512];
gchar info_bitrate[512];
gchar info_uri[512];
gchar info_comment[1024];
GdkPixbuf *info_cover;

enum{
	LEFT_ADD,
	LEFT_IMPORT,
	LEFT_ADDTOQUEUE,
	LEFT_RENAME,
	LEFT_EXPORT,
//	LEFT_SORT,
	LEFT_REFRESH,
	LEFT_DELETE,
	LEFT_DELETEALL,
	LEFT_NUM,
};
enum{
	RIGHT_PLAY,
	RIGHT_ADDTOQUEUE,
	RIGHT_ADDTOPLAYLIST,
	RIGHT_INFORMATION,
	RIGHT_ADDFILES,
	RIGHT_REFRESH,
	RIGHT_DELETE,
	RIGHT_DELETEALL,
	RIGHT_NUM
};

/*
 * FUNCTIONS
 */
const gchar *g_module_check_init(GModule *module);
void g_module_unload(GModule *module);

GtkWidget *ly_plugin_manager_create();
void ly_plugin_manager_destroy();
GtkWidget *ly_plugin_manager_config();

gboolean ly_plugin_manager_left_refresh_cb();
gboolean ly_plugin_manager_right_refresh_cb();

gboolean ly_plugin_manager_left_active_cb(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data);
gboolean ly_plugin_manager_right_active_cb(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data);
gboolean ly_plugin_manager_left_change_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_right_change_cb(GtkWidget *widget, gpointer data);

gboolean ly_plugin_manager_left_mask(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_plugin_manager_right_mask(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_plugin_manager_popup_left_menu_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_plugin_manager_popup_right_menu_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);

gboolean ly_plugin_manager_left_add_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_left_import_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_left_addtoqueue_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_left_rename_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_left_export_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_left_sort_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_left_delete_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_left_deleteall_cb(GtkWidget *widget, gpointer data);

gboolean ly_plugin_manager_right_play_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_right_addtoqueue_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_right_addtoplaylist_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_plugin_manager_right_information_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_right_addfiles_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_right_sort_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_plugin_manager_right_delete_cb(GtkWidget *widget, gpointer data);
gboolean ly_plugin_manager_right_deleteall_cb(GtkWidget *widget, gpointer data);

gboolean ly_plugin_manager_get_artists_cb(gpointer stmt, gpointer data);
gboolean ly_plugin_manager_get_playlists_cb(gpointer stmt, gpointer data);
gboolean ly_plugin_manager_get_metadatas_cb(gpointer stmt, gpointer data);
#endif