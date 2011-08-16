

#ifndef GUI_TREEVIEW_H
#define GUI_TREEVIEW_H

//HEADERS
#include "global.h"
#include "debug.h"
#include "common.h"
#include "configure.h"
#include "metadata.h"
#include "library.h"
#include "playlist.h"
#include "equalizer.h"
#include "core.h"
#include "theme.h"
#include "gui_window.h"

//FUNCTIONS
GtkWidget* gui_treeview_warnning_dialog_new(GtkWidget *parent, gchar *str);

gboolean gui_treeview_left_refresh();
gboolean gui_treeview_right_refresh();

gboolean gui_treeview_left_active(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data);
gboolean gui_treeview_right_active(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data);
gboolean gui_treeview_left_change(GtkWidget *widget, gpointer data);
gboolean gui_treeview_right_change(GtkWidget *widget, gpointer data);

gboolean gui_treeview_left_mask(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean gui_treeview_right_mask(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean gui_treeview_left_popup(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean gui_treeview_right_popup(GtkWidget *widget, GdkEventButton *event, gpointer data);

gboolean gui_treeview_callback_left_add(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_left_import(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_left_addtoqueue(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_left_rename(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_left_export(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_left_sort(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_left_delete(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_left_deleteall(GtkWidget *widget, gpointer data);

gboolean gui_treeview_callback_right_play(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_right_addtoqueue(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_right_addtoplaylist(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean gui_treeview_callback_right_information(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_right_addfromfile(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_right_sort(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean gui_treeview_callback_right_delete(GtkWidget *widget, gpointer data);
gboolean gui_treeview_callback_right_deleteall(GtkWidget *widget, gpointer data);

gboolean tree_treeview_view_right_dnd_received(GtkWidget *wgt, GdkDragContext *context, int x, int y,GtkSelectionData *seldata, guint info, guint time,gpointer userdata);
gboolean tree_treeview_view_right_dnd_get_data(GtkWidget *widget, GdkDragContext *context,GtkSelectionData *sdata, guint info, guint time, gpointer data);
gboolean tree_treeview_view_right_dnd_begin(GtkWidget *widget, GdkDragContext *drag_context, gpointer user_data);

#endif
