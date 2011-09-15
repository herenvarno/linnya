#include "Lrc.h"

int count=0;

cairo_surface_t *image_desktop=NULL;
cairo_surface_t *image=NULL;
gboolean notifyflag=FALSE;

gboolean seekflag=FALSE;
gboolean seekchanged=FALSE;
int oldx=0;
int oldy=0;
int dx=0;
int dy=0;
int layoutX=0;
int layoutY=0;

int olddesktopx=0;
int olddesktopy=0;
gboolean dragflag=FALSE;

int index_mark=0;

const gchar* g_module_check_init(GModule *module)
{
	int i=0;
	for(i=0;i<1024;i++)
	{
		ly_plugin_lrc_lyrics_array[i]=NULL;
	}
	
	ly_plugin_lrc_read_cb(NULL,NULL);
	ly_msg_bind("meta_changed", "core:audio", ly_plugin_lrc_read_cb, NULL);
	
	int lrc_desktop=1;
	int lrc_desktop_fix=0;
	if(!ly_conf_get("lrc_desktop", "%d:%d", &lrc_desktop, &lrc_desktop_fix))
	{
		ly_conf_set("lrc_desktop", "%d:%d", lrc_desktop, lrc_desktop_fix);
	}
	
	if(lrc_desktop>0)
	{
		ly_plugin_lrc_desktop=gtk_window_new(GTK_WINDOW_POPUP);
		int w=1000;
		int h=50;
		int x=0;
		int y=0;
		if(!ly_conf_get("lrc_desktop_pos_and_size", "%d:%d:%d:%d", &x, &y, &w, &h))
		{
			ly_conf_set("lrc_desktop_pos_and_size", "%d:%d:%d:%d", x, y, w, h);
		}
		gtk_window_set_default_size(GTK_WINDOW(ly_plugin_lrc_desktop),w,h);
		gtk_window_move(GTK_WINDOW(ly_plugin_lrc_desktop),x,y);
		gtk_widget_set_app_paintable(ly_plugin_lrc_desktop, TRUE);
		gtk_window_set_decorated(GTK_WINDOW(ly_plugin_lrc_desktop), FALSE);
		
		GdkVisual *visual;
		GdkScreen *screen;
		screen = gtk_widget_get_screen(ly_plugin_lrc_desktop);	
		visual = gdk_screen_get_rgba_visual (screen);
		if (visual == NULL)
			visual = gdk_screen_get_system_visual (screen);
		gtk_widget_set_visual (GTK_WIDGET (ly_plugin_lrc_desktop), visual);

		gtk_widget_set_events(ly_plugin_lrc_desktop,GDK_ALL_EVENTS_MASK);
		if(lrc_desktop_fix>0)
			gtk_widget_set_sensitive(ly_plugin_lrc_desktop,FALSE);
		gtk_widget_show_all(ly_plugin_lrc_desktop);
		g_signal_connect(G_OBJECT(ly_plugin_lrc_desktop), "draw", G_CALLBACK(ly_plugin_lrc_desktop_expose_cb), NULL);
		g_signal_connect(G_OBJECT(ly_plugin_lrc_desktop), "button_press_event", G_CALLBACK(ly_plugin_lrc_desktop_drag_cb), NULL);
		g_signal_connect(G_OBJECT(ly_plugin_lrc_desktop), "button_release_event", G_CALLBACK(ly_plugin_lrc_desktop_drag_cb), NULL);
		g_signal_connect(G_OBJECT(ly_plugin_lrc_desktop), "motion_notify_event", G_CALLBACK(ly_plugin_lrc_desktop_drag_cb), NULL);
		g_signal_connect(G_OBJECT(ly_plugin_lrc_desktop), "enter_notify_event", G_CALLBACK(ly_plugin_lrc_desktop_drag_cb), NULL);
		g_signal_connect(G_OBJECT(ly_plugin_lrc_desktop), "leave_notify_event", G_CALLBACK(ly_plugin_lrc_desktop_drag_cb), NULL);
		
		g_timeout_add(100,ly_plugin_lrc_desktop_update_cb,NULL);
	}
	return NULL;
}
void g_module_unload(GModule *module)
{
	;
}

