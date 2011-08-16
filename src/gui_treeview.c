

//HEADERS
#include "gui_treeview.h"

//VARIABLES
enum{	LEFT_ADD,
		LEFT_IMPORT,
		LEFT_ADDTOQUEUE,
		LEFT_RENAME,
		LEFT_EXPORT,
		LEFT_SORT,
		LEFT_REFRESH,
		LEFT_DELETE,
		LEFT_DELETEALL,
		LEFT_NUM,
	};
enum{	RIGHT_PLAY,
		RIGHT_ADDTOQUEUE,
		RIGHT_ADDTOPLAYLIST,
//		RIGHT_CONVERT,
		RIGHT_INFORMATION,
		RIGHT_ADDFROMFILE,
		RIGHT_SORT,
		RIGHT_REFRESH,
		RIGHT_DELETE,
		RIGHT_DELETEALL,
		RIGHT_NUM
	};

GstElement *pipeline;
gboolean eos=FALSE;
gchar *gui_treeview_info_title=NULL;
gchar *gui_treeview_info_artist=NULL;
gchar *gui_treeview_info_album=NULL;
gchar *gui_treeview_info_genre=NULL;
gint gui_treeview_info_track=0;
gchar *gui_treeview_info_codec=NULL;
gchar *gui_treeview_info_start=NULL;
gchar *gui_treeview_info_duration=NULL;
gchar *gui_treeview_info_bitrate=NULL;
gchar *gui_treeview_info_uri=NULL;
GdkPixbuf *gui_treeview_info_pixbuf=NULL;
gchar *location=NULL;

gboolean gui_treeview_info_lock=FALSE;
gchar *gui_treeview_info_last_uri=NULL;
	
//FUNCTIONS

