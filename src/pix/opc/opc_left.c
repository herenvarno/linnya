/*
 * opc_left.c
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
#include "opc_left.h"

/*
 * TYPES
 */
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

/*
 * VARIABLES
 */
GtkWidget			*treeview_left=NULL;
GtkTreeStore		*store_left=NULL;
GtkTreeSelection	*selection_left=NULL;

/*
 * FUNCTIONS
 */
gboolean ly_3opc_left_on_get_artists_cb(gpointer stmt, gpointer data);
gboolean ly_3opc_left_on_get_playlists_cb(gpointer stmt, gpointer data);
gboolean ly_3opc_left_on_changed_cb(GtkWidget *widget, gpointer data);
void ly_3opc_left_on_plm_update_cb(LyMbsMessage *message, gpointer data);
gboolean ly_3opc_left_mask_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_3opc_left_popup_menu_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_3opc_left_add_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_left_import_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_left_addtoqueue_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_left_rename_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_left_export_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_left_refresh_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_left_delete_cb(GtkWidget *widget, gpointer data);
gboolean ly_3opc_left_deleteall_cb(GtkWidget *widget, gpointer data);
GtkWidget* ly_3opc_left_warning_dialog_create(gchar *str);

void		ly_3opc_left_init		()
{

}
void		ly_3opc_left_fina		()
{

}

GtkWidget*	ly_3opc_left_create		()
{
	GtkWidget *vbox;
	vbox=gtk_vbox_new(FALSE,0);
	
	GtkWidget *scrolled_window;
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window,TRUE,TRUE,0);
	treeview_left=gtk_tree_view_new();
	gtk_container_add(GTK_CONTAINER(scrolled_window),treeview_left);
	
	ly_3opc_left_refresh();
	
	GtkTreeViewColumn *column;
	GtkCellRenderer *cell_renderer;
	column = gtk_tree_view_column_new();
	cell_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, cell_renderer,FALSE);
	gtk_tree_view_column_add_attribute(column,cell_renderer,"text", 1);
	gtk_tree_view_column_set_title(column,_("Manager"));
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),100);
	gtk_tree_view_column_set_max_width  (GTK_TREE_VIEW_COLUMN(column),200);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview_left), column);
	selection_left=gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_left));

	g_signal_connect(G_OBJECT(treeview_left), "button_release_event", G_CALLBACK(ly_3opc_left_popup_menu_cb), NULL);
	g_signal_connect(G_OBJECT(treeview_left), "button_press_event", G_CALLBACK(ly_3opc_left_mask_cb), NULL);
	g_signal_connect(G_OBJECT(selection_left), "changed", G_CALLBACK(ly_3opc_left_on_changed_cb), NULL);
	
	ly_mbs_bind("plm_update", "core:plm", ly_3opc_left_on_plm_update_cb, NULL);
	
	return vbox;

}
void		ly_3opc_left_destroy	()
{
	ly_mbs_unbind("plm_update", "core:plm", ly_3opc_left_on_plm_update_cb);
}
void		ly_3opc_left_refresh	()
{
	if(store_left)
		g_object_unref(store_left);
	store_left=NULL;
	
	GtkTreeIter iter;
	store_left = gtk_tree_store_new (2, G_TYPE_INT, G_TYPE_STRING);
	
	gtk_tree_store_append(store_left, &iter,NULL);
	gtk_tree_store_set(store_left, &iter, 0, -1, 1, _("Playing Queue"), -1);
	
	gtk_tree_store_append(store_left, &iter,NULL);
	gtk_tree_store_set(store_left, &iter, 0, -1, 1, _("Playlists"), -1);
	ly_dbm_exec("SELECT * FROM playlists ORDER BY num", ly_3opc_left_on_get_playlists_cb, &iter);

	gtk_tree_store_append (store_left, &iter,NULL);
	gtk_tree_store_set(store_left, &iter, 0, -1, 1, _("Artists"), -1);
	ly_dbm_exec("SELECT DISTINCT artist FROM metadatas WHERE flag>=0 AND flag<=9 ORDER BY artist", ly_3opc_left_on_get_artists_cb, &iter);
	gtk_tree_view_set_model(GTK_TREE_VIEW (treeview_left), GTK_TREE_MODEL(store_left));
	return;
}

