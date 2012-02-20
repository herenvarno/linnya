/*
 * gud.c
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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
#include "gud.h"

/*
 * VARIABLES
 */
GtkWidget *assistant;


/*
 * FUNCTIONS
 */
gboolean ly_gud_on_audio_button_clicked_cb(GtkWidget *widget, gpointer data);
gboolean ly_gud_on_lib_entry_changed_cb(GtkWidget *widget, gpointer data);
gboolean ly_gud_on_lib_open_clicked_cb(GtkWidget *widget, gpointer data);
gboolean ly_gud_on_lib_load_clicked_cb(GtkWidget *widget, gpointer data);
gpointer ly_gud_on_load_lib_cb(void *data);

void	ly_gud_init()
{
	if(LY_GLA_FIRST_FLAG==FALSE)
		return;
	
	assistant=gtk_assistant_new();
	gtk_window_set_default_size(GTK_WINDOW(assistant),500,400);
	gtk_window_set_position(GTK_WINDOW(assistant),GTK_WIN_POS_CENTER);
	
	g_signal_connect(G_OBJECT(assistant),"delete-event",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(assistant),"delete-event",G_CALLBACK(gtk_widget_destroy),assistant);
	g_signal_connect(G_OBJECT(assistant),"cancel",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(assistant),"cancel",G_CALLBACK(gtk_widget_destroy),assistant);
	g_signal_connect(G_OBJECT(assistant),"close",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(assistant),"close",G_CALLBACK(gtk_widget_destroy),assistant);

	char path[10240]="";
	char text[10240]="";
	GdkPixbuf *pixbuf=NULL;
	GtkWidget *page=NULL;
	GtkWidget *hbox=NULL;
	GtkWidget *label=NULL;
	GtkWidget *entry=NULL;
	GtkWidget *button=NULL;
	GtkWidget *bar=NULL;
	
	page=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(page), _("<b>It seems that this is your first time using Linnya</b>\n<b>Let's start your music discovery tour NOW !</b>"));
	gtk_assistant_append_page(GTK_ASSISTANT(assistant), page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), page, _("WELCOME"));
	g_snprintf(path, sizeof(path), "%sui/icon/wizard.svg", LY_GLA_PROGDIR);
	pixbuf=gdk_pixbuf_new_from_file_at_size(path,64,64,NULL);
	gtk_assistant_set_page_header_image(GTK_ASSISTANT(assistant), page, pixbuf);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), page, TRUE);
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), page, GTK_ASSISTANT_PAGE_INTRO);
	
	page=gtk_vbox_new(FALSE,0);
	gtk_assistant_append_page(GTK_ASSISTANT(assistant),page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant),page,_("SETTING (audio)"));
	g_snprintf(path, sizeof(path), "%sui/icon/audio.svg", LY_GLA_PROGDIR);
	gtk_container_set_border_width(GTK_CONTAINER(page),10);
	pixbuf=gdk_pixbuf_new_from_file_at_size(path,64,64,NULL);
	gtk_assistant_set_page_header_image(GTK_ASSISTANT(assistant),page, pixbuf);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant),page,FALSE);
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("Press \"PLAY\" button to play the test music."));
	gtk_box_pack_start(GTK_BOX(page),label,TRUE,TRUE,0);
	button=gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(button),gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON));
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_gud_on_audio_button_clicked_cb), page);
	gtk_box_pack_start(GTK_BOX(page),button,TRUE,TRUE,0);
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant),page,GTK_ASSISTANT_PAGE_CONTENT);
	
	page=gtk_vbox_new(FALSE,0);
	gtk_assistant_append_page(GTK_ASSISTANT(assistant),page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant),page,_("SETTING (library)"));
	g_snprintf(path, sizeof(path), "%sui/icon/lib.svg", LY_GLA_PROGDIR);
	gtk_container_set_border_width(GTK_CONTAINER(page),10);
	pixbuf=gdk_pixbuf_new_from_file_at_size(path,64,64,NULL);
	gtk_assistant_set_page_header_image(GTK_ASSISTANT(assistant),page,pixbuf);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant),page,FALSE);
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>First,</b> Choose the directory in which you store your music\n<b>Then,</b> press (Load) button to load all of them"));
	gtk_box_pack_start(GTK_BOX(page),label,TRUE,FALSE,0);
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(page),hbox,TRUE,FALSE,0);
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label),_("<b>Library Location:</b>"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entry=gtk_entry_new();
	g_snprintf(text, sizeof(text), "%smusic/", LY_GLA_HOMEDIR);
	if(!ly_reg_get("lib_path", "%s", text))
	{
		ly_reg_set("lib_path", "%s", text);
	}
	gtk_entry_set_text(GTK_ENTRY(entry), text);
	g_signal_connect(G_OBJECT(entry),"changed",G_CALLBACK(ly_gud_on_lib_entry_changed_cb),NULL);
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_gud_on_lib_open_clicked_cb),entry);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	button=gtk_button_new_with_label(_("Load"));
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	bar=gtk_progress_bar_new();
	gtk_orientable_set_orientation(GTK_ORIENTABLE(bar), GTK_ORIENTATION_HORIZONTAL);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_gud_on_lib_load_clicked_cb),bar);
	gtk_box_pack_start(GTK_BOX(page),bar,FALSE,FALSE,0);
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant),page,GTK_ASSISTANT_PAGE_CONTENT);
		gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), page, TRUE);
	
	page=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(page), _("<b>All Settings are complete. Have fun with Linnya</b>"));
	gtk_assistant_append_page(GTK_ASSISTANT(assistant),page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant),page,_("SUMMARY"));
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant),page,TRUE);
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant),page,GTK_ASSISTANT_PAGE_SUMMARY);
	
	gtk_widget_show_all(assistant);
	gtk_main();
}

