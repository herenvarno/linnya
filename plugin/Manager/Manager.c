#include "Manager.h"

gint index_left[2]={0,0};
gchar sql_right[1024];

const gchar* g_module_check_init(GModule *module)
{	
	return NULL;
}
void g_module_unload(GModule *module)
{
	;
}

GtkWidget *ly_plugin_manager_create()
{
	widget=gtk_event_box_new();
	hpaned=gtk_hpaned_new();
	gtk_container_add(GTK_CONTAINER(widget),hpaned);
	
	GtkWidget *vbox;
	vbox=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(hpaned), vbox);
	
	GtkWidget *scrolled_window;
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window,TRUE,TRUE,0);
	treeview_left=gtk_tree_view_new();
	gtk_container_add(GTK_CONTAINER(scrolled_window),treeview_left);
	
	GtkTreeIter iter;
	store_left = gtk_tree_store_new (3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_INT);
	
	gtk_tree_store_append (store_left, &iter,NULL);
	gtk_tree_store_set(store_left, &iter,0,NULL,1,_("Artists"),2,-1,-1);
	ly_db_exec("SELECT DISTINCT artist FROM metadatas ORDER BY num", ly_plugin_manager_get_artists_cb, &iter);
	
	gtk_tree_store_append(store_left, &iter,NULL);
	gtk_tree_store_set(store_left, &iter,0,NULL,1,_("Playlist"),2,-1,-1);
	ly_db_exec("SELECT id,name FROM playlists ORDER BY num", ly_plugin_manager_get_playlists_cb, &iter);
	
	gtk_tree_store_append(store_left, &iter,NULL);
	gtk_tree_store_set(store_left, &iter,0,NULL,1,_("Playing"),2,-1,-1);
	
	GtkTreeViewColumn *column;
	GtkCellRenderer *cell_renderer;
	column = gtk_tree_view_column_new();
	cell_renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_start(column, cell_renderer,FALSE);
	gtk_tree_view_column_add_attribute(column,cell_renderer,"pixbuf",0);
	cell_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, cell_renderer,FALSE);
	gtk_tree_view_column_add_attribute(column,cell_renderer,"text",1);
	gtk_tree_view_column_set_title(column,_("Manager"));
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview_left), column);
	gtk_tree_view_set_model(GTK_TREE_VIEW (treeview_left), GTK_TREE_MODEL(store_left));
	selection_left=gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_left));
	
	vbox=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(hpaned), vbox);
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window,TRUE,TRUE,0);
	treeview_right=gtk_tree_view_new();
	gtk_container_add(GTK_CONTAINER(scrolled_window),treeview_right);
	
	store_right  = gtk_tree_store_new (5,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING, G_TYPE_INT);
	ly_db_exec("SELECT id,title,artist,album FROM metadatas ORDER BY num LIMIT 200", ly_plugin_manager_get_metadatas_cb, NULL);
	
	cell_renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes(_("*"),cell_renderer,"pixbuf",0,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview_right), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Title"),cell_renderer,"text",1,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),50);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview_right), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Artist"),cell_renderer,"text",2,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),50);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview_right), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Album"),cell_renderer,"text",3,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),50);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview_right), column);
	gtk_tree_view_set_model(GTK_TREE_VIEW (treeview_right), GTK_TREE_MODEL(store_right ));
	selection_right=gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_right));
	gtk_tree_selection_set_mode(selection_right,GTK_SELECTION_MULTIPLE);
	
	g_signal_connect(G_OBJECT(treeview_right), "button_release_event", G_CALLBACK(ly_plugin_manager_popup_right_menu_cb), NULL);
	g_signal_connect(G_OBJECT(treeview_left), "button_release_event", G_CALLBACK(ly_plugin_manager_popup_left_menu_cb), NULL);
	g_signal_connect(G_OBJECT(selection_left), "changed", G_CALLBACK(ly_plugin_manager_left_change_cb), NULL);
	g_signal_connect(G_OBJECT(treeview_left), "button_press_event", G_CALLBACK(ly_plugin_manager_left_mask), NULL);
	g_signal_connect(G_OBJECT(treeview_right), "button_press_event", G_CALLBACK(ly_plugin_manager_right_mask), NULL);
	g_signal_connect(G_OBJECT(treeview_right), "row-activated", G_CALLBACK(ly_plugin_manager_right_active_cb), NULL);
	
	ly_msg_bind("lib_changed", "ui", ly_plugin_manager_on_lib_changed_cb, NULL);
	return widget;
}
void ly_plugin_manager_refresh()
{
	;
}
void ly_plugin_manager_destroy()
{
	ly_msg_unbind("lib_changed", "ui", ly_plugin_manager_on_lib_changed_cb);
}

gboolean ly_plugin_manager_on_lib_changed_cb(gpointer object, gpointer data)
{
	ly_plugin_manager_left_refresh_cb(NULL, NULL);
	ly_plugin_manager_right_refresh_cb(NULL, NULL);
	return FALSE;
}


