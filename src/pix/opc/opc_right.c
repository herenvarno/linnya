/*
 * opc_right.c
 * This file is part of linnya:plugin
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya:plugin is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya:plugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya:plugin. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * HEADERS
 */
#include "opc_right.h"

/*
 * TYPES
 */
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

enum {
	COVER_TYPE_JPEG,
	COVER_TYPE_JPG,
	COVER_TYPE_BMP,
	COVER_TYPE_PNG,
	COVER_TYPE_SVG,
	COVER_TYPE_COUNT
};
const char *cover_type[]={
	"jpeg",
	"jpg",
	"bmp",
	"png",
	"svg"
};


/*
 * VARIABLES
 */
GtkWidget			*ly_3opc_right_treeview_right=NULL;
GtkWidget			*ly_3opc_right_label_page=NULL;
GtkTreeStore		*ly_3opc_right_store_right=NULL;
GtkTreeSelection	*ly_3opc_right_selection_right=NULL;
GdkPixbuf			*ly_3opc_right_icon_playing=NULL;
gchar				*ly_3opc_right_str_old=NULL;

/*
 * FUNCTIONS
 */
gboolean ly_3opc_right_on_get_metadatas_cb(gpointer stmt, gpointer data);
char *ly_3opc_right_build_sql();
gboolean ly_3opc_right_on_button_p_clicked_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_right_on_button_n_clicked_cb(GtkWidget *widget, gpointer data);

gboolean ly_3opc_right_on_active_cb(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data);
void ly_3opc_right_on_select_changed_cb(LyMbsMessage *message, gpointer data);
void ly_3opc_right_on_limit_changed_cb(LyMbsMessage *message, gpointer data);
void ly_3opc_right_on_meta_changed_cb(LyMbsMessage *message, gpointer data);
gboolean ly_3opc_right_mask_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_3opc_right_popup_menu_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);

gboolean ly_3opc_right_play_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_right_addtoqueue_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_right_addtoplaylist_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_3opc_right_information_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_right_addfiles_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_right_refresh_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_right_delete_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_right_deleteall_cb(GtkWidget *widget, gpointer data);

gpointer ly_3opc_right_addfiles_cb_cb(gpointer data);

GtkWidget* ly_3opc_warning_dialog_create(gchar *str);


void		ly_3opc_right_init		()
{
	char path[1024]="";
	g_snprintf(path, sizeof(path), "%sopc/playing.svg", LY_GLB_PROG_PIXDIR);
	ly_3opc_right_icon_playing=gdk_pixbuf_new_from_file_at_size(path, 20, 20, NULL);
}
void		ly_3opc_right_fina		()
{
	if(ly_3opc_right_icon_playing)
		g_object_unref(ly_3opc_right_icon_playing);
}

