/*
 * vdo.c
 * This file is part of linnya
 *
 * Copyright (C) 2012 - Edward
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * HEADERS
 */
#include "vdo.h"

/*
 * MACROS
 */

/*
 * VARIABLES
 */
GtkWidget *ly_3vdo_screen=NULL;
GtkWidget *ly_3vdo_list_view=NULL;
GtkListStore *ly_3vdo_list_store=NULL;
GtkTreeSelection *ly_3vdo_list_selection=NULL;

/*
 * FUNCTIONS
 */
static gboolean
ly_3vdo_on_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
void
ly_3vdo_on_meta_changed_cb(LyMbsMessage *message, gpointer data);
static gboolean ly_3vdo_on_get_list_cb(gpointer stmt, gpointer data);
static gboolean ly_3vdo_on_list_active_cb(GtkTreeView *view, GtkTreePath *path,
		GtkTreeViewColumn *column,gpointer data);
static gboolean ly_3vdo_on_list_add_cb(GtkWidget *widget, gpointer data);
gpointer ly_3vdo_on_list_add_cb_cb(gpointer data);
static gboolean ly_3vdo_on_list_refresh_cb(GtkWidget *widget, gpointer data);
static gboolean ly_3vdo_on_list_del_cb(GtkWidget *widget, gpointer data);
static gboolean ly_3vdo_on_expander_hover_cb(GtkWidget *widget, GdkEvent  *event, gpointer data);
static gboolean ly_3vdo_on_screen_action_cb(GtkWidget *widget,
		GdkEventButton *event, gpointer data);

// DEFINATION
const gchar*
g_module_check_init(GModule *module)
{
	return NULL;
}

void
g_module_unload(GModule *module)
{
}

GtkWidget*
ly_3vdo_create()
{
	GtkWidget *widget;
	GtkWidget *screen;
	GtkWidget *overlay;
	GtkWidget *expander;
	GtkWidget *box;
	GtkWidget *scroll;
	GtkWidget *view;
	GtkWidget *btn_add;
	GtkWidget *btn_del;
	GtkWidget *btn_frs;
	GtkWidget *img;

	widget=gtk_grid_new();

	overlay=gtk_overlay_new();
	screen=gtk_drawing_area_new();
	ly_3vdo_screen=screen;
	gtk_widget_set_events(screen,GDK_ALL_EVENTS_MASK);
	gtk_widget_set_visual(ly_win_get_window()->win, NULL);
	gtk_widget_set_vexpand(screen, TRUE);
	gtk_widget_set_hexpand(screen, TRUE);
	gtk_container_add(GTK_CONTAINER(overlay), screen);
	expander=gtk_button_new();
	gtk_overlay_add_overlay (GTK_OVERLAY (overlay), expander);
	gtk_widget_set_vexpand(expander, FALSE);
	gtk_widget_set_hexpand(expander, TRUE);
	gtk_widget_set_halign (expander, GTK_ALIGN_END);
	gtk_grid_attach(GTK_GRID(widget), overlay, 0, 0, 1, 1);

	box=gtk_grid_new();
	gtk_grid_attach(GTK_GRID(widget), box, 1, 0, 1, 1);
	scroll=gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
			GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scroll, 200, -1);
	gtk_widget_set_vexpand(scroll, TRUE);
	gtk_widget_set_hexpand(scroll, FALSE);
	gtk_grid_attach(GTK_GRID(box), scroll, 0, 0, 3, 1);
	view=gtk_tree_view_new();
	gtk_container_add(GTK_CONTAINER(scroll), view);
	ly_3vdo_list_view=view;
	ly_3vdo_on_list_refresh_cb(NULL, NULL);
	GtkTreeViewColumn *column;
	GtkCellRenderer *cell_renderer;
	column = gtk_tree_view_column_new();
	cell_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, cell_renderer,FALSE);
	gtk_tree_view_column_add_attribute(column,cell_renderer,"text", 1);
	gtk_tree_view_column_set_title(column,_("Title"));
	gtk_tree_view_column_set_max_width  (GTK_TREE_VIEW_COLUMN(column),150);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),100);
	gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
	column = gtk_tree_view_column_new();
	cell_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, cell_renderer,FALSE);
	gtk_tree_view_column_add_attribute(column,cell_renderer,"text", 2);
	gtk_tree_view_column_set_title(column,_("Time"));
	gtk_tree_view_column_set_max_width  (GTK_TREE_VIEW_COLUMN(column),50);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),20);
	gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
	ly_3vdo_list_selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_tree_selection_set_mode(ly_3vdo_list_selection,GTK_SELECTION_MULTIPLE);

	btn_add=gtk_button_new();
	img=gtk_image_new_from_stock(GTK_STOCK_ADD,GTK_ICON_SIZE_BUTTON);
	gtk_container_add(GTK_CONTAINER(btn_add), img);
	gtk_widget_set_size_request(btn_add, 30, 30);
	gtk_grid_attach(GTK_GRID(box), btn_add, 0, 1, 1, 1);
	btn_del=gtk_button_new();
	img=gtk_image_new_from_stock(GTK_STOCK_REMOVE,GTK_ICON_SIZE_BUTTON);
	gtk_container_add(GTK_CONTAINER(btn_del), img);
	gtk_widget_set_size_request(btn_del, 30, 30);
	gtk_grid_attach(GTK_GRID(box), btn_del, 1, 1, 1, 1);
	btn_frs=gtk_button_new();
	img=gtk_image_new_from_stock(GTK_STOCK_REFRESH,GTK_ICON_SIZE_BUTTON);
	gtk_container_add(GTK_CONTAINER(btn_frs), img);
	gtk_widget_set_size_request(btn_frs, 30, 30);
	gtk_grid_attach(GTK_GRID(box), btn_frs, 2, 1, 1, 1);

	g_signal_connect(screen,"draw",G_CALLBACK( ly_3vdo_on_expose_cb), NULL);
	ly_mbs_bind("meta_changed", "core:pqm", ly_3vdo_on_meta_changed_cb, screen);

	g_signal_connect(G_OBJECT(view), "row-activated",
			G_CALLBACK(ly_3vdo_on_list_active_cb), NULL);
	g_signal_connect(G_OBJECT(btn_add), "clicked",
			G_CALLBACK(ly_3vdo_on_list_add_cb), NULL);
	g_signal_connect(G_OBJECT(btn_del), "clicked",
			G_CALLBACK(ly_3vdo_on_list_del_cb), NULL);
	g_signal_connect(G_OBJECT(btn_frs), "clicked",
			G_CALLBACK(ly_3vdo_on_list_refresh_cb), NULL);
	g_signal_connect(G_OBJECT(expander), "enter-notify-event",
			G_CALLBACK(ly_3vdo_on_expander_hover_cb), box);


	gtk_widget_set_name(widget, "3vdo_widget");
	gtk_widget_set_name(expander, "3vdo_btn_expander");
	/*
	 * Load Theme
	 */
	GtkCssProvider *provider;
	gchar csspath[1024]="";
	screen = gtk_widget_get_screen(widget);
	provider = gtk_css_provider_new();
	g_snprintf(csspath, sizeof(csspath), "%svdo/style/style.css", LY_GLB_PROG_PIXDIR);
	gtk_css_provider_load_from_path(provider, csspath, NULL);
	gtk_style_context_add_provider_for_screen(screen,GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_style_context_reset_widgets(screen);
	g_object_unref(provider);

	return widget;
}

