#include "config.h"

/*
 * VARIABLES
 */
GtkWidget	*ly_ui_config_dialog	=	NULL;

/*
 * FUNCTIONS
 */
gboolean	ly_ui_config_init					(void);
gboolean	ly_ui_config_finalize				(void);
GtkWidget*	ly_ui_config_new					(void);

GtkWidget*	ly_ui_config_set_title				(GtkWidget *config_page, char *title, char *icon);
void		ly_ui_config_set_index				(GtkWidget *widget, char *str);
gboolean	ly_ui_config_set_index_cb			(GtkWidget *widget, cairo_t *cr, gpointer data);
void		ly_ui_config_draw_text				(cairo_t *cr, char *text, char *font);

gboolean	ly_ui_config_on_create_cb			(gpointer message, gpointer data);
gboolean	ly_ui_config_on_destroy_cb			(GtkWidget *widget, gpointer data);
gboolean	ly_ui_config_on_change_selection_cb	(GtkTreeSelection *selection, gpointer data);
gboolean	ly_ui_config_on_show_about_cb		(GtkWidget *widget, gpointer data);







gboolean ly_ui_config_init(void)
{
	ly_msg_bind("create_config","ui:win", ly_ui_config_on_create_cb, NULL);
	return TRUE;
}
gboolean ly_ui_config_finalize(void)
{
	return TRUE;
}

GtkWidget* ly_ui_config_new(void)
{
	gchar path[1024];
	gchar str[1024];
	GList *list;
	GList *p;
	int i=0;
	GdkPixbuf *pixbuf;
	GdkColor *color;

	GtkWidget *dialog;
	GtkWidget *tree;
	GtkTreeStore *store;
	GtkTreeSelection *selection;
	
	GtkWidget *notebook;
	GtkWidget *vbox;
	GtkWidget *vbox_temp;
	GtkWidget *hbox;
	GtkWidget *hbox_temp;
	
	GtkWidget *button;
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *hpaned;
	GtkWidget *check;
	GtkWidget *combo;
	GtkWidget *table;
	GtkTreeIter iter;

	//创建主对话框
	dialog=gtk_dialog_new_with_buttons(_("Configuration"),
									   GTK_WINDOW(ly_ui_win_window->win),
									   GTK_DIALOG_DESTROY_WITH_PARENT,
									   NULL);
	gtk_window_set_default_size(GTK_WINDOW(dialog),500,440);
	g_signal_connect(G_OBJECT(dialog), "destroy", G_CALLBACK(ly_ui_config_on_destroy_cb), NULL);
	
	//创建按钮
	button=gtk_button_new_from_stock(GTK_STOCK_ABOUT);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_ui_config_on_show_about_cb),NULL);
	gtk_box_pack_end(GTK_BOX(gtk_dialog_get_action_area(GTK_DIALOG(dialog))),button,FALSE,FALSE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_ui_config_on_destroy_cb),NULL);
	gtk_box_pack_end(GTK_BOX(gtk_dialog_get_action_area(GTK_DIALOG(dialog))),button,FALSE,FALSE,0);
	
	
	//创建第一层左右分区，左边为treeview，右边为notebook
	hpaned=gtk_hpaned_new();
	gtk_container_set_border_width(GTK_CONTAINER(hpaned), 10);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),hpaned,TRUE,TRUE,0);
	
	//treeview
	tree=gtk_tree_view_new();
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(tree),FALSE);
	gtk_container_add(GTK_CONTAINER(hpaned), tree);
	store= gtk_tree_store_new (1,G_TYPE_STRING);
	gtk_tree_store_append (store, &iter,NULL);
	gtk_tree_store_set(store, &iter,0,_("Database"),-1);
	gtk_tree_store_append (store, &iter,NULL);
	gtk_tree_store_set(store, &iter,0,_("Audio"),-1);
	gtk_tree_store_append (store, &iter,NULL);
	gtk_tree_store_set(store, &iter,0,_("Keyboard"),-1);
	gtk_tree_store_append (store, &iter,NULL);
	gtk_tree_store_set(store, &iter,0,_("Theme"),-1);
	gtk_tree_store_append (store, &iter,NULL);
	gtk_tree_store_set(store, &iter,0,_("Plug-ins"),-1);
	GtkCellRenderer *cell_renderer = gtk_cell_renderer_text_new ();
	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes( _("Options"),cell_renderer,"text",0,NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
	gtk_tree_view_set_model(GTK_TREE_VIEW (tree), GTK_TREE_MODEL(store));
	selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	g_signal_connect(G_OBJECT(selection),"changed",G_CALLBACK(ly_ui_config_on_change_selection_cb),hpaned);
	
	//notebook
	notebook=gtk_notebook_new();
	gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook),FALSE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook),FALSE);
	gtk_container_add(GTK_CONTAINER(hpaned),notebook);

	//page1 Database
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vbox,NULL);
	g_snprintf(path,sizeof(path),"%sui/icon/database.png",LY_GLOBAL_PROGDIR);
	vbox=ly_ui_config_set_title(vbox,_("<b>Database</b>\nSet the way to store data"), path);

	ly_ui_config_set_index(vbox,_("Database file location"));
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,10);
	label=gtk_label_new(_("Database File:"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entry=gtk_entry_new();
	gtk_widget_set_sensitive(entry,FALSE);
	ly_conf_get("db_path","%s",str);
	gtk_entry_set_text(GTK_ENTRY(entry),str);
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	
	ly_ui_config_set_index(vbox,_("Library location"));
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,10);
	label=gtk_label_new(_("Lib Directory:"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entry=gtk_entry_new();
	gtk_widget_set_sensitive(entry,FALSE);
	ly_conf_get("lib_path","%s",str);
	gtk_entry_set_text(GTK_ENTRY(entry),str);
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	
	//page2 Audio
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vbox,NULL);
	g_snprintf(path,sizeof(path),"%sui/icon/audio.png",LY_GLOBAL_PROGDIR);
	vbox=ly_ui_config_set_title(vbox,_("<b>Audio</b>\nSet all about playing audio"),path);
	
	ly_ui_config_set_index(vbox,_("Play mode"));
	int random=0;
	int repeat=1;
	int single=0;
	if(!ly_conf_get("mode","%d:%d:%d",&random,&repeat,&single))
	{
		ly_conf_set("mode","%d:%d:%d", random, repeat, single);
	}
	vbox_temp=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),vbox_temp,FALSE,FALSE,10);
	check=gtk_check_button_new_with_label(_("Random"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)random);
	gtk_box_pack_start(GTK_BOX(vbox_temp),check,FALSE,FALSE,0);
	check=gtk_check_button_new_with_label(_("Repeat"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)repeat);
	gtk_box_pack_start(GTK_BOX(vbox_temp),check,FALSE,FALSE,0);
	check=gtk_check_button_new_with_label(_("Single"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)single);
	gtk_box_pack_start(GTK_BOX(vbox_temp),check,FALSE,FALSE,0);
	
	//page3 Keyboard
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vbox,NULL);
	g_snprintf(path,sizeof(path),"%sui/icon/keyboard.png",LY_GLOBAL_PROGDIR);
	vbox=ly_ui_config_set_title(vbox,_("<b>Keyboard</b>\nSet all shortcut keys of Linnya"),path);
	