GtkWidget* gui_treeview_warnning_dialog_new(GtkWidget *parent, gchar *str)
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
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox,TRUE,TRUE,0);
	image=gtk_image_new_from_stock(GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_DIALOG);
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), str);
	gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,0);
	gtk_widget_show_all(dialog);
	return dialog;
}
//********************************************************************************
//*	
//*	FUNCTION:		gui_treeview_left_refresh
//*	DISCRIPTION:	refresh the left tree view
//*
//********************************************************************************
gboolean gui_treeview_left_refresh()
{
	GList *temp;
	
	GList *p;
	_gl_playlist *plink;
	
	GtkTreeIter iter;
	GtkTreeIter subiter;
	
	
	//释放
	if(gui_window_store_left)
		g_object_unref(gui_window_store_left);
	gui_window_store_left=NULL;
	
	//重建
	
	temp=pl_find_pl_by_playing();
	gui_window_store_left = gtk_tree_store_new (2,GDK_TYPE_PIXBUF,G_TYPE_STRING);
	gtk_tree_store_append (gui_window_store_left, &iter,NULL);
	gtk_tree_store_set(gui_window_store_left, &iter,0,NULL,1,_("Library"),-1);
	gtk_tree_store_append (gui_window_store_left,&subiter, &iter);
	if(gl_playing&&((_gl_playinglist*)(gl_playing->data))->flag==0)
		gtk_tree_store_set(gui_window_store_left, &subiter,0,gl_pixbuf_playing,1,_("All Tracks"),-1);
	else
		gtk_tree_store_set(gui_window_store_left, &subiter,0,NULL,1,_("All Tracks"),-1);
	
	gtk_tree_store_append (gui_window_store_left, &iter,NULL);
	switch(gl_conf->sublib)
	{
		case 0:
			gtk_tree_store_set(gui_window_store_left, &iter,0,NULL,1,_("No Sublib"),-1);
			break;
		case 1:
			gtk_tree_store_set(gui_window_store_left, &iter,0,NULL,1,_("Artists"),-1);
			break;
		case 2:
			gtk_tree_store_set(gui_window_store_left, &iter,0,NULL,1,_("Albums"),-1);
			break;
		default:
			break;
	}
	p=gl_sublib;
	while(p)
	{
		plink=p->data;
		gtk_tree_store_append (gui_window_store_left,&subiter, &iter);
		if(gl_playing&&(((_gl_playinglist*)(gl_playing->data))->flag==1)&&(temp==p))
			gtk_tree_store_set(gui_window_store_left, &subiter,0,gl_pixbuf_playing,1,plink->name,-1);
		else
			gtk_tree_store_set(gui_window_store_left, &subiter,0,NULL,1,plink->name,-1);
		p=p->next;
	}
	
	gtk_tree_store_append(gui_window_store_left, &iter,NULL);
	gtk_tree_store_set(gui_window_store_left, &iter,0,NULL,1,_("Playlist"),-1);
	p=gl_playlist;
	while(p)
	{
		plink=p->data;
		gtk_tree_store_append (gui_window_store_left,&subiter, &iter);
		if(gl_playing&&(((_gl_playinglist*)(gl_playing->data))->flag==2)&&(temp==p))
			gtk_tree_store_set(gui_window_store_left, &subiter,0,gl_pixbuf_playing,1,plink->name,-1);
		else
			gtk_tree_store_set(gui_window_store_left, &subiter,0,NULL,1,plink->name,-1);
		p=p->next;
	}
	gtk_tree_view_set_model(GTK_TREE_VIEW (gui_window_tree_left), GTK_TREE_MODEL(gui_window_store_left));
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_treeview_right_refresh
//*	DISCRIPTION:	refresh the right tree view
//*
//********************************************************************************
gboolean gui_treeview_right_refresh()
{
	GList *temp;
	
	GList *q;
	_gl_metadata *qlink;
	
	GtkTreeIter iter;
	temp=pl_find_pl_by_playing();
	
	//释放
	if(gui_window_store_right)
		g_object_unref(gui_window_store_right);
	gui_window_store_right=NULL;
	
	//重建
	gui_window_store_right = gtk_tree_store_new (4,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);
	
	q=((_gl_playlist *)(gl_selecting->data))->list;
	while(q)
	{
		qlink=q->data;
		gtk_tree_store_append (gui_window_store_right, &iter,NULL);
		if((temp==gl_selecting)&&(qlink==((_gl_playinglist*)(gl_playing->data))->md))
			gtk_tree_store_set(gui_window_store_right, &iter,
				0,gl_pixbuf_playing,
				1,qlink->title,
				2,qlink->artist,
				3,qlink->album,
				-1);
		else
			gtk_tree_store_set(gui_window_store_right, &iter,
				0,NULL,
				1,qlink->title,
				2,qlink->artist,
				3,qlink->album,
				-1);
		q=q->next;
	}
	gtk_tree_view_set_model(GTK_TREE_VIEW (gui_window_tree_right), GTK_TREE_MODEL(gui_window_store_right));
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_treeview_right_active
//*	DISCRIPTION:	active the right tree selection
//*
//********************************************************************************
gboolean gui_treeview_right_active(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data)
{
	gint *index;
	
	GList *q;
	_gl_metadata *qlink;
	_gl_playinglist *pgl;
	
	index=gtk_tree_path_get_indices(path);
	
	gl_played=pl_find_pl_by_playing();
	
	//释放gl_playinglist
	gl_free_list_playinglist(gl_playinglist);
	gl_playing=NULL;
	gl_playinglist=NULL;
	
	//把gl_selecting所指向的列表载入gl_playinglist
	q=((_gl_playlist*)(gl_selecting->data))->list;
	while(q)
	{
		qlink=q->data;
		pgl=(_gl_playinglist*)g_malloc(sizeof(_gl_playinglist));
		pgl->name=g_strconcat(((_gl_playlist*)(gl_selecting->data))->name,NULL);
		if(g_list_first(gl_selecting)==gl_lib)
			pgl->flag=0;
		else if(g_list_first(gl_selecting)==gl_sublib)
			pgl->flag=1;
		else if(g_list_first(gl_selecting)==gl_playlist)
			pgl->flag=2;
		else
			pgl->flag=0;
		pgl->md=qlink;
		gl_playinglist=g_list_append(gl_playinglist,pgl);
		q=q->next;
	}
	gl_playing=lib_find_pgl_by_index(gl_playinglist,index[0]);
	//播放
	if(co_get_state()==GST_STATE_PLAYING)
	{
		co_stop();
		co_play();
	}
	else
	{
		co_stop();
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(gui_window_button_play), FALSE);
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(gui_window_button_play), TRUE);
	}
	gui_window_update_display();
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_treeview_left_change
//*	DISCRIPTION:	change the left tree selection
//*
//********************************************************************************
gboolean gui_treeview_left_change(GtkWidget *widget, gpointer data)
{
	GList *selectlist=NULL;
	GtkTreeModel *model;
	
	gint *index;
	
	model=GTK_TREE_MODEL(gui_window_store_left);
	
	selectlist=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gui_window_selection_left),&model);
	if(g_list_length(selectlist)!=1)
		return FALSE;
	
	if(gtk_tree_path_get_depth((GtkTreePath*)(selectlist->data))<2)
		return FALSE;
	
	index=gtk_tree_path_get_indices((GtkTreePath*)(selectlist->data));
	
	switch(index[0])
	{
		case 0:
			gl_selecting=gl_lib;
			break;
		case 1:
			gl_selecting=lib_find_pl_by_index(gl_sublib,index[1]);
			break;
		case 2:
			gl_selecting=lib_find_pl_by_index(gl_playlist,index[1]);
			break;
		default:
			break;
	}
	
	gui_treeview_right_refresh();
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_treeview_left_mask
//*	DISCRIPTION:	mask
//*
//********************************************************************************
gboolean gui_treeview_left_mask(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button==3)
	{
		GtkTreeModel *model;
		GList *selectlist=gtk_tree_selection_get_selected_rows(gui_window_selection_left,&model);
		if (g_list_length(selectlist)>1)
			return TRUE;
	}
	return FALSE;
}
//********************************************************************************
//*	
//*	FUNCTION:		gui_treeview_right_mask
//*	DISCRIPTION:	mask
//*
//********************************************************************************
gboolean gui_treeview_right_mask(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button==3)
	{
		GtkTreeModel *model;
		GList *selectlist=gtk_tree_selection_get_selected_rows(gui_window_selection_right,&model);
		if (g_list_length(selectlist)>1)
			return TRUE;
	}
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_treeview_left_popup
//*	DISCRIPTION:	popup right button menu on left treeview
//*
//********************************************************************************
gboolean gui_treeview_left_popup(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	gchar *menu_left[LEFT_NUM]={
						_("Add Playlist"),
						_("Import from File"),
						_("Add to Play Queue"),
						_("Rename"),
						_("Export to File"),
						_("Sort by Name"),
						_("Refresh"),
						_("Delete"),
						_("Delete All")
					};
					
	GtkTreeModel *model;

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
			if(i==LEFT_ADDTOQUEUE||i==LEFT_SORT||i==LEFT_DELETE)
			{
				hseparator=gtk_separator_menu_item_new();
				gtk_menu_append(menu,hseparator);
			}
			menuitem[i]=gtk_menu_item_new_with_label(menu_left[i]);
			gtk_menu_append(menu,menuitem[i]);
		}
		g_signal_connect(G_OBJECT(menuitem[LEFT_ADD]), "activate", G_CALLBACK(gui_treeview_callback_left_add),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_IMPORT]), "activate", G_CALLBACK(gui_treeview_callback_left_import),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_ADDTOQUEUE]), "activate", G_CALLBACK(gui_treeview_callback_left_addtoqueue),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_RENAME]), "activate", G_CALLBACK(gui_treeview_callback_left_rename),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_EXPORT]), "activate", G_CALLBACK(gui_treeview_callback_left_export),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_SORT]), "activate", G_CALLBACK(gui_treeview_callback_left_sort),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_REFRESH]), "activate", G_CALLBACK(gui_treeview_left_refresh),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_DELETE]), "activate", G_CALLBACK(gui_treeview_callback_left_delete),NULL);
		g_signal_connect(G_OBJECT(menuitem[LEFT_DELETEALL]), "activate", G_CALLBACK(gui_treeview_callback_left_deleteall),NULL);

		model=gtk_tree_view_get_model(GTK_TREE_VIEW(gui_window_tree_left));
		selectlist=gtk_tree_selection_get_selected_rows(gui_window_selection_left,&model);
		if(g_list_length(selectlist)==0)
		{
			gtk_widget_set_sensitive(menuitem[LEFT_ADDTOQUEUE],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_RENAME],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_EXPORT],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_SORT],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETE],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETEALL],FALSE);
			gtk_widget_show_all(menu);
			gtk_menu_popup (GTK_MENU(menu),NULL, NULL, NULL, NULL,event->button, event->time);
			return FALSE;
		}
		gint depth;
		gint* index;
		depth=gtk_tree_path_get_depth((GtkTreePath*)(selectlist->data));
		index=gtk_tree_path_get_indices((GtkTreePath*)(selectlist->data));
		if(depth<2)
		{
			gtk_widget_set_sensitive(menuitem[LEFT_RENAME],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_EXPORT],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETE],FALSE);
		}
		if(index[0]<2)
		{
			gtk_widget_set_sensitive(menuitem[LEFT_RENAME],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETE],FALSE);
			gtk_widget_set_sensitive(menuitem[LEFT_DELETEALL],FALSE);
		}
		gtk_widget_show_all(menu);
		gtk_menu_popup (GTK_MENU(menu),NULL, NULL, NULL, NULL,event->button, event->time);
	}
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_treeview_right_popup
//*	DISCRIPTION:	popup right button menu on right treeview
//*
//********************************************************************************
gboolean gui_treeview_right_popup(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	gchar *menu_right[RIGHT_NUM]={
						_("Play"),
						_("Add to Play Queue"),
						_("Add to Playlist"),
//						_("Convert"),
						_("Information"),
						_("Add Music from File"),
						_("Sort by"),
						_("Refresh"),
						_("Delete"),
						_("Delete All")
					};
					
	GtkTreeModel *model;

	GtkWidget	*menu=NULL;
	GtkWidget	*menuitem[RIGHT_NUM];
	GtkWidget	*submenu;
	GtkWidget	*submenuitem;
	GtkWidget	*hseparator;
	
	GList *p;
	_gl_playlist *plink;
	
	GList *selectlist;
	gint i=0;
	gint j=0;
	
	if (event->button==3) 
	{
		menu=gtk_menu_new();
		for(i=0;i<RIGHT_NUM;i++)
		{
			if(i==RIGHT_ADDFROMFILE||i==RIGHT_SORT||i==RIGHT_DELETE)
			{
				hseparator=gtk_separator_menu_item_new();
				gtk_menu_append(menu,hseparator);
			}
			menuitem[i]=gtk_menu_item_new_with_label(menu_right[i]);
			gtk_menu_append(menu,menuitem[i]);
			if(i==RIGHT_ADDTOPLAYLIST)
			{
				submenu=gtk_menu_new();
				p=gl_playlist;
				j=0;
				while(p)
				{
					plink=p->data;
					submenuitem=NULL;
					submenuitem=gtk_menu_item_new_with_label(plink->name);
					g_signal_connect(G_OBJECT(submenuitem), "button_press_event",G_CALLBACK(gui_treeview_callback_right_addtoplaylist),(gpointer)&j);
					j++;
					gtk_menu_append(submenu,submenuitem);
					
					if(g_list_first(gl_selecting)==gl_playlist)
					{
						if(g_strcmp0(((_gl_playlist*)(gl_selecting->data))->name,plink->name)==0)
							gtk_widget_set_sensitive(submenuitem,FALSE);
					}
					p=p->next;
				}
				submenuitem=NULL;
				submenuitem=gtk_menu_item_new_with_label(_("New Playlist"));
				g_signal_connect(G_OBJECT(submenuitem), "button_press_event",G_CALLBACK(gui_treeview_callback_right_addtoplaylist),(gpointer)&j);
				gtk_menu_append(submenu,submenuitem);
				gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem[RIGHT_ADDTOPLAYLIST]),submenu);
			}
			if(i==RIGHT_SORT)
			{
				submenu=gtk_menu_new();
				submenuitem=NULL;
				submenuitem=gtk_menu_item_new_with_label(_("Title"));
				g_signal_connect(G_OBJECT(submenuitem), "button_press_event",G_CALLBACK(gui_treeview_callback_right_sort),(gpointer)0);
				gtk_menu_append(submenu,submenuitem);
				submenuitem=gtk_menu_item_new_with_label(_("Artist"));
				g_signal_connect(G_OBJECT(submenuitem), "button_press_event",G_CALLBACK(gui_treeview_callback_right_sort),(gpointer)1);
				gtk_menu_append(submenu,submenuitem);
				submenuitem=gtk_menu_item_new_with_label(_("Album"));
				g_signal_connect(G_OBJECT(submenuitem), "button_press_event",G_CALLBACK(gui_treeview_callback_right_sort),(gpointer)2);
				gtk_menu_append(submenu,submenuitem);
				gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem[RIGHT_SORT]),submenu);
			}
		}
		g_signal_connect(G_OBJECT(menuitem[RIGHT_PLAY]), "activate", G_CALLBACK(gui_treeview_callback_right_play),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_ADDTOQUEUE]), "activate", G_CALLBACK(gui_treeview_callback_right_addtoqueue),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_INFORMATION]), "activate", G_CALLBACK(gui_treeview_callback_right_information),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_ADDFROMFILE]), "activate", G_CALLBACK(gui_treeview_callback_right_addfromfile),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_REFRESH]), "activate", G_CALLBACK(gui_treeview_right_refresh),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_DELETE]), "activate", G_CALLBACK(gui_treeview_callback_right_delete),NULL);
		g_signal_connect(G_OBJECT(menuitem[RIGHT_DELETEALL]), "activate", G_CALLBACK(gui_treeview_callback_right_deleteall),NULL);
		
		model=gtk_tree_view_get_model(GTK_TREE_VIEW(gui_window_tree_right));
		selectlist=gtk_tree_selection_get_selected_rows(gui_window_selection_right,&model);
		
		if(g_list_length(selectlist)==0)
		{
			gtk_widget_set_sensitive(menuitem[RIGHT_PLAY],FALSE);
			gtk_widget_set_sensitive(menuitem[RIGHT_ADDTOQUEUE],FALSE);
			gtk_widget_set_sensitive(menuitem[RIGHT_ADDTOPLAYLIST],FALSE);
//			gtk_widget_set_sensitive(menuitem[RIGHT_CONVERT],FALSE);
			gtk_widget_set_sensitive(menuitem[RIGHT_INFORMATION],FALSE);
			gtk_widget_set_sensitive(menuitem[RIGHT_DELETE],FALSE);
		}
		else if(g_list_length(selectlist)>1)
		{
			gtk_widget_set_sensitive(menuitem[RIGHT_PLAY],FALSE);
//			gtk_widget_set_sensitive(menuitem[RIGHT_CONVERT],FALSE);
			gtk_widget_set_sensitive(menuitem[RIGHT_INFORMATION],FALSE);
		}
		if(g_list_first(gl_selecting)==gl_sublib)
		{
			gtk_widget_set_sensitive(menuitem[RIGHT_ADDFROMFILE],FALSE);
		}
		if(g_list_first(gl_selecting)==gl_sublib||g_list_first(gl_selecting)==gl_lib)
		{
			gtk_widget_set_sensitive(menuitem[RIGHT_DELETEALL],FALSE);
		}
		gtk_widget_show_all(menu);
		gtk_menu_popup (GTK_MENU(menu),NULL, NULL, NULL, NULL,event->button, event->time);
	}
	return FALSE;
}
///////////////////////////////////////LEFT FUNCTION///////////////////////////////////////////////////////////
gboolean gui_treeview_callback_left_add(GtkWidget *widget, gpointer data)
{
	GtkTreeIter iter,subiter;
	
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	gint result;
	gchar *name=NULL;
	
	GList *p;
	_gl_playlist *plink;
	
	dialog=gtk_dialog_new_with_buttons(	_("New Playlist"),
									GTK_WINDOW(gui_window),
									GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_STOCK_OK,
									GTK_RESPONSE_ACCEPT,
									GTK_STOCK_CANCEL,
									GTK_RESPONSE_REJECT,
									NULL);
	gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox,TRUE,TRUE,0);
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
	name=(gchar*)gtk_entry_get_text(GTK_ENTRY(entry));
	result=0;
	if(name&&g_strcmp0(name,"")!=0)
		result=pl_add_playlist(name);
	else
		result=pl_add_playlist(NULL);
	gtk_widget_destroy(dialog);
	
	if(result==0)
	{
		de_dialog_print(gui_window,__DE_ERROR,__FUNCTION__,_("A playlist with the same name are exist! Add Failed!"));
		return FALSE;
	}
	//更新显示
	p=g_list_last(gl_playlist);
	plink=p->data;
	gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(gui_window_store_left),&iter,"2");
	gtk_tree_store_append (gui_window_store_left,&subiter, &iter);
	gtk_tree_store_set(gui_window_store_left, &subiter,0,NULL,1,plink->name,-1);
	return FALSE;
}