gboolean ly_plugin_manager_get_artists_cb(gpointer stmt, gpointer data)
{
	GtkTreeIter *iter=data;
	GtkTreeIter subiter;
	gchar *name=(gchar *)g_malloc(128*sizeof(gchar));
	gtk_tree_store_append (store_left,&subiter, iter);
	g_strlcpy(name,(const gchar *)sqlite3_column_text(stmt, 0), 128);
	gtk_tree_store_set(store_left, &subiter,0,NULL,1,name,2,1,-1);
	return FALSE;
}
gboolean ly_plugin_manager_get_playlists_cb(gpointer stmt, gpointer data)
{
	GtkTreeIter *iter=data;
	GtkTreeIter subiter;
	gchar *name=(gchar *)g_malloc(128*sizeof(gchar));
	gint id=0;
	gtk_tree_store_append (store_left,&subiter, iter);
	id=sqlite3_column_int(stmt, 0);
	g_strlcpy(name,(const gchar *)sqlite3_column_text(stmt, 1),128);
	gtk_tree_store_set(store_left, &subiter,0,NULL,1,name,2,id,-1);
	return FALSE;
}

gboolean ly_plugin_manager_get_metadatas_cb(gpointer stmt, gpointer data)
{
	GtkTreeIter iter;
	gint id=0;
	gchar *title=(gchar *)g_malloc(128*sizeof(gchar));
	gchar *artist=(gchar *)g_malloc(128*sizeof(gchar));
	gchar *album=(gchar *)g_malloc(128*sizeof(gchar));

	gtk_tree_store_append (store_right , &iter, NULL);
	id=sqlite3_column_int(stmt, 0);
	g_strlcpy(title,(const gchar *)sqlite3_column_text(stmt, 1),128);
	g_strlcpy(artist,(const gchar *)sqlite3_column_text(stmt, 2),128);
	g_strlcpy(album,(const gchar *)sqlite3_column_text(stmt, 3),128);
	gtk_tree_store_set(store_right, &iter, 0, NULL, 1,title, 2,artist, 3,album, 4, id, -1);
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_treeview_left_mask
//*	DISCRIPTION:	mask
//*
//********************************************************************************
gboolean ly_plugin_manager_left_mask(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button==3)
	{
		GList *selectlist=gtk_tree_selection_get_selected_rows(selection_left,NULL);
		if (g_list_length(selectlist)>1)
			return TRUE;
	}
	return FALSE;
}

gboolean ly_plugin_manager_popup_left_menu_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
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
	
	//右键
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
		g_signal_connect(G_OBJECT(menuitem[LEFT_ADD]), "activate", G_CALLBACK(ly_plugin_manager_left_add_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_IMPORT]), "activate", G_CALLBACK(ly_plugin_manager_left_import_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_ADDTOQUEUE]), "activate", G_CALLBACK(ly_plugin_manager_left_addtoqueue_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_RENAME]), "activate", G_CALLBACK(ly_plugin_manager_left_rename_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_EXPORT]), "activate", G_CALLBACK(ly_plugin_manager_left_export_cb),NULL);
		//g_signal_connect(G_OBJECT(menuitem[LEFT_SORT]), "activate", G_CALLBACK(gui_treeview_callback_left_sort),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_REFRESH]), "activate", G_CALLBACK(ly_plugin_manager_left_refresh_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_DELETE]), "activate", G_CALLBACK(ly_plugin_manager_left_delete_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_DELETEALL]), "activate", G_CALLBACK(ly_plugin_manager_left_deleteall_cb),NULL);
		

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
		if(index_left[0]!=1)
		{
			gtk_widget_set_sensitive(menuitem[LEFT_EXPORT],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETE],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETEALL],FALSE);
		}
		if(index_left[0]==2)
		{
			gtk_widget_set_sensitive(menuitem[LEFT_ADDTOQUEUE],FALSE);
		}
		if(index_left[1]<0)
		{
			gtk_widget_set_sensitive(menuitem[LEFT_RENAME],FALSE);
		}
		gtk_widget_show_all(menu);
		gtk_menu_popup (GTK_MENU(menu),NULL, NULL, NULL, NULL,event->button, event->time);
	}
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_treeview_right_mask
//*	DISCRIPTION:	mask
//*
//********************************************************************************
gboolean ly_plugin_manager_right_mask(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button==3)
	{
		GList *selectlist=gtk_tree_selection_get_selected_rows(selection_right,NULL);
		if (g_list_length(selectlist)>1)
			return TRUE;
	}
	return FALSE;
}

gboolean ly_plugin_manager_popup_right_submenu_cb(gpointer stmt, gpointer data)
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
	g_signal_connect(G_OBJECT(submenuitem), "button_press_event",G_CALLBACK(ly_plugin_manager_right_addtoplaylist_cb),(gpointer)id);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu),submenuitem);
	return FALSE;
}

