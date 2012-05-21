#include "lrc_widget.h"

/*
 * TYPES
 */
enum{
	X,
	Y
};


/*
 * VARIABLES
 */
GtkWidget *ly_3lrc_widget=NULL;
GdkPixbuf *ly_3lrc_pixbuf_bg=NULL;
GdkPixbuf *ly_3lrc_pixbuf_bg_copy=NULL;

cairo_surface_t	*image	=	NULL;

gboolean flag_seek		=	FALSE;
gboolean flag_seeked	=	FALSE;

int ly_3lrc_widget_pos_delta[2]={0,0};
int ly_3lrc_widget_pos_old[2]={0,0};

int count=0;
int index_mark=0;

/*
 * FUNCTIONS
 */
gboolean	ly_3lrc_widget_on_expose_cb		(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean	ly_3lrc_widget_on_seek_cb		(GtkWidget *widget, GdkEventButton *event, gpointer data);
void ly_3lrc_widget_draw_text (cairo_t *cr, gchar *text, gchar *font);
void ly_3lrc_widget_draw_text_midx (cairo_t *cr, gchar *text, gchar *font, gint width_x, gint height_y);
gboolean ly_3lrc_widget_on_get_button_clicked_cb(GtkWidget *widget, gpointer data);

GtkWidget *ly_3lrc_widget_create()
{
	ly_3lrc_pixbuf_bg=ly_sss_alloc_bg(NULL);
	ly_3lrc_pixbuf_bg_copy=NULL;
	
	GtkWidget *widget;
	GtkWidget *event_box;
	GtkWidget *button;
	
	widget=gtk_vbox_new(FALSE, 0);
	
	event_box=gtk_event_box_new();
	gtk_box_pack_start(GTK_BOX(widget), event_box, TRUE, TRUE, 0);
	gtk_widget_set_app_paintable(event_box, TRUE);
	
	button=gtk_button_new_with_label(_("Download Lyrics"));
	gtk_box_pack_start(GTK_BOX(widget), button, FALSE, TRUE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_3lrc_widget_on_get_button_clicked_cb), NULL);
	
	g_signal_connect(event_box, "draw" ,G_CALLBACK (ly_3lrc_widget_on_expose_cb) , NULL);
	g_signal_connect(widget, "button_press_event", G_CALLBACK(ly_3lrc_widget_on_seek_cb), NULL);
	g_signal_connect(widget, "motion_notify_event", G_CALLBACK(ly_3lrc_widget_on_seek_cb), NULL);
	g_signal_connect(widget, "button_release_event", G_CALLBACK(ly_3lrc_widget_on_seek_cb), NULL);

	ly_3lrc_widget=event_box;
	return widget;
}
void ly_3lrc_widget_refresh()
{
	gtk_widget_queue_draw (ly_3lrc_widget);
}
void ly_3lrc_widget_destroy()
{
	if(ly_3lrc_pixbuf_bg)
		g_object_unref(ly_3lrc_pixbuf_bg);
	if(ly_3lrc_pixbuf_bg_copy)
		g_object_unref(ly_3lrc_pixbuf_bg_copy);
}

gboolean ly_3lrc_widget_on_seek_cb(GtkWidget * widget, GdkEventButton *event, gpointer data)
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
	
	int length=ly_lrc_get_length();
	if(length<=0)
		return FALSE;
		
	//GdkCursor *cursor;
	int index=0;
	if (event->button == 1)
	{
		switch(event->type)
		{
			case GDK_BUTTON_PRESS:
				flag_seek=TRUE;
				ly_3lrc_widget_pos_old[X] = event->x;
				ly_3lrc_widget_pos_old[Y] = event->y;
				index_mark=ly_lrc_get_index();
				break;
			case GDK_BUTTON_RELEASE:
				if(flag_seek==TRUE&&flag_seeked==TRUE)
				{
					LyLrcLyric **array=ly_lrc_get_array();
					index=ly_lrc_get_index();
					ly_aud_set_position(array[index]->time/(double)ly_mdh_time_str2int(md->duration));
				}
				flag_seek = FALSE;
				flag_seeked=FALSE;
				ly_lrc_set_update_state(TRUE);

				break;
			default:
				break;
		}
	}
	if(flag_seek)
	{
		int pos[2]={0,0};
		gtk_widget_get_pointer(widget, &pos[X], &pos[Y]);
		ly_3lrc_widget_pos_delta[Y]=pos[Y]-ly_3lrc_widget_pos_old[Y];
		if(pos[Y]-ly_3lrc_widget_pos_old[Y])
			flag_seeked=TRUE;
			ly_lrc_set_update_state(FALSE);
	}
	if(flag_seek&&flag_seeked)
	{
		gint lrc_gap=20;
		if(!ly_reg_get("lrc_gap","%d",&lrc_gap))
		{
			ly_reg_set("lrc_gap","%d",lrc_gap);
		}
		index=index_mark;
		if(ly_3lrc_widget_pos_delta[Y]>=0)
		{
			index-=(int)(abs(ly_3lrc_widget_pos_delta[Y])/lrc_gap);
			if(index<0)
			{
				index=0;
			}
		}
		else
		{
			index+=(int)(abs(ly_3lrc_widget_pos_delta[Y])/lrc_gap);
			if(index>=length)
			{
				index=length-1;
			}
		}
		ly_lrc_set_index(index);
	}
	return FALSE;
}