GtkWidget *ly_plugin_lrc_create()
{
	GtkWidget *widget;

	widget=gtk_event_box_new();
	gtk_widget_set_app_paintable(widget, TRUE);
	
	g_signal_connect(widget, "draw" ,G_CALLBACK (ly_plugin_lrc_expose_cb) , NULL) ;
	g_signal_connect(widget, "button_press_event", G_CALLBACK(ly_plugin_lrc_seek_cb), NULL);
	g_signal_connect(widget, "motion_notify_event", G_CALLBACK(ly_plugin_lrc_seek_cb), NULL);
	g_signal_connect(widget, "button_release_event", G_CALLBACK(ly_plugin_lrc_seek_cb), NULL);

	window=widget;
	return widget;
}
void ly_plugin_lrc_refresh()
{
	gtk_widget_queue_draw (window);
}
void ly_plugin_lrc_destroy()
{
	;
}

gboolean ly_plugin_lrc_seek_cb(GtkWidget * widget, GdkEventButton *event, gpointer data)
{
	GdkCursor *cursor;
	if (event->button == 1)
	{
		switch(event->type)
		{
			case GDK_BUTTON_PRESS:
				if(ly_plugin_lrc_lyrics_length!=0)
				{
					seekflag=TRUE;
					oldx = event->x;
					oldy = event->y;
					index_mark=ly_plugin_lrc_lyrics_index;
					
					//cursor= gdk_cursor_new(GDK_HAND1);
					//gdk_window_set_cursor(widget->window,cursor);
					//gdk_cursor_destroy(cursor);
				}
				break;
			case GDK_BUTTON_RELEASE:
				if(ly_plugin_lrc_lyrics_length!=0&&seekchanged==TRUE&&seekchanged==TRUE)
				{
					ly_audio_set_position(ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index]->time/(double)ly_db_duration2dura(ly_audio_meta->duration));
				}
				seekflag = FALSE;
				seekchanged=FALSE;
				
				//cursor= gdk_cursor_new(GDK_LEFT_PTR);
				//gdk_window_set_cursor(widget->window,cursor);
				//gdk_cursor_destroy(cursor);
				break;
			default:
				break;
		}
	}
	if(seekflag)
	{
		if(ly_plugin_lrc_lyrics_length<=0)
			return TRUE;
		
		int x, y;
		gtk_widget_get_pointer(widget, &x, &y);
			
		dx=x-oldx;
		dy=y-oldy;
		if(dy)
			seekchanged=TRUE;
	}
	return TRUE;
}

gboolean ly_plugin_lrc_read_cb(gpointer message, gpointer data)
{	
	if(!ly_audio_meta)
		return FALSE;
	ly_plugin_lrc_lyrics_length=0;
	ly_plugin_lrc_lyrics_index=0;
	
	gchar dir[1024]="./";
	gchar *lrcfile=NULL;
	gchar *path=NULL;
	gchar *filename=NULL;
	FILE *fp=NULL;
	
	if(!ly_conf_get("lrc_dir", "%s", dir))
	{
		ly_conf_set("lrc_dir", "%s", dir);
	}

	if(g_str_equal(dir,"")||g_str_equal(dir,"./"))	//当前目录
	{
		path=ly_global_get_dir(ly_audio_meta->uri);
		filename=ly_global_get_filename(ly_audio_meta->uri);
		if(ly_audio_meta->flag==1)	//如果是cue歌曲
		{
			lrcfile=g_strconcat(path, filename, ly_audio_meta->start,".lrc", NULL);
		}
		else
		{
			lrcfile=g_strconcat(path, filename, ".lrc", NULL);
		}
	}
	else
	{
		if(ly_audio_meta->flag==1)
			lrcfile=g_strconcat(dir, ly_audio_meta->title, "-", ly_audio_meta->artist, ly_audio_meta->start,".lrc", NULL);
		else
			lrcfile=g_strconcat(dir, ly_audio_meta->title, "-", ly_audio_meta->artist, ".lrc", NULL);
	}
	g_free(path);
	g_free(filename);
	path=NULL;
	filename=NULL;
	fp=fopen(lrcfile,"r");
	if(fp==NULL)
	{
		ly_msg_put("lyric_missing", "plugin:lrc", lrcfile);
		return FALSE;
	}
	g_free(lrcfile);
	ly_plugin_lrc_read_lyrics(fp);
	ly_plugin_lrc_sort_lyrics();
	return FALSE;
}