gboolean ly_plugin_manager_popup_right_menu_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	gchar *menu_right[RIGHT_NUM]={
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
	
	if (event->button==3) 
	{
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
				g_signal_connect(G_OBJECT(submenuitem), "button_press_event",G_CALLBACK(ly_plugin_manager_right_addtoplaylist_cb),(gpointer)(-1));
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu),submenuitem);
				
				ly_db_exec("SELECT id, name FROM playlists", ly_plugin_manager_popup_right_submenu_cb,submenu);
				gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem[RIGHT_ADDTOPLAYLIST]),submenu);
			}
		}
		g_signal_connect(G_OBJECT(menuitem[RIGHT_PLAY]), "activate", G_CALLBACK(ly_plugin_manager_right_play_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_ADDTOQUEUE]), "activate", G_CALLBACK(ly_plugin_manager_right_addtoqueue_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_INFORMATION]), "activate", G_CALLBACK(ly_plugin_manager_right_information_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_ADDFILES]), "activate", G_CALLBACK(ly_plugin_manager_right_addfiles_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_REFRESH]), "activate", G_CALLBACK(ly_plugin_manager_right_refresh_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_DELETE]), "activate", G_CALLBACK(ly_plugin_manager_right_delete_cb),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_DELETEALL]), "activate", G_CALLBACK(ly_plugin_manager_right_deleteall_cb),NULL);
		
		selectlist=gtk_tree_selection_get_selected_rows(selection_right, NULL);
		
		if(g_list_length(selectlist)==0)
		{
			gtk_widget_set_sensitive(menuitem[RIGHT_PLAY],FALSE);
			gtk_widget_set_sensitive(menuitem[RIGHT_ADDTOQUEUE],FALSE);
			gtk_widget_set_sensitive(menuitem[RIGHT_ADDTOPLAYLIST],FALSE);
			gtk_widget_set_sensitive(menuitem[RIGHT_INFORMATION],FALSE);
			gtk_widget_set_sensitive(menuitem[RIGHT_DELETE],FALSE);
		}
		else if(g_list_length(selectlist)>1)
		{
			gtk_widget_set_sensitive(menuitem[RIGHT_PLAY],FALSE);
		}
		if(index_left[1]==0)
		{
			gtk_widget_set_sensitive(menuitem[RIGHT_DELETEALL],FALSE);
		}
		gtk_widget_show_all(menu);
		gtk_menu_popup (GTK_MENU(menu),NULL, NULL, NULL, NULL,event->button, event->time);
	}
	return FALSE;
}

gboolean ly_plugin_manager_left_refresh_cb(GtkWidget *widget, gpointer data)
{	
	if(store_left)
		g_object_unref(store_left);
	store_left=NULL;
	
	GtkTreeIter iter;
	store_left = gtk_tree_store_new (3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_INT);
	
	gtk_tree_store_append (store_left, &iter,NULL);
	gtk_tree_store_set(store_left, &iter,0,NULL,1,_("Artists"),2,-1,-1);
	ly_db_exec("SELECT DISTINCT artist FROM metadatas ORDER BY num", ly_plugin_manager_get_artists_cb, &iter);
	
	gtk_tree_store_append(store_left, &iter,NULL);
	gtk_tree_store_set(store_left, &iter,0,NULL,1,_("Playlist"),2,-1,-1);
	ly_db_exec("SELECT id,name FROM playlists ORDER BY num", ly_plugin_manager_get_playlists_cb, &iter);
	
	gtk_tree_store_append(store_left, &iter,NULL);
	gtk_tree_store_set(store_left, &iter,0,NULL,1,_("Playing"),2,-1,-1);
	gtk_tree_view_set_model(GTK_TREE_VIEW (treeview_left), GTK_TREE_MODEL(store_left));
	return FALSE;
}

gboolean ly_plugin_manager_right_refresh_cb(GtkWidget *widget, gpointer data)
{
	if(store_right)
		g_object_unref(store_right);
	store_right=NULL;
	
	store_right = gtk_tree_store_new (5,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING, G_TYPE_INT);
	if(!g_str_equal(sql_right,""))
		ly_db_exec(sql_right, ly_plugin_manager_get_metadatas_cb, NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW (treeview_right), GTK_TREE_MODEL(store_right));
	return FALSE;
}

gboolean ly_plugin_manager_left_change_cb(GtkWidget *widget, gpointer data)
{
	GList *selectlist=NULL;
	GtkTreeModel *model;
	model=GTK_TREE_MODEL(store_left);
	gint *index;

	selectlist=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(selection_left),&model);
	if(g_list_length(selectlist)<1)
		return FALSE;
	if(gtk_tree_path_get_depth((GtkTreePath*)(selectlist->data))<1)
		return FALSE;

	index=gtk_tree_path_get_indices((GtkTreePath*)(selectlist->data));
	
	gchar *str;
	gint id=0;
	GtkTreeIter iter;
	gtk_tree_selection_get_selected(selection_left,&model,&iter);
	gtk_tree_model_get(model,&iter,1,&str,2,&id,-1);
	gchar name[1024]={0};
	g_strlcpy(name,str,sizeof(name));
	ly_global_replace_str(name,sizeof(name),"'","''");
	switch(index[0])
	{
		case 0:
			index_left[0]=0;
			index_left[1]=id;
			if(gtk_tree_path_get_depth((GtkTreePath*)(selectlist->data))==1)
			{
				g_snprintf(sql_right,sizeof(sql_right),"SELECT id,title,artist,album FROM metadatas ORDER BY num LIMIT 200");
			}
			else
			{
				g_snprintf(sql_right,sizeof(sql_right),"SELECT id,title,artist,album FROM metadatas WHERE artist='%s' ORDER BY num LIMIT 200",name);
			}
			break;
		case 1:
			index_left[0]=1;
			index_left[1]=id;
			if(gtk_tree_path_get_depth((GtkTreePath*)(selectlist->data))==1)
			{
				g_snprintf(sql_right,sizeof(sql_right),"SELECT id,title,artist,album FROM metadatas tm, connections tc WHERE tc.mid=tm.id ORDER BY tc.num LIMIT 200");
			}
			else
			{
				g_snprintf(sql_right,sizeof(sql_right),"SELECT id, title, artist, album FROM metadatas tm, connections tc WHERE tc.pid='%d' AND tc.mid=tm.id ORDER BY tc.num LIMIT 200", id);
			}
			break;
		case 2:
			index_left[0]=2;
			index_left[1]=id;
			g_snprintf(sql_right,sizeof(sql_right),"SELECT id,title,artist,album FROM plist LIMIT 200");
			break;
		default:
			break;
	}
	ly_plugin_manager_right_refresh_cb(treeview_left, NULL);
	return FALSE;
}

