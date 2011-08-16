#include "Info.h"

const gchar* g_module_check_init(GModule *module)
{	
	return NULL;
}
void g_module_unload(GModule *module)
{
	;
}

GtkWidget *ly_plugin_info_create()
{
	GtkWidget *widget;
	GtkWidget *hbox;
	GtkWidget *image;
	GtkWidget *table;
	
	widget=gtk_event_box_new();
	hbox=gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(widget),hbox);
	
	gchar path[1024]={0};
	g_snprintf(path,sizeof(path),"%sui/icon/album_default.png",LY_GLOBAL_PROGDIR);
	GdkPixbuf *pixbuf=gdk_pixbuf_new_from_file_at_scale(path,200,200,FALSE,NULL);
	image=gtk_image_new_from_pixbuf(pixbuf);
	gtk_box_pack_start(GTK_BOX(hbox),image, FALSE,FALSE,30);
	
	
	GtkWidget *label;

	table=gtk_table_new(2, 4,FALSE);
	label=gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_markup(GTK_LABEL(label), _("<span size='x-large'>Title</span>"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_markup(GTK_LABEL(label), _("<span size='x-large'>Artist</span>"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_markup(GTK_LABEL(label), _("<span size='x-large'>Album</span>"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_markup(GTK_LABEL(label), _("<span size='x-large'>Duration</span>"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 5);


	if(ly_audio_meta)
		label_title=gtk_label_new(ly_audio_meta->title);
	else
		label_title=gtk_label_new(_("Unknown Title"));
	gtk_misc_set_alignment(GTK_MISC(label_title),0,1);
	gtk_label_set_ellipsize(GTK_LABEL(label_title),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label_title),20);
	gtk_table_attach(GTK_TABLE(table), label_title, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	if(ly_audio_meta)
		label_artist=gtk_label_new(ly_audio_meta->artist);
	else
		label_artist=gtk_label_new(_("Unknown Artist"));
	gtk_misc_set_alignment(GTK_MISC(label_artist),0,1);
	gtk_label_set_ellipsize(GTK_LABEL(label_artist),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label_artist),20);
	gtk_table_attach(GTK_TABLE(table), label_artist, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	if(ly_audio_meta)
		label_album=gtk_label_new(ly_audio_meta->album);
	else
		label_album=gtk_label_new(_("Unknown Album"));
	gtk_misc_set_alignment(GTK_MISC(label_album),0,1);
	gtk_label_set_ellipsize(GTK_LABEL(label_album),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label_album),20);
	gtk_table_attach(GTK_TABLE(table), label_album, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	if(ly_audio_meta)
		label_duration=gtk_label_new(ly_audio_meta->duration);
	else
		label_duration=gtk_label_new(_("Unknown Duration"));
	gtk_misc_set_alignment(GTK_MISC(label_duration),0,1);
	gtk_label_set_ellipsize(GTK_LABEL(label_duration),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label_duration),20);
	gtk_table_attach(GTK_TABLE(table), label_duration, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	
	GtkWidget *alignment;
	alignment=gtk_alignment_new(0.2,0.5,0,0);
	gtk_container_add(GTK_CONTAINER(alignment),table);
	gtk_box_pack_start(GTK_BOX(hbox),alignment,TRUE,TRUE,30);
	ly_msg_bind("meta_changed","core:audio",ly_plugin_info_on_meta_changed, NULL);
	return widget;
}
void ly_plugin_info_refresh()
{
	;
}
void ly_plugin_info_destroy()
{
	;
}

gboolean ly_plugin_info_on_meta_changed(gpointer message, gpointer data)
{
	gtk_label_set_text(GTK_LABEL(label_title), ly_audio_meta->title);
	gtk_label_set_text(GTK_LABEL(label_artist), ly_audio_meta->artist);
	gtk_label_set_text(GTK_LABEL(label_album), ly_audio_meta->album);
	gtk_label_set_text(GTK_LABEL(label_duration), ly_audio_meta->duration);
	return FALSE;
}