void ly_plugin_lrc_read_lyrics(FILE *fp)
{
	ly_plugin_lrc_lyrics_length=0;
	
	/*
	 * Extra Encoding
	 */
	gchar extra_encoding[1024]="GB18030";
	if(!ly_conf_get("db_extra_encoding", "%*[^\n(](%1023[^\n)]", extra_encoding))
	{
		ly_conf_set("db_extra_encoding", "Chinese Simplified (GB18030)");
	}

	lyPluginLrcLyric *lrc=NULL;
	lyPluginLrcLyric *tmplrc[1024];
	
	gint64 time;
	gint time_delay=0;
	gchar line[1024]="";
	
	gchar *p,*q;
	gchar label[1024];
	gchar text[1024];
	
	gchar *tmpstr;
	
	gint i=0;
	gint j=0;
	
	while(fgets(line, sizeof(line), fp))
	{
		j=0;
		line[strlen(line)-1]='\0';
		lrc=NULL;
		p=line;
		while(*p)
		{
			while(*p=='[')
			{
				p++;
				if((*p<='9'&&*p>='0')||(*p=='-'&&*(p+1)<='9'&&*(p+1)>='0'))		//时间标签
				{
					//读取时间
					label[0]='\0';
					q=label;
					while(*p!=']')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';
					p++;
					//读取文本
					text[0]='\0';
					q=text;
					while(*p!='['&&*p!='\0')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';
					
					//读取完成，处理数据
					lrc=(lyPluginLrcLyric*)malloc(sizeof(lyPluginLrcLyric));
					lrc->time=0;
					g_strlcpy(lrc->text, "", sizeof(lrc->text));
					
					time=ly_db_duration2dura(label);
					if(time<0)
						time=0;
					lrc->time=time;
					
					if(*p=='['&&g_str_equal(text,""))
					{
						tmplrc[j]=lrc;
						j++;
					}
					else
					{
						for (i = 0; i <j; i++)
						{
							if(g_utf8_validate(text,-1,NULL))
							{
								g_utf8_strncpy(tmplrc[i]->text,text,sizeof(tmplrc[i]->text));
							}
							else	//非UTF-8字符
							{
								tmpstr=g_convert(text, strlen(text),"UTF-8",extra_encoding,NULL,NULL,NULL);
								g_strlcpy(tmplrc[i]->text,tmpstr, sizeof(tmplrc[i]->text));
								g_free(tmpstr);
							}
							if(ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_length]!=NULL)
							{
								g_free(ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_length]);
							}
							ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_length]=tmplrc[i];
							ly_plugin_lrc_lyrics_length++;
						}
						j=0;
						if(g_utf8_validate(text,-1,NULL))
						{
							g_utf8_strncpy(lrc->text,text,sizeof(lrc->text));
						}
						else	//非UTF-8字符
						{
							tmpstr=g_convert(text, strlen(text),"UTF-8",extra_encoding,NULL,NULL,NULL);
							g_strlcpy(lrc->text,tmpstr, sizeof(lrc->text));
							g_free(tmpstr);
						}
						if(ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_length]!=NULL)
						{
							g_free(ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_length]);
						}
						ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_length]=lrc;
						ly_plugin_lrc_lyrics_length++;
					}
				}
				else	//其他标签
				{
					label[0]='\0';
					q=label;
					while(*p!=':'&&*p!='\0')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';
					p++;
					
					text[0]='\0';
					q=text;
					while(*p!=']'&&*p!='\0')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';
					
					if(g_str_equal(label,"offset")||g_str_equal(label,"OFFSET"))
					{
						if(!g_str_equal(text,""))
							sscanf(text,"%d",&time_delay);
					}
					p++;
					break;
				}
			}
			p++;
		}
	}
	fclose(fp);
}