gboolean ly_plugin_manager_right_change_cb(GtkWidget *widget, gpointer data)
{
	return FALSE;
}


gboolean ly_plugin_manager_right_active_cb(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data)
{
	gchar sql[10240]="";
	int id=0;
	GtkTreeIter iter;
	gchar temp[10240]="";
	
	/*
	 * UPDATE THE PLAYING LIST
	 */
	if(index_left[0]<2)
	{
		g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=-1");
		ly_db_exec(sql,NULL,NULL);
		if(index_left[0]==0)
		{
			g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=num WHERE");
		}
		else
		{
			g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=(SELECT num FROM connections WHERE mid=metadatas.id AND pid=%d) WHERE", index_left[1]);
		}
		if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store_right), &iter))
		{
			g_strlcpy(temp,sql,sizeof(temp));
			gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
			g_snprintf(sql, sizeof(sql), "%s id=%d", temp, id);
		}
		while(gtk_tree_model_iter_next(GTK_TREE_MODEL(store_right), &iter))
		{
			g_strlcpy(temp,sql,sizeof(temp));
			gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
			g_snprintf(sql, sizeof(sql), "%s OR id=%d", temp, id);
		}
		ly_db_exec(sql,NULL,NULL);
	}
	/*
	 * SET THE PLAYING AUDIO AND PLAY
	 */
	gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, path);
	gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
	ly_audio_set_meta(id);
	ly_audio_stop();
	ly_audio_play();
	return FALSE;
}

gboolean ly_plugin_manager_right_play_cb(GtkWidget *widget, gpointer data)
{
	gchar sql[10240]="";
	int id=0;
	GtkTreeIter iter;
	gchar temp[10240]="";
	GList *list=NULL;
	
	/*
	 * UPDATE THE PLAYING LIST
	 */
	if(index_left[0]<2)
	{
		g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=-1");
		ly_db_exec(sql,NULL,NULL);
		if(index_left[0]==0)
		{
			g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=num WHERE");
		}
		else
		{
			g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=(SELECT num FROM connections WHERE mid=metadatas.id AND pid=%d) WHERE", index_left[1]);
		}
		if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store_right), &iter))
		{
			g_strlcpy(temp,sql,sizeof(temp));
			gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
			g_snprintf(sql, sizeof(sql), "%s id=%d", temp, id);
		}
		while(gtk_tree_model_iter_next(GTK_TREE_MODEL(store_right), &iter))
		{
			g_strlcpy(temp,sql,sizeof(temp));
			gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
			g_snprintf(sql, sizeof(sql), "%s OR id=%d", temp, id);
		}
		ly_db_exec(sql,NULL,NULL);
	}
	/*
	 * SET THE PLAYING AUDIO AND PLAY
	 */
	list=gtk_tree_selection_get_selected_rows(selection_right, NULL);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, (GtkTreePath *)(list->data));
	g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (list);
	gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
	ly_audio_set_meta(id);
	ly_audio_stop();
	ly_audio_play();
	return FALSE;
}

gboolean ly_plugin_manager_right_addtoqueue_cb(GtkWidget *widget, gpointer data)
{
	gchar sql[10240]="";
	int id=0;
	GtkTreeIter iter;
	gchar temp[10240]="";
	GList *list=NULL;
	GList *p=NULL;
	/*
	 * UPDATE THE PLAYING LIST
	 */
	if(index_left[0]<2)
	{
		if(index_left[0]==0)
		{
			g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=num+(SELECT MAX(playing) FROM metadatas) WHERE");
		}
		else
		{
			g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=(SELECT num FROM connections WHERE mid=metadatas.id AND pid=%d)+(SELECT MAX(playing) FROM metadatas) WHERE", index_left[1]);
		}
		list=gtk_tree_selection_get_selected_rows(selection_right, NULL);
		p=list;
		while(p)
		{
			
			gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, (GtkTreePath *)(p->data));
			gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
			g_strlcpy(temp,sql,sizeof(temp));
			if(p==list)
			{
				g_snprintf(sql, sizeof(sql), "%s id=%d", temp, id);
			}
			else
			{
				g_snprintf(sql, sizeof(sql), "%s OR id=%d", temp, id);
			}
			p=p->next;
		}
		g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (list);
		ly_db_exec(sql,NULL,NULL);
	}
	return FALSE;
}

