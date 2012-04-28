/*
 * inf.c
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
#include "inf.h"

/*
 * VARIABLES
 */
GtkWidget *ly_3inf_widget=NULL;
GdkPixbuf *ly_3inf_pixbuf_cd=NULL;
GdkPixbuf *ly_3inf_pixbuf_bg=NULL;
GdkPixbuf *ly_3inf_pixbuf_bg_copy=NULL;

/*
 * FUNCTIONS
 */
gboolean ly_3inf_on_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean ly_3inf_on_meta_changed_cb(gpointer message, gpointer data);
gboolean ly_3inf_on_meta_update_cb(gpointer message, gpointer data);
gboolean ly_3inf_on_get_button_clicked_cb(GtkWidget *widget, gpointer data);
void ly_3inf_draw_text_midx (cairo_t *cr, gchar *text, gchar *font, gint width_x, gint height_y);
void ly_3inf_draw_text (cairo_t *cr, gchar *text, gchar *font);

const gchar* g_module_check_init(GModule *module)
{
	gchar title_font[1024]="Sans Regular 18";
	if(!ly_reg_get("3inf_title_font", "%1024[^\n]", title_font))
		ly_reg_set("3inf_title_font", "%s", title_font);
	gchar normal_font[1024]="Sans Regular 10";
	if(!ly_reg_get("3inf_normal_font", "%1024[^\n]", normal_font))
		ly_reg_set("3inf_normal_font", "%s", normal_font);
		
	ly_3inf_cover_init();
	char path[1024]="";
	g_snprintf(path, sizeof(path), "%sicon/cd.png", LY_GLB_PROG_UIXDIR);
	ly_3inf_pixbuf_cd=gdk_pixbuf_new_from_file_at_scale(path, 250, 200, TRUE, NULL);
	return NULL;
}
void g_module_unload(GModule *module)
{
	if(ly_3inf_pixbuf_cd)
	{
		g_object_unref(ly_3inf_pixbuf_cd);
	}
	ly_3inf_cover_fina();
}


GtkWidget *ly_3inf_create()
{
	ly_3inf_pixbuf_bg=ly_sss_alloc_bg(NULL);
	ly_3inf_pixbuf_bg_copy=gdk_pixbuf_copy(ly_3inf_pixbuf_bg);
	
	GtkWidget *widget;
	GtkWidget *event_box;
	GtkWidget *button;
	
	widget=gtk_vbox_new(FALSE, 0);
	
	event_box=gtk_event_box_new();
	gtk_box_pack_start(GTK_BOX(widget), event_box, TRUE, TRUE, 0);
	gtk_widget_set_app_paintable(event_box, TRUE);
	g_signal_connect(G_OBJECT(event_box), "draw" ,G_CALLBACK (ly_3inf_on_expose_cb) , NULL);
	
	button=gtk_button_new_with_label(_("Download Cover"));
	gtk_box_pack_start(GTK_BOX(widget), button, FALSE, TRUE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_3inf_on_get_button_clicked_cb), NULL);
	
	ly_msg_bind("meta_changed", "core:pqm", ly_3inf_on_meta_changed_cb, NULL);
	ly_msg_bind("meta_update", "", ly_3inf_on_meta_update_cb, NULL);
	ly_msg_bind("reg_3inf_title_font_changed", "core:reg", ly_3inf_on_meta_update_cb, NULL);
	ly_msg_bind("reg_3inf_normal_font_changed", "core:reg", ly_3inf_on_meta_update_cb, NULL);
	
	ly_3inf_widget=event_box;
	ly_3inf_cover_on_meta_changed();
	
	return widget;
}

void ly_3inf_destroy()
{
	if(ly_3inf_pixbuf_bg)
		g_object_unref(ly_3inf_pixbuf_bg);
	if(ly_3inf_pixbuf_bg_copy)
		g_object_unref(ly_3inf_pixbuf_bg_copy);
	ly_msg_unbind("meta_changed", "core:pqm", ly_3inf_on_meta_changed_cb);
	ly_msg_unbind("meta_update", "", ly_3inf_on_meta_update_cb);
	ly_msg_unbind("reg_3inf_title_font_changed", "core:reg", ly_3inf_on_meta_update_cb);
	ly_msg_unbind("reg_3inf_normal_font_changed", "core:reg", ly_3inf_on_meta_update_cb);
}