// 	ly_ui_config_set_index(vbox,_("Keybinds"));
// 	lyUiKeybind *keybind=NULL;
// 	keybind=ly_ui_key_get("play");
// 	if(keybind)
// 	{
// 		gtk_label_new()
// 	}
	
	
	
	return dialog;
}


GtkWidget* ly_ui_config_set_title(GtkWidget *widget, char *title, char *icon)
{
	GtkWidget	*hbox;
	GtkWidget	*scrolled_window;
	GtkWidget	*label;
	GtkWidget	*image;
	GtkWidget	*vbox;
	
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(widget),hbox,FALSE,FALSE,0);
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label),title);
	gtk_misc_set_alignment(GTK_MISC(label),0.1,0.5);
	gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,0);
	image=gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_size(icon,64,64,NULL));
	gtk_box_pack_end(GTK_BOX(hbox),image,FALSE,FALSE,0);
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(widget),scrolled_window,TRUE,TRUE,0);
	vbox=gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 6);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),vbox);
	return vbox;
}

void ly_ui_config_set_index(GtkWidget *widget, gchar *title)
{
	GtkWidget *drawing_area=gtk_drawing_area_new();
	gtk_widget_set_size_request(drawing_area,-1,30);
	g_signal_connect(G_OBJECT(drawing_area),"draw",G_CALLBACK(ly_ui_config_set_index_cb), title);
	gtk_box_pack_start(GTK_BOX(widget),drawing_area,FALSE,TRUE,0);
}
gboolean ly_ui_config_set_index_cb( GtkWidget * widget, cairo_t *cr, gpointer data)
{
	GtkStyle *style=gtk_widget_get_style(ly_ui_win_window->win);
	gint width=0,height=0,x,y,r;
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);
	gdk_cairo_set_source_color (cr,&(style->bg[3]));
	cairo_set_line_width (cr, 2);
	
	x =8;
	width=width-8;
	y =8;
	width = width-2*x+4;
	height = height-2*y+4;
	r = height / 4.0;
	
	cairo_move_to (cr, x + r, y);
	cairo_line_to (cr, x + width - r, y);
	
	cairo_move_to (cr, x + width, y + r);
	cairo_line_to (cr, x + width, y + height - r);
	
	cairo_move_to (cr, x + width - r, y + height);
	cairo_line_to (cr, x + r, y + height);
	
	cairo_move_to (cr, x, y + height - r);
	cairo_line_to (cr, x, y + r);
	
	cairo_arc (cr, x + r, y + r, r, M_PI, 3 * M_PI / 2.0);
	cairo_arc (cr, x + width - r, y + r, r, 3 * M_PI / 2, 2 * M_PI);
	cairo_arc (cr, x + width - r, y + height - r, r, 0, M_PI / 2);
	cairo_arc (cr, x + r, y + height - r, r, M_PI / 2, M_PI);
	
	cairo_stroke_preserve (cr);
	cairo_fill (cr);
	
	gdk_cairo_set_source_color (cr,&(style->fg[3]));
	cairo_move_to(cr,18,10);
	ly_ui_config_draw_text(cr,(gchar *)data,"Sans Regular 9");
	
	
	
	return TRUE;
}