gboolean ly_plugin_manager_right_addtoplaylist_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
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
						   GTK_WINDOW(ly_ui_win_window->win),
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
			ly_global_replace_str(tmp, sizeof(tmp), "'", "''");
			g_snprintf(sql,sizeof(sql),"INSERT INTO playlists (name, num) VALUES ('%s', ifnull((SELECT MAX(num) FROM playlists),0)+1)", tmp);
			ly_db_exec(sql,NULL,NULL);
			pid=ly_db_get_last_insert_rowid();
			ly_plugin_manager_left_refresh_cb(treeview_left,NULL);
		}
		gtk_widget_destroy(dialog);
	}
	list=gtk_tree_selection_get_selected_rows(selection_right, NULL);
	if(list==NULL)
		return FALSE;
	p=list;
	g_strlcpy(sql,"begin",sizeof(sql));
	ly_db_exec(sql,NULL,NULL);
	while(p)
	{
		gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, (GtkTreePath *)(p->data));
		gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &mid, -1);
		g_snprintf(sql, sizeof(sql), "INSERT INTO connections (pid, mid, num) VALUES (%d, %d, ifnull((select max(num) from connections where pid=%d), 0)+1)", pid, mid, pid);
		ly_global_replace_str(sql, sizeof(sql), "'", "''");
		ly_db_exec(sql,NULL,NULL);
		p=p->next;
	}
	g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (list);
	g_strlcpy(sql,"commit",sizeof(sql));
	ly_db_exec(sql,NULL,NULL);
	
	return FALSE;
}

gboolean ly_plugin_manager_right_information_cb(GtkWidget *widget, gpointer data)
{
	//获取选中音乐
	GList *list;
	GtkTreeIter iter;
	gint id=0;
	lyDbMetadata *md=NULL;
	
	list=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(selection_right), NULL);
	if(g_list_length(list)==0)
		return FALSE;
	gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, (GtkTreePath *)(list->data));
	gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
	md=ly_db_get_metadata_by_id(id);
	
/*	index=gtk_tree_path_get_indices(selectlist->data);
	list=lib_find_md_by_index(((_gl_playlist*)(gl_selecting->data))->list,index[0]);
	md=list->data;
	
	if(g_strcmp0(md->uri,gui_treeview_info_last_uri)==0)
	{
		//等待上一个事件结束
		while(gui_treeview_info_lock);
		gui_treeview_info_lock=TRUE;
		gui_treeview_info_last_uri=md->uri;
	}*/
	
	//init
	g_strlcpy(info_title, md->title, sizeof(info_title));
	g_strlcpy(info_artist, md->artist, sizeof(info_artist));
	g_strlcpy(info_album, md->album, sizeof(info_album));
	g_strlcpy(info_genre, "unknown", sizeof(info_genre));
	g_strlcpy(info_track, "unknown", sizeof(info_track));
	g_strlcpy(info_codec, md->codec, sizeof(info_codec));
	g_strlcpy(info_start, md->start, sizeof(info_start));
	g_strlcpy(info_duration, md->duration, sizeof(info_duration));
	g_strlcpy(info_bitrate, "unknown", sizeof(info_bitrate));
	g_strlcpy(info_uri, md->uri, sizeof(info_uri));
	g_strlcpy(info_comment, md->uri, sizeof(info_comment));
	info_cover=NULL;
	
	//build pipeline
/*	GstElement *urisrc;
	GstElement *decodebin;
	GstElement *fakesink;
	GstBus *bus=NULL;
	gint changeTimeout = 0;
	GstStateChangeReturn ret;
	GstMessage *msg;
	
	pipeline=gst_pipeline_new("pipeline");
	urisrc=gst_element_make_from_uri(GST_URI_SRC,gui_treeview_info_uri,"urisrc");
	decodebin=gst_element_factory_make("decodebin","decodebin");
	fakesink=gst_element_factory_make("fakesink","fakesink");
	
	gst_bin_add_many(GST_BIN(pipeline),urisrc,decodebin,fakesink,NULL);
	gst_element_link(urisrc,decodebin);
	
	g_signal_connect_object(G_OBJECT(decodebin),"new-decoded-pad",G_CALLBACK(gui_treeview_callback_callback_build_pipeline),fakesink,0);
	
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_element_set_state(pipeline, GST_STATE_NULL);
	ret = gst_element_set_state(pipeline,GST_STATE_PAUSED);
	if(!ret)
	{
		if(pipeline!=NULL)
			gst_object_unref(GST_OBJECT(pipeline));
		pipeline=NULL;
		return FALSE;
	}
	while(ret==GST_STATE_CHANGE_ASYNC && !eos && changeTimeout < 5) 
	{
		msg = gst_bus_timed_pop(bus, 1 * GST_SECOND);
		if(msg!=NULL) 
		{
			gui_treeview_callback_callback_get_metadata(bus, msg, NULL);
			gst_message_unref(msg);
			changeTimeout = 0;
		}
		else
			changeTimeout++;
		ret = gst_element_get_state(pipeline, NULL, NULL, 0);
	}
	gst_object_unref(bus);
	gui_treeview_callback_callback_event_loop(pipeline,FALSE);
	if(ret!=GST_STATE_CHANGE_SUCCESS)
	{
		gst_element_set_state(pipeline,GST_STATE_NULL);
		return FALSE;
	}
	
	pipeline=NULL;
	eos=FALSE;
	*/
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
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *entry[INFO_NUM];
	GtkWidget *image;
	
	dialog=gtk_dialog_new_with_buttons(	_("Information"),
						GTK_WINDOW(ly_ui_win_window->win),
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				     GTK_STOCK_OK,
				     GTK_RESPONSE_ACCEPT,
				     GTK_STOCK_CANCEL,
				     GTK_RESPONSE_REJECT,
				     NULL);
	gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
	gtk_window_set_default_size(GTK_WINDOW(dialog),400,350);

	
	frame=gtk_frame_new(_("Basic Information"));
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),frame,TRUE,TRUE,0);
	hbox=gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame),hbox);
	table=gtk_table_new(5,2,FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(table),5);
	gtk_box_pack_start(GTK_BOX(hbox),table,TRUE,TRUE,0);
	label=gtk_label_new(_("Codec:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Start:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Duration:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Bitrate:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Uri:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(info_codec);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(info_start);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(info_duration);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(info_bitrate);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(info_uri);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_MIDDLE);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 4, 5, GTK_FILL, GTK_FILL, 5, 5);
	image=gtk_image_new_from_pixbuf(info_cover);
	gtk_table_attach(GTK_TABLE(table), image, 2, 3, 0, 5, GTK_FILL, GTK_FILL, 5, 5);
	
	frame=gtk_frame_new(_("Advance Information"));
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),frame,TRUE,TRUE,0);
	table=gtk_table_new(5,2,FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(table),5);
	gtk_container_add(GTK_CONTAINER(frame),table);
	label=gtk_label_new(_("Title:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Artist:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Album:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("genre:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("track:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5, GTK_FILL, GTK_FILL, 5, 5);
	entry[INFO_TITLE]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_TITLE]),info_title);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_TITLE], 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