gboolean ly_3lrc_widget_on_expose_cb(GtkWidget * widget, cairo_t *cr, gpointer data)
{
	
	gint width;
	gint height;
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	gchar title_font[1024]="Sans Regular 18";
	ly_reg_get("3lrc_title_font", "%1024[^\n]", title_font);
	gchar normal_font[1024]="Sans Regular 10";
	ly_reg_get("3lrc_normal_font", "%1024[^\n]", normal_font);

	/*
	 * draw bg
	 */
	if(ly_3lrc_pixbuf_bg)
	{
		if(ly_3lrc_pixbuf_bg_copy)
		{
			int h=gdk_pixbuf_get_height(ly_3lrc_pixbuf_bg_copy);
			int w=gdk_pixbuf_get_width(ly_3lrc_pixbuf_bg_copy);
			if(h<height||h-height>2||w<width||w-width>2)
			{
				g_object_unref(ly_3lrc_pixbuf_bg_copy);
				ly_3lrc_pixbuf_bg_copy=gdk_pixbuf_scale_simple(ly_3lrc_pixbuf_bg, width, height, GDK_INTERP_BILINEAR);
			}
		}
		else
		{
			ly_3lrc_pixbuf_bg_copy=gdk_pixbuf_scale_simple(ly_3lrc_pixbuf_bg, width, height, GDK_INTERP_BILINEAR);
		}
		gdk_cairo_set_source_pixbuf(cr, ly_3lrc_pixbuf_bg_copy, 0, 0);	
		cairo_paint(cr);
	}
	cairo_rectangle (cr, 0, 0, width, height);
	cairo_set_source_rgba (cr, 0, 0, 0, 0.5);
	cairo_fill(cr);

	//画标题
	cairo_pattern_t *pat;
	pat = cairo_pattern_create_linear (0, 0, 0, 45);
	cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 0.5);
	cairo_pattern_add_color_stop_rgba (pat, 0.18, 1, 1, 1, 0.3);
	cairo_pattern_add_color_stop_rgba (pat, 0.25, 1, 1, 1, 0.1);
	cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 1, 0.1);
	cairo_rectangle (cr, 0, 0, width, 45);
	cairo_set_source (cr, pat);
	cairo_fill(cr);
	cairo_pattern_destroy (pat);

	cairo_set_line_width(cr, 0.5);
	cairo_set_source_rgba (cr, 0.9, 0.9, 0.9, 0.6);
	cairo_move_to(cr, 0, 44);
	cairo_line_to(cr, width, 44);
	cairo_stroke(cr);
	cairo_set_source_rgba (cr, 0, 0, 0, 0.9);
	cairo_move_to(cr, 0, 44.5);
	cairo_line_to(cr, width, 44.5);
	cairo_stroke(cr);

	LyMdhMetadata *md=ly_pqm_get_current_md();
	int length=ly_lrc_get_length();

	if(md)
	{
		gchar title[1024]="";
		g_snprintf(title, sizeof(title), "%s - %s", md->title,md->artist);
		cairo_set_source_rgb ( cr, 0.1, 0.1, 0.1);
		ly_3lrc_widget_draw_text_midx(cr, title, title_font, width ,6);
		cairo_set_source_rgb ( cr, 0.9, 0.9, 0.9);
		ly_3lrc_widget_draw_text_midx(cr, title, title_font, width ,7);
	}
	
	
	//没有找到歌词
	if(length<=0||!md)
	{
		//画边框
		cairo_set_source_rgba ( cr, 0.3 , 0.3 , 0.3 ,0.8) ;
		cairo_set_line_width ( cr,1);
		cairo_translate ( cr, width/2 , height/2);
		cairo_move_to(cr, -70, -30);
		cairo_line_to(cr, 70, -30);
		cairo_arc(cr, 70, 0.0, 30.0 ,-M_PI/2 ,-M_PI*3/2);
		cairo_line_to(cr, 70, 30);
		cairo_arc(cr, -70, 0.0, 30.0 ,M_PI/2 ,M_PI*3/2);
		cairo_fill (cr);
		
		//画闪烁的原点
		cairo_translate ( cr, -60 , 0 );
		static  gdouble const  trs[ 8 ] [ 8 ]  = {
			{ 0.0 , 0.15 , 0.30 , 0.5 , 0.65 , 0.80 , 0.9 , 1.0 } ,
			{ 1.0 , 0.0 , 0.15 , 0.30 , 0.5 , 0.65 , 0.8 , 0.9 } ,
			{ 0.9 , 1.0 , 0.0 , 0.15 , 0.3 , 0.5 , 0.65 , 0.8 } ,
			{ 0.8 , 0.9 , 1.0 , 0.0 , 0.15 , 0.3 , 0.5 , 0.65 } ,
			{ 0.65 , 0.8 , 0.9 , 1.0 , 0.0 , 0.15 , 0.3 , 0.5 } ,
			{ 0.5 , 0.65 , 0.8 , 0.9 , 1.0 , 0.0 , 0.15 , 0.3 } ,
			{ 0.3 , 0.5 , 0.65 , 0.8 , 0.9 , 1.0 , 0.0 , 0.15 } ,
			{ 0.15 , 0.3 , 0.5 , 0.65 , 0.8 , 0.9 , 1.0 , 0.0 ,}
		};
		gint i = 0 ;
		for ( i = 0 ; i < 8 ; i++)
		{
			cairo_set_line_width ( cr, 8 ) ;
			cairo_set_line_cap ( cr, CAIRO_LINE_CAP_ROUND) ;
			cairo_set_source_rgba ( cr, 1.0 , 1.0 , 1.0 , trs[count%8][i]) ;
			
			cairo_move_to ( cr, 0.0 , -15.0 );
			cairo_line_to (cr, 0.0, -15.0);
			cairo_rotate ( cr, M_PI / 4 ) ;
			
			cairo_stroke ( cr) ;
		}
		
		//写字“Serching Lyrics...”
		cairo_set_source_rgba ( cr, 1.0 , 1.0 , 1.0 , 1.0) ;
		cairo_move_to (cr, 30.0 , -6.0);
		ly_3lrc_widget_draw_text(cr,"Missing Lyrics...", "Sans Regular 10");
		
		count=(count+1)%8;
	}
	
	//有歌词
	else
	{
		gint lrc_gap=20;
		if(!ly_reg_get("lrc_gap","%d",&lrc_gap))
		{
			ly_reg_set("lrc_gap","%d",lrc_gap);
		}
		
		int y=height/2;
		LyLrcLyric **array=ly_lrc_get_array();
		int index=ly_lrc_get_index();
		if(flag_seek&&flag_seeked)
		{
			cairo_set_source_rgba ( cr, 0.8, 0.8, 0.8, 0.8);
			cairo_set_line_width ( cr,20);
			cairo_set_line_cap ( cr, CAIRO_LINE_CAP_ROUND) ;
			cairo_move_to(cr, 10.0, height/2.0+8);
			cairo_line_to(cr, width-10.0, height/2.0+8);
			cairo_stroke (cr);
			y=height/2.0;
		}
		else
		{
			gint64 t1=0;
			gint64 t2=0;
			
			if(index+1<length)
			{
				t1=array[index+1]->time-array[index]->time;
			}
			else
				t1=ly_mdh_time_str2int(md->duration)-array[index]->time;
			t2=ly_aud_get_position_abs()-array[index]->time;
			
			if(t1!=0)
			{
				y=y-(int)((t2/(gdouble)t1)*lrc_gap);
			}
		}
		//正式画歌词区域
		//画当前歌词
		cairo_set_source_rgb(cr,1, 1, 1);
		if(y>height/5.0&&y<height*4/5.0)
			ly_3lrc_widget_draw_text_midx(cr,array[index]->text, normal_font,width,y);
		
		//画普通歌词
		cairo_set_source_rgb(cr,0.5,0.5,0.5);
		int i=1;
		for(i=1;i<=10;i++)
		{
			if(index-i>=0 &&(y-i*(lrc_gap)>height/5.0))
			{
				ly_3lrc_widget_draw_text_midx(cr,array[index-i]->text, normal_font,width,y-i*(lrc_gap));
			}
			else
				break;
		}
		for(i=1;i<=10;i++)
		{
			if(index+i<length &&(y+i*(lrc_gap)<(height/5.0*4)))
			{
				ly_3lrc_widget_draw_text_midx(cr,array[index+i]->text,normal_font,width,y+i*(lrc_gap));
			}
			else
				break;
		}
	}
	return FALSE;
}


void ly_3lrc_widget_draw_text (cairo_t *cr, gchar *text, gchar *font)
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

void ly_3lrc_widget_draw_text_midx (cairo_t *cr, gchar *text, gchar *font, gint width_x, gint height_y)
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

gboolean ly_3lrc_widget_on_get_button_clicked_cb(GtkWidget *widget, gpointer data)
{
	ly_mbs_put("lrc_missing", "plugin:lrc", NULL);
	return FALSE;
}