gboolean gui_treeview_callback_left_import(GtkWidget *widget, gpointer data)
{
	GSList *filelist;
	GSList *q;
	gchar *filename;
	GtkFileFilter *filter;
	GtkWidget *dialog;
	
	dialog =gtk_file_chooser_dialog_new(	_("Import Playlist..."),
						GTK_WINDOW(gui_window),
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
		pl_import_playlist(filename);
		g_free(filename);
		q=q->next;
	}
	g_slist_free(filelist);
	
	gui_treeview_left_refresh();
	gtk_widget_destroy (dialog);
	return FALSE;
}

gboolean gui_treeview_callback_left_addtoqueue(GtkWidget *widget, gpointer data)
{
	_gl_playinglist *pgl;
	
	GList *q;
	_gl_metadata *qlink;
	
	//把gl_selecting所指向的列表载入gl_playinglist
	q=((_gl_playlist*)(gl_selecting->data))->list;
	while(q)
	{
		qlink=q->data;
		pgl=(_gl_playinglist*)g_malloc(sizeof(_gl_playinglist));
		pgl->name=g_strconcat(((_gl_playlist*)(gl_selecting->data))->name,NULL);
		if(g_list_first(gl_selecting)==gl_lib)
			pgl->flag=0;
		else if(g_list_first(gl_selecting)==gl_sublib)
			pgl->flag=1;
		else if(g_list_first(gl_selecting)==gl_playlist)
			pgl->flag=2;
		else
			pgl->flag=0;
		pgl->md=qlink;
		pl_add_to_playinglist(pgl);
		q=q->next;
	}
	return FALSE;
}
gboolean gui_treeview_callback_left_rename(GtkWidget *widget, gpointer data)
{
	GtkTreeIter iter;
	
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	gint result;
	gchar *name=NULL;
	
	dialog=gtk_dialog_new_with_buttons(	_("New Playlist"),
									GTK_WINDOW(gui_window),
									GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_STOCK_OK,
									GTK_RESPONSE_ACCEPT,
									GTK_STOCK_CANCEL,
									GTK_RESPONSE_REJECT,
									NULL);
	gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox,TRUE,TRUE,0);
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
	name=g_strconcat((gchar*)gtk_entry_get_text(GTK_ENTRY(entry)),NULL);
	gtk_widget_destroy(dialog);
	if(name&&g_strcmp0(name,"")==0)
	{
		g_free(name);
		de_dialog_print(gui_window,__DE_ERROR,__FUNCTION__,_("Illegal playlist name!!"));
		return FALSE;
	}
	if(lib_find_pl_by_name(gl_playlist,name)!=NULL)
	{
		g_free(name);
		de_dialog_print(gui_window,__DE_ERROR,__FUNCTION__,_("A playlist with the same name are exist!"));
		return FALSE;
	}
	((_gl_playlist*)(gl_selecting->data))->name=g_strconcat(name,NULL);
	//更新显示
	int i=g_list_index(gl_playlist,gl_selecting->data);
	gchar *str=(gchar*)g_malloc(sizeof(64));
	g_snprintf(str,sizeof(str),"2:%d",i);
	gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(gui_window_store_left),&iter,str);
	gtk_tree_store_set(gui_window_store_left, &iter,1,name,-1);
	g_free(name);
	g_free(str);
	return FALSE;
}
gboolean gui_treeview_callback_left_export(GtkWidget *widget, gpointer data)
{
	gchar *filename;
	gchar *type;
	GtkFileFilter *filter;
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *combo;
	
	dialog =gtk_file_chooser_dialog_new(	_("Export Playlist..."),
						GTK_WINDOW(gui_window),
						GTK_FILE_CHOOSER_ACTION_SAVE,
						GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
						NULL);
	filter=gtk_file_filter_new();
	gtk_file_filter_set_name(filter,_("Playlist File"));
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.cue");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.CUE");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.m3u");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.M3U");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog),GTK_FILE_FILTER(filter));
	hbox=gtk_hbox_new(FALSE,0);
	gtk_file_chooser_set_extra_widget (GTK_FILE_CHOOSER (dialog), hbox);
	label=gtk_label_new(_("File Type:"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	combo=gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo),".M3U");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo),".CUE");
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo),0);
	gtk_box_pack_start(GTK_BOX(hbox),combo,TRUE,TRUE,0);
	gtk_widget_show_all(dialog);
	
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
	filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
	type=gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
	if(filename==NULL||g_strcmp0(filename,NULL)==0)
	{
		gtk_widget_destroy (dialog);
		return FALSE;
	}
	if(com_get_filetype(filename)!=com_type_cue&&com_get_filetype(filename)!=com_type_m3u)
	{
		gchar *temp=g_strconcat(filename,type,NULL);
		g_free(filename);
		filename=temp;
	}
	gtk_widget_destroy (dialog);
	puts(filename);
	pl_export_playlist(gl_selecting,filename);
	g_free(filename);
	
	return FALSE;
}
gboolean gui_treeview_callback_left_sort(GtkWidget *widget, gpointer data)
{
	if(g_list_first(gl_selecting)==gl_lib)
		gl_lib=lib_order_pl_by_name(gl_lib);
	else if(g_list_first(gl_selecting)==gl_sublib)
		gl_sublib=lib_order_pl_by_name(gl_sublib);
	else if(g_list_first(gl_selecting)==gl_playlist)
		gl_playlist=lib_order_pl_by_name(gl_playlist);
	gui_treeview_left_refresh();
	return FALSE;
}
gboolean gui_treeview_callback_left_delete(GtkWidget *widget, gpointer data)
{
	GtkTreeIter iter;
	gchar *str=(gchar *)g_malloc(64*sizeof(gchar));
	g_snprintf(str,sizeof(str),"2:%d",g_list_index(gl_playlist,gl_selecting->data));
	gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(gui_window_store_left),&iter,str);
	gtk_tree_store_remove((GtkTreeStore *)(gui_window_store_left),&iter);
	pl_remove_playlist(gl_selecting);
	gl_selecting=gl_lib;
	g_free(str);
	return FALSE;
}
gboolean gui_treeview_callback_left_deleteall(GtkWidget *widget, gpointer data)
{
	int i=0;
	GtkTreeIter iter;
	for(i=0;i<g_list_length(gl_playlist);i++)
	{
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(gui_window_store_left),&iter,"2:0");
		gtk_tree_store_remove((GtkTreeStore *)(gui_window_store_left),&iter);
		g_list_free(((_gl_playlist*)(g_list_nth_data(gl_playlist,i)))->list);
		((_gl_playlist*)(g_list_nth_data(gl_playlist,i)))->list=NULL;
	}
	
	gl_free_list_playlist(gl_playlist);
	gl_playlist=NULL;
	gl_selecting=gl_lib;
	return FALSE;
}
///////////////////////////////////////RIGHT FUNCTION//////////////////////////////////////////////////////////
gboolean gui_treeview_callback_right_play(GtkWidget *widget, gpointer data)
{
	GtkTreeModel  *model;
	
	GList *q;
	_gl_metadata *qlink;
	_gl_playinglist *pgl;
	GList *selectlist;
	
	gint *index;
	
	model=gtk_tree_view_get_model(GTK_TREE_VIEW(gui_window_tree_right));
	selectlist=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gui_window_selection_right),&model);
	if(g_list_length(selectlist)!=1)
		return FALSE;
	
	index=gtk_tree_path_get_indices((GtkTreePath*)(selectlist->data));
	
	gl_played=pl_find_pl_by_playing();
	
	//释放gl_playinglist
	gl_free_list_playinglist(gl_playinglist);
	gl_playing=NULL;
	gl_playinglist=NULL;
	
	//把gl_selecting所指向的列表载入gl_playinglist
	q=((_gl_playlist*)(gl_selecting->data))->list;
	while(q)
	{
		qlink=q->data;
		pgl=(_gl_playinglist*)g_malloc(sizeof(_gl_playinglist));
		pgl->name=g_strconcat(((_gl_playlist*)(gl_selecting->data))->name,NULL);
		if(g_list_first(gl_selecting)==gl_lib)
			pgl->flag=0;
		else if(g_list_first(gl_selecting)==gl_sublib)
			pgl->flag=1;
		else if(g_list_first(gl_selecting)==gl_playlist)
			pgl->flag=2;
		else
			pgl->flag=0;
		pgl->md=qlink;
		pl_add_to_playinglist(pgl);
		q=q->next;
	}
	gl_playing=lib_find_pgl_by_index(gl_playinglist,index[0]);
	//播放
	if(co_get_state()==GST_STATE_PLAYING)
	{
		co_stop();
		co_play();
	}
	else
	{
		co_stop();
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(gui_window_button_play), TRUE);
	}
	gui_window_update_display();
	return FALSE;
}
gboolean gui_treeview_callback_right_addtoqueue(GtkWidget *widget, gpointer data)
{
	GtkTreeModel  *model;
	
	_gl_playinglist *pgl;
	GList *selectlist;
	GList *r;
	GtkTreePath *rlink;
	
	gint *index;
	
	model=gtk_tree_view_get_model(GTK_TREE_VIEW(gui_window_tree_right));
	selectlist=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gui_window_selection_right),&model);
	if(g_list_length(selectlist)==0)
		return FALSE;
	
	//把gl_selecting所指向的列表载入gl_playinglist
	r=selectlist;
	while(r)
	{
		rlink=r->data;
		index=gtk_tree_path_get_indices(rlink);
		pgl=(_gl_playinglist*)g_malloc(sizeof(_gl_playinglist));
		pgl->name=g_strconcat(((_gl_playlist*)(gl_selecting->data))->name,NULL);
		if(g_list_first(gl_selecting)==gl_lib)
			pgl->flag=0;
		else if(g_list_first(gl_selecting)==gl_sublib)
			pgl->flag=1;
		else if(g_list_first(gl_selecting)==gl_playlist)
			pgl->flag=2;
		else
			pgl->flag=0;
		pgl->md=lib_find_md_by_index(((_gl_playlist*)(gl_selecting->data))->list,index[0])->data;
		pl_add_to_playinglist(pgl);
		r=r->next;
	}
	return FALSE;
}
gboolean gui_treeview_callback_right_addtoplaylist(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	GtkTreeModel  *model;
	GtkTreeIter iter,subiter;
	
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	gint result;
	gchar *name=NULL;
	
	GList *p;
	_gl_playlist *plink;
	GList *selectlist;
	GList *r;
	GtkTreePath *rlink;
	
	gint *index;
	gint *l=data;
	gint i=*l;
	
	model=gtk_tree_view_get_model(GTK_TREE_VIEW(gui_window_tree_right));
	selectlist=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gui_window_selection_right),&model);
	if(g_list_length(selectlist)==0)
		return FALSE;
	
	
	p=lib_find_pl_by_index(gl_playlist,i);
	if(!p)
	{
		dialog=gtk_dialog_new_with_buttons(	_("New Playlist"),
									GTK_WINDOW(gui_window),
									GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_STOCK_OK,
									GTK_RESPONSE_ACCEPT,
									GTK_STOCK_CANCEL,
									GTK_RESPONSE_REJECT,
									NULL);
		gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
		hbox=gtk_hbox_new(FALSE,0);
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox,TRUE,TRUE,0);
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
		name=(gchar*)gtk_entry_get_text(GTK_ENTRY(entry));
		if(name&&g_strcmp0(name,"")!=0)
			pl_add_playlist(name);
		else
			pl_add_playlist(NULL);
		gtk_widget_destroy(dialog);
		
		if(result==0)
		{
			de_dialog_print(gui_window,__DE_ERROR,__FUNCTION__,_("A playlist with the same name are exist! Add Failed!"));
			return FALSE;
		}
		
		p=g_list_last(gl_playlist);
		plink=p->data;
		//更新显示
		if(gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(gui_window_store_left),&iter,"2"))
		gtk_tree_store_append (gui_window_store_left,&subiter, &iter);
		gtk_tree_store_set(gui_window_store_left, &subiter,0,NULL,1,plink->name,-1);
	}
	//把gl_selecting所指向的列表载入gl_playinglist
	r=selectlist;
	while(r)
	{
		rlink=r->data;
		index=gtk_tree_path_get_indices(rlink);
		pl_add_to_playlist(p,lib_find_md_by_index(((_gl_playlist*)(gl_selecting->data))->list,index[0])->data);
		r=r->next;
	}
	return FALSE;
}