GtkWidget*	ly_3opc_right_create		()
{
	GtkWidget *vbox;
	vbox=gtk_vbox_new(FALSE,0);
	
	GtkWidget *scrolled_window;
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window,TRUE,TRUE,0);
	
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *button_p;
	GtkWidget *button_n;
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox,FALSE,FALSE,0);
	button_p=gtk_button_new_from_stock(GTK_STOCK_MEDIA_PREVIOUS);
	gtk_widget_set_size_request(button_p, 100, -1);
	gtk_box_pack_start(GTK_BOX(hbox), button_p,FALSE,FALSE,0);
	label=gtk_label_new(_("Page 1"));
	ly_3opc_right_label_page=label;
	gtk_box_pack_start(GTK_BOX(hbox), label,TRUE,TRUE,0);
	button_n=gtk_button_new_from_stock(GTK_STOCK_MEDIA_NEXT);
	gtk_widget_set_size_request(button_n, 100, -1);
	gtk_box_pack_start(GTK_BOX(hbox), button_n,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button_p), "clicked", G_CALLBACK(ly_3opc_right_on_button_p_clicked_cb), NULL);
	g_signal_connect(G_OBJECT(button_n), "clicked", G_CALLBACK(ly_3opc_right_on_button_n_clicked_cb), NULL);
	
	ly_3opc_right_treeview_right=gtk_tree_view_new();
	gtk_container_add(GTK_CONTAINER(scrolled_window),ly_3opc_right_treeview_right);
	ly_3opc_right_store_right  = gtk_tree_store_new (5,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING, G_TYPE_INT);
	
	char *sql=NULL;
	sql=ly_3opc_right_build_sql();
	if(sql)
	{
		ly_dbm_exec(sql, ly_3opc_right_on_get_metadatas_cb, NULL);
		g_free(sql);
	}

	GtkTreeViewColumn *column;
	GtkCellRenderer *cell_renderer;
	cell_renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes(_("*"),cell_renderer,"pixbuf",0,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (ly_3opc_right_treeview_right), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Title"),cell_renderer,"text",1,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),100);
	gtk_tree_view_column_set_max_width  (GTK_TREE_VIEW_COLUMN(column),200);
	gtk_tree_view_append_column (GTK_TREE_VIEW (ly_3opc_right_treeview_right), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Artist"),cell_renderer,"text",2,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),100);
	gtk_tree_view_column_set_max_width  (GTK_TREE_VIEW_COLUMN(column),250);
	gtk_tree_view_append_column (GTK_TREE_VIEW (ly_3opc_right_treeview_right), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Album"),cell_renderer,"text",3,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),100);
	gtk_tree_view_column_set_max_width  (GTK_TREE_VIEW_COLUMN(column),200);
	gtk_tree_view_append_column (GTK_TREE_VIEW (ly_3opc_right_treeview_right), column);
	gtk_tree_view_set_model(GTK_TREE_VIEW (ly_3opc_right_treeview_right), GTK_TREE_MODEL(ly_3opc_right_store_right ));
	ly_3opc_right_selection_right=gtk_tree_view_get_selection(GTK_TREE_VIEW(ly_3opc_right_treeview_right));
	gtk_tree_selection_set_mode(ly_3opc_right_selection_right,GTK_SELECTION_MULTIPLE);
	
	g_signal_connect(G_OBJECT(ly_3opc_right_treeview_right), "row-activated", G_CALLBACK(ly_3opc_right_on_active_cb), NULL);
	g_signal_connect(G_OBJECT(ly_3opc_right_treeview_right), "button_release_event", G_CALLBACK(ly_3opc_right_popup_menu_cb), NULL);
	g_signal_connect(G_OBJECT(ly_3opc_right_treeview_right), "button_press_event", G_CALLBACK(ly_3opc_right_mask_cb), NULL);
	
	ly_mbs_bind("reg_3opc_select_changed", "core:reg", ly_3opc_right_on_select_changed_cb, NULL);
	ly_mbs_bind("reg_3opc_limit_changed", "core:reg", ly_3opc_right_on_limit_changed_cb, NULL);
	ly_mbs_bind("meta_changed", "core:pqm", ly_3opc_right_on_meta_changed_cb, NULL);

	return vbox;
}
void		ly_3opc_right_destroy	()
{
	if(ly_3opc_right_str_old)
		g_free(ly_3opc_right_str_old);
	ly_3opc_right_str_old=NULL;
	ly_mbs_unbind("reg_3opc_select_changed", "core:reg", ly_3opc_right_on_select_changed_cb);
	ly_mbs_unbind("reg_3opc_limit_changed", "core:reg", ly_3opc_right_on_limit_changed_cb);
	ly_mbs_unbind("meta_changed", "core:pqm", ly_3opc_right_on_meta_changed_cb);
}
void		ly_3opc_right_refresh	()
{
	if(ly_3opc_right_store_right)
		g_object_unref(ly_3opc_right_store_right);
	ly_3opc_right_store_right=NULL;
	
	ly_3opc_right_store_right = gtk_tree_store_new (5,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING, G_TYPE_INT);
	char *sql=NULL;
	sql=ly_3opc_right_build_sql();
	if(sql)
	{
		ly_dbm_exec(sql, ly_3opc_right_on_get_metadatas_cb, NULL);
		g_free(sql);
	}
	gtk_tree_view_set_model(GTK_TREE_VIEW (ly_3opc_right_treeview_right), GTK_TREE_MODEL(ly_3opc_right_store_right));
	char str[1024]="";
	int offset=0;
	int limit=-1;
	ly_reg_get("3opc_limit", "%d:%d", &offset, &limit);
	if(limit>0)
	{
		g_snprintf(str, sizeof(str), _("Page %d"), offset/limit+1);
	}
	else
	{
		g_snprintf(str, sizeof(str), _("Page 1"));
	}
	gtk_label_set_text(GTK_LABEL(ly_3opc_right_label_page), str);
}

gboolean ly_3opc_right_on_get_metadatas_cb(gpointer stmt, gpointer data)
{
	GtkTreeIter iter;
	gint id=0;
	gchar title[128]="";
	gchar artist[128]="";
	gchar album[128]="";

	gtk_tree_store_append (ly_3opc_right_store_right , &iter, NULL);
	id=sqlite3_column_int(stmt, 0);
	g_strlcpy(title,(const gchar *)sqlite3_column_text(stmt, 1),128);
	g_strlcpy(artist,(const gchar *)sqlite3_column_text(stmt, 2),128);
	g_strlcpy(album,(const gchar *)sqlite3_column_text(stmt, 3),128);
	LyMdhMetadata *md=NULL;
	md=ly_pqm_get_current_md();
	if(!md)
	{
		gtk_tree_store_set(ly_3opc_right_store_right, &iter, 0, NULL, 1,title, 2,artist, 3,album, 4, id, -1);
		return FALSE;
	}
	if(md->id==id)
	{
		if(ly_3opc_right_str_old)
			g_free(ly_3opc_right_str_old);
		gtk_tree_store_set(ly_3opc_right_store_right, &iter, 0, ly_3opc_right_icon_playing, 1,title, 2,artist, 3,album, 4, id, -1);
		ly_3opc_right_str_old=gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter);
	}
	else
		gtk_tree_store_set(ly_3opc_right_store_right, &iter, 0, NULL, 1,title, 2,artist, 3,album, 4, id, -1);
	return FALSE;
}

char *ly_3opc_right_build_sql()
{
	char sql[10240]="";
	char tmp[10240]="";
	int offset=0;
	int limit=0;
	int index0=0;
	int index1=0;
	int id=0;
	char name[1024]="";
	ly_reg_get("3opc_select", "%d:%d:%d:%1023[^\n]", &index0, &index1, &id, name);
	ly_reg_get("3opc_limit", "%d:%d", &offset, &limit);
	ly_dbm_replace_str(name, sizeof(name));
	switch(index0)
	{
		case 0:
			g_snprintf(sql, sizeof(sql), "SELECT id, title, artist, album FROM plist WHERE flag>=0 AND flag<=9 ORDER BY playing");
			break;
		case 1:
			if(id<=0)
			{
				g_snprintf(sql, sizeof(sql), "SELECT metadatas.id, metadatas.title, metadatas.artist, metadatas.album FROM metadatas, playlists, connections WHERE metadatas.id=connections.mid AND playlists.id=connections.pid AND flag>=0 AND flag<=9 ORDER BY connections.num");
			}
			else
			{
				g_snprintf(sql, sizeof(sql), "SELECT metadatas.id, metadatas.title, metadatas.artist, metadatas.album FROM metadatas, playlists, connections WHERE metadatas.id=connections.mid AND playlists.id=connections.pid AND playlists.id=%d AND flag>=0 AND flag<=9 ORDER BY connections.num", id);
			}
			break;
		case 2:
			if(id<=0)
			{
				g_snprintf(sql, sizeof(sql), "SELECT id, title, artist, album FROM metadatas WHERE flag>=0 AND flag<=9 ORDER BY num");
			}
			else
			{
				g_snprintf(sql, sizeof(sql), "SELECT id, title, artist, album FROM metadatas WHERE artist='%s' AND flag>=0 AND flag<=9 ORDER BY num", name);
			}
			break;
		default:
			break;
	}
	if(!g_str_equal(sql, ""))
	{
		g_strlcpy(tmp, sql, sizeof(tmp));
		g_snprintf(sql, sizeof(sql), "%s LIMIT %d OFFSET %d", tmp, limit, offset);
		return g_strdup(sql);
	}
	return NULL;
}

gboolean ly_3opc_right_mask_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button==3)
	{
		GList *selectlist=gtk_tree_selection_get_selected_rows(ly_3opc_right_selection_right,NULL);
		if (g_list_length(selectlist)>1)
			return TRUE;
	}
	return FALSE;
}


