#include "Info.h"

GtkWidget *label_title, *label_artist, *label_album, *label_duration;

const gchar* g_module_check_init(GModule *module)
{
	LY_PLUGIN_INFODIR=g_strconcat(LY_GLOBAL_PLUGINDIR, "info/",NULL);
	LY_PLUGIN_INFO_COVERDIR=g_strconcat(LY_PLUGIN_INFODIR, "cover/",NULL);

	mkdir(LY_PLUGIN_INFODIR, 0755);
	mkdir(LY_PLUGIN_INFO_COVERDIR, 0755);
	
//	ly_plugin_python_init_pyplugin("Info");
//	pModule=ly_plugin_python_load_module("InfoCoverDown");
	
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
	GtkWidget *table;
	GtkWidget *image;
	
	widget=gtk_event_box_new();
	hbox=gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(widget),hbox);
	
	ly_plugin_info_cover_set(NULL, NULL);
	image=ly_plugin_info_cover_image;
	gtk_box_pack_start(GTK_BOX(hbox),image, FALSE,FALSE,30);
	
	GtkWidget *label;
	table=gtk_table_new(2, 4,FALSE);
	label=gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_markup(GTK_LABEL(label), _("<span size='x-large'>Title</span>"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 5, 5);
	label=gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_markup(GTK_LABEL(label), _("<span size='x-large'>Artist</span>"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 5, 5);
	label=gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_markup(GTK_LABEL(label), _("<span size='x-large'>Album</span>"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 5, 5);
	label=gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_label_set_markup(GTK_LABEL(label), _("<span size='x-large'>Duration</span>"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 5, 5);


	if(ly_audio_meta)
		label_title=gtk_label_new(ly_audio_meta->title);
	else
		label_title=gtk_label_new(_("Unknown Title"));
	gtk_misc_set_alignment(GTK_MISC(label_title),0,1);
	//gtk_label_set_ellipsize(GTK_LABEL(label_title),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label_title),20);
	gtk_table_attach(GTK_TABLE(table), label_title, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 5, 5);
	if(ly_audio_meta)
		label_artist=gtk_label_new(ly_audio_meta->artist);
	else
		label_artist=gtk_label_new(_("Unknown Artist"));
	gtk_misc_set_alignment(GTK_MISC(label_artist),0,1);
	//gtk_label_set_ellipsize(GTK_LABEL(label_artist),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label_artist),20);
	gtk_table_attach(GTK_TABLE(table), label_artist, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 5, 5);
	if(ly_audio_meta)
		label_album=gtk_label_new(ly_audio_meta->album);
	else
		label_album=gtk_label_new(_("Unknown Album"));
	gtk_misc_set_alignment(GTK_MISC(label_album),0,1);
	//gtk_label_set_ellipsize(GTK_LABEL(label_album),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label_album),20);
	gtk_table_attach(GTK_TABLE(table), label_album, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 5, 5);
	if(ly_audio_meta)
		label_duration=gtk_label_new(ly_audio_meta->duration);
	else
		label_duration=gtk_label_new(_("Unknown Duration"));
	gtk_misc_set_alignment(GTK_MISC(label_duration),0,1);
	//gtk_label_set_ellipsize(GTK_LABEL(label_duration),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(label_duration),20);
	gtk_table_attach(GTK_TABLE(table), label_duration, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 5, 5);
	
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
	ly_plugin_info_cover_image=NULL;
	ly_msg_unbind("meta_changed", "core:audio", ly_plugin_info_on_meta_changed);
}

gboolean ly_plugin_info_on_meta_changed(gpointer message, gpointer data)
{
	gtk_label_set_text(GTK_LABEL(label_title), ly_audio_meta->title);
	gtk_label_set_text(GTK_LABEL(label_artist), ly_audio_meta->artist);
	gtk_label_set_text(GTK_LABEL(label_album), ly_audio_meta->album);
	gtk_label_set_text(GTK_LABEL(label_duration), ly_audio_meta->duration);
	
	ly_plugin_info_cover_set(NULL, NULL);
	return FALSE;
}