static gboolean gui_treeview_callback_callback_get_metadata(GstBus *bus, GstMessage *message, gpointer data)
{
	gchar *tag_genre=NULL;
	guint tag_track=0;
	gchar *tag_comment=NULL;
	guint tag_bitrate=0;
	
	switch(GST_MESSAGE_TYPE(message))
	{
		case GST_MESSAGE_EOS:
		{
			eos = TRUE;
			return TRUE;
		}
		case GST_MESSAGE_ERROR:
		{
			eos = TRUE;
			return TRUE;
		}
		case GST_MESSAGE_TAG:
		{
			GstTagList *tags;
			gst_message_parse_tag(message,&tags);
			//获取比特率
			if(gst_tag_list_get_uint(tags,GST_TAG_BITRATE,&tag_bitrate))
			{
				if(gui_treeview_info_bitrate!=NULL)
					g_free(gui_treeview_info_bitrate);
				gui_treeview_info_bitrate = (gchar*)g_malloc(256);
				g_snprintf(gui_treeview_info_bitrate,256,"%02d kbps",tag_bitrate/1000);

			}
			//获取风格
			if(gst_tag_list_get_string(tags,GST_TAG_GENRE,&tag_genre))
			{
				if(g_utf8_validate(tag_genre,-1,NULL))
				{
					if(gui_treeview_info_genre!=NULL)
						g_free(gui_treeview_info_genre);
					gui_treeview_info_genre = (gchar*)g_malloc(256);
					gui_treeview_info_genre = g_utf8_strncpy(gui_treeview_info_genre,tag_genre,255);
					gui_treeview_info_genre[255]='\0';
				}
				g_free(tag_genre);
			}
			//获取曲目号
			if(gst_tag_list_get_uint(tags,GST_TAG_TRACK_NUMBER,&tag_track))
			{
				gui_treeview_info_track = (gint)tag_track;
			}
			gst_tag_list_free(tags);
			return TRUE;
			break;
		}
		default:
			break;
	}
	return FALSE;
}
static void gui_treeview_callback_callback_event_loop(GstElement *element, gboolean block)
{
	GstBus *bus;
	GstMessage *message;
	gboolean done = FALSE;
	bus = gst_element_get_bus(element);
	g_return_if_fail(bus!=NULL);
	while(!done && !eos)
	{
		if(block)
			message = gst_bus_timed_pop(bus, GST_CLOCK_TIME_NONE);
		else
			message = gst_bus_timed_pop(bus, 0);
		if(message==NULL)
		{
			gst_object_unref(bus);
			return;
		}
		done = gui_treeview_callback_callback_get_metadata(bus,message,NULL);
		gst_message_unref(message);
	}
	gst_object_unref(bus);
}
static void gui_treeview_callback_callback_build_pipeline(GstElement *decodebin,GstPad *pad, gboolean last, gpointer data)
{
	static GstCaps *caps;
	static GstPad *sink_pad;
	
	caps = gst_pad_get_caps(pad);
	if(gst_caps_is_empty(caps) || gst_caps_is_any(caps))
	{
		de_print(__DE_ERROR,__FUNCTION__,_("Decoded pad with no caps or any caps, boring."));
	}
	else
	{
		sink_pad = gst_element_get_static_pad(GST_ELEMENT(data), "sink");
		gst_pad_link(pad, sink_pad);
		gst_object_unref(sink_pad);
	}
	gst_caps_unref (caps);
}