void	ly_gud_fina()
{
}


gboolean ly_gud_on_audio_button_clicked_cb(GtkWidget *widget, gpointer data)
{
	if(ly_aud_get_state()==GST_STATE_PLAYING)
	{
		ly_aud_stop();
		gtk_button_set_image(GTK_BUTTON(widget),gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON));
	}
	else
	{
		char path[1024];
		g_snprintf(path,sizeof(path), "file://%sui/audio/test.ogg", LY_GLA_PROGDIR);
		LyMdhMetadata *md=ly_pqm_get_current_md();
		if(md)
		{
			g_free(md);
		}
		md=ly_mdh_new_with_uri(path);
		ly_aud_play();
		gtk_button_set_image(GTK_BUTTON(widget),gtk_image_new_from_stock(GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_BUTTON));
		
		GtkWidget *dialog=gtk_dialog_new_with_buttons(	_("Does it work?"),
						GTK_WINDOW(assistant),
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				     GTK_STOCK_YES,
				     GTK_RESPONSE_ACCEPT,
				     GTK_STOCK_NO,
				     GTK_RESPONSE_REJECT,
				     NULL);
		GtkWidget *label=gtk_label_new(_("Do you hear any sound?"));
		gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label);
		gtk_widget_show_all(dialog);
		switch(gtk_dialog_run(GTK_DIALOG(dialog)))
		{
			case GTK_RESPONSE_REJECT:
				ly_msg_put("error", "ui:guide", "Audio pipeline error, linnya maybe not works properly!");
				break;
			default:
				break;
		}
		gtk_widget_destroy(dialog);
		
		ly_aud_stop();
		gtk_button_set_image(GTK_BUTTON(widget),gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON));
		
		GtkWidget *page=(GtkWidget*)data;
		gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), page, TRUE);
	}
	return FALSE;
}

gboolean ly_gud_on_lib_entry_changed_cb(GtkWidget *widget, gpointer data)
{
	ly_reg_set("lib_path", "%s", gtk_entry_get_text(GTK_ENTRY(widget)));
	return FALSE;
}

gboolean ly_gud_on_lib_open_clicked_cb(GtkWidget *widget, gpointer data)
{
	gint result=0;
	GtkWidget *dialog=gtk_file_chooser_dialog_new(_("Select library directory"),
												  GTK_WINDOW(assistant),
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
	
	ly_reg_set("lib_path", "%s", str);
	g_free(str);
	gtk_widget_destroy(dialog);
	return FALSE;
}

gboolean ly_gud_on_lib_load_clicked_cb(GtkWidget *widget, gpointer data)
{
	GThread *thread=g_thread_create(ly_gud_on_load_lib_cb, data, FALSE, NULL);
	return FALSE;
}

gpointer ly_gud_on_load_lib_cb(void *data)
{
	/*
	 * get library path
	 */
	gchar path[10240]="";
	g_snprintf(path,sizeof(path),"%smusic/", LY_GLA_HOMEDIR);
	if(!ly_reg_get("lib_path","%s",path))
	{
		ly_reg_set("lib_path","%s",path);
	}

	/*
	 * get filenames
	 */
	GList *list=NULL;
	list=ly_gla_traverse_dir(path, 5, TRUE);
	if(list==NULL)
		return FALSE;
	GList *p=list;
	gchar *sql=NULL;
	
	/*
	 * add new music
	 */
	ly_lib_check_is_exist_list(&list);
	
	int l=g_list_length(list);
	if(l<=0)
		return FALSE;
	
	ly_db_exec("begin",NULL,NULL);
	printf("%d\n", l);
	GtkWidget *pbar = (GtkWidget *)data;
	gtk_progress_bar_set_pulse_step(GTK_PROGRESS_BAR(pbar), 1/(double)l);
	
	LyMdhMetadata *metadata=NULL;
	p=list;
	while(p)
	{
		metadata=ly_mdh_new_with_uri((gchar *)(p->data));
		ly_lib_add(metadata);
		g_free(metadata);
		metadata=NULL;
		g_free(p->data);
		p->data=NULL;
		p=p->next;
		gtk_progress_bar_pulse(GTK_PROGRESS_BAR(pbar));
	}
	g_list_free(list);
	ly_db_exec("commit",NULL,NULL);
	
	return FALSE;
}