void
ly_3vdo_destroy()
{
	ly_ppl_video_set_screen(0);

	GdkScreen *screen = gtk_widget_get_screen(ly_win_get_window()->win);
	if(gtk_widget_is_composited(ly_win_get_window()->win))
	{
		GdkVisual *visual;
		visual = gdk_screen_get_rgba_visual (screen);
		if (visual == NULL)
			visual = gdk_screen_get_system_visual (screen);
		gtk_widget_set_visual (GTK_WIDGET (ly_win_get_window()->win), visual);
	}
	ly_mbs_unbind("meta_changed", "core:pqm", ly_3vdo_on_meta_changed_cb);
}

static gboolean
ly_3vdo_on_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	guintptr xid=GDK_WINDOW_XID(gtk_widget_get_window(GTK_WIDGET(widget)));
	ly_ppl_video_set_screen(xid);
	return FALSE;
}

void
ly_3vdo_on_meta_changed_cb(LyMbsMessage *message, gpointer data)
{
	guintptr xid=GDK_WINDOW_XID(gtk_widget_get_window(GTK_WIDGET(data)));
	ly_ppl_video_set_screen(xid);
}

static gboolean
ly_3vdo_on_get_list_cb(gpointer stmt, gpointer data)
{
	GtkTreeIter iter;
	gchar *title=(gchar *)g_malloc(128*sizeof(gchar));
	gchar *time=(gchar *)g_malloc(64*sizeof(gchar));
	gint id=0;
	gtk_list_store_append (ly_3vdo_list_store, &iter);
	id=sqlite3_column_int(stmt, 0);
	g_strlcpy(title,(const gchar *)sqlite3_column_text(stmt, 1), 128);
	g_strlcpy(time,(const gchar *)sqlite3_column_text(stmt, 2), 64);
	gtk_list_store_set(ly_3vdo_list_store, &iter,0 , id,1, title, 2, time, -1);
	return FALSE;
}

static gboolean
ly_3vdo_on_list_active_cb(GtkTreeView *view, GtkTreePath *path,
		GtkTreeViewColumn *column,gpointer data)
{
	ly_pqm_clear_md();
	ly_dbm_exec("UPDATE metadatas SET playing=num WHERE flag>=10 AND flag<=19",
			NULL, NULL);

	GList *list=NULL;
	GtkTreeIter iter;
	int id=0;
	list=gtk_tree_selection_get_selected_rows(ly_3vdo_list_selection, NULL);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_3vdo_list_store), &iter, path);
	g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (list);
	gtk_tree_model_get(GTK_TREE_MODEL(ly_3vdo_list_store), &iter, 0, &id, -1);
	ly_aud_stop();
	ly_pqm_set_current_md(id);
	ly_aud_play();
	return FALSE;
}