void ly_3opc_right_on_select_changed_cb(LyMbsMessage *message, gpointer data)
{
	int limit=-1;
	ly_reg_get("3opc_limit", "%*d:%d", &limit);
	ly_reg_set("3opc_limit", "0:%d", limit);
	g_free(ly_3opc_right_str_old);
	ly_3opc_right_str_old=NULL;
	ly_3opc_right_refresh();
}
void ly_3opc_right_on_limit_changed_cb(LyMbsMessage *message, gpointer data)
{
	g_free(ly_3opc_right_str_old);
	ly_3opc_right_str_old=NULL;

	//refresh
	ly_3opc_right_refresh();
}


void ly_3opc_right_on_meta_changed_cb(LyMbsMessage *message, gpointer data)
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return;

	int id;
	GtkTreeIter iter;
	if(ly_3opc_right_str_old)
	{
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, ly_3opc_right_str_old);
		if(gtk_tree_store_iter_is_valid(ly_3opc_right_store_right, &iter))
		{
			gtk_tree_store_set(ly_3opc_right_store_right, &iter, 0, NULL, -1);
		}
		g_free(ly_3opc_right_str_old);
		ly_3opc_right_str_old=NULL;
	}
	
	if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter))
	{
		return;
	}
	gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &id, -1);
	if(id==md->id)
	{
		ly_3opc_right_str_old=gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter);
		gtk_tree_store_set(ly_3opc_right_store_right, &iter, 0, ly_3opc_right_icon_playing, -1);
		return;
	}
	else
	{
		while(gtk_tree_model_iter_next(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter))
		{
			gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &id, -1);
			if(id==md->id)
			{
				ly_3opc_right_str_old=gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter);
				gtk_tree_store_set(ly_3opc_right_store_right, &iter, 0, ly_3opc_right_icon_playing, -1);
				return;
			}
		}
	}
}


gboolean ly_3opc_right_popup_submenu_cb(gpointer stmt, gpointer data)
{
	if(data==NULL||stmt==NULL)
		return TRUE;
	
	gint id=0;
	gchar name[512]="";
	GtkWidget *submenu=(GtkWidget*)data;
	GtkWidget *submenuitem=NULL;
	id=sqlite3_column_int(stmt, 0);
	g_strlcpy(name,(const gchar *)sqlite3_column_text(stmt, 1),sizeof(name));
	
	submenuitem=gtk_menu_item_new_with_label(name);
	g_signal_connect(G_OBJECT(submenuitem), "button_press_event",G_CALLBACK(ly_3opc_right_addtoplaylist_cb),(gpointer)id);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu),submenuitem);
	return FALSE;
}

