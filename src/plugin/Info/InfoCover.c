#include "InfoCover.h"


enum {
	COVER_TYPE_JPEG,
	COVER_TYPE_JPG,
	COVER_TYPE_BMP,
	COVER_TYPE_PNG,
	COVER_TYPE_SVG,
	COVER_TYPE_COUNT
};
const char *cover_type[]={
	"jpeg",
	"jpg",
	"bmp",
	"png",
	"svg"
};

gboolean	ly_plugin_info_cover_set		(char *artist, char *album)
{
	if(artist==NULL && ly_audio_meta)
	{
		artist=ly_audio_meta->artist;
	}
	if(album==NULL && ly_audio_meta)
	{
		album=ly_audio_meta->album;
	}
	
	if(artist==NULL || album==NULL)
	{
		return FALSE;
	}
	
	GdkPixbuf *pixbuf=NULL;
	char *cover_path=NULL;
	char path[1024]="";
	
	cover_path=ly_plugin_info_cover_get(ly_audio_meta->artist, ly_audio_meta->album);
	if(cover_path)
	{
		g_strlcpy(path, cover_path, sizeof(path));
		g_free(cover_path);
	}
	else
	{
		cover_path=ly_plugin_info_cover_down(ly_audio_meta->artist, ly_audio_meta->album);
		if(cover_path)
		{
			g_strlcpy(path, cover_path, sizeof(path));
			g_free(cover_path);
		}	
		else
		{
			g_snprintf(path,sizeof(path),"%sui/icon/album_default.png",LY_GLOBAL_PROGDIR);
		}
	}
	pixbuf=gdk_pixbuf_new_from_file_at_scale(path,200,200,FALSE,NULL);
	if(pixbuf)
	{
		if(ly_plugin_info_cover_image==NULL)
		{
			ly_plugin_info_cover_image=gtk_image_new_from_pixbuf(pixbuf);
		}
		else
		{
			gtk_image_set_from_pixbuf(GTK_IMAGE(ly_plugin_info_cover_image), pixbuf);
		}
		return TRUE;
	}
	return FALSE;
}
char*		ly_plugin_info_cover_get		(char *artist, char *album)
{
	char path[1024]="";
	int i=0;
	for(i=0; i<COVER_TYPE_COUNT; i++)
	{
		g_snprintf(path, sizeof(path), "%s%s-%s.%s",LY_PLUGIN_INFO_COVERDIR, artist, album, cover_type[i]);
		if(g_file_test(path, G_FILE_TEST_EXISTS))
		{
			return g_strdup(path);
		}
	}
	return NULL;
}
char*		ly_plugin_info_cover_down		(char *artist, char *album)
{
/*	puts("********");
	PyObject *rt=NULL;
	rt=ly_plugin_python_func(pModule, "cover_search_and_down", "(s,s,s)", artist, album, LY_PLUGIN_INFO_COVERDIR);
	puts(PyBytes_AsString(rt));
	return PyBytes_AsString(rt);*/
	return NULL;
}
