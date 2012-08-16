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
	g_signal_connect(G_OBJECT(ly_win_get_window()->btn_adds), "clicked", G_CALLBACK(ly_sss_tab_add_create), NULL);
	g_timeout_add(100, ly_sss_refresh, NULL);

}
void	ly_sss_fina()
{
	ly_sss_tab_add_fina();
}
GtkWidget *ly_sss_tab_create(GdkPixbuf *pixbuf, gchar *name, GtkWidget *widget)
{
	gchar path[1024]="";
	g_snprintf(path,sizeof(path),"%sicon/default_plugin_logo.png",LY_GLB_PROG_UIXDIR);
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

	int n=gtk_notebook_page_num(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),tab_add);

	if(!session)
		return FALSE;
	if(session->daemon)
		return FALSE;

	if(session->widget!=NULL)
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn), gtk_notebook_page_num(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),session->widget));
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

	gtk_notebook_insert_page(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),widget,hbox,n);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),widget, TRUE);
	gtk_widget_show_all(hbox);
	gtk_widget_show_all(widget);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),n);

	ly_sss_tab_add_destroy(NULL, tab_add);

	return TRUE;
}
gboolean ly_sss_destroy(GtkWidget *widget)
{
	GtkWidget *hbox=gtk_notebook_get_tab_label(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),widget);
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
	n=gtk_notebook_page_num(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),widget);
	gtk_notebook_remove_page(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),n);

	void (*f)(void);
	g_module_symbol(session->module, session->destroy_symbol, (gpointer)&f);
	if(f!=NULL)
		f();

	session->widget=NULL;

	if(gtk_notebook_get_n_pages(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn))<1)
	{
		ly_sss_tab_add_create(NULL,NULL);
	}
	return TRUE;
}

gboolean ly_sss_refresh()
{
	gint n=gtk_notebook_get_current_page(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn));
	GtkWidget *widget=gtk_notebook_get_nth_page(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),n);
	GtkWidget *hbox=gtk_notebook_get_tab_label(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),widget);
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

	GtkWidget *sw = gtk_viewport_new(NULL, NULL);
	gtk_widget_set_name(sw, "sss_vwpt");
	ly_sss_store=gtk_list_store_new(3, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	GtkWidget *icon_view=gtk_icon_view_new_with_model(GTK_TREE_MODEL(ly_sss_store));
	gtk_widget_set_name(icon_view, "sss_adds");
	gtk_icon_view_set_reorderable(GTK_ICON_VIEW(icon_view), TRUE);
	gtk_container_add(GTK_CONTAINER(sw), icon_view);
	gtk_icon_view_set_text_column(GTK_ICON_VIEW(icon_view), 2);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), 1);

	gtk_notebook_append_page(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn), sw, hbox);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn), sw,TRUE);
	gtk_widget_show_all(hbox);
	gtk_widget_show_all(sw);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),gtk_notebook_get_n_pages(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn))-1);

	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_sss_tab_add_destroy), sw);
	g_signal_connect(G_OBJECT(icon_view),"item-activated",G_CALLBACK(ly_sss_active_cb), sw);

	ly_sss_tab_add_refresh();
	return TRUE;
}

gboolean ly_sss_tab_add_destroy(GtkWidget *widget, gpointer data)
{
	if(gtk_notebook_get_n_pages(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn))<=1)
		return FALSE;
	GtkWidget *w=(GtkWidget*)data;

	gint n=0;
	n=gtk_notebook_page_num(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),w);
	gtk_notebook_remove_page(GTK_NOTEBOOK(ly_win_get_window()->nbk_sssn),n);
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

GdkPixbuf* ly_sss_alloc_bg(char *bg)
{
	GdkPixbuf *image=NULL;
	if(bg==NULL)
	{
		LyThmItem *th=NULL;
		char dir[1024]="";
		char *pic=NULL;
		int custom_sssbg=0;
		int i;
		ly_reg_get("thm_custom_sssbg", "%d", &custom_sssbg);
		if(custom_sssbg)
		{
			ly_reg_get("thm_sssbg", "%[^\n]", dir);
		}
		else
		{
			th=ly_thm_item_new_from_conf();
			if(!th || g_str_equal(th->sssbg, ""))
			{
				ly_thm_item_free(th);
				return NULL;
			}
			g_strlcpy(dir, th->sssbg, sizeof(dir));
		}
		GList *list=ly_gla_traverse_dir(dir, ".(?i:jpg|png|jpeg|bmp)$", 5, FALSE);
		GList *p=list;

		if(!list)
			return NULL;

		/*
		 * create random number
		 */
		srand(time(0));
		i=random()%(g_list_length(list));
		pic=(char *)(g_list_nth_data(list, i));
		if(pic)
		{
			char *path=ly_gla_uri_get_path(pic);
			image=gdk_pixbuf_new_from_file(path, NULL);
			g_free(path);
		}
		while(p)
		{
			g_free(p->data);
			p->data=NULL;
			p=p->next;
		}
		g_list_free(list);
	}
	else
	{
		image=gdk_pixbuf_new_from_file(bg, NULL);
	}

	return image;
}


void ly_sss_tab_add_refresh_cb(gpointer key, gpointer value, gpointer data)
{
	gchar name[64]="";
	gchar alias[512]="";
	GdkPixbuf *pixbuf=NULL;
	GtkTreeIter iter;

	LyPliPlugin *session=value;
	if(!session||!(session->module)||session->daemon)
	{
		return;
	}
	g_strlcpy(name, session->name, sizeof(name));
	g_strlcpy(alias, session->alias, sizeof(alias));
	pixbuf = gdk_pixbuf_new_from_file_at_scale(session->logo, 64, 64, FALSE, NULL);
	gtk_list_store_append(ly_sss_store, &iter);
	gtk_list_store_set(ly_sss_store, &iter, 0, name, 1, pixbuf, 2, alias, -1);
	g_object_unref(pixbuf);
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