gboolean ly_3opc_right_popup_menu_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button!=3) 
	{
		return FALSE;
	}
	
	char *menu_right[RIGHT_NUM]={
		_("Play"),
		_("Add to Play Queue"),
		_("Add to Playlist"),
		_("Information"),
		_("Add Files"),
		_("Refresh"),
		_("Delete"),
		_("Delete All")
	};
	
	GtkWidget	*menu=NULL;
	GtkWidget	*menuitem[RIGHT_NUM];
	GtkWidget	*submenu;
	GtkWidget	*submenuitem;
	GtkWidget	*hseparator;
	
	GList *selectlist;
	gint i=0;
	
	menu=gtk_menu_new();
	for(i=0;i<RIGHT_NUM;i++)
	{
		if(i==RIGHT_ADDFILES||i==RIGHT_DELETE)
		{
			hseparator=gtk_separator_menu_item_new();
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),hseparator);
		}
		menuitem[i]=gtk_menu_item_new_with_label(menu_right[i]);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem[i]);
		if(i==RIGHT_ADDTOPLAYLIST)
		{
			submenu=gtk_menu_new();
			submenuitem=NULL;
			submenuitem=gtk_menu_item_new_with_label(_("New Playlist"));
			g_signal_connect(G_OBJECT(submenuitem), "button_press_event",G_CALLBACK(ly_3opc_right_addtoplaylist_cb),(gpointer)(-1));
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu),submenuitem);
				
			ly_dbm_exec("SELECT id, name FROM playlists", ly_3opc_right_popup_submenu_cb,submenu);
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem[RIGHT_ADDTOPLAYLIST]),submenu);
		}
	}
	g_signal_connect(G_OBJECT(menuitem[RIGHT_PLAY]), "activate", G_CALLBACK(ly_3opc_right_play_cb),NULL);
	g_signal_connect(G_OBJECT(menuitem[RIGHT_ADDTOQUEUE]), "activate", G_CALLBACK(ly_3opc_right_addtoqueue_cb),NULL);
	g_signal_connect(G_OBJECT(menuitem[RIGHT_INFORMATION]), "activate", G_CALLBACK(ly_3opc_right_information_cb),NULL);
	g_signal_connect(G_OBJECT(menuitem[RIGHT_ADDFILES]), "activate", G_CALLBACK(ly_3opc_right_addfiles_cb),NULL);
	g_signal_connect(G_OBJECT(menuitem[RIGHT_REFRESH]), "activate", G_CALLBACK(ly_3opc_right_refresh_cb),NULL);
	g_signal_connect(G_OBJECT(menuitem[RIGHT_DELETE]), "activate", G_CALLBACK(ly_3opc_right_delete_cb),NULL);
	g_signal_connect(G_OBJECT(menuitem[RIGHT_DELETEALL]), "activate", G_CALLBACK(ly_3opc_right_deleteall_cb),NULL);

	int index0=0;
	int index1=0;
	int id=0;
	char name[1024]="";
	ly_reg_get("3opc_select", "%d:%d:%d:%[^\n]s", &index0, &index1, &id, name);
	
	selectlist=gtk_tree_selection_get_selected_rows(ly_3opc_right_selection_right, NULL);
	
	if(g_list_length(selectlist)==0)
	{
		gtk_widget_set_sensitive(menuitem[RIGHT_PLAY],FALSE);
		gtk_widget_set_sensitive(menuitem[RIGHT_ADDTOQUEUE],FALSE);
		gtk_widget_set_sensitive(menuitem[RIGHT_ADDTOPLAYLIST],FALSE);
		gtk_widget_set_sensitive(menuitem[RIGHT_INFORMATION],FALSE);
		gtk_widget_set_sensitive(menuitem[RIGHT_DELETE],FALSE);
		gtk_widget_set_sensitive(menuitem[RIGHT_DELETEALL],FALSE);
	}
	else if(g_list_length(selectlist)>1)
	{
		gtk_widget_set_sensitive(menuitem[RIGHT_PLAY],FALSE);
	}
	if(index0==2)
	{
		gtk_widget_set_sensitive(menuitem[RIGHT_DELETEALL],FALSE);
	}
	gtk_widget_show_all(menu);
	gtk_menu_popup (GTK_MENU(menu),NULL, NULL, NULL, NULL,event->button, event->time);
	return FALSE;
}


gboolean ly_3opc_right_on_active_cb(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data)
{
	ly_3opc_right_play_cb(NULL, NULL);
	return FALSE;
}


gboolean ly_3opc_right_play_cb(GtkWidget *widget, gpointer data)
{
	gchar tmp[10240]="";
	gchar *tmpwhere=NULL;
	gchar *where=NULL;
	int id=0;
	int pid=0;
	GtkTreeIter iter;
	
	int index0=0;
	ly_reg_get("3opc_select", "%d:%*d:%d:%*s", &index0, &pid);
	if(index0==1||index0==2)
	{
		ly_pqm_clear_md();
		if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter))
		{
			gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &id, -1);
			g_snprintf(tmp, sizeof(tmp), "id=%d", id);
			where=g_strconcat(tmp, NULL);
		}
		while(gtk_tree_model_iter_next(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter))
		{
			gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &id, -1);
			g_snprintf(tmp, sizeof(tmp), " OR id=%d", id);
			tmpwhere=g_strconcat(where, tmp, NULL);
			g_free(where);
			where=tmpwhere;
		}
		if(index0==2)
		{
			ly_pqm_add_md_by_where_from_lib(where);
		}
		else if(index0==1)
		{
			ly_pqm_add_md_by_where_from_plm(pid, where);
		}
	}

	GList *list=NULL;
	list=gtk_tree_selection_get_selected_rows(ly_3opc_right_selection_right, NULL);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, (GtkTreePath *)(list->data));
	g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (list);
	gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &id, -1);
	ly_aud_stop();
	ly_pqm_set_current_md(id);
	ly_aud_play();
	return FALSE;
}

