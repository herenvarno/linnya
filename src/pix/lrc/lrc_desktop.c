/*
 * lrc_desktop.c
 * This file is part of linnya:lrc
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya:lrc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya:lrc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya:lrc. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * HEADERS
 */
#include "lrc_desktop.h"

/*
 * TYPES
 */
enum{
	X,
	Y
};

/*
 * VARIABLE
 */
GtkWidget		*ly_3lrc_desktop=NULL;
gint			ly_3lrc_desktop_timeout=0;

cairo_surface_t	*desktop_bg=NULL;
gboolean flag_notify	=	FALSE;
gboolean flag_drag		=	FALSE;

int pos_layout[2]={0,0};
int pos_old[2]={0,0};

/*
 * FUNCTIONS
 */
gboolean ly_3lrc_desktop_on_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean ly_3lrc_desktop_on_drag_cb( GtkWidget * widget, GdkEvent *event, gpointer data);
gboolean ly_3lrc_desktop_on_update_cb(gpointer data);

void		ly_3lrc_desktop_init			()
{
	int w=1000;
	int h=50;
	int x=0;
	int y=0;
	if(!ly_reg_get("3lrc_desktop_pos_and_size", "%d:%d:%d:%d", &x, &y, &w, &h))
	{
		ly_reg_set("3lrc_desktop_pos_and_size", "%d:%d:%d:%d", x, y, w, h);
	}
	ly_3lrc_desktop_create();
}

void		ly_3lrc_desktop_fina			()
{
	ly_3lrc_desktop_destroy();
}

void		ly_3lrc_desktop_create			()
{
	int lrc_desktop_visible=1;
	if(!gtk_widget_is_composited(ly_win_get_window()->win))
	{
		lrc_desktop_visible=0;
	}
	int lrc_desktop_fixed=0;
	if(!ly_reg_get("3lrc_desktop_state", "%d:%d", &lrc_desktop_visible, &lrc_desktop_fixed))
	{
		ly_reg_set("3lrc_desktop_state", "%d:%d", lrc_desktop_visible, lrc_desktop_fixed);
	}

	int w=1000;
	int h=50;
	int x=0;
	int y=0;
	ly_reg_get("3lrc_desktop_pos_and_size", "%d:%d:%d:%d", &x, &y, &w, &h);	
	ly_3lrc_desktop=gtk_window_new(GTK_WINDOW_POPUP);
	gtk_window_set_default_size(GTK_WINDOW(ly_3lrc_desktop),w,h);
	gtk_window_move(GTK_WINDOW(ly_3lrc_desktop),x,y);
	gtk_widget_set_app_paintable(ly_3lrc_desktop, TRUE);
	gtk_window_set_decorated(GTK_WINDOW(ly_3lrc_desktop), FALSE);
		
	GdkVisual *visual;
	GdkScreen *screen;
	screen = gtk_widget_get_screen(ly_3lrc_desktop);	
	visual = gdk_screen_get_rgba_visual (screen);
	if (visual == NULL)
		visual = gdk_screen_get_system_visual (screen);
	gtk_widget_set_visual (GTK_WIDGET (ly_3lrc_desktop), visual);

	gtk_widget_set_events(ly_3lrc_desktop,GDK_ALL_EVENTS_MASK);
		
	if(lrc_desktop_visible<=0)
		gtk_widget_hide(ly_3lrc_desktop);
	if(lrc_desktop_fixed>0)
		gtk_widget_set_sensitive(ly_3lrc_desktop,FALSE);
			
	gtk_widget_show_all(ly_3lrc_desktop);
	g_signal_connect(G_OBJECT(ly_3lrc_desktop), "draw", G_CALLBACK(ly_3lrc_desktop_on_expose_cb), NULL);
	g_signal_connect(G_OBJECT(ly_3lrc_desktop), "button_press_event", G_CALLBACK(ly_3lrc_desktop_on_drag_cb), NULL);
	g_signal_connect(G_OBJECT(ly_3lrc_desktop), "button_release_event", G_CALLBACK(ly_3lrc_desktop_on_drag_cb), NULL);
	g_signal_connect(G_OBJECT(ly_3lrc_desktop), "motion_notify_event", G_CALLBACK(ly_3lrc_desktop_on_drag_cb), NULL);
	g_signal_connect(G_OBJECT(ly_3lrc_desktop), "enter_notify_event", G_CALLBACK(ly_3lrc_desktop_on_drag_cb), NULL);
	g_signal_connect(G_OBJECT(ly_3lrc_desktop), "leave_notify_event", G_CALLBACK(ly_3lrc_desktop_on_drag_cb), NULL);

	ly_3lrc_desktop_timeout=g_timeout_add(100, ly_3lrc_desktop_on_update_cb, NULL);
}
void		ly_3lrc_desktop_destroy			()
{
	g_source_remove(ly_3lrc_desktop_timeout);
	gtk_widget_destroy(ly_3lrc_desktop);
}