gboolean ly_3opc_left_on_get_artists_cb(gpointer stmt, gpointer data)
{
	GtkTreeIter *iter=data;
	GtkTreeIter subiter;
	gchar *name=(gchar *)g_malloc(128*sizeof(gchar));
	gtk_tree_store_append (store_left,&subiter, iter);
	g_strlcpy(name,(const gchar *)sqlite3_column_text(stmt, 0), 128);
	gtk_tree_store_set(store_left, &subiter,0, 1, 1, name, -1);
	return FALSE;
}

gboolean ly_3opc_left_on_get_playlists_cb(gpointer stmt, gpointer data)
{
	GtkTreeIter *iter=data;
	GtkTreeIter subiter;
	gchar *name=(gchar *)g_malloc(128*sizeof(gchar));
	gint id=0;
	gtk_tree_store_append (store_left,&subiter, iter);
	id=sqlite3_column_int(stmt, 0);
	g_strlcpy(name,(const gchar *)sqlite3_column_text(stmt, 1),128);
	gtk_tree_store_set(store_left, &subiter,0, id, 1, name, -1);
	return FALSE;
}

gboolean ly_3opc_left_on_changed_cb(GtkWidget *widget, gpointer data)
{
	GList *selectlist=NULL;
	GtkTreeModel *model=GTK_TREE_MODEL(store_left);
	gint *index;

	selectlist=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(selection_left), NULL);
	if(g_list_length(selectlist)<1)
		return FALSE;
	if(gtk_tree_path_get_depth((GtkTreePath*)(selectlist->data))<1)
		return FALSE;

	index=gtk_tree_path_get_indices((GtkTreePath*)(selectlist->data));
	
	gchar *str;
	gint id=0;
	GtkTreeIter iter;
	gtk_tree_selection_get_selected(selection_left, NULL, &iter);
	gtk_tree_model_get(model,&iter,0,&id,1,&str,-1);
	gchar name[1024]="";
	g_strlcpy(name, str, sizeof(name));
	ly_dbm_replace_str(name, sizeof(name));
	
	ly_reg_set("3opc_select", "%d:%d:%d:%s", index[0], index[1], id, name);

	return FALSE;
}

void
ly_3opc_left_on_plm_update_cb(LyMbsMessage *message, gpointer data)
{
	ly_3opc_left_refresh();
}

gboolean
ly_3opc_left_mask_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button==3)
	{
		GList *selectlist=gtk_tree_selection_get_selected_rows(selection_left,NULL);
		if (g_list_length(selectlist)>1)
			return TRUE;
	}
	return FALSE;
}

