/*
 * sss.c
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
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
#include "sss.h"

/*
 * VARIABLES
 */
GtkListStore *ly_sss_store;
GtkWidget *ly_sss_tab_add_label;


/*
 * FUNCTIONS
 */

gboolean ly_sss_close_cb(GtkWidget *widget, gpointer data);
gboolean ly_sss_active_cb(GtkIconView *iconview,GtkTreePath *path,gpointer data);
gboolean ly_sss_select_cb(GtkIconView *iconview,GtkTreePath *path,gpointer data);
void ly_sss_tab_add_refresh_cb(gpointer key, gpointer value, gpointer data);
gboolean ly_sss_create(gchar *name, GtkWidget *tab_add);


void	ly_sss_init()
{
	ly_sss_tab_add_init();
	ly_sss_tab_add_create();
	
	GtkWidget *hbox=gtk_hbox_new(FALSE,0);
	gchar path[1024]="";
	g_snprintf(path, sizeof(path), "%sui/icon/icon.svg", LY_GLA_PROGDIR);
	GtkWidget *logo = gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_scale(path,50,16,FALSE,NULL));
	gtk_box_pack_start(GTK_BOX(hbox), logo, FALSE,0,0);

	GtkWidget *button = gtk_button_new();
	GtkWidget *image = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);  
	ly_key_set("session_add", NULL, NULL, NULL, KEY_BIND_TYPE_SIGNAL, button, "clicked");
	ly_key_bind("session_add");
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE,0,0);

	gtk_notebook_set_action_widget(GTK_NOTEBOOK(ly_win_get_window()->notebook_session), hbox, GTK_PACK_END);
	gtk_widget_show_all(hbox);
	
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_sss_tab_add_create), NULL);
	g_timeout_add(100, ly_sss_refresh, NULL);
}
void	ly_sss_fina()
{
	ly_sss_tab_add_fina();
}
GtkWidget *ly_sss_tab_create(GdkPixbuf *pixbuf, gchar *name, GtkWidget *widget)
{
	gchar path[1024]="";
	g_snprintf(path,sizeof(path),"%s/ui/icon/default_plugin_logo.svg",LY_GLA_PROGDIR);
	GtkWidget *hbox=gtk_hbox_new(FALSE,0);
	if(!pixbuf)
		pixbuf=gdk_pixbuf_new_from_file_at_scale(path,16,16,FALSE,NULL);
	GtkWidget *logo=gtk_image_new_from_pixbuf(pixbuf);
	gtk_box_pack_start(GTK_BOX(hbox),logo,FALSE,FALSE,0);
	GtkWidget *title=gtk_label_new(name);
	gtk_box_pack_start(GTK_BOX(hbox),title,FALSE,FALSE,0);
	GtkWidget *button=gtk_button_new();
	GtkWidget *image = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON);  
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE); 
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_sss_close_cb),widget);
	return hbox;
}

gboolean ly_sss_create(gchar *name, GtkWidget *tab_add)
{
	LyPliPlugin *session=NULL;
	session=ly_pli_get(name);
	
	int n=gtk_notebook_page_num(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),tab_add);
	
	if(!session)
		return FALSE;
	if(session->daemon)
		return FALSE;
	
	if(session->widget!=NULL)
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(ly_win_get_window()->notebook_session), gtk_notebook_page_num(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),session->widget));
		ly_sss_tab_add_destroy(NULL, tab_add);
		return TRUE;
	}
	
	GtkWidget *(*f)(void);
	g_module_symbol(session->module, session->create_symbol, (gpointer)&f);
	if(f==NULL)
		return FALSE;
	
	GtkWidget *widget=NULL;
	widget=f();
	if(widget==NULL)
		return FALSE;
	session->widget=widget;
	
	GdkPixbuf *pixbuf=NULL;
	pixbuf=gdk_pixbuf_new_from_file_at_scale(session->logo,16,16,FALSE,NULL);
	GtkWidget *hbox=ly_sss_tab_create(pixbuf, name, widget);
	
	gtk_notebook_insert_page(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),widget,hbox,n);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),widget,TRUE);
	gtk_widget_show_all(hbox);
	gtk_widget_show_all(widget);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),n);
	
	ly_sss_tab_add_destroy(NULL, tab_add);

	return TRUE;
}
gboolean ly_sss_destroy(GtkWidget *widget)
{
	if(gtk_notebook_get_n_pages(GTK_NOTEBOOK(ly_win_get_window()->notebook_session))<=1)
	{
		ly_sss_tab_add_create(NULL,NULL);
	}
	GtkWidget *hbox=gtk_notebook_get_tab_label(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),widget);
	GList *tab=gtk_container_get_children(GTK_CONTAINER(hbox));
	GList *p=tab;
	p=p->next;
	GtkWidget *title=(GtkWidget*)(p->data);
	gchar name[256];
	g_strlcpy(name,gtk_label_get_text(GTK_LABEL(title)),sizeof(name));
	
	LyPliPlugin *session=NULL;
	session=ly_pli_get(name);
	if(!session)
		return FALSE;
	
	gint n=0;
	n=gtk_notebook_page_num(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),widget);
	gtk_notebook_remove_page(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),n);
	
	void (*f)(void);
	g_module_symbol(session->module, session->destroy_symbol, (gpointer)&f);
	if(f!=NULL)
		f();
	
	session->widget=NULL;
	return TRUE;
}