//	g_signal_connect(G_OBJECT(entry[INFO_TITLE]),"changed",G_CALLBACK(gui_treeview_callback_set_changed3),&changed);
	entry[INFO_ARTIST]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_ARTIST]),info_artist);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_ARTIST], 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
//	g_signal_connect(G_OBJECT(entry[INFO_ARTIST]),"changed",G_CALLBACK(gui_treeview_callback_set_changed1),&changed);
	entry[INFO_ALBUM]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_ALBUM]),info_album);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_ALBUM], 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
//	g_signal_connect(G_OBJECT(entry[INFO_ALBUM]),"changed",G_CALLBACK(gui_treeview_callback_set_changed2),&changed);
	entry[INFO_GENRE]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_GENRE]),info_genre);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_GENRE], 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
//	g_signal_connect(G_OBJECT(entry[INFO_GENRE]),"changed",G_CALLBACK(gui_treeview_callback_set_changed3),&changed);
	entry[INFO_TRACK]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_TRACK]),info_track);
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
			return FALSE;
			break;
		default:
			gtk_widget_destroy (dialog);
			return FALSE;
			break;
	}															return FALSE;
}


gboolean ly_plugin_manager_right_addfiles_cb(GtkWidget *widget, gpointer data)
{
	GSList *filelist;
	GSList *q;
	gchar *filename,*fileuri, tmp[1024];
	lyDbMetadata *md;
	GtkFileFilter *filter;
	GtkWidget *dialog;
	gchar sql[1024];
	
	dialog =gtk_file_chooser_dialog_new(	_("Add From File..."),
						GTK_WINDOW(ly_ui_win_window->win),
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
	q=filelist;
	ly_db_exec("begin",NULL,NULL);
	while(q)
	{
		filename=g_filename_from_uri(q->data,NULL,NULL);
		realpath(filename,tmp);
		fileuri=g_strconcat("file://",tmp, NULL);
		g_free(filename);

		md=ly_db_read_metadata(fileuri);
		if(index_left[0]==0)
		{
			g_snprintf(sql, sizeof(sql), "INSERT INTO metadatas(title, artist, album, codec, start, duration, uri, playing, num, flag, tmpflag) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', 0, ifnull((SELECT MAX(num) FROM metadatas),0)+1, 0, 1)", md->title, md->artist, md->album, md->codec, md->start, md->duration, md->uri);
			ly_global_replace_str(sql, sizeof(sql), "'", "''");
			ly_db_exec(sql,NULL,NULL);
		}
		else if(index_left[0]==1 && index_left[1]>0)
		{
			g_snprintf(sql, sizeof(sql), "INSERT INTO metadatas(title, artist, album, codec, start, duration, uri, playing, num, flag, tmpflag) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', 0, ifnull((SELECT MAX(num) FROM metadatas),0)+1, 0, 1)", md->title, md->artist, md->album, md->codec, md->start, md->duration, md->uri);
			ly_db_exec(sql,NULL,NULL);
			g_snprintf(sql, sizeof(sql), "INSERT INTO connections(pid, mid, num) VALUES ( %d, %d, ifnull((SELECT MAX(num) FROM connections WHERE pid=%d),0)+1)",index_left[1], (gint)ly_db_get_last_insert_rowid(), index_left[1]);
			ly_global_replace_str(sql, sizeof(sql), "'", "''");
			ly_db_exec(sql,NULL,NULL);
		}
		else if(index_left[0]==1)
		{
			g_snprintf(sql, sizeof(sql), "INSERT INTO metadatas(title, artist, album, codec, start, duration, uri, playing, num, flag, tmpflag) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', ifnull((SELECT MAX(playing) FROM metadatas),0)+1, ifnull((SELECT MAX(num) FROM metadatas),0)+1, 0, 1)", md->title, md->artist, md->album, md->codec, md->start, md->duration, md->uri);
			ly_global_replace_str(sql, sizeof(sql), "'", "''");
			ly_db_exec(sql,NULL,NULL);
		}
		ly_db_free_metadata(md);
		q=q->next;
	}
	ly_db_exec("commit",NULL,NULL);
	g_slist_free(filelist);
	
	ly_plugin_manager_left_refresh_cb(NULL,NULL);
	ly_plugin_manager_right_refresh_cb(NULL,NULL);
	gtk_widget_destroy (dialog);
	return FALSE;
}

GtkWidget* ly_plugin_manager_dialog_new(GtkWidget *parent, gchar *str)
{
	GtkWidget *dialog=NULL;
	GtkWidget *hbox=NULL;
	GtkWidget *image=NULL;
	GtkWidget *label=NULL;
	dialog=gtk_dialog_new_with_buttons(	_("Warnning"),
						GTK_WINDOW(parent),
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

gboolean ly_plugin_manager_right_delete_cb(GtkWidget *widget, gpointer data)
{
	GList *list;
	GList *p;
	
	GtkTreeIter iter;
	
	GtkWidget *dialog;
	gint result;
	
	gint id;
	gchar sql[10240];
	gchar temp[10240];
	
	list=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(selection_right),NULL);
	if(list==NULL)
		return FALSE;
	
	if(index_left[0]==0)
	{
		dialog=ly_plugin_manager_dialog_new(ly_ui_win_window->win, _("<b>This is a Dangerous function!!</b>\n The music you wanna delete is belongs to your music library,\n It will be delete physically and permanently.\n Do you really want to do it?"));
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
		g_strlcpy(temp, "DELETE FROM metadatas WHERE", sizeof(temp));
		p=list;
		gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, (GtkTreePath *)(p->data));
		gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
		g_snprintf(sql, sizeof(sql), "%s id=%d",temp, id);
		p=p->next;
		while(p)
		{
			g_strlcpy(temp, sql, sizeof(temp));
			gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, (GtkTreePath *)(p->data));
			gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
			g_snprintf(sql, sizeof(sql), "%s OR id=%d",temp, id);
			p=p->next;
		}
		g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (list);
		ly_db_exec(sql,NULL,NULL);
	}
	else if(index_left[0]==1)
	{
		if(index_left[1]<=0)
		{
			g_snprintf(temp, sizeof(temp), "DELETE FROM connections WHERE (");
		}
		else
		{
			g_snprintf(temp, sizeof(temp), "DELETE FROM connections WHERE pid=%d AND (", index_left[1]);
		}
		p=list;
		gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, (GtkTreePath *)(p->data));
		gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
		g_snprintf(sql, sizeof(sql), "%s mid=%d", temp, id);
		p=p->next;
		while(p)
		{
			g_strlcpy(temp, sql, sizeof(temp));
			gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, (GtkTreePath *)(p->data));
			gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
			g_snprintf(sql, sizeof(sql), "%s OR mid=%d", temp, id);
			p=p->next;
		}
		g_strlcpy(temp, sql, sizeof(temp));
		g_snprintf(sql, sizeof(sql), "%s )",temp);
		g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (list);
		ly_db_exec(sql,NULL,NULL);
	}
	else if(index_left[0]==2)
	{
		g_snprintf(temp,sizeof(temp), "UPDATE metadatas SET playing=-1 WHERE ");
		p=list;
		gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, (GtkTreePath *)(p->data));
		gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
		g_snprintf(sql, sizeof(sql), "%s id=%d",temp, id);
		p=p->next;
		while(p)
		{
			g_strlcpy(temp, sql, sizeof(temp));
			gtk_tree_model_get_iter(GTK_TREE_MODEL(store_right), &iter, (GtkTreePath *)(p->data));
			gtk_tree_model_get(GTK_TREE_MODEL(store_right), &iter, 4, &id, -1);
			g_snprintf(sql, sizeof(sql), "%s OR id=%d",temp, id);
			p=p->next;
		}
		g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (list);
		ly_db_exec(sql,NULL,NULL);
	}
	ly_plugin_manager_right_refresh_cb(NULL, NULL);
	return FALSE;
}