static void gui_treeview_callback_add_ogg_pad(GstElement *demux, GstPad *pad,GstElement *tagger)
{
	GstCaps *caps;
	GstPad *conn_pad = NULL;
	caps = gst_pad_get_caps (pad);
	conn_pad = gst_element_get_compatible_pad(tagger, pad, NULL);
	gst_pad_link(pad, conn_pad);
	gst_object_unref(conn_pad);
}
static void gui_treeview_callback_add_id3_pad(GstElement *demux, GstPad *pad,GstElement *tagger)
{
	GstCaps *caps;
	GstPad *conn_pad = NULL;
	caps = gst_pad_get_caps (pad);
	conn_pad = gst_element_get_compatible_pad(tagger, pad, NULL);
	gst_pad_link(pad, conn_pad);
	gst_object_unref(conn_pad);
}

static void gui_treeview_callback_move_file()
{
	if(pipeline!=NULL)
	{
		gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(pipeline);
		pipeline = NULL;
		gint output;
		gchar *cmd;
		cmd=g_strconcat("mv /tmp/temp.audio \"",location,"\"",NULL);
		output=system(cmd);
		gui_treeview_info_lock=FALSE;
	}
}

static int tag_setter_handler(GstBus *bus, GstMessage *msg, gpointer data)
{
	switch(GST_MESSAGE_TYPE(msg))
	{
		case GST_MESSAGE_EOS:
			gui_treeview_callback_move_file();
			break;
		default:
			break;
	}
	return GST_BUS_ASYNC;
}

