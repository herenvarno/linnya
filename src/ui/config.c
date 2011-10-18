#include "config.h"

/*
 * MACRO
 */
#define BACKUP_ENCODING_COUNT 72

/*
 * VARIABLES
 */
GtkWidget	*ly_ui_config_dialog	=	NULL;
char *encoding[BACKUP_ENCODING_COUNT] = {
"Arabic (IBM-864)",
"Arabic (ISO-8859-6)",
"Arabic (Windows-1256)",
"Armenian (ARMSCII-8)",

"Baltic (ISO-8859-13)",
"Baltic (ISO-8859-4)",
"Baltic (Windows-1257)",

"Celtic (ISO-8859-14)",
"Central European (IBM-852)",
"Central European (ISO-8859-2)",
"Central European (MacCE)",
"Central European (Windows-1250)",
"Chinese Simplified (GB18030)",
"Chinese Simplified (GB2312)",
"Chinese Simplified (GBK)",
"Chinese Simplified (HZ)",
"Chinese Simplified (ISO-2022-CN)",
"Chinese Traditional (Big-5)",
"Chinese Traditional (Big-5-HKSCS)",
"Chinese Traditional (EUC-TW)",
"Croatian (MacCroatian)",
"Cyrillic (IBM-855)",
"Cyrillic (ISO-8859-5)",
"Cyrillic (ISO-IR-111)",
"Cyrillic (KOI8-R)",
"Cyrillic (MacCyrillic)",
"Cyrillic/Russian (CP-866)",
"Cyrillic/Ukrainian (KOI8-U)",
"Cyrillic (Windows-1251)",

"Georgian (GEOSTD8)",
"Greek (ISO-8859-7)",
"Greek (MacGreek)",
"Greek (Windows-1253)",
"Gujarati (MacGujarati)",
"Gurmukhi (MacGurmukhi)",

"Hebrew (IBM-862)",
"Hebrew (ISO-8859-8-I)",
"Hebrew Visual (ISO-8859-8)",
"Hebrew (Windows-1255)",
"Hindi (MacDevanagari)",

"Icelandic (MacIcelandic)",

"Japanese (EUC-JP)",
"Japanese (ISO-2022-JP)",
"Japanese (Shift-JIS)",

"Korean (EUC-KR)",
"Korean (ISO-2022-KR)",
"Korean (JOHAB)",
"Korean (UHC)",

"Nordic (ISO-8859-10)",

"Romanian (ISO-8859-16)",
"Romanian (MacRomanian)",

"South European (ISO-8859-3)",

"Thai (ISO-8859-11)",
"Thai (TIS-620)",
"Thai (Windows-874)",
"Turkish (IBM-857)",
"Turkish (ISO-8859-9)",
"Turkish (MacTurkish)",
"Turkish (Windows-1254)",

"Unicode (UTF-16)",
"Unicode (UTF-16BE)",
"Unicode (UTF-16LE)",
"Unicode (UTF-8)",

"Vietnamese (TCVN)",
"Vietnamese (VISCII)",
"Vietnamese (VPS)",
"Vietnamese (Windows-1258)",

"Western (IBM-850)",
"Western (ISO-8859-1)",
"Western (ISO-8859-15)",
"Western (MacRoman)",
"Western (Windows-1252)"
};

/*
 * FUNCTIONS
 */
gboolean	ly_ui_config_init							(void);
gboolean	ly_ui_config_finalize						(void);
GtkWidget*	ly_ui_config_new							(void);

GtkWidget*	ly_ui_config_set_title					(GtkWidget *config_page, char *title, char *icon);
void		ly_ui_config_set_index					(GtkWidget *widget, char *str);
gboolean	ly_ui_config_set_index_cb				(GtkWidget *widget, cairo_t *cr, gpointer data);
void		ly_ui_config_draw_text					(cairo_t *cr, char *text, char *font);

gboolean	ly_ui_config_on_create_cb				(gpointer message, gpointer data);
gboolean	ly_ui_config_on_destroy_cb				(GtkWidget *widget, gpointer data);
gboolean	ly_ui_config_on_change_selection_cb		(GtkTreeSelection *selection, gpointer data);
gboolean	ly_ui_config_on_show_about_cb			(GtkWidget *widget, gpointer data);