static gboolean
ly_3vdo_on_list_add_cb(GtkWidget *widget, gpointer data)
{
	GSList *filelist;
	GtkFileFilter *filter;
	GtkWidget *dialog;

	dialog =gtk_file_chooser_dialog_new(_("Add Video Files..."),
			GTK_WINDOW(ly_win_get_window()->win), GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL,
			GTK_RESPONSE_REJECT, NULL);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER (dialog),TRUE);
	filter=gtk_file_filter_new();
	gtk_file_filter_set_name(filter,_("Video File"));
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.avi");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.AVI");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.wmv");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.WMV");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.rm");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.RM");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.rmvb");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.RMVB");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.mkv");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.MKV");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.ogg");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.OGG");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.ogv");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.OGV");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.mp4");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.MP4");	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.mov");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.MOV");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog),
			GTK_FILE_FILTER(filter));
	gint result = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (result)
	{
		case GTK_RESPONSE_ACCEPT:
			break;
		case GTK_RESPONSE_REJECT:
			gtk_widget_destroy (dialog);
			return FALSE;
			break;
		default:
			gtk_widget_destroy (dialog);
			return FALSE;
			break;
	}

	filelist=gtk_file_chooser_get_uris(GTK_FILE_CHOOSER (dialog));
	g_thread_create(ly_3vdo_on_list_add_cb_cb, filelist, FALSE, NULL);
	gtk_widget_destroy (dialog);
	return FALSE;
}

gpointer
ly_3vdo_on_list_add_cb_cb(gpointer data)
{
	GSList *filelist=(GSList*)data;
	GSList *q=filelist;
	int mid=0;
	gchar *filename,*fileuri, tmp[10240];
	LyMdhMetadata *md;

	ly_dbm_exec("begin",NULL,NULL);
	while(q)
	{
		filename=g_filename_from_uri(q->data,NULL,NULL);
		realpath(filename,tmp);
		fileuri=g_strconcat("file://",tmp, NULL);
		g_free(filename);
		md=ly_mdh_new_with_uri(fileuri);
 		md->flag=10;
		g_free(fileuri);
		if(md)
		{
			mid=ly_lib_get_id(md);
			if(mid<=0)
			{
	 			ly_lib_add_md(md);
				mid=ly_lib_get_id(md);
			}
			if(mid>0)
				ly_pqm_add_md(mid);
			ly_mdh_free(md);
		}
		q=q->next;
	}

	ly_dbm_exec("commit",NULL,NULL);
	g_slist_free(filelist);

	ly_3vdo_on_list_refresh_cb(NULL,NULL);
	return NULL;
}

static gboolean
ly_3vdo_on_list_refresh_cb(GtkWidget *widget, gpointer data)
{
	if(ly_3vdo_list_store)
		g_object_unref(ly_3vdo_list_store);
	ly_3vdo_list_store=NULL;

	ly_3vdo_list_store = gtk_list_store_new (3,G_TYPE_INT,G_TYPE_STRING,
			G_TYPE_STRING);
	ly_dbm_exec("SELECT "\
			"id, title, duration "\
			"FROM metadatas "\
			"WHERE flag>=10 AND flag<=19 ORDER BY num",
			ly_3vdo_on_get_list_cb, NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW (ly_3vdo_list_view),
			GTK_TREE_MODEL(ly_3vdo_list_store));
	return FALSE;
}

static gboolean
ly_3vdo_on_list_del_cb(GtkWidget *widget, gpointer data)
{
	GList *list;
	GList *p;
	GtkTreeIter iter;
	GtkWidget *dialog;
	gint result;

	gint id;
	gchar *where=NULL;
	gchar *tmpwhere=NULL;
	gchar tmp[1024]="";

	list=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(
				ly_3vdo_list_selection),NULL);
	if(list==NULL)
		return FALSE;

	p=list;
	while(p)
	{
		gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_3vdo_list_store), &iter,
				(GtkTreePath *)(p->data));
		gtk_tree_model_get(GTK_TREE_MODEL(ly_3vdo_list_store), &iter, 0, &id,
				-1);
		if(p==list)
		{
			g_snprintf(tmp, sizeof(tmp), "id=%d", id);
			where=g_strconcat(tmp, NULL);
		}
		else
		{
			g_snprintf(tmp, sizeof(tmp), " OR id=%d", id);
			tmpwhere=g_strconcat(where, tmp, NULL);
			g_free(where);
			where=tmpwhere;
		}
		p=p->next;
	}
	ly_pqm_del_md_by_where(where);
	ly_lib_del_md_by_where(where);
	g_free(where);
	g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (list);
	ly_3vdo_on_list_refresh_cb(NULL, NULL);
	return FALSE;
}

static gboolean
ly_3vdo_on_expander_hover_cb(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	if(!data)
		return FALSE;
	if(gtk_widget_get_visible(GTK_WIDGET(data)))
	{
		gtk_widget_set_visible(GTK_WIDGET(data), FALSE);
	}
	else
	{
		gtk_widget_set_visible(GTK_WIDGET(data), TRUE);
	}
	return FALSE;
}
