/*
 * HEADERS
 */
#include "lrc_config.h"

/*
 * FUNCTIONS
 */
 
gboolean	ly_3lrc_config_on_desktop_changed_cb		(GtkWidget *widget, gpointer data);
gboolean	ly_3lrc_config_on_fixed_changed_cb			(GtkWidget *widget, gpointer data);
gboolean	ly_3lrc_config_on_location_changed_cb		(GtkWidget *widget, gpointer data);
gboolean	ly_3lrc_config_on_location_set_cb			(GtkWidget *widget, gpointer data);
gboolean	ly_3lrc_config_on_title_font_set_cb			(GtkWidget *widget, gpointer data);
gboolean	ly_3lrc_config_on_normal_font_set_cb		(GtkWidget *widget, gpointer data);
gboolean	ly_3lrc_config_on_desktop_font_set_cb		(GtkWidget *widget, gpointer data);

GtkWidget *ly_3lrc_config()
{
	GtkWidget *page;
	GtkWidget *item;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *check;
	GtkWidget *button;	
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *table;
	char str[1024]="";
	
	char name[1024]="";
	LyPliPlugin *pl=ly_pli_get("lrc");
	g_snprintf(name, sizeof(name), "PLUGIN:%s", pl->name);
	page=ly_cfg_page_new(name, pl->alias, pl->logo);
	
	item=ly_cfg_item_new(_("Desktop Lyrics"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	vbox=gtk_vbox_new(FALSE, 0);
	ly_cfg_item_append(LY_CFG_ITEM(item), vbox);
	int lrc_desktop=0;
	int lrc_desktop_fixed=0;
	ly_reg_get("3lrc_desktop_state", "%d:%d", &lrc_desktop, &lrc_desktop_fixed);
	check=gtk_check_button_new_with_label(_("Show desktop lyrics"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)lrc_desktop);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_3lrc_config_on_desktop_changed_cb), NULL);
	gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);
	check=gtk_check_button_new_with_label(_("Set desktop lyrics FIXED"));
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_3lrc_config_on_fixed_changed_cb), NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)lrc_desktop_fixed);
	gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);
	
	item=ly_cfg_item_new(_("Lyrics File Storage"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	char location[1024]="./";
	ly_reg_get("lrc_dir", "%s", location);
	vbox=gtk_vbox_new(FALSE, 0);
	ly_cfg_item_append(LY_CFG_ITEM(item), vbox);
	hbox=gtk_hbox_new(FALSE,0);
	if(g_str_equal(location,"")||g_str_equal(location,"./"))
		gtk_widget_set_sensitive(hbox, FALSE);
	check=gtk_check_button_new_with_label(_("The same directory of music file"));
	if(g_str_equal(location,"")||g_str_equal(location,"./"))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_3lrc_config_on_location_changed_cb), hbox);
	gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(_("Custom LRC location:"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entry=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), location);
	check=gtk_check_button_new_with_label(_("The same directory of music file"));
	gtk_widget_set_sensitive(entry,FALSE);
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_3lrc_config_on_location_set_cb), entry);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	
	item=ly_cfg_item_new(_("Font"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	table=gtk_table_new(3, 2, FALSE);
	ly_cfg_item_append(LY_CFG_ITEM(item), table);
	label=gtk_label_new(_("Title Font: "));
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 0, 1);
	ly_reg_get("3lrc_title_font", "%1024[^\n]", str);
	button=gtk_font_button_new_with_font(str);
	g_signal_connect(G_OBJECT(button), "font-set", G_CALLBACK(ly_3lrc_config_on_title_font_set_cb), NULL);
	gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, 0, 1);
	label=gtk_label_new(_("Normal Font: "));
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 1, 2);
	ly_reg_get("3lrc_normal_font", "%1024[^\n]", str);
	button=gtk_font_button_new_with_font(str);
	g_signal_connect(G_OBJECT(button), "font-set", G_CALLBACK(ly_3lrc_config_on_normal_font_set_cb), NULL);
	gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, 1, 2);
	label=gtk_label_new(_("Desktop Font: "));
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);
	ly_reg_get("3lrc_desktop_font", "%1024[^\n]", str);
	button=gtk_font_button_new_with_font(str);
	g_signal_connect(G_OBJECT(button), "font-set", G_CALLBACK(ly_3lrc_config_on_desktop_font_set_cb), NULL);
	gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, 2, 3);
	
	return page;
}