void ly_ui_config_draw_text (cairo_t *cr, gchar *text, gchar *font)
{
	PangoLayout *layout;
	PangoFontDescription *desc;
	
	layout = pango_cairo_create_layout (cr);
	
	pango_layout_set_text (layout, text, -1);
	desc = pango_font_description_from_string (font);
	pango_layout_set_font_description (layout, desc);
	pango_font_description_free (desc);
	
	pango_cairo_update_layout (cr, layout);
	pango_cairo_show_layout (cr, layout);
	g_object_unref (layout);
}




gboolean ly_ui_config_on_create_cb(gpointer message, gpointer data)
{
	if(ly_ui_config_dialog)
		return FALSE;
	ly_ui_config_dialog=ly_ui_config_new();
	gtk_widget_show_all(ly_ui_config_dialog);
	return TRUE;
}

gboolean ly_ui_config_on_destroy_cb(GtkWidget *widget, gpointer data)
{
	if(ly_ui_config_dialog)
		gtk_widget_destroy(ly_ui_config_dialog);
	ly_ui_config_dialog=NULL;
	return FALSE;
}

gboolean ly_ui_config_on_change_selection_cb(GtkTreeSelection *selection, gpointer data)
{
	GtkWidget *hpaned=(GtkWidget *)data;
	GtkWidget *notebook=gtk_paned_get_child2(GTK_PANED(hpaned));
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;
	gchar *str;
	gint num;
	gtk_tree_selection_get_selected(selection ,&model,&iter);
	path=gtk_tree_model_get_path(model,&iter);
	str=gtk_tree_path_to_string(path);
	sscanf(str,"%d",&num);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook),num);
	return FALSE;
}
gboolean ly_ui_config_on_show_about_cb(GtkWidget *widget, gpointer data)
{
	gchar *path=NULL;
	GdkPixbuf *logo=NULL;
	
	path=g_strconcat(LY_GLOBAL_PROGDIR,"ui/icon/linnya.svg",NULL);
	logo=gdk_pixbuf_new_from_file_at_size(path,100,100,NULL);
	g_free(path);
	
	char *artists[]={"Edward<edward@linnya.org>","Edward<edward@linnya.org>",NULL};
	char *authors[]={"Edward<edward@linnya.org>",NULL};
	char translators[]={"Edward<edward@linnya.org>"};
	
	char lincense[102400]="";
	g_strlcpy(lincense, 
			  _("Linnya\n\n\
Copyright (C) 2010-2011 Edward\n\n\
Linnya is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2 of the License, or\n\
(at your option) any later version.\n\n\
Linnya is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\n\
You should have received a copy of the GNU General Public License\n\
along with Linnya; if not, write to the Free Software\n\
Foundation, Inc., 51 Franklin St, Fifth Floor, \n\
Boston, MA  02110-1301  USA"),
		sizeof(lincense));
	
	gtk_show_about_dialog(GTK_WINDOW(ly_ui_config_dialog),
						  "artists",		&artists,
						  "authors",		&authors,
						  "comments",		_("An audio player for Linux, with unlimited feathers through plug-ins."),
						  "copyright",		_("(c) Edward<edward@linnya.org>"),
						  "documenters",	&authors,
						  "license",		lincense,
						  "logo",			logo,
						  "logo-icon-name",	_("linnya"),
						  "translator-credits",&translators,
						  "version",		_("2.0.0 Alpha"),
						  "website",		"http://www.linnya.org",
						  "website-label",	_("Official Website"),
						  "wrap-license",	TRUE,
						  NULL);
	g_object_unref(logo);
	return FALSE;
}