gboolean	ly_ui_config_on_lib_change_cb			(GtkWidget *widget, gpointer data);
gboolean	ly_ui_config_on_encoding_changed_cb		(GtkWidget *widget, gpointer data);
gboolean	ly_ui_config_on_audio_mode_change_cb	(GtkWidget *widget, gpointer data);
gboolean	ly_ui_config_on_key_change_cb			(GtkWidget *widget, gpointer data);
gboolean	ly_ui_config_on_key_press_cb			(GtkWidget *widget, GdkEvent  *event, gpointer data);
gboolean	ly_ui_config_on_theme_changed_cb		(GtkWidget *widget, gpointer data);
gboolean	ly_ui_config_on_plugin_show_about_cb	(GtkWidget *widget, gpointer data);
gboolean	ly_ui_config_on_plugin_show_config_cb	(GtkWidget *widget, gpointer data);
gboolean	ly_ui_config_on_plugin_change_cb		(GtkWidget *widget, gpointer data);


gboolean
ly_ui_config_init (void)
{
	ly_msg_bind("create_config","ui:win", ly_ui_config_on_create_cb, NULL);
	return TRUE;
}
gboolean
ly_ui_config_finalize (void)
{
	return TRUE;
}

GtkWidget*
ly_ui_config_new (void)
{
	gchar path[1024];
	gchar str[1024];
	int i=0;

	GtkWidget *dialog;
	GtkWidget *tree;
	GtkTreeStore *store;
	GtkTreeSelection *selection;
	
	GtkWidget *notebook;
	GtkWidget *vbox;
	GtkWidget *vbox_temp;
	GtkWidget *hbox;
	
	GtkWidget *button;
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *hpaned;
	GtkWidget *check;
	GtkWidget *combo;
	GtkWidget *table;
	GtkTreeIter iter;
	
	GHashTableIter iter1;
	gpointer key, value;

	//main dialog
	dialog=gtk_dialog_new_with_buttons(_("Configuration"),
									   GTK_WINDOW(ly_ui_win_window->win),
									   GTK_DIALOG_DESTROY_WITH_PARENT,
									   NULL);
	gtk_window_set_default_size(GTK_WINDOW(dialog),500,440);
	g_signal_connect(G_OBJECT(dialog), "destroy", G_CALLBACK(ly_ui_config_on_destroy_cb), NULL);
	
	//button
	button=gtk_button_new_from_stock(GTK_STOCK_ABOUT);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_ui_config_on_show_about_cb),NULL);
	gtk_box_pack_end(GTK_BOX(gtk_dialog_get_action_area(GTK_DIALOG(dialog))),button,FALSE,FALSE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_ui_config_on_destroy_cb),NULL);
	gtk_box_pack_end(GTK_BOX(gtk_dialog_get_action_area(GTK_DIALOG(dialog))),button,FALSE,FALSE,0);
	
	
	//main frame
	hpaned=gtk_hpaned_new();
	gtk_container_set_border_width(GTK_CONTAINER(hpaned), 10);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),hpaned,TRUE,TRUE,0);
	
	//treeview
	tree=gtk_tree_view_new();
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(tree),FALSE);
	gtk_container_add(GTK_CONTAINER(hpaned), tree);
	store= gtk_tree_store_new (1,G_TYPE_STRING);
	gtk_tree_store_append (store, &iter,NULL);
	gtk_tree_store_set(store, &iter,0,_("Library"),-1);
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

	//page1 Library
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vbox,NULL);
	g_snprintf(path,sizeof(path),"%sui/icon/lib.svg",LY_GLOBAL_PROGDIR);
	vbox=ly_ui_config_set_title(vbox,_("<b>Library</b>\nAll the Music Data"), path);
	
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
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_ui_config_on_lib_change_cb), entry);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	
	ly_ui_config_set_index(vbox,_("Backup Encoding"));
	char extra_encoding[1024]="Chinese Simplified (GB18030)";
	if(ly_conf_get("db_extra_encoding", "%1023[^\n]", extra_encoding))
	{
		ly_conf_set("db_extra_encoding", "%s", extra_encoding);
	}
	combo=gtk_combo_box_text_new();
	i=0;
	for(i=0;i<BACKUP_ENCODING_COUNT;i++)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), encoding[i]);
		if(g_str_equal(extra_encoding, encoding[i]))
		{
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo), i);
		}
	}
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(ly_ui_config_on_encoding_changed_cb), NULL);
	gtk_box_pack_start(GTK_BOX(vbox),combo,FALSE, FALSE,0);
	
	//page2 Audio
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vbox,NULL);
	g_snprintf(path,sizeof(path),"%sui/icon/audio.svg",LY_GLOBAL_PROGDIR);
	vbox=ly_ui_config_set_title(vbox,_("<b>Audio</b>\nSet all about playing audio"),path);
	
	ly_ui_config_set_index(vbox,_("Play mode"));
	int random=0;
	int repeat=1;
	int single=0;
	if(!ly_conf_get("audio_mode","%d:%d:%d",&random,&repeat,&single))
	{
		ly_conf_set("audio_mode","%d:%d:%d", random, repeat, single);
	}
	vbox_temp=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),vbox_temp,FALSE,FALSE,10);
	check=gtk_check_button_new_with_label(_("Random"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)random);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_ui_config_on_audio_mode_change_cb), "random");
	gtk_box_pack_start(GTK_BOX(vbox_temp),check,FALSE,FALSE,0);
	check=gtk_check_button_new_with_label(_("Repeat"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)repeat);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_ui_config_on_audio_mode_change_cb), "repeat");
	gtk_box_pack_start(GTK_BOX(vbox_temp),check,FALSE,FALSE,0);
	check=gtk_check_button_new_with_label(_("Single"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)single);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_ui_config_on_audio_mode_change_cb), "single");
	gtk_box_pack_start(GTK_BOX(vbox_temp),check,FALSE,FALSE,0);
	
	//page3 Keyboard
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vbox,NULL);
	g_snprintf(path,sizeof(path),"%sui/icon/key.svg",LY_GLOBAL_PROGDIR);
	vbox=ly_ui_config_set_title(vbox,_("<b>Keyboard</b>\nSet all shortcut keys of Linnya"),path);
	ly_ui_config_set_index(vbox, _("Shortcuts"));
	table=gtk_table_new(g_hash_table_size(ly_ui_pl_plugins)+1, 2, FALSE);
	gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE,0);
	
	gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(_("Name and Keys")), 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(_("Preferense")), 1, 2, 0, 1);
	
	g_hash_table_iter_init (&iter1, ly_ui_key_keybinds);
	i=1;
	char str_key[1024]="";
	while (g_hash_table_iter_next (&iter1, &key, &value)) 
	{
		g_snprintf(str_key, sizeof(str_key),"{%s}%s_%s:%s", (char *)key, ((lyUiKeyKeybind*)value)->mask0, ((lyUiKeyKeybind*)value)->mask1, ((lyUiKeyKeybind*)value)->key);
		label=gtk_label_new(str_key);
		gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, i, i+1);
		button=gtk_button_new_from_stock(GTK_STOCK_PREFERENCES);
		g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_ui_config_on_key_change_cb), label);
		gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, i, i+1);
		i++;
	}
	
	//page4 Theme
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vbox,NULL);
	g_snprintf(path,sizeof(path),"%sui/icon/theme.svg",LY_GLOBAL_PROGDIR);
	vbox=ly_ui_config_set_title(vbox,_("<b>Theme</b>\nSet the looks of Linnya."),path);
	ly_ui_config_set_index(vbox,_("Theme"));
	char theme_name[128]="default";
	if(!ly_conf_get("theme", "%s", theme_name))
	{
		ly_conf_set("theme", "%s", theme_name);
	}
	combo=gtk_combo_box_text_new();
	GList *list=ly_ui_theme_get_list();
	GList *p=list;
	char *th_name;
	i=0;
	while(p)
	{
		th_name=ly_global_get_filename((gchar *)(p->data));
		
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), th_name);
		if(g_str_equal(th_name, theme_name))
		{
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo), i);
		}
		g_free(th_name);
		i++;
		p=p->next;
	}
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(ly_ui_config_on_theme_changed_cb), NULL);
	gtk_box_pack_start(GTK_BOX(vbox),combo,FALSE, FALSE,0);
	
	//page5 Plug-ins
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vbox,NULL);
	g_snprintf(path,sizeof(path),"%sui/icon/plugin.svg",LY_GLOBAL_PROGDIR);
	vbox=ly_ui_config_set_title(vbox,_("<b>Plugins</b>\nAll the plug-ins of Linnya."),path);
	ly_ui_config_set_index(vbox,_("Plugins"));
	table=gtk_table_new(g_hash_table_size(ly_ui_pl_plugins),4,FALSE);
	gtk_box_pack_start(GTK_BOX(vbox),table,FALSE,FALSE,0);

	i=0;
	lyUiPlPlugin *pl;
	GtkWidget* (*f)(void);
	g_hash_table_iter_init (&iter1, ly_ui_pl_plugins);
	while (g_hash_table_iter_next (&iter1, &key, &value)) 
	{
		pl=(lyUiPlPlugin *)value;
		check=gtk_check_button_new();
		if(pl->module)
		{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
		}
		g_signal_connect(G_OBJECT(check),"toggled",G_CALLBACK(ly_ui_config_on_plugin_change_cb), pl->name);
		gtk_table_attach_defaults(GTK_TABLE(table),check,0,1,i,i+1);
		label=gtk_label_new(pl->name);
		gtk_table_attach_defaults(GTK_TABLE(table),label,1,2,i,i+1);
		
		if(pl->module)
		{
			g_module_symbol(pl->module, pl->config_symbol, (gpointer)&f);
			if(f)
			{
				button=gtk_button_new_from_stock(GTK_STOCK_PREFERENCES);
				g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_ui_config_on_plugin_show_config_cb), f);
				gtk_table_attach_defaults(GTK_TABLE(table),button,2,3,i,i+1);
			}
		}
		button=gtk_button_new_from_stock(GTK_STOCK_ABOUT);
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_ui_config_on_plugin_show_about_cb), pl);
		gtk_table_attach_defaults(GTK_TABLE(table),button,3,4,i,i+1);
		i++;
	}
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