gboolean gui_treeview_callback_set_changed3(GtkWidget *widget, gpointer data)
{
	gint* changed=data;
	*changed=3;
	return FALSE;
}
gboolean gui_treeview_callback_set_changed1(GtkWidget *widget, gpointer data)
{
	gint* changed=data;
	*changed=1;
	return FALSE;
}
gboolean gui_treeview_callback_set_changed2(GtkWidget *widget, gpointer data)
{
	gint* changed=data;
	*changed=2;
	return FALSE;
}



gboolean gui_treeview_callback_right_information(GtkWidget *widget, gpointer data)
{
	pipeline=NULL;
	eos=FALSE;
	
	//获取选中音乐
	GtkTreeModel  *model;
	GList *selectlist;
	gint *index;
	
	GList *list;
	_gl_metadata *md;
	
	model=gtk_tree_view_get_model(GTK_TREE_VIEW(gui_window_tree_right));
	selectlist=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gui_window_selection_right),&model);
	if(g_list_length(selectlist)==0)
		return FALSE;
	index=gtk_tree_path_get_indices(selectlist->data);
	list=lib_find_md_by_index(((_gl_playlist*)(gl_selecting->data))->list,index[0]);
	md=list->data;
	
	if(g_strcmp0(md->uri,gui_treeview_info_last_uri)==0)
	{
		//等待上一个事件结束
		while(gui_treeview_info_lock);
		gui_treeview_info_lock=TRUE;
		gui_treeview_info_last_uri=md->uri;
	}

	//给存储变量赋初值
	gui_treeview_info_title=g_strconcat(md->title,NULL);
	gui_treeview_info_artist=g_strconcat(md->artist,NULL);
	gui_treeview_info_album=g_strconcat(md->album,NULL);
	gui_treeview_info_genre=g_strconcat("unknown",NULL);
	gui_treeview_info_track=0;
	gui_treeview_info_codec=g_strconcat(md->codec,NULL);
	gui_treeview_info_start=g_strconcat(md->start,NULL);
	gui_treeview_info_duration=g_strconcat(md->duration,NULL);
	gui_treeview_info_bitrate=g_strconcat("unknown",NULL);
	gui_treeview_info_uri=g_strconcat(md->uri,NULL);
	gui_treeview_info_pixbuf=gl_pixbuf_cd;

	//链接管道获取信息
	GstElement *urisrc;
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
	
	//建立对话框
	enum{	INFO_TITLE,
			INFO_ARTIST,
			INFO_ALBUM,
			INFO_GENRE,
			INFO_TRACK,
			INFO_NUM
		};
	GtkWidget *dialog,*dialog1;
	GtkWidget *frame;
	GtkWidget *hbox;
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *entry[INFO_NUM];
	GtkWidget *image;

	gchar *track=(gchar*)g_malloc(64);
	g_snprintf(track,63,"%02d",gui_treeview_info_track);
	dialog=gtk_dialog_new_with_buttons(	_("Information"),
										   GTK_WINDOW(gui_window),
										   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
										   GTK_STOCK_OK,
										   GTK_RESPONSE_ACCEPT,
										   GTK_STOCK_CANCEL,
										   GTK_RESPONSE_REJECT,
										   NULL);
	gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
	gtk_window_set_default_size(GTK_WINDOW(dialog),400,350);

	gint changed=0;
	
	frame=gtk_frame_new(_("Basic Information"));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),frame,TRUE,TRUE,0);
	hbox=gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame),hbox);
	table=gtk_table_new(5,2,FALSE);
	gtk_container_border_width(GTK_CONTAINER(table),5);
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
	label=gtk_label_new(gui_treeview_info_codec);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(gui_treeview_info_start);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(gui_treeview_info_duration);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(gui_treeview_info_bitrate);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(gui_treeview_info_uri);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_MIDDLE);
	gtk_label_set_max_width_chars(GTK_LABEL(label),25);
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 4, 5, GTK_FILL, GTK_FILL, 5, 5);
	image=gtk_image_new_from_pixbuf(gui_treeview_info_pixbuf);
	gtk_table_attach(GTK_TABLE(table), image, 2, 3, 0, 5, GTK_FILL, GTK_FILL, 5, 5);

	frame=gtk_frame_new(_("Advance Information"));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),frame,TRUE,TRUE,0);
	table=gtk_table_new(5,2,FALSE);
	gtk_container_border_width(GTK_CONTAINER(table),5);
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
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_TITLE]),gui_treeview_info_title);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_TITLE], 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	g_signal_connect(G_OBJECT(entry[INFO_TITLE]),"changed",G_CALLBACK(gui_treeview_callback_set_changed3),&changed);
	entry[INFO_ARTIST]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_ARTIST]),gui_treeview_info_artist);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_ARTIST], 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	g_signal_connect(G_OBJECT(entry[INFO_ARTIST]),"changed",G_CALLBACK(gui_treeview_callback_set_changed1),&changed);
	entry[INFO_ALBUM]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_ALBUM]),gui_treeview_info_album);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_ALBUM], 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	g_signal_connect(G_OBJECT(entry[INFO_ALBUM]),"changed",G_CALLBACK(gui_treeview_callback_set_changed2),&changed);
	entry[INFO_GENRE]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_GENRE]),gui_treeview_info_genre);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_GENRE], 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	g_signal_connect(G_OBJECT(entry[INFO_GENRE]),"changed",G_CALLBACK(gui_treeview_callback_set_changed3),&changed);
	entry[INFO_TRACK]=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry[INFO_TRACK]),track);
	gtk_table_attach(GTK_TABLE(table), entry[INFO_TRACK], 1, 2, 4, 5, GTK_FILL, GTK_FILL, 5, 5);
	g_signal_connect(G_OBJECT(entry[INFO_TRACK]),"changed",G_CALLBACK(gui_treeview_callback_set_changed3),&changed);

	//释放
	g_free(gui_treeview_info_codec);
	g_free(gui_treeview_info_start);
	g_free(gui_treeview_info_duration);
	g_free(gui_treeview_info_bitrate);
	g_free(gui_treeview_info_uri);
	g_free(gui_treeview_info_title);
	g_free(gui_treeview_info_artist);
	g_free(gui_treeview_info_album);
	g_free(gui_treeview_info_genre);
	g_free(track);
	gui_treeview_info_codec=NULL;
	gui_treeview_info_start=NULL;
	gui_treeview_info_duration=NULL;
	gui_treeview_info_bitrate=NULL;
	gui_treeview_info_uri=NULL;
	gui_treeview_info_title=NULL;
	gui_treeview_info_artist=NULL;
	gui_treeview_info_album=NULL;
	gui_treeview_info_genre=NULL;
	track=NULL;
	
	gtk_widget_show_all(dialog);
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
	
	//确定写入metadata，执行
	if(changed==0)
	{
		gtk_widget_destroy (dialog);
		return FALSE;
	}
	
	if(md->title)
		g_free(md->title);
	md->title=g_strconcat(gtk_entry_get_text(GTK_ENTRY(entry[INFO_TITLE])),NULL);
	if(md->artist)
		g_free(md->artist);
	md->artist=g_strconcat(gtk_entry_get_text(GTK_ENTRY(entry[INFO_ARTIST])),NULL);
	if(md->album)
		g_free(md->album);
	md->album=g_strconcat(gtk_entry_get_text(GTK_ENTRY(entry[INFO_ALBUM])),NULL);

	if(g_strcmp0(md->start,"00:00:000")!=0)	//cue歌曲，TAG不写入文件
	{
		if(changed==gl_conf->sublib)
		{
			gl_free_list_playlist(gl_sublib);
			gl_sublib=NULL;
			lib_check_sublib();
			gl_selecting=gl_lib;
		}
		gui_treeview_left_refresh();
		gui_treeview_right_refresh();
		gtk_widget_destroy (dialog);
		return FALSE;
	}
	if((strstr(md->codec,"FLAC")!=NULL)||(strstr(md->codec,"Monkey's Audio")!=NULL))	//无损格式的文件，提示用户确认是否写入TAG,
																						//如果文件较大，可能引起崩溃
	{
		dialog1=gui_treeview_warnning_dialog_new(gui_window, _("This music file seems to be a Lossless Format,\nUsually a cue file should be used instead of the original file itself. \nDo you really want to tag the original music file?"));
		result=gtk_dialog_run(GTK_DIALOG(dialog1));
		switch(result)
		{
			case GTK_RESPONSE_ACCEPT:
				break;
			default:
				if(changed==gl_conf->sublib)
				{
					gl_free_list_playlist(gl_sublib);
					gl_sublib=NULL;
					lib_check_sublib();
					gl_selecting=gl_lib;
				}
				gui_treeview_left_refresh();
				gui_treeview_right_refresh();
				gtk_widget_destroy(dialog1);
				gtk_widget_destroy(dialog);
				return FALSE;
				break;
		}
		gtk_widget_destroy(dialog1);
	}
	
	location=(md->uri)+7;
	pipeline=NULL;
	GstElement *filesrc=NULL;
	GstElement *demux=NULL;
	GstElement *mux=NULL;
	GstElement *parse=NULL;
	GstElement *filesink=NULL;
	GstElement *tagger=NULL;
	bus = NULL;
	
	const gchar *codec=NULL;
	
	pipeline=gst_pipeline_new("pipeline");
	filesrc=gst_element_factory_make("filesrc","filesrc");
	filesink=gst_element_factory_make("filesink","filesink");
	if(!pipeline||!filesrc||!filesink)
	{
		gtk_widget_destroy (dialog);
		return FALSE;
	}
	
	//MP3
	if(strstr(md->codec,"MP3")!=NULL)
	{
		demux=gst_element_factory_make("id3demux","demux");
		tagger=gst_element_factory_make("id3v2mux","tagger");
		codec="LAME";
	}
	//OGG
	else if(strstr(md->codec,"Vorbis")!=NULL)
	{
		tagger = gst_element_factory_make("vorbistag", "tagger");
		demux=gst_element_factory_make("oggdemux","demux");
		mux=gst_element_factory_make("oggmux","mux");
		parse = gst_element_factory_make("vorbisparse", "parse");
		codec = "Vorbis";
	}
	//FLAC
	else if(strstr(md->codec,"FLAC")!=NULL)
	{
		tagger = gst_element_factory_make("flactag", "tagger");
		codec="FLAC";
	}
	//APE
	else if(strstr(md->codec,"Monkey's Audio")!=NULL)
	{
			demux=gst_element_factory_make("apedemux","demux");
			tagger=gst_element_factory_make("apev2mux","tagger");
			codec="LAME";
	}
	else
	{
		gtk_widget_destroy (dialog);
		return FALSE;
	}

	g_object_set(G_OBJECT(filesrc), "location", location, NULL);
	g_object_set(G_OBJECT(filesink), "location", "/tmp/temp.audio", NULL);

	sscanf(gtk_entry_get_text(GTK_ENTRY(entry[INFO_TRACK])),"%d", &gui_treeview_info_track);
	gst_tag_setter_add_tags(GST_TAG_SETTER(tagger),
							GST_TAG_MERGE_REPLACE_ALL,
							GST_TAG_TITLE, gtk_entry_get_text(GTK_ENTRY(entry[INFO_TITLE])),
							GST_TAG_ARTIST, gtk_entry_get_text(GTK_ENTRY(entry[INFO_ARTIST])),
							GST_TAG_ALBUM, gtk_entry_get_text(GTK_ENTRY(entry[INFO_ALBUM])),
							GST_TAG_GENRE, gtk_entry_get_text(GTK_ENTRY(entry[INFO_GENRE])),
							GST_TAG_TRACK_NUMBER, gui_treeview_info_track,
							GST_TAG_ENCODER, "Linnya",
							GST_TAG_ENCODER_VERSION, 0,
							GST_TAG_CODEC,codec,
							NULL);
	gtk_widget_destroy (dialog);
	
	//MP3
	if(strstr(md->codec,"MP3")!=NULL)
	{
		gst_bin_add_many(GST_BIN(pipeline), filesrc, demux,tagger,filesink, NULL);
		g_signal_connect(demux, "pad-added",G_CALLBACK(gui_treeview_callback_add_id3_pad), tagger);
		gst_element_link(filesrc, demux);
		gst_element_link(tagger, filesink);
	}
	//OGG
	else if(strstr(md->codec,"Vorbis")!=NULL)
	{
		gst_bin_add_many(GST_BIN(pipeline), filesrc, demux, tagger, parse, mux, filesink, NULL);
		g_signal_connect(demux, "pad-added",G_CALLBACK(gui_treeview_callback_add_ogg_pad), tagger);
		gst_element_link(filesrc, demux);
		gst_element_link_many(tagger, parse, mux, filesink,NULL);
	}
	//FLAC
	else if(strstr(md->codec,"FLAC")!=NULL)
	{
		gst_bin_add_many(GST_BIN(pipeline), filesrc, tagger,filesink, NULL);
		gst_element_link_many(filesrc, tagger, filesink, NULL);
	}
	//APE
	else if(strstr(md->codec,"Monkey's Audio")!=NULL)
	{
		gst_bin_add_many(GST_BIN(pipeline), filesrc, demux,tagger,filesink, NULL);
		g_signal_connect(demux, "pad-added",G_CALLBACK(gui_treeview_callback_add_id3_pad), tagger);
		gst_element_link(filesrc, demux);
		gst_element_link(tagger, filesink);
	}
	else
	{
		gst_bin_add_many(GST_BIN(pipeline), filesrc, tagger,filesink, NULL);
		gst_element_link_many(filesrc, tagger, filesink, NULL);
	}
	
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, (GstBusFunc)tag_setter_handler, NULL);
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_element_set_state(pipeline, GST_STATE_READY);
	if(gst_element_set_state(pipeline, GST_STATE_PLAYING)==GST_STATE_CHANGE_FAILURE)
	{
		gst_object_unref(pipeline);
	}
	if(changed==gl_conf->sublib)
	{
		gl_free_list_playlist(gl_sublib);
		gl_sublib=NULL;
		lib_check_sublib();
		gl_selecting=gl_lib;
	}
	gui_treeview_left_refresh();
	gui_treeview_right_refresh();
	return FALSE;
}
gboolean gui_treeview_callback_right_addfromfile(GtkWidget *widget, gpointer data)
{
	GSList *filelist;
	GSList *q;
	gchar *filename,*fileuri;
	_gl_metadata *md;
	GtkFileFilter *filter;
	GtkWidget *dialog;
	
	GList *p;
	_gl_metadata *plink;
	
	dialog =gtk_file_chooser_dialog_new(	_("Add From File..."),
						GTK_WINDOW(gui_window),
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
// 	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.mp4");
// 	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter),"*.MP4");
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
	while(q)
	{
		md=(_gl_metadata*)g_malloc(sizeof(_gl_metadata));
		
		filename=g_filename_from_uri (q->data,NULL,NULL);
		fileuri=g_strconcat("file://",filename,NULL);
		g_free(filename);
		
		p=lib_find_md_by_uri(((_gl_playlist*)(gl_lib->data))->list,"00:00:000",NULL,fileuri);
		if(p)
		{
			if(g_list_first(gl_selecting)!=gl_lib)
			{
				plink=p->data;
				if(!lib_find_md_by_uri(((_gl_playlist*)(gl_selecting->data))->list,plink->start,plink->duration,plink->uri))
				{
					pl_add_to_playlist(gl_selecting,p->data);
				}
			}
			q=q->next;
			continue;
		}
		md=md_get_metadata(fileuri);
		lib_add_md(md);
		
		if(g_list_first(gl_selecting)!=gl_lib)
		{
			if(!lib_find_md_by_uri(((_gl_playlist*)(gl_selecting->data))->list,md->start,md->duration,md->uri))
			{
				pl_add_to_playlist(gl_selecting,md);
			}
		}
		q=q->next;
	}
	g_slist_free(filelist);
	gl_free_list_playlist(gl_sublib);
	gl_sublib=NULL;
	lib_check_sublib();
	gui_treeview_left_refresh();
	gui_treeview_right_refresh();
	gtk_widget_destroy (dialog);
	return FALSE;
}
gboolean gui_treeview_callback_right_sort(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	gint i=(gint)data;
	GList *q;
	q=((_gl_playlist*)(gl_selecting->data))->list;
	switch(i)
	{
		case 0:
			((_gl_playlist*)(gl_selecting->data))->list=lib_order_md_by_title(q);
			break;
		case 1:
			((_gl_playlist*)(gl_selecting->data))->list=lib_order_md_by_artist(q);
			break;
		case 2:
			((_gl_playlist*)(gl_selecting->data))->list=lib_order_md_by_album(q);
			break;
		default:
			break;
	}
	gui_treeview_right_refresh();
	return FALSE;
}
gboolean gui_treeview_callback_right_delete(GtkWidget *widget, gpointer data)
{
	GtkTreeModel  *model;
	
	GList *selectlist;
	GList *r;
	GtkTreePath *rlink;
	GList *q;
	_gl_metadata *qlink;
	GList *s;
	
	gint *index;
	
	GtkWidget *dialog;
	gint result;
	
	model=gtk_tree_view_get_model(GTK_TREE_VIEW(gui_window_tree_right));
	selectlist=gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gui_window_selection_right),&model);
	if(g_list_length(selectlist)==0)
		return FALSE;

	if(g_list_first(gl_selecting)==gl_lib||g_list_first(gl_selecting)==gl_sublib)
	{
		dialog=gui_treeview_warnning_dialog_new(gui_window, _("<b>This is a Dangerous function!!</b>\n The music you wanna delete is belongs to your music library,\n It will be delete physically and permanently.\n Do you really want to do it?"));
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
	
	int i=-1;
	r=selectlist;
	while(r)
	{
		i++;
		rlink=r->data;
		index=gtk_tree_path_get_indices(rlink);
		if(g_list_first(gl_selecting)==gl_lib)
		{
			q=lib_find_md_by_index(((_gl_playlist *)(gl_lib->data))->list,index[0]-i);
			if(q)
			{
				qlink=q->data;
				s=gl_playinglist;
				while(s)
				{
					if(((_gl_playinglist*)(s->data))->md==qlink)
					{
						if(s==gl_playing)
						{
							gui_window_stop(gui_window,NULL);
							gl_playing=g_list_first(gl_playinglist);
						}
						pl_remove_md_from_playinglist(s);
					}
					s=s->next;
				}
				lib_delete_md(q);
			}
		}
		else if(g_list_first(gl_selecting)==gl_sublib)
		{
			q=lib_find_md_by_index(((_gl_playlist *)(gl_selecting->data))->list,index[0]-i);
			if(q)
			{
			qlink=q->data;
			q=lib_find_md_by_uri(((_gl_playlist *)(gl_lib->data))->list,qlink->start,qlink->duration,qlink->uri);
			if(q)
			{
				qlink=q->data;
				s=gl_playinglist;
				while(s)
				{
					if(((_gl_playinglist*)(s->data))->md==qlink)
					{
						if(s==gl_playing)
						{
							gui_window_stop(gui_window,NULL);
							gl_playing=g_list_first(gl_playinglist);
						}
						pl_remove_md_from_playinglist(s);
					}
					s=s->next;
				}
				lib_delete_md(q);
			}
			}
		}
		else if(g_list_first(gl_selecting)==gl_playlist)
		{
			q=lib_find_md_by_index(((_gl_playlist *)(gl_selecting->data))->list,index[0]-i);
			pl_remove_md_from_playlist(gl_selecting,q);
			pl_find_pl_by_playing();
		}
		r=r->next;
	}
	gui_treeview_right_refresh();
	return FALSE;
}
gboolean gui_treeview_callback_right_deleteall(GtkWidget *widget, gpointer data)
{
	
	GtkWidget *dialog;
	gint result;

	dialog=gui_treeview_warnning_dialog_new(gui_window, _("<b>This is a Dangerous function!!</b>\n It will delete all music belongs to the active list.\n Do you really want to do it?"));
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
	
	g_list_free(((_gl_playlist*)(gl_selecting->data))->list);
	((_gl_playlist*)(gl_selecting->data))->list=NULL;
	gui_treeview_right_refresh();
	return FALSE;
}