gboolean ly_3opc_right_addtoqueue_cb(GtkWidget *widget, gpointer data)
{
	gchar *where=NULL;
	gchar *tmpwhere=NULL;
	int id=0;
	int pid=0;
	GtkTreeIter iter;
	gchar tmp[1024]="";
	GList *list=NULL;
	GList *p=NULL;
	
	int index0=0;
	ly_reg_get("3opc_select", "%d:%*d:%d:%*s", &index0, &pid);
	if(index0==1||index0==2)
	{
		list=gtk_tree_selection_get_selected_rows(ly_3opc_right_selection_right, NULL);
		p=list;
		while(p)
		{
			gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, (GtkTreePath *)(p->data));
			gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &id, -1);
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
		if(index0==2)
		{
			ly_pqm_add_md_by_where_from_lib(where);
		}
		else if(index0==1)
		{
			ly_pqm_add_md_by_where_from_plm(pid, where);
		}
		g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (list);
	}
	return FALSE;
}


gboolean ly_3opc_right_addtoplaylist_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	gchar sql[10240]="";
	gchar tmp[10240]="";
	int pid=(int)data;
	int mid=0;
	GList *list=NULL;
	GList *p=NULL;
	GtkTreeIter iter;
	
	if(pid<=0)
	{
		const gchar *name=NULL;
		
		GtkWidget *dialog;
		GtkWidget *hbox;
		GtkWidget *label;
		GtkWidget *entry;
		int result;
		dialog=gtk_dialog_new_with_buttons(_("New Playlist"),
						   GTK_WINDOW(ly_win_get_window()->win),
						   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				      GTK_STOCK_OK,
				      GTK_RESPONSE_ACCEPT,
				      GTK_STOCK_CANCEL,
				      GTK_RESPONSE_REJECT,
				      NULL);
		gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
		hbox=gtk_hbox_new(FALSE,0);
		gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),hbox,TRUE,TRUE,0);
		label=gtk_label_new(_("Name"));
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		entry=gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,0);
		gtk_widget_show_all(dialog);
		result=gtk_dialog_run(GTK_DIALOG(dialog));
		switch(result)
		{
			case GTK_RESPONSE_ACCEPT:
				break;
			default:
				gtk_widget_destroy(dialog);
				return FALSE;
				break;
		}
		name=gtk_entry_get_text(GTK_ENTRY(entry));
		if(name&&!g_str_equal(name,""))
		{
			g_strlcpy(tmp, name, sizeof(tmp));
			ly_dbm_replace_str(tmp, sizeof(tmp));
			ly_plm_add_pl(tmp);
			pid=ly_dbm_get_last_insert_rowid();
		}
		gtk_widget_destroy(dialog);
	}
	list=gtk_tree_selection_get_selected_rows(ly_3opc_right_selection_right, NULL);
	if(list==NULL)
		return FALSE;
	p=list;
	ly_dbm_exec("begin",NULL,NULL);
	while(p)
	{
		gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, (GtkTreePath *)(p->data));
		gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &mid, -1);
		ly_plm_add_md(pid, mid);
		p=p->next;
	}
	g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (list);
	ly_dbm_exec("commit",NULL,NULL);
	
	return FALSE;
}