gboolean ly_3opc_left_popup_menu_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	gchar *menu_left[LEFT_NUM]={
		_("Add Playlist"),
		_("Import Playlist"),
		_("Add to Play Queue"),
		_("Rename"),
		_("Export Playlist"),
		//_("Sort by Name"),
		_("Refresh"),
		_("Delete"),
		_("Delete All")
	};
	
	GtkWidget	*menu=NULL;
	GtkWidget	*menuitem[LEFT_NUM];
	GtkWidget	*hseparator;
	GList *selectlist;
	gint i=0;
	
	if (event->button==3) 
	{
		menu=gtk_menu_new();
		for(i=0;i<LEFT_NUM;i++)
		{
			if(i==LEFT_ADDTOQUEUE||i==LEFT_EXPORT||i==LEFT_DELETE)
			{
				hseparator=gtk_separator_menu_item_new();
				gtk_menu_shell_append(GTK_MENU_SHELL(menu),hseparator);
			}
			menuitem[i]=gtk_menu_item_new_with_label(menu_left[i]);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem[i]);
		}
		g_signal_connect(G_OBJECT(menuitem[LEFT_ADD]), "activate", G_CALLBACK(ly_3opc_left_add_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_IMPORT]), "activate", G_CALLBACK(ly_3opc_left_import_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_ADDTOQUEUE]), "activate", G_CALLBACK(ly_3opc_left_addtoqueue_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_RENAME]), "activate", G_CALLBACK(ly_3opc_left_rename_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_EXPORT]), "activate", G_CALLBACK(ly_3opc_left_export_cb),NULL);
		//g_signal_connect(G_OBJECT(menuitem[LEFT_SORT]), "activate", G_CALLBACK(gui_treeview_callback_left_sort),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_REFRESH]), "activate", G_CALLBACK(ly_3opc_left_refresh_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_DELETE]), "activate", G_CALLBACK(ly_3opc_left_delete_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_DELETEALL]), "activate", G_CALLBACK(ly_3opc_left_deleteall_cb),NULL);
		

		selectlist=gtk_tree_selection_get_selected_rows(selection_left, NULL);
		if(g_list_length(selectlist)==0)
		{
			gtk_widget_set_sensitive(menuitem[LEFT_ADDTOQUEUE],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_RENAME],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_EXPORT],FALSE);
			//gtk_widget_set_sensitive(menuitem[LEFT_SORT],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETE],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETEALL],FALSE);
			gtk_widget_show_all(menu);
			gtk_menu_popup (GTK_MENU(menu),NULL, NULL, NULL, NULL,event->button, event->time);
			return FALSE;
		}
		
		int index0=0;
		int id=0;
		ly_reg_get("3opc_select", "%d:%*d:%d:%*s", &index0, &id);
		if(index0!=1)
		{
			gtk_widget_set_sensitive(menuitem[LEFT_EXPORT],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETE],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETEALL],FALSE);
		}
		if(index0==0)
		{
			gtk_widget_set_sensitive(menuitem[LEFT_ADDTOQUEUE],FALSE);
		}
		if(index0!=1||id<=0)
		{
			gtk_widget_set_sensitive(menuitem[LEFT_RENAME],FALSE);
		}
		gtk_widget_show_all(menu);
		gtk_menu_popup (GTK_MENU(menu),NULL, NULL, NULL, NULL,event->button, event->time);
	}
	return FALSE;
}

gboolean
ly_3opc_left_add_cb(GtkWidget *widget, gpointer data)
{
	gchar sql[10240]="";
	gchar tmp[10240]="";
	const gchar *name=NULL;
		
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	int result;
	
	gboolean rt=FALSE;
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
		ly_plm_add_pl((char*)name);
	}
	gtk_widget_destroy(dialog);
	ly_3opc_left_refresh();
	
	return FALSE;
}


gboolean
ly_3opc_left_import_cb(GtkWidget *widget, gpointer data)
{
	GSList *filelist;
	GSList *q;
	gchar *path;
	GtkFileFilter *filter;
	GtkWidget *dialog;
	
	dialog =gtk_file_chooser_dialog_new(	_("Import Playlist..."),
						GTK_WINDOW(ly_win_get_window()->win),
						GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
				      NULL);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER (dialog),TRUE);
	filter=gtk_file_filter_new();
	gtk_file_filter_set_name(filter,_("Playlist File"));
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.cue");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.CUE");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.m3u");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.M3U");
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
	q=filelist;
	while(q)
	{
		path=g_filename_from_uri (q->data,NULL,NULL);
		ly_plm_import_pl(path);
		g_free(path);
		q=q->next;
	}
	g_slist_free(filelist);
	gtk_widget_destroy (dialog);
	return FALSE;
}

gboolean
ly_3opc_left_addtoqueue_cb(GtkWidget *widget, gpointer data)
{	
	int index0=0;
	int id=0;
	char name[1024]="";
	ly_reg_get("3opc_select", "%d:%*d:%d:%1023[^\n]", &index0, &id, name);
	
	char where[1024]="";
	if(index0==2)
	{
		if(id<0)
		{
			ly_pqm_add_md_by_where_from_lib(NULL);
		}
		else
		{
			ly_dbm_replace_str(name, sizeof(name));
			g_snprintf(where, sizeof(where), "artist='%s'", name);
			ly_pqm_add_md_by_where_from_lib(where);
		}
	}
	else if(index0==1)
	{
		if(id<0)
		{
			ly_pqm_add_md_by_where_from_plm	(-1, NULL);
		}
		else
		{
			ly_pqm_add_md_by_where_from_plm	(id, NULL);
		}
	}
	return FALSE;
}