gboolean	ly_3lrc_config_on_desktop_changed_cb		(GtkWidget *widget, gpointer data)
{
	int lrc_desktop=0;
	int lrc_desktop_fixed=0;
	
	ly_reg_get("3lrc_desktop_state", "%d:%d", &lrc_desktop, &lrc_desktop_fixed);
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	{
		lrc_desktop=1;
	}
	else
	{
		lrc_desktop=0;
	}
	ly_reg_set("3lrc_desktop_state", "%d:%d", lrc_desktop, lrc_desktop_fixed);
	return FALSE;
}
gboolean	ly_3lrc_config_on_fixed_changed_cb		(GtkWidget *widget, gpointer data)
{
	int lrc_desktop=0;
	int lrc_desktop_fixed=0;
	
	ly_reg_get("3lrc_desktop_state", "%d:%d", &lrc_desktop, &lrc_desktop_fixed);
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	{
		lrc_desktop_fixed=1;
	}
	else
	{
		lrc_desktop_fixed=0;
	}
	ly_reg_set("3lrc_desktop_state", "%d:%d", lrc_desktop, lrc_desktop_fixed);
	return FALSE;
}
gboolean	ly_3lrc_config_on_location_changed_cb	(GtkWidget *widget, gpointer data)
{
	GtkWidget *w=(GtkWidget *)data;
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	{
		ly_reg_set("lrc_dir", "./");
		gtk_widget_set_sensitive(w, FALSE);
		GList *list=NULL;
		list=gtk_container_get_children(GTK_CONTAINER(w));
		if(!list)
			return FALSE;
		GList *p=list->next;
		gtk_entry_set_text(GTK_ENTRY(p->data), "./");
		g_list_free(list);
	}
	else
	{
		gtk_widget_set_sensitive(w, TRUE);
	}
	return FALSE;
}
gboolean	ly_3lrc_config_on_location_set_cb		(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	dialog=gtk_file_chooser_dialog_new(_("Select library directory"),
												  GTK_WINDOW(ly_win_get_window()->win),
												  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
												  GTK_STOCK_OPEN,
												  GTK_RESPONSE_ACCEPT,
												  GTK_STOCK_CANCEL,
												  GTK_RESPONSE_CANCEL,
												  NULL);
	gtk_widget_show_all(dialog);
	int result=gtk_dialog_run (GTK_DIALOG (dialog));
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
	
	char *path=gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
	char *str=g_strconcat(path,"/",NULL);
	g_free(path);
	
	gtk_entry_set_text(GTK_ENTRY(data),str);
	
	ly_reg_set("lrc_dir", "%s", str);
	g_free(str);
	gtk_widget_destroy(dialog);
	
	return FALSE;
}

gboolean ly_3lrc_config_on_title_font_set_cb(GtkWidget *widget, gpointer data)
{
	gchar font[1024]="Sans Regular 20";
	g_strlcpy(font, gtk_font_button_get_font_name(GTK_FONT_BUTTON(widget)), sizeof(font));
	ly_reg_set("3lrc_title_font", "%s", font);
	return FALSE;
}
gboolean ly_3lrc_config_on_normal_font_set_cb(GtkWidget *widget, gpointer data)
{
	gchar font[1024]="Sans Regular 10";
	g_strlcpy(font, gtk_font_button_get_font_name(GTK_FONT_BUTTON(widget)), sizeof(font));
	ly_reg_set("3lrc_normal_font", "%s", font);
	return FALSE;
}
gboolean ly_3lrc_config_on_desktop_font_set_cb(GtkWidget *widget, gpointer data)
{
	gchar font[1024]="Sans Regular 25";
	g_strlcpy(font, gtk_font_button_get_font_name(GTK_FONT_BUTTON(widget)), sizeof(font));
	ly_reg_set("3lrc_desktop_font", "%s", font);
	return FALSE;
}		