gboolean ly_3opc_right_information_cb(GtkWidget *widget, gpointer data)
{
	GList *list;
	GtkTreeIter iter;
	gint id=0;
	LyMdhMetadata *md=NULL;
	
	list=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(ly_3opc_right_selection_right), NULL);
	if(g_list_length(list)==0)
		return FALSE;
	gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, (GtkTreePath *)(list->data));
	gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &id, -1);
	md=ly_lib_get_md(id);
	
	if(md->flag==0)
	{
		LyMdhMetadata *md0=NULL;
		md0=ly_mdh_new_with_uri_full(md->uri);
		md0->id=md->id;
		ly_mdh_free(md);
		md=md0;
	}

	//build dialog
	enum{
		INFO_TITLE,
		INFO_ARTIST,
		INFO_ALBUM,
		INFO_GENRE,
		INFO_TRACK,
		INFO_NUM
	};
	GtkWidget *dialog;
	GtkWidget *frame;
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *entry[INFO_NUM];
	GtkWidget *image;
	char str[1024]="";
	
	GFile *file=g_file_new_for_uri(md->uri);
	GFileInfo *info=g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_SIZE, G_FILE_QUERY_INFO_NONE, FALSE, NULL);
	guint64 size=g_file_info_get_size(info);
	if(md->flag==0 && size>1024*1024*50)
	{
		dialog=gtk_dialog_new_with_buttons(	_("Information"),
						GTK_WINDOW(ly_win_get_window()->win),
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				     GTK_STOCK_CANCEL,
				     GTK_RESPONSE_REJECT,
				     NULL);
	}
	else
	{
		dialog=gtk_dialog_new_with_buttons(	_("Information"),
						GTK_WINDOW(ly_win_get_window()->win),
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
//				     GTK_STOCK_SAVE,
//				     GTK_RESPONSE_ACCEPT,
				     GTK_STOCK_CANCEL,
				     GTK_RESPONSE_REJECT,
				     NULL);
	}
	gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
	gtk_window_set_default_size(GTK_WINDOW(dialog),400,350);

	
	frame=gtk_frame_new(_("Basic Information"));
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),frame,TRUE,TRUE,0);
	vbox=gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame),vbox);
	hbox=gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	table=gtk_table_new(4,2,FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(table),5);
	gtk_box_pack_start(GTK_BOX(hbox),table,TRUE,TRUE,0);
	label=gtk_label_new(_("Start"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Duration"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Bitrate"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Codec"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(md->start);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(md->duration);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	g_snprintf(str, sizeof(str), "%d kbps", md->bitrate/1024);
	label=gtk_label_new(str);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(md->codec);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 5);

	GdkPixbuf *pixbuf=NULL;
	GdkPixbuf *pixbuf_old=NULL;
	char path[1024]="";
	gint i=0;
	for(i=0; i<COVER_TYPE_COUNT; i++)
	{
		g_snprintf(path, sizeof(path), "%sinf/%s-%s.%s",LY_GLB_USER_PIXDIR, md->artist, md->album, cover_type[i]);
		if(g_file_test(path, G_FILE_TEST_EXISTS))
		{
			pixbuf=gdk_pixbuf_new_from_file_at_scale(path, 120, 120, TRUE, NULL);
			if(pixbuf)
				break;
		}
	}
	if(!pixbuf)
	{
		GstBuffer *buf=md->cover;
		if(buf)
		{
			GdkPixbufLoader *loader;
			GdkPixbuf *pixbuf_old;
			loader = gdk_pixbuf_loader_new();
    		if(!gdk_pixbuf_loader_write(loader, buf->data, buf->size, NULL))
			{
				g_object_unref(loader);
				return FALSE;
			}
			pixbuf_old = gdk_pixbuf_loader_get_pixbuf(loader);
			pixbuf=gdk_pixbuf_scale_simple(pixbuf_old, 120, 120, GDK_INTERP_BILINEAR);
			if(pixbuf_old)
				g_object_ref(pixbuf_old);
			gdk_pixbuf_loader_close(loader, NULL);
			g_object_unref(loader);
		}
		else
		{
			g_snprintf(path, sizeof(path), "%sicon/default_album.png", LY_GLB_PROG_UIXDIR);
			pixbuf=gdk_pixbuf_new_from_file_at_scale(path, 120, 120, TRUE, NULL);
		}
	}
	if(pixbuf)
	{
		image=gtk_image_new_from_pixbuf(pixbuf);
		gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
		g_object_unref(pixbuf);
	}

	hbox=gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	label=gtk_label_new(_("Uri"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	label=gtk_label_new(md->uri);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_MIDDLE);
	gtk_label_set_max_width_chars(GTK_LABEL(label),30);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	

	frame=gtk_frame_new(_("Advanced Information"));
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),frame,TRUE,TRUE,0);
	table=gtk_table_new(5,2,FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(table),5);
	gtk_container_add(GTK_CONTAINER(frame),table);
	label=gtk_label_new(_("Title"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Artist"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Album"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("genre"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("track"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5, GTK_FILL, GTK_FILL, 5, 5);
	entry[INFO_TITLE]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_TITLE]),md->title);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_TITLE], 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
//	g_signal_connect(G_OBJECT(entry[INFO_TITLE]),"changed",G_CALLBACK(gui_treeview_callback_set_changed3),&changed);
	entry[INFO_ARTIST]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_ARTIST]),md->artist);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_ARTIST], 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
//	g_signal_connect(G_OBJECT(entry[INFO_ARTIST]),"changed",G_CALLBACK(gui_treeview_callback_set_changed1),&changed);
	entry[INFO_ALBUM]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_ALBUM]),md->album);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_ALBUM], 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
//	g_signal_connect(G_OBJECT(entry[INFO_ALBUM]),"changed",G_CALLBACK(gui_treeview_callback_set_changed2),&changed);
	entry[INFO_GENRE]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_GENRE]), md->genre);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_GENRE], 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
