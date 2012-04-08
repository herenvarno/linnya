/*
 * rdo.c
 * This file is part of linnya:plugin
 *
 * Copyright (C) 2012 - Edward Yang
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
#include "rdo.h"

/*
 * TYPES
 */
enum{
	RIGHT_PLAY,
	RIGHT_ADD,
	RIGHT_INFORMATION,
	RIGHT_REFRESH,
	RIGHT_DELETE,
	RIGHT_NUM
};

/*
 * VARIABLES
 */
GtkWidget *ly_3rdo_widget=NULL;
char *ly_3rdo_str_old=NULL;
GdkPixbuf *icon_playing=NULL;

/*
 * FUNCTIONS
 */
void ly_3rdo_refresh();
gboolean ly_3rdo_on_get_metadatas_cb(gpointer stmt, gpointer data);
gboolean ly_3rdo_on_meta_changed_cb(gpointer message, gpointer data);
gboolean ly_3rdo_mask_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_3rdo_popup_menu_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_3rdo_on_active_cb(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data);
gboolean ly_3rdo_play_cb(GtkWidget *widget, gpointer data);
gboolean ly_3rdo_add_cb(GtkWidget *widget, gpointer data);
gboolean ly_3rdo_refresh_cb(GtkWidget *widget, gpointer data);
gboolean ly_3rdo_delete_cb(GtkWidget *widget, gpointer data);

const gchar* g_module_check_init(GModule *module)
{
	char path[1024]="";
	g_snprintf(path, sizeof(path), "%srdo/playing.svg", LY_GLB_PROG_PLUGINDIR);
	icon_playing=gdk_pixbuf_new_from_file_at_size(path, 20, 20, NULL);
	return NULL;
}
void g_module_unload(GModule *module)
{
	if(icon_playing)
		g_object_unref(icon_playing);
}

GtkWidget *ly_3rdo_create()
{
	GtkWidget *widget=NULL;
	GtkWidget *scrolled_window=NULL;
	GtkWidget *treeview=NULL;
	GtkTreeSelection *selection=NULL;
	GtkTreeStore *store=NULL;

	widget=gtk_vbox_new(FALSE, 0);
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(widget), scrolled_window,TRUE,TRUE,0);
	
	treeview=gtk_tree_view_new();
	ly_3rdo_widget=treeview;
	gtk_container_add(GTK_CONTAINER(scrolled_window),treeview);
	store=gtk_tree_store_new(5, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
	gtk_tree_view_set_model(GTK_TREE_VIEW (treeview), GTK_TREE_MODEL(store));
	char sql[1024]="SELECT * FROM metadatas WHERE flag=100 ORDER BY num";
	ly_dbm_exec(sql, ly_3rdo_on_get_metadatas_cb, NULL);

	GtkTreeViewColumn *column;
	GtkCellRenderer *cell_renderer;
	cell_renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes(_("*"),cell_renderer,"pixbuf",0,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Chanel"),cell_renderer,"text",1,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),100);
	gtk_tree_view_column_set_max_width  (GTK_TREE_VIEW_COLUMN(column),200);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("By"),cell_renderer,"text",2,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),100);
	gtk_tree_view_column_set_max_width  (GTK_TREE_VIEW_COLUMN(column),250);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Category"),cell_renderer,"text",3,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),100);
	gtk_tree_view_column_set_max_width  (GTK_TREE_VIEW_COLUMN(column),200);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(selection,GTK_SELECTION_MULTIPLE);
	
	g_signal_connect(G_OBJECT(treeview), "row-activated", G_CALLBACK(ly_3rdo_on_active_cb), NULL);
	g_signal_connect(G_OBJECT(treeview), "button_release_event", G_CALLBACK(ly_3rdo_popup_menu_cb), NULL);
	g_signal_connect(G_OBJECT(treeview), "button_press_event", G_CALLBACK(ly_3rdo_mask_cb), NULL);
	ly_msg_bind("meta_changed", "core:pqm", ly_3rdo_on_meta_changed_cb, NULL);

	gtk_widget_show_all(widget);
	return widget;
}

void ly_3rdo_destroy()
{
	if(ly_3rdo_str_old)
		g_free(ly_3rdo_str_old);
	ly_3rdo_str_old=NULL;
	ly_msg_unbind("meta_changed", "core:pqm", ly_3rdo_on_meta_changed_cb);
}



