/*
 * HEADERS
 */
#include "LrcConf.h"

/*
 * FUNCTIONS
 */
 
gboolean	ly_plugin_lrc_config_on_desktop_changed_cb		(GtkWidget *widget, gpointer data);
gboolean	ly_plugin_lrc_config_on_fixed_changed_cb		(GtkWidget *widget, gpointer data);
gboolean	ly_plugin_lrc_config_on_location_changed_cb		(GtkWidget *widget, gpointer data);
gboolean	ly_plugin_lrc_config_on_location_set_cb			(GtkWidget *widget, gpointer data);


GtkWidget *ly_plugin_lrc_config()
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *check;
	GtkWidget *button;	
	GtkWidget *entry;
	GtkWidget *label;
	
	vbox=gtk_vbox_new(FALSE,0);
	ly_ui_config_set_index(vbox,_("Desktop Lyrics"));

	int lrc_desktop=0;
	int lrc_desktop_fixed=0;
	ly_conf_get("lrc_desktop", "%d:%d", &lrc_desktop, &lrc_desktop_fixed);
	check=gtk_check_button_new_with_label(_("Show desktop lyrics"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)lrc_desktop);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_plugin_lrc_config_on_desktop_changed_cb), NULL);
	gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);
	check=gtk_check_button_new_with_label(_("Set desktop lyrics FIXED"));
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_plugin_lrc_config_on_fixed_changed_cb), NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)lrc_desktop_fixed);
	gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);
	
	ly_ui_config_set_index(vbox,_("LRC File Location"));


	char location[1024]="./";
	ly_conf_get("lrc_dir", "%s", location);
	
	hbox=gtk_hbox_new(FALSE,0);
	if(g_str_equal(location,"")||g_str_equal(location,"./"))
		gtk_widget_set_sensitive(hbox, FALSE);
	check=gtk_check_button_new_with_label(_("The same directory of music file"));
	if(g_str_equal(location,"")||g_str_equal(location,"./"))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_plugin_lrc_config_on_location_changed_cb), hbox);
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
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_plugin_lrc_config_on_location_set_cb), entry);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	
	return vbox;
}

gboolean	ly_plugin_lrc_config_on_desktop_changed_cb		(GtkWidget *widget, gpointer data)
{
	int lrc_desktop=0;
	int lrc_desktop_fixed=0;
	
	ly_conf_get("lrc_desktop", "%d:%d", &lrc_desktop, &lrc_desktop_fixed);
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	{
		lrc_desktop=1;
	}
	else
	{
		lrc_desktop=0;
	}
	ly_conf_set("lrc_desktop", "%d:%d", lrc_desktop, lrc_desktop_fixed);
	return FALSE;
}
gboolean	ly_plugin_lrc_config_on_fixed_changed_cb		(GtkWidget *widget, gpointer data)
{
	int lrc_desktop=0;
	int lrc_desktop_fixed=0;
	
	ly_conf_get("lrc_desktop", "%d:%d", &lrc_desktop, &lrc_desktop_fixed);
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	{
		lrc_desktop_fixed=1;
	}
	else
	{
		lrc_desktop_fixed=0;
	}
	ly_conf_set("lrc_desktop", "%d:%d", lrc_desktop, lrc_desktop_fixed);
	return FALSE;
}
gboolean	ly_plugin_lrc_config_on_location_changed_cb	(GtkWidget *widget, gpointer data)
{
	GtkWidget *w=(GtkWidget *)data;
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	{
		ly_conf_set("lrc_dir", "./");
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
gboolean	ly_plugin_lrc_config_on_location_set_cb		(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	dialog=gtk_file_chooser_dialog_new(_("Select library directory"),
												  GTK_WINDOW(ly_ui_win_window->win),
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
	
	ly_conf_set("lrc_dir", "%s", str);
	g_free(str);
	gtk_widget_destroy(dialog);
	
	return FALSE;
}
							