gboolean ly_sss_refresh()
{
	gint n=gtk_notebook_get_current_page(GTK_NOTEBOOK(ly_win_get_window()->notebook_session));
	GtkWidget *widget=gtk_notebook_get_nth_page(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),n);
	GtkWidget *hbox=gtk_notebook_get_tab_label(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),widget);
	GList *tab=gtk_container_get_children(GTK_CONTAINER(hbox));
	GList *p=tab;
	p=p->next;
	GtkWidget *title=(GtkWidget*)(p->data);
	gchar name[256];
	g_strlcpy(name,gtk_label_get_text(GTK_LABEL(title)),sizeof(name));
	
	LyPliPlugin *session=NULL;
	session=ly_pli_get(name);
	if(!session)
		return TRUE;
	
	void (*f)(void);
	g_module_symbol(session->module, session->refresh_symbol, (gpointer)&f);
	if(f!=NULL)
		f();
	return TRUE;
}

gboolean ly_sss_tab_add_init()
{
	ly_sss_store=gtk_list_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
	return FALSE;
}

gboolean ly_sss_tab_add_create(GtkWidget *widget, gpointer data)
{
	GtkWidget *hbox=gtk_hbox_new(FALSE,0);
	
	GtkWidget *logo=gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_BUTTON);
	gtk_box_pack_start(GTK_BOX(hbox),logo,FALSE,FALSE,0);
	GtkWidget *title=gtk_label_new(_("New Session"));
	gtk_box_pack_start(GTK_BOX(hbox),title,FALSE,FALSE,0);
	GtkWidget *button=gtk_button_new();
	GtkWidget *image = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON);  
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE); 
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	
	GtkWidget *sw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	ly_sss_store=gtk_list_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
	GtkWidget *icon_view=gtk_icon_view_new_with_model(GTK_TREE_MODEL(ly_sss_store));
	gtk_container_add(GTK_CONTAINER(sw), icon_view);
	gtk_icon_view_set_text_column(GTK_ICON_VIEW(icon_view), 0);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), 1);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),sw, hbox);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),sw,TRUE);
	gtk_widget_show_all(hbox);
	gtk_widget_show_all(sw);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),gtk_notebook_get_n_pages(GTK_NOTEBOOK(ly_win_get_window()->notebook_session))-1);
	
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_sss_tab_add_destroy), sw);
	g_signal_connect(G_OBJECT(icon_view),"item-activated",G_CALLBACK(ly_sss_active_cb),sw);
	
	ly_sss_tab_add_refresh();
	return TRUE;
}

gboolean ly_sss_tab_add_destroy(GtkWidget *widget, gpointer data)
{
	if(gtk_notebook_get_n_pages(GTK_NOTEBOOK(ly_win_get_window()->notebook_session))<=1)
		return FALSE;
	GtkWidget *w=(GtkWidget*)data;
	
	gint n=0;
	n=gtk_notebook_page_num(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),w);
	gtk_notebook_remove_page(GTK_NOTEBOOK(ly_win_get_window()->notebook_session),n);
	return TRUE;
}
gboolean ly_sss_tab_add_refresh()
{
	if(ly_sss_store)
		gtk_list_store_clear(ly_sss_store);
	
	g_hash_table_foreach(ly_pli_get_plugins(), ly_sss_tab_add_refresh_cb, NULL);
	return TRUE;
}
gboolean ly_sss_tab_add_fina()
{
	if(ly_sss_store)
		gtk_list_store_clear(ly_sss_store);
	return TRUE;
}

void ly_sss_tab_add_refresh_cb(gpointer key, gpointer value, gpointer data)
{
	LyPliPlugin *session=value;
	if(!session)
		return;
	if(!(session->module))
		return;
	if(session->daemon)
		return;
	gchar *name=g_strconcat(key,NULL);
	GtkTreeIter iter;
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(session->logo, 64, 64,FALSE, NULL);
	gtk_list_store_append(ly_sss_store, &iter);
	gtk_list_store_set(ly_sss_store, &iter, 0, name, 1, pixbuf, -1);
}

gboolean ly_sss_active_cb(GtkIconView *iconview,GtkTreePath *path,gpointer data)
{
	GtkTreeIter iter;
	gchar *name=NULL;
	
	gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_sss_store),&iter,path);
	gtk_tree_model_get(GTK_TREE_MODEL(ly_sss_store), &iter, 0, &name, -1);
	
	ly_sss_create(name, data);
	g_free(name);
	return FALSE;
}
gboolean ly_sss_select_cb(GtkIconView *iconview,GtkTreePath *path,gpointer data)
{
	GtkTreeIter iter;
	gchar *name=NULL;
	
	gtk_tree_model_get_iter(GTK_TREE_MODEL(ly_sss_store),&iter,path);
	gtk_tree_model_get(GTK_TREE_MODEL(ly_sss_store), &iter, 0, &name, -1);
	
	ly_sss_create(name, data);
	g_free(name);
	return FALSE;
}

gboolean ly_sss_close_cb(GtkWidget *widget, gpointer data)
{
	GtkWidget *w=data;
	ly_sss_destroy(w);
	return FALSE;
}
