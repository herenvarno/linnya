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
void ly_3inf_on_meta_changed_cb(LyMbsMessage *message, gpointer data);
void ly_3inf_on_meta_update_cb(LyMbsMessage *message, gpointer data);
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
	ly_3inf_pixbuf_bg_copy=NULL;

	GtkWidget *widget;
	GtkWidget *event_box;
	GtkWidget *button;

	widget=gtk_overlay_new();
	event_box=gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(widget), event_box);
	gtk_widget_set_app_paintable(event_box, TRUE);
	g_signal_connect(G_OBJECT(event_box), "draw" ,G_CALLBACK (ly_3inf_on_expose_cb) , NULL);

	button=gtk_button_new();
	gtk_overlay_add_overlay (GTK_OVERLAY (widget), button);
	gtk_widget_set_size_request(button, 200, 200);
	gtk_widget_set_vexpand(button, FALSE);
	gtk_widget_set_hexpand(button, FALSE);
	gtk_widget_set_halign (button, GTK_ALIGN_START);
	gtk_widget_set_valign (button, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_left(button, 100);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_3inf_on_get_button_clicked_cb), NULL);

	ly_mbs_bind("meta_changed", "core:pqm", ly_3inf_on_meta_changed_cb, NULL);
	ly_mbs_bind("meta_update", "", ly_3inf_on_meta_update_cb, NULL);
	ly_mbs_bind("reg_3inf_title_font_changed", "core:reg", ly_3inf_on_meta_update_cb, NULL);
	ly_mbs_bind("reg_3inf_normal_font_changed", "core:reg", ly_3inf_on_meta_update_cb, NULL);

	ly_3inf_widget=event_box;
	ly_3inf_cover_on_meta_changed();

	gtk_widget_set_name(widget, "3inf_widget");
	gtk_widget_set_name(button, "3inf_btn_cover");

	/*
	 * Load Theme
	 */
	GdkScreen *screen;
	GtkCssProvider *provider;
	gchar csspath[1024]="";
	screen = gtk_widget_get_screen(widget);
	provider = gtk_css_provider_new();
	g_snprintf(csspath, sizeof(csspath), "%sinf/style/style.css", LY_GLB_PROG_PIXDIR);
	gtk_css_provider_load_from_path(provider, csspath, NULL);
	gtk_style_context_add_provider_for_screen(screen,GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_style_context_reset_widgets(screen);
	g_object_unref(provider);

	return widget;
}

void ly_3inf_destroy()
{
	if(ly_3inf_pixbuf_bg)
		g_object_unref(ly_3inf_pixbuf_bg);
	if(ly_3inf_pixbuf_bg_copy)
		g_object_unref(ly_3inf_pixbuf_bg_copy);
	ly_mbs_unbind("meta_changed", "core:pqm", ly_3inf_on_meta_changed_cb);
	ly_mbs_unbind("meta_update", "", ly_3inf_on_meta_update_cb);
	ly_mbs_unbind("reg_3inf_title_font_changed", "core:reg", ly_3inf_on_meta_update_cb);
	ly_mbs_unbind("reg_3inf_normal_font_changed", "core:reg", ly_3inf_on_meta_update_cb);
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
		if(ly_3inf_pixbuf_bg_copy)
		{
			int h=gdk_pixbuf_get_height(ly_3inf_pixbuf_bg_copy);
			int w=gdk_pixbuf_get_width(ly_3inf_pixbuf_bg_copy);
			if(h<height||h-height>2||w<width||w-width>2)
			{
				g_object_unref(ly_3inf_pixbuf_bg_copy);
				ly_3inf_pixbuf_bg_copy=gdk_pixbuf_scale_simple(ly_3inf_pixbuf_bg, width, height, GDK_INTERP_BILINEAR);
			}
		}
		else
		{
			ly_3inf_pixbuf_bg_copy=gdk_pixbuf_scale_simple(ly_3inf_pixbuf_bg, width, height, GDK_INTERP_BILINEAR);
		}
		gdk_cairo_set_source_pixbuf(cr, ly_3inf_pixbuf_bg_copy, 0, 0);
		cairo_paint(cr);
	}

	/*
	 * draw banner
	 */
	cairo_rectangle (cr, 0, height/2-120, width, 240);
	cairo_set_source_rgba (cr, 0, 0, 0, 0.65);
	cairo_fill(cr);

	cairo_set_line_width(cr, 0.5);
	cairo_set_source_rgba (cr, 0, 0, 0, 0.8);
	cairo_move_to(cr, 0, height/2-119.5);
	cairo_line_to(cr, width, height/2-118.5);
	cairo_stroke(cr);
	cairo_move_to(cr, 0, height/2+119.5);
	cairo_line_to(cr, width, height/2+118.5);
	cairo_stroke(cr);
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
	x=100;
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
	x=100+270;
	y=height/2-60;
	cairo_set_source_rgba ( cr, 0.1 , 0.1 , 0.1 ,1.0);
	cairo_move_to ( cr, x, y-2);
	ly_3inf_draw_text(cr, md->title, title_font);
	cairo_set_source_rgba ( cr, 0.9 , 0.9 , 0.9 ,1.0);
	cairo_move_to ( cr, x, y);
	ly_3inf_draw_text(cr, md->title, title_font);

	x=x;
	y=y+50;
	gchar str[1024]="";
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

	gint width, height;

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

void ly_3inf_on_meta_changed_cb(LyMbsMessage *message, gpointer data)
{
	ly_3inf_cover_on_meta_changed();
	gtk_widget_queue_draw (ly_3inf_widget);
}

void ly_3inf_on_meta_update_cb(LyMbsMessage *message, gpointer data)
{
	if(g_str_equal(ly_mbs_message_get_body(message), "cover"))
	{
		ly_3inf_cover_on_meta_changed();
	}
	gtk_widget_queue_draw (ly_3inf_widget);
}

gboolean ly_3inf_on_get_button_clicked_cb(GtkWidget *widget, gpointer data)
{
	ly_mbs_put("cov_missing", "plugin:inf", NULL);
	return FALSE;
}