gboolean
ly_3opc_left_rename_cb(GtkWidget *widget, gpointer data)
{
	int index0=0;
	int id=0;
	char str[1024]="";
	ly_reg_get("3opc_select", "%d:%*d:%d:%1023[^\n]", &index0, &id, str);
	if((index0!=1)||(id<=0))
	{
		return FALSE;
	}
	
	const gchar *name=NULL;
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	int result;
	
	dialog=gtk_dialog_new_with_buttons(_("Rename Playlist"),
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
	entry=gtk_entry_new_with_buffer(gtk_entry_buffer_new(str,-1));
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
	ly_plm_rename_pl(id,(char *)name);
	gtk_widget_destroy(dialog);
	return FALSE;
}

gboolean
ly_3opc_left_export_cb(GtkWidget *widget, gpointer data)
{
	int index0=0;
	int id=0;
	ly_reg_get("3opc_select", "%d:%*d:%d:%*[^\n]", &index0, &id);
	if((index0!=1)||(id<=0))
	{
		return FALSE;
	}
	
	GSList *filelist;
	GSList *q;
	gchar *path;
	GtkFileFilter *filter;
	GtkWidget *dialog;
	
	dialog =gtk_file_chooser_dialog_new(	_("Export Playlist..."),
						GTK_WINDOW(ly_win_get_window()->win),
						GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
				      NULL);
	filter=gtk_file_filter_new();
	gtk_file_filter_set_name(filter,_("M3U Playlist File"));
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.m3u");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.M3U");
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
	q=filelist;
	gboolean flag=TRUE;
	if(q)
	{
		path=g_filename_from_uri (q->data,NULL,NULL);
		if(g_file_test(path, G_FILE_TEST_EXISTS))
		{
			GtkWidget *d=gtk_message_dialog_new(GTK_WINDOW(dialog),
					GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_OK_CANCEL,
					_("File exist, Replace it anyway?"));
			result = gtk_dialog_run (GTK_DIALOG (d));
			gtk_widget_destroy (d);
			switch (result)
			{
				case GTK_RESPONSE_OK:
					break;
				case GTK_RESPONSE_CANCEL:
					flag=FALSE;
					break;
				default:
					flag=FALSE;
					break;
			}
		}
		if(flag==TRUE)
		{
			ly_plm_export_pl(id, path);
		}
		g_free(path);
	}
	g_slist_free(filelist);
	gtk_widget_destroy (dialog);
	return FALSE;
}

gboolean
ly_3opc_left_refresh_cb(GtkWidget *widget, gpointer data)
{
	ly_3opc_left_refresh();
	return FALSE;
}

gboolean
ly_3opc_left_delete_cb(GtkWidget *widget, gpointer data)
{
	int index0=0;
	int id=0;
	ly_reg_get("3opc_select", "%d:%*d:%d:%*s", &index0, &id);
	if(index0==1)
	{
		ly_plm_del_pl(id);
	}
	return FALSE;
}

gboolean ly_3opc_left_deleteall_cb(GtkWidget *widget, gpointer data)
{
	int index0=0;
	ly_reg_get("3opc_select", "%d:%*d:%*d:%*s", &index0);
	if(index0!=1)
		return FALSE;
	
	GtkWidget *dialog;
	gint result;
	
	dialog=ly_3opc_left_warning_dialog_create(_("<b>This is a Dangerous function!!</b>\n Do you really want to DELETE ALL playlists?"));
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
	
	ly_plm_clear_pl();
	return FALSE;
}


GtkWidget* ly_3opc_left_warning_dialog_create(gchar *str)
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