gboolean	ly_3lrc_desktop_on_expose_cb	(GtkWidget * widget, cairo_t *cr, gpointer data)
{
	gchar desktop_font[1024]="Sans Regular 25";
	ly_reg_get("3lrc_desktop_font", "%1024[^\n]", desktop_font);

	gchar path[1024];
	g_snprintf(path,sizeof(path),"%sicon/null.png",LY_GLB_PROG_UIXDIR);
	if(!desktop_bg)
		desktop_bg=cairo_image_surface_create_from_png(path);

	cairo_set_source_surface(cr, desktop_bg, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	
	int width;
	int height;
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);
	
	int x=0;
	int y=height;
	
	//鼠标进入
	if(flag_notify)
	{
		//填充圆角矩形拖动区域
		cairo_set_source_rgba(cr,0,0,0,0.3);
		cairo_move_to (cr, 0 + 5, 0);
		cairo_line_to (cr, 0 + width - 5, 0);
		cairo_move_to (cr, 0 + width, 0 + 5);
		cairo_line_to (cr, 0 + width, 0 + height - 5);
		cairo_move_to (cr, 0 + width - 5, 0 + height);
		cairo_line_to (cr, 0 + 5, 0 + height);
		cairo_move_to (cr, 0, 0 + height - 5);
		cairo_line_to (cr, 0, 0 + 5);
		cairo_arc (cr, 0 + 5, 0 + 5, 5, M_PI, 3 * M_PI / 2.0);
		cairo_arc (cr, 0 + width - 5, 0 + 5, 5, 3 * M_PI / 2, 2 * M_PI);
		cairo_arc (cr, 0 + width - 5, 0 + height - 5, 5, 0, M_PI / 2);
		cairo_arc (cr, 0 + 5, 0 + height - 5, 5, M_PI / 2, M_PI);
		cairo_fill(cr);
	}
	
	if(ly_lrc_get_length()>0)
	{
		//计算占空比
		gint64 t1=0;
		gint64 t2=0;
		
		LyMdhMetadata *md=NULL;
		md=ly_pqm_get_current_md();
		if(!md)
			return FALSE;
		if(ly_lrc_get_index()+1<ly_lrc_get_length())
			t1=(ly_lrc_get_array()[ly_lrc_get_index()+1])->time-(ly_lrc_get_array()[ly_lrc_get_index()])->time;
		else
			t1=ly_mdh_time_str2int(md->duration)-ly_mdh_time_str2int(md->start)-(ly_lrc_get_array()[ly_lrc_get_index()])->time;
		
		t2=ly_aud_get_position_abs()-ly_mdh_time_str2int(md->start)-(ly_lrc_get_array()[ly_lrc_get_index()])->time;
		if(t1!=0)
		{
			x=(int)((t2/(gdouble)t1)*pos_layout[X]);
		}
		
		//画歌词
		cairo_set_source_rgb(cr,0,0,0);
		
		//确定起始点
		if((x>=width/2)&&(pos_layout[X]>width)&&(width<(pos_layout[X]-x)*2))
		{
			cairo_move_to(cr, 0-(x-width/2), 5);
			x=width/2;
		}
		else if((x>=width/2)&&(pos_layout[X]>width)&&(width>=(pos_layout[X]-x)*2))
			cairo_move_to(cr, 0-(pos_layout[X]-width), 5);
		else
			cairo_move_to(cr, 0, 5);
		
		PangoLayout *layout;
		PangoFontDescription *desc;
		layout = pango_cairo_create_layout (cr);
		
		pango_layout_set_text(layout, ly_lrc_get_array()[ly_lrc_get_index()]->text, -1);
		desc = pango_font_description_from_string (desktop_font);
		pango_layout_set_font_description (layout, desc);
		pango_font_description_free (desc);
		
		
		pango_layout_get_size(layout,&pos_layout[X],&pos_layout[Y]);
		pos_layout[X]=pos_layout[X]/1000;
		
		pango_cairo_update_layout (cr, layout);
		pango_cairo_layout_path (cr, layout);
		cairo_clip(cr);
		g_object_unref (layout);
		
		//画背景条
		cairo_pattern_t *pat;
		pat = cairo_pattern_create_linear (0, 0,10.0, 150);
		cairo_pattern_add_color_stop_rgb (pat, 0.1, 65535/65535.0, 10449/65535.0, 0/65535.0);
		cairo_pattern_add_color_stop_rgb (pat, 0.5, 65535/65535.0, 61062/65535.0, 0/65535.0);
		cairo_pattern_add_color_stop_rgb (pat, 0.9, 65535/65535.0, 10449/65535.0, 0/65535.0);
		//画矩形
		if((x>=width/2)&&(pos_layout[X]>width)&&(width<(pos_layout[X]-x)*2))
			cairo_rectangle (cr, 0, 0, width/2, y);
		else if((x>=width/2)&&(pos_layout[X]>width)&&(width>=(pos_layout[X]-x)*2))
			cairo_rectangle (cr, 0, 0, width-(pos_layout[X]-x), y);
		else
			cairo_rectangle (cr, 0, 0, x, y);
		cairo_set_source (cr, pat);
		cairo_fill(cr);
		cairo_pattern_destroy (pat);
		pat = cairo_pattern_create_linear (0, 0,10.0, 150);
		cairo_pattern_add_color_stop_rgb (pat, 0.1, 19532/65535.0, 65535/65535.0, 65535/65535.0);
		cairo_pattern_add_color_stop_rgb (pat, 0.5, 5539/65535.0, 0/65535.0, 65535/65535.0);
		cairo_pattern_add_color_stop_rgb (pat, 0.9, 19532/65535.0, 65535/65535.0, 65535/65535.0);
		//画矩形
		if((x>=width/2)&&(pos_layout[X]>width)&&(width<(pos_layout[X]-x)*2))
			cairo_rectangle (cr, width/2, 0, width-(width/2), y);
		else if((x>=width/2)&&(pos_layout[X]>width)&&(width>=(pos_layout[X]-x)*2))
			cairo_rectangle (cr, width-(pos_layout[X]-x), 0, pos_layout[X]-x, y);
		else
			cairo_rectangle (cr, x, 0, width-x, y);
		cairo_set_source (cr, pat);
		cairo_fill(cr);
		cairo_pattern_destroy (pat);
	}
	
	int lrc_desktop=1;
	int lrc_desktop_fix=0;
	if(!ly_reg_get("3lrc_desktop_state", "%d:%d", &lrc_desktop, &lrc_desktop_fix))
	{
		ly_reg_set("3lrc_desktop_state", "%d:%d", lrc_desktop, lrc_desktop_fix);
	}
	if(lrc_desktop_fix>0)
		gtk_widget_set_sensitive(widget,FALSE);
	else
		gtk_widget_set_sensitive(widget,TRUE);
	cairo_region_t *region;
	if(!(lrc_desktop_fix))
	{
		cairo_rectangle_int_t rect;
		rect.x=rect.y=0;
		rect.width=width;
		rect.height=height;
		region=cairo_region_create_rectangle(&rect);
	}
	else
	{
		region=cairo_region_create();
	}
	gdk_window_input_shape_combine_region (gtk_widget_get_window(widget), region, 0, 0);
	cairo_region_destroy (region);
	
	return FALSE;
}

