/*
 * inf_cover.c
 * This file is part of linnya:plugin
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya:plugin is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya:plugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya:plugin. If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * HEADERS
 */
#include "inf_cover.h"

/*
 * TYPES
 */
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

/*
 * VARIABLES
 */
GdkPixbuf *ly_3inf_cover_pixbuf_default=NULL;
GdkPixbuf *ly_3inf_cover_pixbuf=NULL;
/*
 * FUNCTIONS
 */

void		ly_3inf_cover_init()
{
	char path[1024]="";
	g_snprintf(path, sizeof(path), "%sinf/", LY_GLB_USER_PLUGINDIR);
	mkdir(path, 0755);
	LY_3INF_COVERDIR=g_strconcat(LY_GLB_USER_PLUGINDIR, "inf/cover/",NULL);
	mkdir(LY_3INF_COVERDIR, 0755);

	g_snprintf(path, sizeof(path), "%sicon/default_album.png", LY_GLB_PROG_UIDIR);
	ly_3inf_cover_pixbuf_default=gdk_pixbuf_new_from_file_at_scale(path, 164, 164, TRUE, NULL);
}

void		ly_3inf_cover_fina()
{
	if(ly_3inf_cover_pixbuf_default)
	{
		g_object_unref(ly_3inf_cover_pixbuf_default);
	}
	if(ly_3inf_cover_pixbuf)
	{
		g_object_unref(ly_3inf_cover_pixbuf);
	}
}


GdkPixbuf*		ly_3inf_cover_get()
{
	if(ly_3inf_cover_pixbuf)
	{
		return gdk_pixbuf_copy(ly_3inf_cover_pixbuf);
	}
	return gdk_pixbuf_copy(ly_3inf_cover_pixbuf_default);
}

gboolean	ly_3inf_cover_on_cover_got()
{
	if(ly_3inf_cover_pixbuf)
	{
		g_object_unref(ly_3inf_cover_pixbuf);
		ly_3inf_cover_pixbuf=NULL;
	}
	
	/*
	 * get cover from GstBuffer
	 */
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
	
	GstBuffer *buf=md->cover;
	if(!buf)
		return FALSE;

	GdkPixbufLoader *loader;
    GdkPixbuf *pixbuf_old;
    loader = gdk_pixbuf_loader_new();
    if(!gdk_pixbuf_loader_write(loader, buf->data, buf->size, NULL))
    {
        g_object_unref(loader);
        return FALSE;
    }
	pixbuf_old = gdk_pixbuf_loader_get_pixbuf(loader);
	ly_3inf_cover_pixbuf=gdk_pixbuf_scale_simple(pixbuf_old, 164, 164, GDK_INTERP_BILINEAR);
    if(pixbuf_old)
    	g_object_ref(pixbuf_old);
    gdk_pixbuf_loader_close(loader, NULL);
	g_object_unref(loader);
	
	return FALSE;
}

gboolean	ly_3inf_cover_on_meta_changed()
{
	/*
	 * get cover from custom directory
	 */
	if(ly_3inf_cover_pixbuf)
		g_object_unref(ly_3inf_cover_pixbuf);
	ly_3inf_cover_pixbuf=NULL;
	
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
	char path[1024]="";
	gint i=0;
	for(i=0; i<COVER_TYPE_COUNT; i++)
	{
		g_snprintf(path, sizeof(path), "%s%s-%s.%s",LY_3INF_COVERDIR, md->artist, md->album, cover_type[i]);
		if(g_file_test(path, G_FILE_TEST_EXISTS))
		{
			ly_3inf_cover_pixbuf=gdk_pixbuf_new_from_file_at_scale(path, 164, 164, TRUE, NULL);
			if(ly_3inf_cover_pixbuf)
				return FALSE;
		}
	}
}
