#include "Downlrc.h"
GtkWidget *window=NULL;
GtkWidget *vbox=NULL;
GtkWidget *button=NULL;
GThread *thread=NULL;
PyObject *pModule=NULL;

const gchar* g_module_check_init(GModule *module)
{
	ly_plugin_lrc_sort_lyrics();
	ly_plugin_python_init_pyplugin("Downlrc");
	pModule=ly_plugin_python_load_module("Downlrc");
	ly_msg_bind("lyric_missing", "plugin:lrc", ly_plugin_downlrc_download_cb, NULL);
	return NULL;
}
void g_module_unload(GModule *module)
{
	Py_Finalize();
}

GtkWidget *ly_plugin_downlrc_config()
{
	return NULL;
}

gboolean ly_plugin_downlrc_download_cb(gpointer message, gpointer data)
{
	if(!ly_audio_meta)
		return FALSE;
	
	thread=g_thread_create(ly_plugin_downlrc_create_thread_cb,((lyMsgMessage*)(message))->msg,FALSE,NULL);
	return TRUE;
}

void *ly_plugin_downlrc_create_thread_cb(void *arg)
{
	lyDbMetadata *meta=ly_audio_meta;
	
	char title[1024]="";
	char artist[1024]="";
	char dest[1024]="";
	g_strlcpy(title, ly_audio_meta->title, sizeof(title));
	g_strlcpy(artist, ly_audio_meta->artist, sizeof(artist));
	g_strlcpy(dest, (char *)arg, sizeof(dest));
	
	char downlrc_server[1024]="lrc123";
	if(!ly_conf_get("downlrc","%s", downlrc_server))
	{
		ly_conf_set("downlrc","%s", downlrc_server);
	}
	
	gboolean rt=FALSE;
	if(g_str_equal(downlrc_server,"lrc123"))
	{
		rt=ly_plugin_downlrc_download_lrc123(title, artist, dest);
	}
	else if(g_str_equal(downlrc_server,"qianqian"))
	{
		rt=ly_plugin_downlrc_download_lrc123(title, artist, dest);
	}
	if(rt && (ly_audio_meta==meta))
	{
		FILE *fp=fopen(dest,"r");
		if(fp)
		{
			ly_plugin_lrc_read_lyrics(fp);
			ly_plugin_lrc_sort_lyrics();
		}
	}
	return NULL;
}

gboolean ly_plugin_downlrc_download_lrc123(char *title, char *artist, char *dest)
{
	PyObject *urls=NULL;
	urls=ly_plugin_python_func(pModule, "download_lrc123", "(s,s)", title, artist);
	if(urls==NULL)
	{
		return FALSE;
	}
	
// 	if(window)
// 		gtk_widget_destroy(window);
// 	window=NULL;
// 	
// 	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
// 	gtk_window_set_default_size(GTK_WINDOW(window),100,50);
// 	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),window);
// 	vbox=gtk_vbox_new(TRUE,0);
// 	gtk_container_add(GTK_CONTAINER(window),vbox);
	
	PyObject *key, *value;
	Py_ssize_t pos = 0;
	char *keybuffer;
	char *valbuffer;
	Py_ssize_t l;
	char tmpstr[1024]="";
	while(PyDict_Next(urls, &pos, &key, &value)){
		
		keybuffer=NULL;
		valbuffer=NULL;
		PyBytes_AsStringAndSize(PyUnicode_AsUTF8String(key), &keybuffer, &l);
		PyBytes_AsStringAndSize(PyUnicode_AsUTF8String(value), &valbuffer, &l);
		printf("%d\n",l);
		g_strlcpy(tmpstr,valbuffer,sizeof(tmpstr));
		puts(tmpstr);
// 		button=gtk_button_new_with_label(g_strconcat(tmpstr,NULL));
// 		gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
//		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(),window);
	}
// 	gtk_widget_show_all(window);
	return TRUE;
}
gboolean ly_plugin_downlrc_download_qianqian(char *title, char *artist, char *dest)
{
	sleep(30);
	return FALSE;
}