gboolean ly_ui_config_on_plugin_show_config_cb(GtkWidget *widget, gpointer data)
{
	GtkWidget* (*f)(void);
	f=data;
	if(!f)
	{
		return FALSE;
	}
	GtkWidget *w=f();
	if(!w)
	{
		return FALSE;
	}
	GtkWidget *dialog=gtk_dialog_new_with_buttons(_("Plugin Configureation"),
									   GTK_WINDOW(ly_ui_config_dialog),
									   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
									   GTK_STOCK_OK,
									   GTK_RESPONSE_ACCEPT,
									   NULL);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), w, FALSE, FALSE, 0);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return FALSE;
}
gboolean ly_ui_config_on_show_about_cb(GtkWidget *widget, gpointer data)
{
	gchar *path=NULL;
	GdkPixbuf *logo=NULL;
	
	path=g_strconcat(LY_GLOBAL_PROGDIR,"ui/icon/linnya.svg",NULL);
	logo=gdk_pixbuf_new_from_file_at_size(path,100,100,NULL);
	g_free(path);
	
	char *artists[]={"Hillar Liiv<liivhillar@gmail.com>","Edward<edward@linnya.org>",NULL};
	char *authors[]={"Edward<edward@linnya.org>","Carl Yu<shdxcy@gmail.com>",NULL};
	char translators[]={"Edward<edward@linnya.org>"};
	
	char license[102400]="";
	g_strlcpy(license, 
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
		sizeof(license));
	
	gtk_show_about_dialog(GTK_WINDOW(ly_ui_config_dialog),
						  "artists",		&artists,
						  "authors",		&authors,
						  "comments",		_("An audio player for Linux, with unlimited feathers by plug-ins."),
						  "copyright",		_("(c) 2010-2011 Edward<edward@linnya.org>"),
						  "license",		license,
						  "logo",			logo,
						  "logo-icon-name",	_("linnya"),
						  "translator-credits",&translators,
						  "version",		LY_GLOBAL_VERSION_STR,
						  "website",		"http://linnya.org",
						  "website-label",	_("Official Website"),
						  "wrap-license",	TRUE,
						  NULL);
	g_object_unref(logo);
	return FALSE;
}