gboolean ly_3inf_on_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	/*
	 * get width & height
	 */
	gint width;
	gint height;
	gint x;
	gint y;
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);
	
	/*
	 * draw bg
	 */
	if(ly_3inf_pixbuf_bg)
	{
		int h=gdk_pixbuf_get_height(ly_3inf_pixbuf_bg_copy);
		int w=gdk_pixbuf_get_width(ly_3inf_pixbuf_bg_copy);
		if(!ly_3inf_pixbuf_bg_copy||h<height||h-height>2||w<width||w-width>2)
		{
			if(ly_3inf_pixbuf_bg_copy)
			{
				g_object_unref(ly_3inf_pixbuf_bg_copy);
			}
			ly_3inf_pixbuf_bg_copy=gdk_pixbuf_scale_simple(ly_3inf_pixbuf_bg, width, height, GDK_INTERP_BILINEAR);	
		}
		gdk_cairo_set_source_pixbuf(cr, ly_3inf_pixbuf_bg_copy, 0, 0);	
		cairo_paint(cr);
	}

	/*
	 * draw banner
	 */
	cairo_rectangle (cr, 0, height/2-120, width, 240);
	cairo_set_source_rgba (cr, 0, 0, 0, 0.5);
	cairo_fill(cr);
	
	cairo_set_line_width(cr, 0.5);
	cairo_set_source_rgba (cr, 0.9, 0.9, 0.9, 0.7);
	cairo_move_to(cr, 0, height/2-118.5);
	cairo_line_to(cr, width, height/2-118.5);
	cairo_stroke(cr);
	cairo_move_to(cr, 0, height/2+118.5);
	cairo_line_to(cr, width, height/2+118.5);
	cairo_stroke(cr);
	
	/*
	 * draw image
	 */
	x=width/10;
	y=height/2-100;
	GdkPixbuf *pixbuf=NULL;
	pixbuf=ly_3inf_cover_get();
	if(pixbuf)
	{
		gdk_cairo_set_source_pixbuf(cr,	pixbuf, x, y);
		cairo_paint(cr);
		g_object_unref(pixbuf);
	}
	cairo_move_to(cr, 100, 100);
	if(ly_3inf_pixbuf_cd)
	{
		gdk_cairo_set_source_pixbuf(cr, ly_3inf_pixbuf_cd, x, y);
		cairo_paint(cr);
	}
	
	/*
	 * draw information
	 */
	gchar title_font[1024]="Sans Regular 18";
	ly_reg_get("3inf_title_font", "%1024[^\n]", title_font);
	gchar normal_font[1024]="Sans Regular 10";
	ly_reg_get("3inf_normal_font", "%1024[^\n]", normal_font);
	
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
	x=width/10+270;
	y=height/2-60;
	cairo_set_source_rgba ( cr, 0.1 , 0.1 , 0.1 ,1.0);
	cairo_move_to ( cr, x, y-2);
	ly_3inf_draw_text(cr, md->title, title_font);
	cairo_set_source_rgba ( cr, 0.9 , 0.9 , 0.9 ,1.0);
	cairo_move_to ( cr, x, y);
	ly_3inf_draw_text(cr, md->title, title_font);

	x=x;
	y=y+50;
	char str[1024]="";
	cairo_set_source_rgba ( cr, 0.7 , 0.7 , 0.7 ,0.8);
	cairo_move_to ( cr, x, y);
	ly_3inf_draw_text(cr, md->artist, normal_font);
	cairo_move_to ( cr, x, y+30);
	ly_3inf_draw_text(cr, md->album, normal_font);
	cairo_move_to ( cr, x, y+60);
	g_snprintf(str, sizeof(str), "%s - %d kb/s", md->codec, md->bitrate/1024);
	ly_3inf_draw_text(cr, str, normal_font);
	
	return FALSE;
}

void ly_3inf_draw_text (cairo_t *cr, gchar *text, gchar *font)
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

void ly_3inf_draw_text_midx (cairo_t *cr, gchar *text, gchar *font, gint width_x, gint height_y)
{
	PangoLayout *layout;
	PangoFontDescription *desc;
	
	int width, height;
	
	layout = pango_cairo_create_layout (cr);
	
	pango_layout_set_text (layout, text, -1);
	desc = pango_font_description_from_string (font);
	pango_layout_set_font_description (layout, desc);
	pango_font_description_free (desc);
	
	pango_cairo_update_layout (cr, layout);
	
	pango_layout_get_size (layout, &width, &height);
	if(width_x-(double)width/PANGO_SCALE>0)
		cairo_move_to (cr, (width_x - (double)width / PANGO_SCALE) / 2, height_y);
	else
		cairo_move_to (cr, 0, height_y);
	pango_cairo_show_layout (cr, layout);
	
	g_object_unref (layout);
}

gboolean ly_3inf_on_meta_changed_cb(gpointer message, gpointer data)
{
	ly_3inf_cover_on_meta_changed();
	gtk_widget_queue_draw (ly_3inf_widget);
}

gboolean ly_3inf_on_meta_update_cb(gpointer message, gpointer data)
{
	LyMsgMsg *m=(LyMsgMsg*)message;
	if(g_str_equal(m->msg, "cover"))
	{
		ly_3inf_cover_on_meta_changed();
	}
	gtk_widget_queue_draw (ly_3inf_widget);
}

gboolean ly_3inf_on_get_button_clicked_cb(GtkWidget *widget, gpointer data)
{
	ly_msg_put("cov_missing", "plugin:inf", NULL);
	return FALSE;
}