//	g_signal_connect(G_OBJECT(entry[INFO_GENRE]),"changed",G_CALLBACK(gui_treeview_callback_set_changed3),&changed);
	entry[INFO_TRACK]=gtk_entry_new();
	g_snprintf(str, sizeof(str), "%d", md->track);
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_TRACK]),str);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_TRACK], 1, 2, 4, 5, GTK_FILL, GTK_FILL, 5, 5);
//	g_signal_connect(G_OBJECT(entry[INFO_TRACK]),"changed",G_CALLBACK(gui_treeview_callback_set_changed3),&changed);
	
	gtk_widget_show_all(dialog);
	gint rt = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (rt)
	{
		case GTK_RESPONSE_ACCEPT:
			break;
		case GTK_RESPONSE_REJECT:
			gtk_widget_destroy (dialog);
			ly_mdh_free(md);
			return FALSE;
			break;
		default:
			gtk_widget_destroy (dialog);
			ly_mdh_free(md);
			return FALSE;
			break;
	}
	g_snprintf(md->title, sizeof(md->title), gtk_entry_get_text(GTK_ENTRY(entry[INFO_TITLE])));
	g_snprintf(md->artist, sizeof(md->artist), gtk_entry_get_text(GTK_ENTRY(entry[INFO_ARTIST])));
	g_snprintf(md->album, sizeof(md->album), gtk_entry_get_text(GTK_ENTRY(entry[INFO_ALBUM])));
	g_snprintf(md->genre, sizeof(md->genre), gtk_entry_get_text(GTK_ENTRY(entry[INFO_GENRE])));
	sscanf(gtk_entry_get_text(GTK_ENTRY(entry[INFO_TITLE])), "%d", &md->track);
	ly_lib_update_md(md);
	if(md->flag==0)
	{
		ly_mdh_push(md);
	}
	gtk_widget_destroy (dialog);
	ly_mdh_free(md);
	ly_3opc_right_refresh();
	return FALSE;
}


gboolean ly_3opc_right_addfiles_cb(GtkWidget *widget, gpointer data)
{
	GSList *filelist;
	GtkFileFilter *filter;
	GtkWidget *dialog;
	
	dialog =gtk_file_chooser_dialog_new(	_("Add From File..."),
						GTK_WINDOW(ly_win_get_window()->win),
						GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
				      NULL);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER (dialog),TRUE);
	filter=gtk_file_filter_new();
	gtk_file_filter_set_name(filter,_("Music File"));
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.mp3");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.MP3");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.flac");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.FLAC");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.cda");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.CDA");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.mid");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.MID");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.midi");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.MIDI");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.ogg");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.OGG");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.wav");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.WAV");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.acc");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.ACC");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.ape");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.APE");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog),GTK_FILE_FILTER(filter));
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
	g_thread_create(ly_3opc_right_addfiles_cb_cb, filelist, FALSE, NULL);
	gtk_widget_destroy (dialog);
	return FALSE;
}
gpointer ly_3opc_right_addfiles_cb_cb(gpointer data)
{
	GSList *filelist=(GSList*)data;
	GSList *q=filelist;
	int index0=0;
	int pid=0;
	int mid=0;
	gchar *filename,*fileuri, tmp[1024];
	LyMdhMetadata *md;
	
	ly_reg_get("3opc_select", "%d:%*d:%d:%*s", &index0, &pid);
	
	ly_dbm_exec("begin",NULL,NULL);
	if(index0==0)
	{
		while(q)
		{
			filename=g_filename_from_uri(q->data,NULL,NULL);
			realpath(filename,tmp);
			fileuri=g_strconcat("file://",tmp, NULL);
			g_free(filename);
			
			md=ly_mdh_new_with_uri(fileuri);
			if(md)
			{
				mid=ly_lib_get_id(md);
				if(mid<0)
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
	}
	else if(index0==1&& pid>0)
	{
		while(q)
		{
			filename=g_filename_from_uri(q->data,NULL,NULL);
			realpath(filename,tmp);
			fileuri=g_strconcat("file://",tmp, NULL);
			g_free(filename);
			
			md=ly_mdh_new_with_uri(fileuri);
			if(md)
			{
				mid=ly_lib_get_id(md);
				if(mid<0)
				{
					ly_lib_add_md(md);
					mid=ly_lib_get_id(md);
				}
				if(mid>0)
					ly_plm_add_md(pid, mid);
				ly_mdh_free(md);
			}
			q=q->next;
		}
	}
	else if(index0==2)
	{
		while(q)
		{
			filename=g_filename_from_uri(q->data,NULL,NULL);
			realpath(filename,tmp);
			fileuri=g_strconcat("file://",tmp, NULL);
			g_free(filename);
			
			md=ly_mdh_new_with_uri(fileuri);
			if(md)
			{
				ly_lib_add_md(md);
				ly_mdh_free(md);
			}
			q=q->next;
		}
	}
	
	ly_dbm_exec("commit",NULL,NULL);
	g_slist_free(filelist);
	
	ly_3opc_left_refresh_cb(NULL,NULL);
	ly_3opc_right_refresh_cb(NULL,NULL);
	return NULL;
}



gboolean ly_3opc_right_refresh_cb(GtkWidget *widget, gpointer data)
{
	ly_3opc_right_refresh();
	return FALSE;
}


GtkWidget* ly_3opc_warning_dialog_create(gchar *str)
{
	GtkWidget *dialog=NULL;
	GtkWidget *hbox=NULL;
	GtkWidget *image=NULL;
	GtkWidget *label=NULL;
	dialog=gtk_dialog_new_with_buttons(	_("Warning"),
						GTK_WINDOW(ly_win_get_window()->win),
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				     GTK_STOCK_OK,
				     GTK_RESPONSE_ACCEPT,
				     GTK_STOCK_CANCEL,
				     GTK_RESPONSE_REJECT,
				     NULL);
	gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),hbox,TRUE,TRUE,0);
	image=gtk_image_new_from_stock(GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_DIALOG);
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), str);
	gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,0);
	gtk_widget_show_all(dialog);
	return dialog;
}