gboolean ly_ui_config_on_plugin_show_about_cb(GtkWidget *widget, gpointer data)
{
	lyUiPlPlugin *pl=(lyUiPlPlugin*)data;
	GdkPixbuf *logo=NULL;
	GtkWidget *about=gtk_about_dialog_new();
	if(!g_str_equal(pl->author,""))
	{
		char *authors[]={pl->author,NULL};
		gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about), (const char **)authors);
		char copyright[1024]="";
		g_snprintf(copyright, sizeof(copyright), _("(c) %s"), pl->author);
		gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), copyright);
	}
	if(!g_str_equal(pl->comment,""))
	{
		gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about), pl->comment);
	}
	if(!g_str_equal(pl->license,""))
	{
		gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about), pl->license);
	}
	if(!g_str_equal(pl->license,""))
	{
		gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about), pl->license);
	}
	if(!g_str_equal(pl->logo,""))
	{
		logo=gdk_pixbuf_new_from_file_at_size(pl->logo,100,100,NULL);
		gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), logo);
	}
	if(!g_str_equal(pl->name,""))
	{
		char name[1024]="";
		g_snprintf(name, sizeof(name),"%s [Plugin for Linnya]", pl->alias);
		gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about), name);
	}
	if(!g_str_equal(pl->version,""))
	{
		gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), pl->version);
	}
	if(!g_str_equal(pl->website,""))
	{
		gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about), pl->website);
		gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about), _("Official Website"));
	}
	gtk_dialog_run(GTK_DIALOG(about));
	if(logo)
	{
		g_object_unref(logo);
	}
	gtk_widget_destroy(about);
	return FALSE;
}