gboolean ly_plugin_manager_right_deleteall_cb(GtkWidget *widget, gpointer data)
{	
	GtkWidget *dialog;
	gint result;
	
	gchar sql[10240];
	
	if(index_left[0]==0)
	{
		dialog=ly_plugin_manager_dialog_new(ly_ui_win_window->win, _("<b>This is a Dangerous function!!</b>\n The music you wanna delete is belongs to your music library,\n It will be delete physically and permanently.\n Do you really want to do it?"));
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
		ly_db_exec("DELETE FROM metadatas",NULL,NULL);
	}
	else if(index_left[0]==1)
	{
		if(index_left[1]<=0)
		{
			g_snprintf(sql,sizeof(sql), "DELETE FROM connections");
		}
		else
		{
			g_snprintf(sql,sizeof(sql), "DELETE FROM connections WHERE pid=%d", index_left[1]);
		}
		ly_db_exec(sql,NULL,NULL);
	}
	else if(index_left[0]==2)
	{
		g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=-1");
		ly_db_exec(sql,NULL,NULL);
	}
	ly_plugin_manager_right_refresh_cb(NULL, NULL);
	return FALSE;
}

gboolean ly_plugin_manager_left_add_cb(GtkWidget *widget, gpointer data)
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
	while(!rt)
	{
		dialog=gtk_dialog_new_with_buttons(_("New Playlist"),
					   GTK_WINDOW(ly_ui_win_window->win),
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
			ly_global_replace_str(tmp, sizeof(tmp), "'", "''");
			g_snprintf(sql,sizeof(sql),"INSERT INTO playlists (name, num) VALUES ('%s', ifnull((SELECT MAX(num) FROM playlists),0)+1)", tmp);
			rt=ly_db_exec(sql,NULL,NULL);
		}
		gtk_widget_destroy(dialog);
		if(!rt)
		{
			dialog=ly_plugin_manager_dialog_new(ly_ui_win_window->win, _("<b>Illeagl Playlist Name</b>\n The playlist name you just entered is illeagl. Please type again."));
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
		}
	}
	ly_plugin_manager_left_refresh_cb(treeview_left,NULL);
	
	return FALSE;
}