gboolean ly_3rdo_on_get_metadatas_cb(gpointer stmt, gpointer data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	gint id=0;
	gchar title[128]="";
	gchar artist[128]="";
	gchar album[128]="";
	model=gtk_tree_view_get_model(GTK_TREE_VIEW(ly_3rdo_widget));
	gtk_tree_store_append (GTK_TREE_STORE(model) , &iter, NULL);
	id=sqlite3_column_int(stmt, 0);
	g_strlcpy(title,(const gchar *)sqlite3_column_text(stmt, 1), sizeof(title));
	g_strlcpy(artist,(const gchar *)sqlite3_column_text(stmt, 2),sizeof(artist));
	g_strlcpy(album,(const gchar *)sqlite3_column_text(stmt, 3), sizeof(album));
	LyMdhMetadata *md=NULL;
	md=ly_pqm_get_current_md();
	if(!md)
	{
		gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, NULL, 1,title, 2,artist, 3,album, 4, id, -1);
		return FALSE;
	}
	if(md->id==id)
	{
		if(ly_3rdo_str_old)
			g_free(ly_3rdo_str_old);
		gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, icon_playing, 1,title, 2,artist, 3,album, 4, id, -1);
		ly_3rdo_str_old=gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(model), &iter);
	}
	else
		gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, NULL, 1,title, 2,artist, 3,album, 4, id, -1);
	return FALSE;
}

gboolean ly_3rdo_on_meta_changed_cb(gpointer message, gpointer data)
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;

	int id;
	GtkTreeModel *model;
	GtkTreeIter iter;
	model=gtk_tree_view_get_model(GTK_TREE_VIEW(ly_3rdo_widget));
	
	if(ly_3rdo_str_old)
	{
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(model), &iter, ly_3rdo_str_old);
		if(gtk_tree_store_iter_is_valid(GTK_TREE_STORE(model), &iter))
		{
			gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, NULL, -1);
		}
		g_free(ly_3rdo_str_old);
		ly_3rdo_str_old=NULL;
	}
	
	if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &iter))
	{
		return FALSE;
	}
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 4, &id, -1);
	if(id==md->id)
	{
		ly_3rdo_str_old=gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(model), &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, icon_playing, -1);
		return FALSE;
	}
	else
	{
		while(gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &iter))
		{
			gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 4, &id, -1);
			if(id==md->id)
			{
				ly_3rdo_str_old=gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(model), &iter);
				gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, icon_playing, -1);
				return FALSE;
			}
		}
	}
	return FALSE;
}






gboolean ly_3rdo_mask_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button==3)
	{
		GList *selectlist=gtk_tree_selection_get_selected_rows(gtk_tree_view_get_selection(GTK_TREE_VIEW(ly_3rdo_widget)),NULL);
		if (g_list_length(selectlist)>1)
			return TRUE;
	}
	return FALSE;
}

gboolean ly_3rdo_popup_menu_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button!=3) 
	{
		return FALSE;
	}
	
	char *menu_right[RIGHT_NUM]={
		_("Play"),
		_("Add"),
		_("Information"),
		_("Refresh"),
		_("Delete")
	};
	
	GtkWidget	*menu=NULL;
	GtkWidget	*menuitem[RIGHT_NUM];
	
	GList *selectlist;
	gint i=0;
	
	menu=gtk_menu_new();
	for(i=0;i<RIGHT_NUM;i++)
	{
		menuitem[i]=gtk_menu_item_new_with_label(menu_right[i]);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem[i]);
	}
	g_signal_connect(G_OBJECT(menuitem[RIGHT_PLAY]), "activate", G_CALLBACK(ly_3rdo_play_cb),NULL);
	g_signal_connect(G_OBJECT(menuitem[RIGHT_ADD]), "activate", G_CALLBACK(ly_3rdo_add_cb),NULL);
//	g_signal_connect(G_OBJECT(menuitem[RIGHT_INFORMATION]), "activate", G_CALLBACK(ly_3rdo_information_cb),NULL);
	g_signal_connect(G_OBJECT(menuitem[RIGHT_REFRESH]), "activate", G_CALLBACK(ly_3rdo_refresh_cb),NULL);
	g_signal_connect(G_OBJECT(menuitem[RIGHT_DELETE]), "activate", G_CALLBACK(ly_3rdo_delete_cb),NULL);

	int index0=0;
	int index1=0;
	int id=0;
	char name[1024]="";
	ly_reg_get("3opc_select", "%d:%d:%d:%[^\n]s", &index0, &index1, &id, name);
	
	selectlist=gtk_tree_selection_get_selected_rows(gtk_tree_view_get_selection(GTK_TREE_VIEW(ly_3rdo_widget)), NULL);
	
	if(g_list_length(selectlist)==0)
	{
		gtk_widget_set_sensitive(menuitem[RIGHT_PLAY],FALSE);
		gtk_widget_set_sensitive(menuitem[RIGHT_INFORMATION],FALSE);
		gtk_widget_set_sensitive(menuitem[RIGHT_DELETE],FALSE);
	}
	else if(g_list_length(selectlist)>1)
	{
		gtk_widget_set_sensitive(menuitem[RIGHT_PLAY],FALSE);
		gtk_widget_set_sensitive(menuitem[RIGHT_INFORMATION],FALSE);
	}
	gtk_widget_show_all(menu);
	gtk_menu_popup (GTK_MENU(menu),NULL, NULL, NULL, NULL,event->button, event->time);
	return FALSE;
}

