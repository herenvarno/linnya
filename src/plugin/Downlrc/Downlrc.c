#include "Downlrc.h"
GThread *thread=NULL;
PyObject *pModule=NULL;

const gchar* g_module_check_init(GModule *module)
{
	if(!g_thread_get_initialized())
	{
		g_thread_init(NULL);
	}
	ly_plugin_lrc_sort_lyrics();
	ly_plugin_python_init_pyplugin("Downlrc");
	pModule=ly_plugin_python_load_module("Downlrc");
	ly_msg_bind("lyric_missing", "plugin:lrc", ly_plugin_downlrc_download_cb, NULL);
	return NULL;
}
void g_module_unload(GModule *module)
{
	ly_msg_unbind("lyric_missing", "plugin:lrc", ly_plugin_downlrc_download_cb);
}

GtkWidget *ly_plugin_downlrc_config()
{
	return NULL;
}

gboolean ly_plugin_downlrc_download_cb(gpointer message, gpointer data)
{
	if(!ly_audio_meta)
		return FALSE;
	if(thread)
		return FALSE;
	thread=g_thread_create(ly_plugin_downlrc_create_thread_cb, ((lyMsgMessage*)(message))->msg, FALSE, NULL);
	return TRUE;
}

gpointer ly_plugin_downlrc_create_thread_cb(void *arg)
{
	lyDbMetadata *meta=ly_audio_meta;
	
	char title[1024]="";
	char artist[1024]="";
	char dest[1024]="";
	g_strlcpy(title, ly_audio_meta->title, sizeof(title));
	g_strlcpy(artist, ly_audio_meta->artist, sizeof(artist));
	g_strlcpy(dest, (char *)arg, sizeof(dest));
	
	char downlrc_server[1024]="qianqian";
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
		rt=ly_plugin_downlrc_download_qianqian(title, artist, dest);
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
	thread=NULL;
	return NULL;
}

gboolean ly_plugin_downlrc_download_lrc123(char *title, char *artist, char *dest)
{
	PyObject *rt=NULL;
	rt=ly_plugin_python_func(pModule, "download_lrc123_search_and_down", "(s,s,s)", title, artist, dest);
	if(rt)
		return TRUE;
	return FALSE;
}
gboolean ly_plugin_downlrc_download_qianqian(char *title, char *artist, char *dest)
{
	PyObject *rt=NULL;
	rt=ly_plugin_python_func(pModule, "download_qianqian_search_and_down", "(s,s,s)", title, artist, dest);
	if(rt)
		return TRUE;
	return FALSE;
}