gboolean ly_3lrc_desktop_on_update_cb(gpointer data)
{
	if(!ly_3lrc_desktop)
		return TRUE;
		
	int lrc_desktop_visible=0;
	ly_reg_get("3lrc_desktop_state", "%d:%*d", &lrc_desktop_visible);
	if(lrc_desktop_visible<=0)
	{
		gtk_widget_hide(ly_3lrc_desktop);
		return TRUE;
	}
	else
	{
		gtk_widget_show(ly_3lrc_desktop);
		gtk_widget_queue_draw (ly_3lrc_desktop);
	}	
	return TRUE;
}

gboolean ly_3lrc_desktop_on_drag_cb( GtkWidget * widget, GdkEvent *event, gpointer data)
{
	//GdkCursor *cursor;
	if (event->button.button == 1)
	{
		switch(event->type)
		{
			case GDK_BUTTON_PRESS:
				pos_old[X] = event->button.x;
				pos_old[Y] = event->button.y;
				flag_drag=TRUE;
				
				//鼠标变成手形
				//cursor= gdk_cursor_new(GDK_FLEUR);
				//gdk_window_set_cursor(widget->window,cursor);
				//gdk_cursor_destroy(cursor);
				break;
			case GDK_BUTTON_RELEASE:
				flag_drag=FALSE;
				
				//鼠标变成默认箭头
				//cursor= gdk_cursor_new(GDK_LEFT_PTR);
				//gdk_window_set_cursor(widget->window,cursor);
				//gdk_cursor_destroy(cursor);
				gtk_window_get_position(GTK_WINDOW(widget), &(pos_old[X]), &(pos_old[Y]));
				
				int x=0;
				int y=0;
				int w=1000;
				int h=50;
				
				ly_reg_get("3lrc_desktop_pos_and_size", "%d:%d:%d:%d", &x, &y, &w, &h);
				ly_reg_set("3lrc_desktop_pos_and_size", "%d:%d:%d:%d", pos_old[X], pos_old[Y], w, h);
				break;
			case GDK_MOTION_NOTIFY:
				if(flag_drag)
				{
					int x, y;
					GtkWidget *window = widget;
					gtk_window_get_position(GTK_WINDOW(window), &x, &y);
					gtk_window_move((GtkWindow *) window,x+event->button.x-pos_old[X],y+event->button.y-pos_old[Y]);
				}	
			default:
				break;
		}
	}
	switch(event->type)
	{
		case GDK_ENTER_NOTIFY:
			flag_notify=TRUE;
			break;
		case GDK_LEAVE_NOTIFY:
			if(!flag_drag)
				flag_notify=FALSE;
			break;
		default:
			break;
	}
	return TRUE;
}