gboolean ly_3opc_right_delete_cb(GtkWidget *widget, gpointer data)
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
	
	list=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(ly_3opc_right_selection_right),NULL);
	if(list==NULL)
		return FALSE;
	
	int index0=0;
	int index1=0;
	int pid=0;
	ly_reg_get("3opc_select", "%d:%d:%d:%*[^\n]s", &index0, &index1, &pid);
	
	if(index0==2)
	{
		dialog=ly_3opc_warning_dialog_create(_("<b>This is a Dangerous function!!</b>\n The music you wanna delete is belongs to your music library,\n It will be delete physically and permanently.\n Do you really want to do it?"));
		result=gtk_dialog_run(GTK_DIALOG(dialog));
		switch(result)
		{
			case GTK_RESPONSE_ACCEPT:
				break;
			default:
				gtk_widget_destroy(dialog);
				return FALSE;
				break;
		}
		gtk_widget_destroy(dialog);
		list=gtk_tree_selection_get_selected_rows(ly_3opc_right_selection_right, NULL);
		p=list;
		while(p)
		{
			gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, (GtkTreePath *)(p->data));
			gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &id, -1);
			if(p==list)
			{
				g_snprintf(tmp, sizeof(tmp), "id=%d", id);
				where=g_strconcat(tmp, NULL);
				ly_lib_del_md_from_disk(id);
			}
			else
			{
				g_snprintf(tmp, sizeof(tmp), " OR id=%d", id);
				tmpwhere=g_strconcat(where, tmp, NULL);
				g_free(where);
				where=tmpwhere;
				ly_lib_del_md_from_disk(id);
			}
			p=p->next;
		}
		ly_lib_del_md_by_where(where);
		g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (list);
	}
	else if(index0==1)
	{
		list=gtk_tree_selection_get_selected_rows(ly_3opc_right_selection_right, NULL);
		p=list;
		while(p)
		{
			gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, (GtkTreePath *)(p->data));
			gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &id, -1);
			if(p==list)
			{
				g_snprintf(tmp, sizeof(tmp), "mid=%d", id);
				where=g_strconcat(tmp, NULL);
			}
			else
			{
				g_snprintf(tmp, sizeof(tmp), " OR mid=%d", id);
				tmpwhere=g_strconcat(where, tmp, NULL);
				g_free(where);
				where=tmpwhere;
			}
			p=p->next;
		}
		ly_plm_del_md_by_where(pid, where);
		g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (list);
	}
	else if(index0==0)
	{
		list=gtk_tree_selection_get_selected_rows(ly_3opc_right_selection_right, NULL);
		p=list;
		while(p)
		{
			gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, (GtkTreePath *)(p->data));
			gtk_tree_model_get(GTK_TREE_MODEL(ly_3opc_right_store_right), &iter, 4, &id, -1);
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
		g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (list);
	}
	ly_3opc_right_refresh();
	return FALSE;
}


gboolean ly_3opc_right_deleteall_cb(GtkWidget *widget, gpointer data)
{	
	GtkWidget *dialog;
	gint result;
	
	int index0=0;
	int index1=0;
	int pid=0;
	ly_reg_get("3opc_select", "%d:%d:%d:%*[^\n]s", &index0, &index1, &pid);
	
	dialog=ly_3opc_warning_dialog_create(_("<b>This is a Dangerous function!!</b>\n Do you really want to DELETE ALL information belongs the current list?"));
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	switch(result)
	{
		case GTK_RESPONSE_ACCEPT:
			break;
		default:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
	}
	gtk_widget_destroy(dialog);
	

	if(index0==1)
	{
		ly_plm_clear_md(pid);
	}
	else if(index0==0)
	{
		ly_pqm_clear_md();
	}
	ly_3opc_right_refresh();
	return FALSE;
}

gboolean ly_3opc_right_on_button_p_clicked_cb(GtkWidget *widget, gpointer data)
{
	gint limit=-1;
	gint offset=0;
	
	ly_reg_get("3opc_limit", "%d:%d", &offset, &limit);
	limit=limit>=-1?limit:-1;
	if(limit<0)
		return FALSE;
	
	offset=offset-limit>=0?offset-limit:0;
	ly_reg_set("3opc_limit", "%d:%d", offset, limit);
}
gboolean ly_3opc_right_on_button_n_clicked_cb(GtkWidget *widget, gpointer data)
{
	gint limit=-1;
	gint offset=0;
	
	ly_reg_get("3opc_limit", "%d:%d", &offset, &limit);
	limit=limit>=-1?limit:-1;
	if(limit<0)
		return FALSE;
	offset=offset>=0?offset+limit:limit;
	ly_reg_set("3opc_limit", "%d:%d", offset, limit);
}