gboolean ly_3rdo_on_active_cb(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data)
{
	ly_3rdo_play_cb(NULL, NULL);
	return FALSE;
}

gboolean ly_3rdo_play_cb(GtkWidget *widget, gpointer data)
{
	ly_pqm_clear_md();
	ly_pqm_add_md_by_where_from_lib("flag=100");
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	gint id=0;
	model=gtk_tree_view_get_model(GTK_TREE_VIEW(ly_3rdo_widget));
	selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(ly_3rdo_widget));
	GList *list=NULL;
	list=gtk_tree_selection_get_selected_rows(selection, NULL);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, (GtkTreePath *)(list->data));
	g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (list);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 4, &id, -1);
	ly_aud_stop();
	ly_pqm_set_current_md(id);
	ly_aud_play();
	return FALSE;
}

gboolean ly_3rdo_add_cb(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *entry_title;
	GtkWidget *entry_by;
	GtkWidget *entry_category;
	GtkWidget *entry_uri;
	
	dialog=gtk_dialog_new_with_buttons(	_("Add New Radio Station ..."),
						GTK_WINDOW(ly_win_get_window()->win),
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				     GTK_STOCK_SAVE,
				     GTK_RESPONSE_ACCEPT,
				     GTK_STOCK_CANCEL,
				     GTK_RESPONSE_REJECT,
				     NULL);
	table=gtk_table_new(4,2,FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(table),5);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),table,TRUE,TRUE,0);
	label=gtk_label_new(_("Title:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("By:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Category:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Uri:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	entry_title=gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table), entry_title, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	entry_by=gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table), entry_by, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	entry_category=gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table), entry_category, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	entry_uri=gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table), entry_uri, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	
	gtk_widget_show_all(dialog);
	gint rt = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (rt)
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
	
	LyMdhMetadata *md=ly_mdh_new();
	char title[1024]="";
	char by[1024]="";
	char category[1024]="";
	char uri[1024]="";
	g_strlcpy(md->title, gtk_entry_get_text(GTK_ENTRY(entry_title)), sizeof(md->title));
	g_strlcpy(md->artist, gtk_entry_get_text(GTK_ENTRY(entry_by)), sizeof(md->artist));
	g_strlcpy(md->album, gtk_entry_get_text(GTK_ENTRY(entry_category)), sizeof(md->album));
	g_strlcpy(md->uri, gtk_entry_get_text(GTK_ENTRY(entry_uri)), sizeof(md->uri));
	gtk_widget_destroy(dialog);
	if(g_str_equal(md->title, "") || g_str_equal(md->uri, ""))
	{
		ly_msg_put("error", "pli:rdo", "Title or Uri cannot be NULL! Failed to add a radio station!");
		return FALSE;
	}
	if(g_str_equal(md->artist, ""))
		g_strlcpy(md->artist, "unknown", sizeof(md->artist));
	if(g_str_equal(md->album, ""))
		g_strlcpy(md->album, "unknown", sizeof(md->album));
	ly_dbm_replace_str(md->title, sizeof(md->title));
	ly_dbm_replace_str(md->artist, sizeof(md->artist));
	ly_dbm_replace_str(md->album, sizeof(md->album));
	ly_dbm_replace_str(md->uri, sizeof(md->uri));
	md->flag=100;
	ly_lib_add_md(md);
	ly_mdh_free(md);
	ly_3rdo_refresh();
	return FALSE;
}


gboolean ly_3rdo_delete_cb(GtkWidget *widget, gpointer data)
{
	GList *list;
	GList *p;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreeSelection *selection;
	GtkWidget *dialog;
	gint result;
	
	gint id;
	gchar *where=NULL;
	gchar *tmpwhere=NULL;
	gchar tmp[1024]="";
	
	model=gtk_tree_view_get_model(GTK_TREE_VIEW(ly_3rdo_widget));
	selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(ly_3rdo_widget));
	
	list=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(selection),NULL);
	if(list==NULL)
		return FALSE;
	
	list=gtk_tree_selection_get_selected_rows(selection, NULL);
	p=list;
	while(p)
	{
		gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, (GtkTreePath *)(p->data));
		gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 4, &id, -1);
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
	ly_3rdo_refresh();
	return FALSE;
}

gboolean ly_3rdo_refresh_cb(GtkWidget *widget, gpointer data)
{
	ly_3rdo_refresh();
	return FALSE;
}

void		ly_3rdo_refresh	()
{
	GtkTreeStore *store = gtk_tree_store_new (5,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING, G_TYPE_INT);
	gtk_tree_view_set_model(GTK_TREE_VIEW (ly_3rdo_widget), GTK_TREE_MODEL(store));
	char sql[1024]="SELECT * FROM metadatas WHERE flag=100 ORDER BY num";
	ly_dbm_exec(sql, ly_3rdo_on_get_metadatas_cb, NULL);
	
}