gboolean ly_plugin_manager_left_import_cb(GtkWidget *widget, gpointer data)
{
	GSList *filelist;
	GSList *q;
	gchar *filename;
	GtkFileFilter *filter;
	GtkWidget *dialog;
	
	dialog =gtk_file_chooser_dialog_new(	_("Import Playlist..."),
						GTK_WINDOW(ly_ui_win_window->win),
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
		filename=g_filename_from_uri (q->data,NULL,NULL);
		//pl_import_playlist(filename);
		g_free(filename);
		q=q->next;
	}
	g_slist_free(filelist);
	
	ly_plugin_manager_left_refresh_cb(NULL,NULL);
	gtk_widget_destroy (dialog);
	return FALSE;
}

gboolean ly_plugin_manager_left_addtoqueue_cb(GtkWidget *widget, gpointer data)
{printf("%d\n",index_left[1]);
	gchar *str;
	gchar sql[1024]="";
	GtkTreeIter iter;
	gtk_tree_selection_get_selected(selection_left, NULL, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(store_left),&iter,1,&str ,-1);
	if(index_left[0]==0)
	{
		if(index_left[1]<0)
		{
			g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=ifnull((select max(playing) from metadatas where playing>0), 0)+num");
		}
		else
		{
			g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=ifnull((select max(playing) from metadatas where playing>0), 0)+num WHERE artist='%s'", str);
		}
		ly_db_exec(sql,NULL,NULL);
	}
	else if(index_left[0]==1)
	{
		if(index_left[1]<0)
		{
			g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=ifnull((select max(playing) from metadatas where playing>0), 0)+(SELECT num FROM connections WHERE mid=metadatas.id) WHERE id IN (SELECT mid FROM connections) AND playing<=0");
		}
		else
		{
			g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET playing=ifnull((select max(playing) from metadatas where playing>0), 0)+(SELECT num FROM connections WHERE mid=metadatas.id) WHERE id IN (SELECT mid FROM connections WHERE pid=%d) AND playing<=0", index_left[1]);
		}
		ly_db_exec(sql,NULL,NULL);
	}
	return FALSE;
}

gboolean ly_plugin_manager_left_rename_cb(GtkWidget *widget, gpointer data)
{
	gchar sql[10240]="";
	gchar tmp[10240]="";
	
	gchar *str=NULL;
	const gchar *name=NULL;
	
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	int result;
	
	GtkTreeIter iter;
	gtk_tree_selection_get_selected(selection_left, NULL, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(store_left),&iter,1,&str ,-1);
	
	gboolean rt=FALSE;
	while(!rt)
	{
		dialog=gtk_dialog_new_with_buttons(_("Rename Playlist"),
						   GTK_WINDOW(ly_ui_win_window->win),
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
		if(name&&!g_str_equal(name,""))
		{
			g_strlcpy(tmp, name, sizeof(tmp));
			ly_global_replace_str(tmp, sizeof(tmp), "'", "''");
			g_snprintf(sql,sizeof(sql),"UPDATE playlists SET name='%s' WHERE id=%d", tmp, index_left[1]);
			rt=ly_db_exec(sql,NULL,NULL);
		}
		gtk_widget_destroy(dialog);
		if(!rt)
		{
			dialog=ly_plugin_manager_dialog_new(ly_ui_win_window->win, _("<b>Illeagl Playlist Name</b>\n The playlist name you just entered is illeagl. Please type again."));
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
		}
	}
	ly_plugin_manager_left_refresh_cb(NULL,NULL);
	return FALSE;
}

gboolean ly_plugin_manager_left_export_cb(GtkWidget *widget, gpointer data)
{
	return FALSE;
}

gboolean ly_plugin_manager_left_delete_cb(GtkWidget *widget, gpointer data)
{
	gchar sql[10240]="";
	if(index_left[0]==1)
	{
		snprintf(sql,sizeof(sql),"DELETE FROM playlists WHERE id=%d", index_left[1]);
		ly_db_exec(sql,NULL,NULL);
		ly_plugin_manager_left_refresh_cb(NULL,NULL);
	}
	return FALSE;
}

gboolean ly_plugin_manager_left_deleteall_cb(GtkWidget *widget, gpointer data)
{
	if(index_left[0]==1)
	{
		ly_db_exec("DELETE FROM playlists",NULL,NULL);
		ly_plugin_manager_left_refresh_cb(NULL,NULL);
	}
	return FALSE;
}