static gint ly_plugin_lrc_sort_lyrics_cb(gconstpointer c, gconstpointer d, gpointer data)
{
	lyPluginLrcLyric *a=*((lyPluginLrcLyric**)c);
	lyPluginLrcLyric *b=*((lyPluginLrcLyric**)d);
	
	if(a->time<=b->time)
		return -1;
	else
		return 1;
}
void ly_plugin_lrc_sort_lyrics()
{
	g_qsort_with_data(ly_plugin_lrc_lyrics_array, ly_plugin_lrc_lyrics_length, sizeof(lyPluginLrcLyric *), ly_plugin_lrc_sort_lyrics_cb, NULL);
}

gboolean ly_plugin_lrc_expose_cb(GtkWidget * widget, cairo_t *cr, gpointer data)
{
	
	gint width;
	gint height;
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	//画标题
	cairo_set_source_rgba ( cr, 0.0, 0.0 , 0.0, 0.8);
	cairo_rectangle (cr, 0, 0, width, 50);
	cairo_fill(cr);
	
	cairo_set_source_rgba ( cr, 0.8 , 0.8, 0.8, 0.9);
	cairo_set_line_width (cr,1);
	cairo_set_line_cap ( cr, CAIRO_LINE_CAP_ROUND) ;
	cairo_move_to(cr, 0, 51);
	cairo_line_to(cr, width, 51);
	cairo_stroke (cr) ;
	if(ly_audio_meta)
	{
		gchar title[1024]="";
		cairo_set_source_rgb ( cr, 0.8, 0.8 , 0.8);
		g_snprintf(title, sizeof(title), "%s - %s", ly_audio_meta->title,ly_audio_meta->artist);
		ly_plugin_lrc_draw_text_midx(cr,title,"Sans Regular 20", width ,5);
	}
	
	//没有找到歌词
	if(ly_plugin_lrc_lyrics_length==0)
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
		ly_plugin_lrc_draw_text(cr,"Missing Lyrics...","Sans Regular 10");
		
		count=(count+1)%8;
	}
	
	//有歌词
	else
	{
		gint lrc_gap=20;
		if(!ly_conf_get("lrc_gap","%d",&lrc_gap))
		{
			ly_conf_set("lrc_gap","%d",lrc_gap);
		}
		
		int y=height/2;

		if(seekflag&&seekchanged)
		{
			cairo_set_source_rgba ( cr, 0.8, 0.8, 0.8, 0.8);
			cairo_set_line_width ( cr,20);
			cairo_set_line_cap ( cr, CAIRO_LINE_CAP_ROUND) ;
			cairo_move_to(cr, 10.0, height/2.0+8);
			cairo_line_to(cr, width-10.0, height/2.0+8);
			cairo_stroke (cr);
			
			ly_plugin_lrc_lyrics_index=index_mark;
			if(dy>=0)
			{
				
				ly_plugin_lrc_lyrics_index-=(int)(abs(dy)/lrc_gap);
				if(ly_plugin_lrc_lyrics_index<0)
				{
					ly_plugin_lrc_lyrics_index=0;
				}
			}
			else
			{
				ly_plugin_lrc_lyrics_index+=(int)(abs(dy)/lrc_gap);
				if(ly_plugin_lrc_lyrics_index>=ly_plugin_lrc_lyrics_length)
				{
					ly_plugin_lrc_lyrics_index=ly_plugin_lrc_lyrics_length-1;
				}
			}
			y=height/2.0;
		}
		else
		{
			
			gint64 t1=0;
			gint64 t2=0;
			
			if(ly_plugin_lrc_lyrics_index+1<ly_plugin_lrc_lyrics_length)
				t1=ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index+1]->time-ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index]->time;
			else
				t1=ly_db_duration2dura(ly_audio_meta->duration)-ly_db_duration2dura(ly_audio_meta->start)-ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index]->time;
			
			t2=ly_audio_get_position_abs()-ly_db_duration2dura(ly_audio_meta->start)-ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index]->time;
			
			if(t1!=0)
			{
				y=y-(int)((t2/(gdouble)t1)*lrc_gap);
			}
		}
		
		//正式画歌词区域
		//画当前歌词
		cairo_set_source_rgb(cr,1, 1, 1);
		if(y>height/5.0&&y<height*4/5.0)
			ly_plugin_lrc_draw_text_midx(cr,ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index]->text,"Sans Regular 10",width,y);
		
		//画普通歌词
		cairo_set_source_rgb(cr,0.5,0.5,0.5);
		int i=1;
		for(i=1;i<=10;i++)
		{
			if(ly_plugin_lrc_lyrics_index-i>=0 &&(y-i*(lrc_gap)>height/5.0))
			{
				ly_plugin_lrc_draw_text_midx(cr,ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index-i]->text,"Sans Regular 10",width,y-i*(lrc_gap));
			}
			else
				break;
		}
		for(i=1;i<=10;i++)
		{
			if(ly_plugin_lrc_lyrics_index+i<ly_plugin_lrc_lyrics_length &&(y+i*(lrc_gap)<(height/5.0*4)))
			{
				ly_plugin_lrc_draw_text_midx(cr,ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index+i]->text,"Sans Regular 10",width,y+i*(lrc_gap));
			}
			else
				break;
		}
	}
	return FALSE;
}

