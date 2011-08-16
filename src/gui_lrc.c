
//HEADERS
#include "gui_lrc.h"

//VARIABLES
cairo_surface_t *image=NULL;
cairo_surface_t *image_desktop=NULL;
gint	count=0;

gint X=0,Y=0;
gint dX=0,dY=0;
gboolean seekflag=FALSE;
gboolean seekchanged=FALSE;
GList *lrc_mark;

gint desktopX=0,desktopY=0;
gint layoutX=0, layoutY=0;
gboolean dragflag=FALSE;
gboolean notifyflag=FALSE;

//FUNCTIONS
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_lrc_eventbox_expose
//*	DISCRIPTION:	the expose handler of lyrics eventbox
//*
//*****************************************************************************************************
gboolean gui_lrc_eventbox_expose( GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
	_gl_theme *th=th_get_th_by_name("lyrics");
	if(th&&th->path)
	{
		cairo_t *cr;
		if(!image)
			image=cairo_image_surface_create_from_png(th->path);
		cr = gdk_cairo_create(widget->window);
		cairo_set_source_surface(cr, image, 0, 0);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint(cr);
		cairo_destroy(cr);
	}

	//设置样式
	gchar *font_highlight,*font_normal;
	gchar *color_highlight,*color_normal,*color_spectrum;
	th=th_get_th_by_name("highlight");
	if((!(gl_conf->custom_lrc))&&th&&th->font&&th->color_fg1)
	{
		font_highlight=th->font;
		color_highlight=th->color_fg1;
	}
	else
	{
		font_highlight=gl_conf->font_highlight;
		color_highlight=gl_conf->color_highlight;
	}
	th=th_get_th_by_name("normal");
	if((!(gl_conf->custom_lrc))&&(th)&&(th->font)&&(th->color_fg1))
	{
		font_normal=th->font;
		color_normal=th->color_fg1;
	}
	else
	{
		font_normal=gl_conf->font_normal;
		color_normal=gl_conf->color_normal;
	}
	th=th_get_th_by_name("spectrum");
	if((!(gl_conf->custom_lrc))&&(th)&&(th->color_fg1))
	{
		color_spectrum=th->color_fg1;
	}
	else
	{
		color_spectrum=gl_conf->color_spectrum;
	}
	
	int i;
	int j;
	GList *p;
	_gl_lyrics *plink;
	gchar *title;
	
	gint width;
	gint height;
	
	gint r1=0,g1=0,b1=0;
	gint r2=0,g2=0,b2=0;
	gint r3=0,g3=0,b3=0;
	
	cairo_t *cr = gdk_cairo_create (widget->window) ;
	gdk_drawable_get_size(GDK_DRAWABLE(widget->window),&width,&height);
	
	double x;
	double y;
	
	x=4;
	y=height;
	sscanf(color_spectrum,"%d:%d:%d",&r3,&g3,&b3);
	cairo_set_source_rgb ( cr, r3/65535.0, g3/65535.0, b3/65535.0);
	cairo_set_line_width ( cr,1);
	cairo_set_line_cap ( cr, CAIRO_LINE_CAP_ROUND) ;

	for(i=0;i<40;i++)
	{
		y=height;
		for(j=0;j<co_mag[39-i]/5;j++)
		{
			cairo_rectangle (cr, x, y, (width-100)/80.0,((height/3.5)/20.0));
			y=y-((height/3.0)/20.0);
		}
		x=x+(width/80.0);
	}
	for(i=1;i<40;i++)
	{
		y=height;
		for(j=0;j<co_mag[i]/5;j++)
		{
			cairo_rectangle (cr, x, y, (width-100)/80.0,((height/3.5)/20.0));
			y=y-((height/3.0)/20.0);
		}
		x=x+(width/80.0);
	}
	cairo_fill ( cr) ;
	
	sscanf(color_highlight,"%d:%d:%d",&r1,&g1,&b1);
	sscanf(color_normal,"%d:%d:%d",&r2,&g2,&b2);
	
	//画标题
	if(gl_playing)
	{
		cairo_set_source_rgb ( cr, r1/65535.0,g1/65535.0 ,b1/65535.0);
		title=g_strconcat(((_gl_playinglist*)(gl_playing->data))->md->title," - ",((_gl_playinglist*)(gl_playing->data))->md->artist,NULL);
		gui_window_draw_text_midx(cr,title,font_highlight, width ,5);
		g_free(title);
	}

	x=30;
	y=height/2.0;
	
	//没有找到歌词
	if(!gl_lyrics)
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
			cairo_set_source_rgba ( cr, 1.0 , 1.0 , 1.0 , trs[ count%8][i]) ;

			cairo_move_to ( cr, 0.0 , -15.0 );
			cairo_line_to (cr, 0.0, -15.0);
			cairo_rotate ( cr, M_PI / 4 ) ;

			cairo_stroke ( cr) ;
		}

		//写字“Serching Lyrics...”
		cairo_set_source_rgba ( cr, 1.0 , 1.0 , 1.0 , 1.0) ;
		cairo_move_to (cr, 30.0 , -6.0);
		gui_window_draw_text(cr,"Searching Lyrics...","Sans Regular 10");
		
		count=(count+1)%8;
	}

	//有歌词
	else
	{
		//当前歌词为空，返回
		if(gl_lrc==NULL)
		{
			cairo_destroy(cr);
			return FALSE;
		}
		
		//正在seek
		if(seekflag&&seekchanged)
		{
			//画基准线
			cairo_set_source_rgba ( cr, 0.2 , 0.2 , 0.2, 0.7);
			cairo_set_line_width ( cr,20);
			cairo_set_line_cap ( cr, CAIRO_LINE_CAP_ROUND) ;
			cairo_move_to(cr, 10.0, height/2.0+8);
			cairo_line_to(cr, width-10.0, height/2.0+8);
			cairo_stroke ( cr) ;
			
			gl_lrc=lrc_mark;
			for(i=0;(i<abs(dY)/gl_conf->lrcgap)&&dY>=0;i++)
			{
				if(gl_lrc->prev)
					gl_lrc=gl_lrc->prev;
				else
					break;
			}
			for(i=0;(i<abs(dY)/gl_conf->lrcgap)&&dY<0;i++)
			{
				if(gl_lrc->next)
					gl_lrc=gl_lrc->next;
				else
					break;
			}
			y=height/2.0;
		}
		
		//没有seek
		else
		{
			GstFormat fmt=GST_FORMAT_TIME;
			gint64 position;
			
			gint time=0;
			gint pos=0;
			gint start;
			gint min;
			gint sec;
			gint frm;
			if(gl_lrc->next)
				time=((_gl_lyrics*)(gl_lrc->next->data))->time-((_gl_lyrics*)(gl_lrc->data))->time;
			if(gl_core->play && gst_element_query_position(gl_core->play, &fmt, &position))
			{
				sscanf(((_gl_playinglist*)(gl_playing->data))->md->start,"%d:%d:%d",&min,&sec,&frm);
				start=(min*60+sec)*100+frm;
				pos=(gint)(position/10000000)-start;
				pos=pos-((_gl_lyrics*)(gl_lrc->data))->time;
			}
			if(time!=0)
			{
				y=y-(int)((pos/(gdouble)time)*gl_conf->lrcgap);
			}
		}
			
			//正式画歌词区域
			//画当前歌词
			if(seekflag&&seekchanged)
			  cairo_set_source_rgb(cr,1.0,1.0,1.0);
			else
			  cairo_set_source_rgb(cr,r1/65535.0,g1/65535.0,b1/65535.0);
			if(y>height/5.0&&y<height*2/3.0)
				gui_window_draw_text_midx(cr,((_gl_lyrics*)(gl_lrc->data))->text,font_highlight,width,y);
			
			//画普通歌词
			cairo_set_source_rgb(cr,r2/65535.0,g2/65535.0,b2/65535.0);
			p=gl_lrc;
			for(i=1;i<=10;i++)
			{
				p=p->prev;
				if(p&&(y-i*(gl_conf->lrcgap)>height/5.0))
				{
					plink=p->data;
					gui_window_draw_text_midx(cr,plink->text,font_normal,width,y-i*(gl_conf->lrcgap));
				}
				else
					break;
			}
			p=gl_lrc;
			for(i=1;i<=10;i++)
			{
				p=p->next;
				if(p&&(y+i*(gl_conf->lrcgap)<height*2/3.0))
				{
					plink=p->data;
					gui_window_draw_text_midx(cr,plink->text,font_normal,width,y+i*(gl_conf->lrcgap));
				}
				else
					break;
			}
	}
	cairo_destroy ( cr) ;
	return FALSE ;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_lrc_callback_download_lrc