gboolean	ly_ui_config_on_plugin_change_cb	(GtkWidget *widget, gpointer data)
{
	lyUiPlPlugin *pl=(lyUiPlPlugin*)data;
	if(!pl)
		return FALSE;
	gboolean state=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	ly_ui_pl_set_active(pl->name, state);
	ly_ui_session_tab_add_refresh();
	if(!state)
	{
		ly_msg_put("warning", "ui:config", "Plugin will be COMPLETELY inactived after next program start!");
	}
	return FALSE;
}

gboolean ly_ui_config_on_lib_change_cb(GtkWidget *widget, gpointer data)
{
	gint result=0;
	GtkWidget *dialog=gtk_file_chooser_dialog_new(_("Select library directory"),
												  GTK_WINDOW(ly_ui_config_dialog),
												  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
												  GTK_STOCK_OPEN,
												  GTK_RESPONSE_ACCEPT,
												  GTK_STOCK_CANCEL,
												  GTK_RESPONSE_CANCEL,
												  NULL);
	result=gtk_dialog_run (GTK_DIALOG (dialog));
	switch (result)
	{
		case GTK_RESPONSE_ACCEPT:
			break;
		case GTK_RESPONSE_CANCEL:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
		default:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
	}
	
	gchar *path=gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
	gchar *str=g_strconcat(path,"/",NULL);
	g_free(path);
	
	gtk_entry_set_text(GTK_ENTRY(data),str);
	
	ly_conf_set("lib_path", "%s", str);
	g_free(str);
	gtk_widget_destroy(dialog);
	ly_db_check_library();
	ly_msg_put("lib_changed", "ui:config", NULL);
	return FALSE;
}
gboolean ly_ui_config_on_encoding_changed_cb(GtkWidget *widget, gpointer data)
{
	char *encoding=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	ly_conf_set("db_extra_encoding", "%s", encoding);
	ly_msg_put("info", "ui:config", _("Setting will not be actived until program restart!"));
	return FALSE;
}
gboolean ly_ui_config_on_audio_mode_change_cb(GtkWidget *widget, gpointer data)
{
	char *type=(char *)data;
	int random=0;
	int repeat=1;
	int single=0;
	
	ly_conf_get("audio_mode", "%d:%d:%d", &random, &repeat, &single);
	
	if(g_str_equal(type, "random"))
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		{
			random=1;
		}
		else
		{
			random=0;
		}
	}
	else if(g_str_equal(type,"repeat"))
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		{
			repeat=1;
		}
		else
		{
			repeat=0;
		}
	}
	else if(g_str_equal(type, "single"))
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		{
			single=1;
		}
		else
		{
			single=0;
		}
	}
	ly_conf_set("audio_mode", "%d:%d:%d", random, repeat, single);
	return FALSE;
}