gboolean ly_plugin_lrc_update_index()
{
	if(ly_plugin_lrc_lyrics_length<=0)
		return FALSE;
		
	gint64 pos;
	gint64 time;
	gint64 start;
	
	pos=ly_audio_get_position_abs();	
	start=ly_db_duration2dura(ly_audio_meta->start);
	time=pos-start;
			
	int min=0;
	int max=ly_plugin_lrc_lyrics_length-1;
	int index=(max-min)/2;
	while(max-min>=2)
	{
		if(ly_plugin_lrc_lyrics_array[index]->time<time)
		{
			min=index;
			index=(max+min)/2;
		}
		else if(ly_plugin_lrc_lyrics_array[index]->time>time)
		{
			max=index;
			index=(max+min)/2;
		}
		else
		{
			ly_plugin_lrc_lyrics_index=index;
			return TRUE;
		}
	}
	ly_plugin_lrc_lyrics_index=min;
	return TRUE;
}


void ly_plugin_lrc_draw_text (cairo_t *cr, gchar *text, gchar *font)
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

void ly_plugin_lrc_draw_text_midx (cairo_t *cr, gchar *text, gchar *font, gint width_x, gint height_y)
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

gboolean ly_plugin_lrc_desktop_expose_cb(GtkWidget * widget, cairo_t *cr, gpointer data)
{
	
	gchar path[1024];
	g_snprintf(path,sizeof(path),"%sui/icon/null.png",LY_GLOBAL_PROGDIR);
	if(!image_desktop)
		image_desktop=cairo_image_surface_create_from_png(path);

	cairo_set_source_surface(cr, image_desktop, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	
	int width;
	int height;
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);
	
	int x=0;
	int y=height;
	
	//鼠标进入
	if(notifyflag)
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
	
	if(ly_plugin_lrc_lyrics_length>0)
	{
		//计算占空比
		gint64 t1=0;
		gint64 t2=0;
		
		if(ly_plugin_lrc_lyrics_index+1<ly_plugin_lrc_lyrics_length)
			t1=ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index+1]->time-ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index]->time;
		else
			t1=ly_db_duration2dura(ly_audio_meta->duration)-ly_db_duration2dura(ly_audio_meta->start)-ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index]->time;
		
		t2=ly_audio_get_position_abs()-ly_db_duration2dura(ly_audio_meta->start)-ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index]->time;
		if(t1!=0)
		{
			x=(int)((t2/(gdouble)t1)*layoutX);
		}
		
		//画歌词
		cairo_set_source_rgb(cr,0,0,0);
		
		//确定起始点
		if((x>=width/2)&&(layoutX>width)&&(width<(layoutX-x)*2))
		{
			cairo_move_to(cr, 0-(x-width/2), 5);
			x=width/2;
		}
		else if((x>=width/2)&&(layoutX>width)&&(width>=(layoutX-x)*2))
			cairo_move_to(cr, 0-(layoutX-width), 5);
		else
			cairo_move_to(cr, 0, 5);
		
		PangoLayout *layout;
		PangoFontDescription *desc;
		layout = pango_cairo_create_layout (cr);
		
		pango_layout_set_text(layout, ly_plugin_lrc_lyrics_array[ly_plugin_lrc_lyrics_index]->text, -1);
		desc = pango_font_description_from_string ("Sans Regular 25");
		pango_layout_set_font_description (layout, desc);
		pango_font_description_free (desc);
		
		
		pango_layout_get_size(layout,&layoutX,&layoutY);
		layoutX=layoutX/1000;
		
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
		if((x>=width/2)&&(layoutX>width)&&(width<(layoutX-x)*2))
			cairo_rectangle (cr, 0, 0, width/2, y);
		else if((x>=width/2)&&(layoutX>width)&&(width>=(layoutX-x)*2))
			cairo_rectangle (cr, 0, 0, width-(layoutX-x), y);
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
		if((x>=width/2)&&(layoutX>width)&&(width<(layoutX-x)*2))
			cairo_rectangle (cr, width/2, 0, width-(width/2), y);
		else if((x>=width/2)&&(layoutX>width)&&(width>=(layoutX-x)*2))
			cairo_rectangle (cr, width-(layoutX-x), 0, layoutX-x, y);
		else
			cairo_rectangle (cr, x, 0, width-x, y);
		cairo_set_source (cr, pat);
		cairo_fill(cr);
		cairo_pattern_destroy (pat);
	}
	
	int lrc_desktop=1;
	int lrc_desktop_fix=0;
	if(!ly_conf_get("lrc_desktop", "%d:%d", &lrc_desktop, &lrc_desktop_fix))
	{
		ly_conf_set("lrc_desktop", "%d:%d", lrc_desktop, lrc_desktop_fix);
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

gboolean ly_plugin_lrc_desktop_drag_cb( GtkWidget * widget, GdkEvent *event, gpointer data)
{
	GdkCursor *cursor;
	if (event->button.button == 1)
	{
		switch(event->type)
		{
			case GDK_BUTTON_PRESS:
				olddesktopx = event->button.x;
				olddesktopy = event->button.y;
				dragflag=TRUE;
				
				//鼠标变成手形
				//cursor= gdk_cursor_new(GDK_FLEUR);
				//gdk_window_set_cursor(widget->window,cursor);
				//gdk_cursor_destroy(cursor);
				break;
			case GDK_BUTTON_RELEASE:	//释放
				dragflag=FALSE;
				
				//鼠标变成默认箭头
				//cursor= gdk_cursor_new(GDK_LEFT_PTR);
				//gdk_window_set_cursor(widget->window,cursor);
				//gdk_cursor_destroy(cursor);;
				gtk_window_get_position(GTK_WINDOW(widget), &(olddesktopx), &(olddesktopy));
				
				int x=0;
				int y=0;
				int w=1000;
				int h=50;
				
				ly_conf_get("lrc_desktop_pos_and_size", "%d:%d:%d:%d", &x, &y, &w,&h);
				ly_conf_set("lrc_desktop_pos_and_size", "%d:%d:%d:%d", olddesktopx, olddesktopy, w,h);
				break;
			case GDK_MOTION_NOTIFY:
				if(dragflag)
				{
					int x, y;
					GtkWidget *window = widget;
					gtk_window_get_position(GTK_WINDOW(window), &x, &y);
					gtk_window_move((GtkWindow *) window,x+event->button.x-olddesktopx,y+event->button.y-olddesktopy);
				}	
			default:
				break;
		}
	}
	switch(event->type)
	{
		case GDK_ENTER_NOTIFY:
			notifyflag=TRUE;
			break;
		case GDK_LEAVE_NOTIFY:
			if(!dragflag)
				notifyflag=FALSE;
			break;
		default:
			break;
	}
	return TRUE;
}

gboolean ly_plugin_lrc_desktop_update_cb(gpointer data)
{
	ly_plugin_lrc_update_index();
	int lrc_desktop=1;
	int lrc_desktop_fix=0;
	ly_conf_get("lrc_desktop", "%d:%d", &lrc_desktop, &lrc_desktop_fix);
	if(!lrc_desktop)
		return TRUE;
	gtk_widget_queue_draw(ly_plugin_lrc_desktop);
	return TRUE;
}