//*	DISCRIPTION:	download lrc file from web
//*
//*****************************************************************************************************
void *gui_lrc_callback_download_lrc(void *arg)
{
	gchar *path=arg;

	_gl_metadata *md=((_gl_playinglist*)(gl_playing->data))->md;
	gchar *title=g_strconcat(md->title,NULL);
	gchar *artist=g_strconcat(md->artist,NULL);
	
	int output;
	gchar *cmd=g_strconcat("python \"",gl_dir_prog,"scripts/LyricsDownloader.py\" \"",path,"\" \"",title,"\" \"",artist,"\"",NULL);
	output=system(cmd);
	g_free(cmd);
	g_free(title);
	g_free(artist);
	if(((_gl_playinglist*)(gl_playing->data))->md!=md)
	{
		g_free(path);
		path=NULL;
		gl_thread_download=NULL;
		return NULL;
	}
	FILE *fp=NULL;
	fp=fopen(path,"r");
	g_free(path);
	path=NULL;
	if(!fp)
	{
		gl_thread_download=NULL;
		return NULL;
	}
	gui_lrc_analyse_lyrics(fp);
	gl_thread_download=NULL;
	return NULL;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_lrc_read_lyrics
//*	DISCRIPTION:	read lyrics
//*
//*****************************************************************************************************
gboolean	gui_lrc_read_lyrics()
{	
	gchar *path=NULL;
	gchar *p=NULL,*q=NULL;
	FILE *fp=NULL;

	_gl_metadata *md=((_gl_playinglist*)(gl_playing->data))->md;
	if(gl_conf->lrc_location==NULL||g_strcmp0(gl_conf->lrc_location,"")==0||g_strcmp0(gl_conf->lrc_location,"./")==0)
	{//当前目录
		q=g_strconcat(md->uri+7,NULL);
		p=q+strlen(q);
		
		while(*p!='.')
		{
			p--;
		}
		*p='\0';
		p=NULL;
		if(g_strcmp0(md->start,"00:00:000")!=0)	//如果是cue歌曲
		{
			path=g_strconcat(q,md->start,".LRC",NULL);
		}
		else
		{
			path=g_strconcat(q,".LRC",NULL);
		}

		fp=fopen(path,"r");
		if(fp==NULL)
		{
			g_free(path);
			path=NULL;
			if(g_strcmp0(md->start,"00:00:000")!=0)	//如果是cue歌曲
			{
				path=g_strconcat(q,md->start,".lrc",NULL);
			}
			else
			{
				path=g_strconcat(q,".lrc",NULL);
			}
			fp=fopen(path,"r");
			if(fp==NULL)
			{
				de_print(__DE_INFORMATION,__FUNCTION__,_("No lyrics found, downloading lrc file from Internet"));
				//开始下载
				gl_thread_download=g_thread_create(gui_lrc_callback_download_lrc,path,FALSE,NULL);
				path=NULL;
				g_free(q);
				return FALSE;
			}
		}
		g_free(q);
	}
	else
	{//自定义目录
		path=g_strconcat(gl_conf->lrc_location,md->title,"-",md->artist,".LRC",NULL);
		fp=fopen(path,"r");
		if(fp==NULL)
		{
			g_free(path);
			path=NULL;
			path=g_strconcat(gl_conf->lrc_location,md->title,"-",md->artist,".lrc",NULL);
			fp=fopen(path,"r");
			if(fp==NULL)
			{
				de_print(__DE_INFORMATION,__FUNCTION__,_("No lyrics found, downloading lrc file from Internet"));
				//开始下载
				gl_thread_download=g_thread_create(gui_lrc_callback_download_lrc,path,FALSE,NULL);
				path=NULL;
				return FALSE;
			}
		}
	}
	gui_lrc_analyse_lyrics(fp);
	g_free(path);
	return FALSE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_lrc_analyse_lyrics
//*	DISCRIPTION:	analyse lyrics
//*
//*****************************************************************************************************
gboolean gui_lrc_analyse_lyrics(FILE *fp)
{
	//加锁
	g_mutex_lock(gl_mutex_download);
	
	gl_free_list_lyrics(gl_lyrics);
	gl_lyrics=NULL;
	gint time_delay=0;
	gchar *line=NULL;
	_gl_lyrics *lrc=NULL;
	_gl_lyrics *tmp_lrc[100];
	
	gchar *p,*q;
	gchar *str_time;
	gchar *text;
	gint time;
	gint min;
	gint sec;
	gint frm;
	
	gint i=0;
	gint j=0;
	
	line=(gchar *)g_malloc(256);
	while(fgets(line,256,fp))
	{
		j=0;
		line[strlen(line)-1]='\0';	//去掉'\n'
		lrc=NULL;
		p=line;
		while(*p)
		{
			while(*p=='[')
			{
				p++;
				if((*p<='9'&&*p>='0')||(*p=='-'&&*(p+1)<='9'&&*p>='0'))		//时间标签
				{
					//读取时间
					str_time=(gchar *)g_malloc(20*sizeof(gchar));
					q=str_time;
					while(*p!=']')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';
					p++;
					//读取文本
					text=(gchar *)g_malloc(256*sizeof(gchar));
					q=text;
					while(*p!='['&&*p!='\0')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';
					//读取完成，处理数据
					sscanf(str_time,"%d:%d.%d",&min,&sec,&frm);
					time=((min*60)+sec)*100+frm-(time_delay/10);
					if(time<0)
						time=0;
					
					lrc=(_gl_lyrics*)malloc(sizeof(_gl_lyrics));
					lrc->time=time;
					if(*p=='['&&(g_strcmp0(text,"")==0||text==NULL))
					{
						tmp_lrc[j]=lrc;
						lrc->text=NULL;
						text=NULL;
						j++;
					}
					else
					{
						for (i = 0; i <j; i++)
						{
							if(g_utf8_validate(text,-1,NULL))
							{
								tmp_lrc[i]->text = (gchar *)g_malloc(128);
								tmp_lrc[i]->text = g_utf8_strncpy(tmp_lrc[i]->text,text,128);
							}
							else	//非UTF-8字符
							{
								tmp_lrc[i]->text=g_convert(text,strlen(text),"UTF-8",gl_conf->encode,NULL,NULL,NULL);
							}
						}
						j=0;
						if(g_utf8_validate(text,-1,NULL))
						{
							lrc->text = (gchar *)g_malloc(128);
							lrc->text = g_utf8_strncpy(lrc->text,text,128);
						}
						else	//非UTF-8字符
						{
							lrc->text=g_convert(text,strlen(text),"UTF-8",gl_conf->encode,NULL,NULL,NULL);
						}
					}
					if(text)
						g_free(text);
					g_free(str_time);
					gl_lyrics=g_list_append(gl_lyrics,lrc);
				}
				else														//其他标签
				{
					str_time=(gchar *)g_malloc(20*sizeof(gchar));
					q=str_time;
					while(*p!=':'&&*p!='\0')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';
					p++;
					text=(gchar *)g_malloc(256*sizeof(gchar));
					q=text;
					while(*p!=']'&&*p!='\0')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';
					if(g_strcmp0(str_time,"offset")==0||g_strcmp0(str_time,"OFFSET")==0)
					{
						sscanf(text,"%d",&time_delay);
					}
					g_free(str_time);
					g_free(text);
					p++;
					break;
				}
			}
			p++;
		}
		g_free(line);
		line=(gchar *)g_malloc(256);
	}
	g_free(line);
	fclose(fp);
	
	//对lyrics排序
	gui_lrc_sort_lyrics();
	gl_lrc=gl_lyrics;

	//释放锁
	g_mutex_unlock(gl_mutex_download);
	return TRUE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:	compare_func
//*	DISCRIPTION:	compare function to sort lyrics, served for lyrics_sort
//*
//*****************************************************************************************************
gint  gui_lrc_callback_sort_lyrics (gconstpointer c, gconstpointer d)
{
	_gl_lyrics *a=(_gl_lyrics*)c;
	_gl_lyrics *b=(_gl_lyrics*)d;
	
	if(a->time<=b->time)
		return -1;
	else
		return 1;
}

//*****************************************************************************************************
//*
//*	FUNCTION:	lyrics_sort
//*	DISCRIPTION:	sort
//*
//*****************************************************************************************************
gboolean gui_lrc_sort_lyrics()
{
	gl_lyrics=g_list_sort(gl_lyrics,gui_lrc_callback_sort_lyrics);
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_lrc_update_lrc
//*	DISCRIPTION:	update the lrc cursor
//*
//*****************************************************************************************************
gboolean gui_lrc_update_lrc()
{
	if(gl_lyrics)
	{

		GstFormat fmt=GST_FORMAT_TIME;
		gint64 position;
		gint time;
		gint start;
		
		gint min;
		gint sec;
		gint frm;
	
		GList *p=gl_lyrics;
		_gl_lyrics *plink;
		
		if(gl_core->play && gst_element_query_position(gl_core->play, &fmt, &position))
		{	
			time=position/10000000;
			sscanf(((_gl_playinglist*)(gl_playing->data))->md->start,"%d:%d:%d",&min,&sec,&frm);
			start=(min*60+sec)*100+frm;
			time=time-start;
			
			while(p)
			{
				plink=p->data;
				if(time<plink->time)
				{
					if(p!=gl_lyrics)
						gl_lrc=p->prev;
					return TRUE;
				}
				gl_lrc=p;
				p=p->next;
			}
		}
	}
	
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_lrc_seek
//*	DISCRIPTION:	seek the process of music by drag lyrics
//*
//*****************************************************************************************************
gboolean	gui_lrc_seek( GtkWidget * widget, GdkEventButton *event, gpointer data)
{
	gint duration;
	gint min;
	gint sec;
	gint frm;
	
	if (event->button == 1)			// 判断是否左键按下
	{
		switch(event->type)	//事件循环
		{
			case GDK_BUTTON_PRESS:		//按下
				if(gl_lrc)
					seekflag = TRUE;
				X = event->x;		// 取得鼠标相对位置
				Y = event->y;
				lrc_mark=gl_lrc;
				com_set_cursor(widget,com_cursor_drag);
				break;
			case GDK_BUTTON_RELEASE:	//释放
				if(gl_lyrics&&gl_lrc&&seekchanged)
				{
					if(gl_playing)
					{
						sscanf(((_gl_playinglist*)(gl_playing->data))->md->duration,"%d:%d:%d",&min,&sec,&frm);
						duration=(min*60+sec)*100+frm;
						co_set_position((((_gl_lyrics*)(gl_lrc->data))->time/(double)duration));
					}
				}
				
				seekflag = FALSE;
				seekchanged=FALSE;
				
				//鼠标变成默认箭头
				com_set_cursor(widget,com_cursor_default);
				break;
			default:
				break;
		}
	}
	if(seekflag)
	{
		//没有歌词，返回
		if(!gl_lyrics)
			return TRUE;
		
		// 取窗体鼠标相对位置
		int x, y;
		gtk_widget_get_pointer(widget, &x, &y);
					
		//计算偏移坐标
		dX=x-X;
		dY=y-Y;
		if(dX&&dY)
			seekchanged=TRUE;
	}
	return TRUE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		gui_lrc_expose_desktop
//*	DISCRIPTION:	draw desktop lyrics
//*
//*****************************************************************************************************
gboolean gui_lrc_expose_desktop( GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
	cairo_t *cr;
	gchar *path;
	path=g_strconcat(gl_dir_icon,"null.png",NULL);
	if(!image_desktop)
		image_desktop=cairo_image_surface_create_from_png(path);
	g_free(path);
	cr = gdk_cairo_create(widget->window);
	cairo_set_source_surface(cr, image_desktop, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	
	//设置样式
	gchar *font_desktop;
	gchar *color_desktop_fg1,*color_desktop_fg2,*color_desktop_bg1,*color_desktop_bg2;
	_gl_theme *th;
	th=th_get_th_by_name("desktop");
	if((!(gl_conf->custom_lrc))&&(th)&&(th->font)&&(th->color_fg1)&&(th->color_fg2)&&(th->color_bg1)&&(th->color_bg2))
	{
		font_desktop=th->font;
		color_desktop_fg1=th->color_fg1;
		color_desktop_fg2=th->color_fg2;
		color_desktop_bg1=th->color_bg1;
		color_desktop_bg2=th->color_bg2;
	}
	else
	{
		font_desktop=gl_conf->font_desktop;
		color_desktop_fg1=gl_conf->color_desktop_fg1;
		color_desktop_fg2=gl_conf->color_desktop_fg2;
		color_desktop_bg1=gl_conf->color_desktop_bg1;
		color_desktop_bg2=gl_conf->color_desktop_bg2;
	}


	GstFormat fmt=GST_FORMAT_TIME;
	gint64 position;
	
	int width;
	int height;
	
	gdk_drawable_get_size(GDK_DRAWABLE(widget->window),&width,&height);
	
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

	if(gl_lrc)
	{
		if(gl_lrc->data==NULL)
		{
			cairo_destroy(cr);
			return FALSE;
		}
		if(((_gl_lyrics*)(gl_lrc->data))->text==NULL)
		{
			cairo_destroy(cr);
			return FALSE;
		}

		//计算占空比
		gint time=0;
		gint pos=0;
		gint start;
		gint min;
		gint sec;
		gint frm;
		if(gl_lrc->next)
			time=((_gl_lyrics*)(gl_lrc->next->data))->time-((_gl_lyrics*)(gl_lrc->data))->time;
		if(gl_core->play && gst_element_query_position(gl_core->play, &fmt, &position))
		{
			sscanf(((_gl_playinglist*)(gl_playing->data))->md->start,"%d:%d:%d",&min,&sec,&frm);
			start=(min*60+sec)*100+frm;
			pos=(gint)(position/10000000)-start;
			pos=pos-((_gl_lyrics*)(gl_lrc->data))->time;
		}
		if(time!=0)
		{
			x=(int)((pos/(gdouble)time)*layoutX);
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

		pango_layout_set_text (layout, ((_gl_lyrics*)(gl_lrc->data))->text, -1);
		desc = pango_font_description_from_string (font_desktop);
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
		gint r,g,b;
		pat = cairo_pattern_create_linear (0, 0,10.0, 150);
		sscanf(color_desktop_bg1,"%d:%d:%d",&r,&g,&b);
		cairo_pattern_add_color_stop_rgb (pat, 0.1, r/65535.0, g/65535.0, b/65535.0);
		sscanf(color_desktop_bg2,"%d:%d:%d",&r,&g,&b);
		cairo_pattern_add_color_stop_rgb (pat, 0.5, r/65535.0, g/65535.0, b/65535.0);
		sscanf(color_desktop_bg1,"%d:%d:%d",&r,&g,&b);
		cairo_pattern_add_color_stop_rgb (pat, 0.9, r/65535.0, g/65535.0, b/65535.0);
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
		sscanf(color_desktop_fg1,"%d:%d:%d",&r,&g,&b);
		cairo_pattern_add_color_stop_rgb (pat, 0.1, r/65535.0, g/65535.0, b/65535.0);
		sscanf(color_desktop_fg2,"%d:%d:%d",&r,&g,&b);
		cairo_pattern_add_color_stop_rgb (pat, 0.5, r/65535.0, g/65535.0, b/65535.0);
		sscanf(color_desktop_fg1,"%d:%d:%d",&r,&g,&b);
		cairo_pattern_add_color_stop_rgb (pat, 0.9, r/65535.0, g/65535.0, b/65535.0);
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
	
	cairo_destroy (cr) ;
	gtk_widget_set_sensitive(widget,!(gl_conf->desktoplyrics_fix));
	GdkRegion *region;
	if(!(gl_conf->desktoplyrics_fix))
	{
		GdkRectangle *rect=(GdkRectangle*)g_malloc(sizeof(GdkRectangle));
		rect->x=rect->y=0;
		rect->width=width;
		rect->height=height;
		region=gdk_region_rectangle((const GdkRectangle *)rect);
		g_free(rect);
	}
	else
	{
		region=gdk_region_new();
	}
	gdk_window_input_shape_combine_region (widget->window, region, 0, 0);
	gdk_region_destroy (region);

	return FALSE ;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		gui_lrc_drag_desktop
//*	DISCRIPTION:	handler of the drag event on desktop lyrics window
//*
//*****************************************************************************************************
gboolean gui_lrc_move_desktop( GtkWidget * widget, GdkEvent *event, gpointer data)
{
	if (event->button.button == 1)       // 判断是否左键按下
	{
		switch(event->type)	//事件循环
		{
			case GDK_BUTTON_PRESS:		//按下
				desktopX = event->button.x;		// 取得鼠标相对位置
				desktopY = event->button.y;
				dragflag=TRUE;
				
				//鼠标变成手形
				com_set_cursor(widget,com_cursor_move);
				break;
			case GDK_BUTTON_RELEASE:	//释放
				dragflag=FALSE;

				//鼠标变成默认箭头
				com_set_cursor(widget,com_cursor_default);
				gtk_window_get_position(GTK_WINDOW(widget), &(gl_conf->desktoplyrics_x), &(gl_conf->desktoplyrics_y)); 
				break;
			case GDK_MOTION_NOTIFY:
				if(dragflag)
				{
					// 取窗体鼠标相对位置
					int x, y;
					GtkWidget *window = widget;
					gtk_window_get_position(GTK_WINDOW(window), &x, &y);         // 取窗体绝对坐标
					gtk_window_move((GtkWindow *) window,
							x + event->button.x - desktopX,
							y + event->button.y - desktopY); // 移动窗体
				}	
			default:
				break;
		}
	}
	switch(event->type)	//事件循环
	{
		case GDK_ENTER_NOTIFY:	//进入
			notifyflag=TRUE;
			break;
		case GDK_LEAVE_NOTIFY:	//退出
			if(!dragflag)//非拖动状态
				notifyflag=FALSE;
			break;
		default:
			break;
	}
	return TRUE;
}