gboolean ly_ui_config_on_key_change_cb(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *label;
	dialog=gtk_dialog_new_with_buttons(_("Key"),
									   GTK_WINDOW(ly_ui_config_dialog),
									   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
									   GTK_STOCK_OK,
									   GTK_RESPONSE_ACCEPT,
									   GTK_STOCK_CANCEL,
									   GTK_RESPONSE_CANCEL,
									   NULL);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 100, 80);
	
	label=gtk_label_new(_("Press key to change shortcut ..."));
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label, FALSE, FALSE, 0);
	
	label=gtk_label_new(gtk_label_get_text(GTK_LABEL(data)));
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(dialog), "key_press_event", G_CALLBACK(ly_ui_config_on_key_press_cb), label);
	
	gtk_widget_show_all(dialog);
	
	int result=0;
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	switch(result)
	{
		case GTK_RESPONSE_CANCEL:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
		case GTK_RESPONSE_ACCEPT:
			break;
		default:
			return FALSE;
			break;
	}

	char keyname[128]="";
	lyUiKeyKeybind *k=(lyUiKeyKeybind*)g_malloc(sizeof(lyUiKeyKeybind));
	const char *str=gtk_label_get_text(GTK_LABEL(label));
	sscanf(str,"{%128[^\n}]%128[^\n_]%128[^\n:]%128[^\n]",keyname, k->mask0, k->mask1, k->key);
	if(ly_ui_key_get_conflict(keyname, (k->mask0)+1, (k->mask1)+1, (k->key)+1))
 	{
 		ly_msg_put("error", "ui:config", "Shortcuts conflict!");
 		gtk_widget_destroy(dialog);
 		return FALSE;
 	}
 	
	gtk_label_set_text(GTK_LABEL(data),str);
 	ly_ui_key_unbind(keyname);
	ly_ui_key_set_keys(keyname, (k->mask0)+1, (k->mask1)+1, (k->key)+1);
	ly_ui_key_bind(keyname);
	gtk_widget_destroy(dialog);
	return FALSE;
}

gboolean ly_ui_config_on_key_press_cb(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	if(event->type!=GDK_KEY_PRESS)
	{
		return FALSE;
	}

	gchar mask0[64]="";
	gchar mask1[64]="";
	gchar key[64]="";
	
	if((event->key).state&GDK_CONTROL_MASK)
	{
		g_strlcpy(mask0,"ctrl",sizeof(mask0));
	}
	if((event->key).state&GDK_MOD1_MASK)
	{
		if(g_str_equal(mask0,""))
		{
			g_strlcpy(mask0,"alt",sizeof(mask0));
		}
		else if(g_str_equal(mask1,""))
		{
			g_strlcpy(mask1,"alt",sizeof(mask1));
		}
	}
	if((event->key).state&GDK_SUPER_MASK)
	{
		if(g_str_equal(mask0,""))
		{
			g_strlcpy(mask0,"super",sizeof(mask0));
		}
		else if(g_str_equal(mask1,""))
		{
			g_strlcpy(mask1,"super",sizeof(mask1));
		}
	}
	if((event->key).state&GDK_SHIFT_MASK)
	{
		if(g_str_equal(mask0,""))
		{
			g_strlcpy(mask0,"shift",sizeof(mask0));
		}
		else if(g_str_equal(mask1,""))
		{
			g_strlcpy(mask1,"shift",sizeof(mask1));
		}
	}
	if((event->key).state&GDK_META_MASK)
	{
		if(g_str_equal(mask0,""))
		{
			g_strlcpy(mask0,"meta",sizeof(mask0));
		}
		else if(g_str_equal(mask1,""))
		{
			g_strlcpy(mask1,"meta",sizeof(mask1));
		}
	}
	if((event->key).state&GDK_HYPER_MASK)
	{
		if(g_str_equal(mask0,""))
		{
			g_strlcpy(mask0,"hyper",sizeof(mask0));
		}
		else
		{
			g_strlcpy(mask1,"hyper",sizeof(mask1));
		}
	}
	g_strlcpy(key,gdk_keyval_name((event->key).keyval), sizeof(key));
	char keyname[128]="";
	sscanf(gtk_label_get_text(GTK_LABEL(data)), "{%128[^\n}]", keyname);
	gchar str[512]="";
	g_snprintf(str, sizeof(str), "{%s}%s_%s:%s", keyname, mask0, mask1, key);
	gtk_label_set_text(GTK_LABEL(data), str);
	return FALSE;
}

gboolean ly_ui_config_on_theme_changed_cb(GtkWidget *widget, gpointer data)
{
	char *theme_name=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	ly_conf_set("theme", "%s", theme_name);
	ly_msg_put("info", "ui:config", _("Setting will not be actived until program restart!"));
	return FALSE;
}
