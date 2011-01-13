#include "core.h"

static guint spect_bands = 40;
#define AUDIOFREQ 32000

//FUNCTIONS
// static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data)
// {
// 	GMainLoop *loop = data;
// 	switch (GST_MESSAGE_TYPE (msg))
// 	{
// 		case GST_MESSAGE_EOS:
// 			g_main_loop_quit (loop);
// 			break;
// 		case GST_MESSAGE_ERROR:
// 		{
// 			gchar *debug;
// 			GError *err;
// 			gst_message_parse_error (msg, &err, &debug);
// 			g_free (debug);
// 			g_print ("Error: %s\n", err->message);
// 			g_error_free (err);
// 			g_main_loop_quit (loop);
// 			break;
// 		}
// 		default:
// 			if(msg&&msg->structure&&gst_structure_has_name(msg->structure, "preroll-pixbuf"))
// 			{
// 				g_main_loop_quit (loop);
// 			}
// 			break;
// 	}
// 	return TRUE;
// }

gboolean co_callback_core_bus(GstBus *bus,GstMessage *message,gpointer data)
{
	
	const GstStructure *s;
	const gchar *name;
	GstTagList *tags;
	GstBuffer *buffer;
	
	switch (message->type)
	{
		case GST_MESSAGE_EOS:
			co_set_next();
			co_play();
			gui_window_update_display();
			break;
		case GST_MESSAGE_ELEMENT:
			s= gst_message_get_structure (message);
			name= gst_structure_get_name (s);

			if (strcmp (name, "spectrum") == 0)
			{
				const GValue *magnitudes;
				const GValue *phases;
				const GValue *mag, *phase;
				gdouble freq;
				guint i;

				magnitudes = gst_structure_get_value (s, "magnitude");
				phases = gst_structure_get_value (s, "phase");

				for (i = 0; i < spect_bands; ++i)
				{
					freq = (gdouble) ((AUDIOFREQ /1.5) * i + AUDIOFREQ / 4) / spect_bands;
					mag = gst_value_list_get_value (magnitudes, i);
					phase = gst_value_list_get_value (phases, i);
					if (mag != NULL && phase != NULL)
					{
						co_mag[i]=100+g_value_get_float (mag);
						co_phase[i]=g_value_get_float (phase);
					}
				}
			}
			break;
		case GST_MESSAGE_TAG:
		{
			_gl_metadata *md=((_gl_playinglist*)(gl_playing->data))->md;
			gchar *codec=NULL;
			gchar *title=NULL;
			gchar *artist=NULL;
			gchar *album=NULL;
			gchar *genre=NULL;

			gint min=0;
			gint sec=0;
			gint frm=0;
			gint64 dura=0;
			GstFormat fmt=GST_FORMAT_TIME;
			
			gst_message_parse_tag(message,&tags);

			//专辑封面
			//此处存在内存泄露问题
			if(gst_tag_list_get_buffer(tags,GST_TAG_IMAGE,&buffer))
			{
// 				GMainLoop *loop;
// 				GstElement *appsrc;
// 				GstElement *dec;
// 				GstElement *colorspace;
// 				GstElement *scale;
// 				GstElement *sink;
// 				GstElement *pipeline;
// 				GstBus *bus;
// 				
// 				loop=g_main_loop_new(NULL,FALSE);
// 				if(g_strrstr(gst_caps_to_string(buffer->caps),"jpeg"))
// 					dec=gst_element_factory_make("jpegdec", "dec");
// 				else if(g_strrstr(gst_caps_to_string(buffer->caps),"png"))
// 					dec=gst_element_factory_make("pngdec", "dec");
// 				else
// 					break;
// 				
// 				appsrc=gst_element_factory_make("appsrc","appsrc");
// 				colorspace=gst_element_factory_make("ffmpegcolorspace", "colorspace");
// 				scale=gst_element_factory_make("videoscale", "scale");
// 				sink=gst_element_factory_make("gdkpixbufsink", "sink");
// 				pipeline=gst_pipeline_new("pipeline");
// 				bus=gst_pipeline_get_bus(GST_PIPELINE(pipeline));
// 				gst_bus_add_watch (bus, bus_call, loop);
// 				gst_object_unref(bus);
// 				gst_app_src_push_buffer((GstAppSrc *)(appsrc),buffer);
// 				gst_bin_add_many (GST_BIN (pipeline),appsrc, dec,colorspace,scale, sink, NULL);
// 				gst_element_link_many(appsrc, dec,colorspace, scale, sink, NULL);
// 				
// 				gst_element_set_state(pipeline, GST_STATE_NULL);
// 				gst_element_set_state(pipeline, GST_STATE_PLAYING);
// 				g_main_loop_run(loop);
// 				
// 				GdkPixbuf *pixbuf=NULL;
// 				g_object_get(G_OBJECT(sink),"last-pixbuf",&pixbuf,NULL);
// 				if(pixbuf)
// 				{
// 					GdkPixbuf *temp=NULL;
// 					gint width=0,height=0;
// 					
// 					width=gdk_pixbuf_get_width(gl_pixbuf_cd);
// 					height=gdk_pixbuf_get_height(gl_pixbuf_cd);
// 					
// 					if(width==0) width=150;
// 					if(height==0) height=150;
// 					
// 					temp=gdk_pixbuf_scale_simple(pixbuf,width,height, GDK_INTERP_BILINEAR);
// 					gdk_pixbuf_unref(pixbuf);
// 					gtk_image_set_from_pixbuf(GTK_IMAGE(gui_window_image_cd),temp);
// 				}
				gst_buffer_unref(buffer);
			}
			//自动均衡器
			if(gst_tag_list_get_string(tags,GST_TAG_GENRE,&genre)&&gl_conf->autoeq)
			{
				gl_eq=eq_find_best(gl_equalizer,genre);
				eq_set_equalizer();
				g_free(genre);
			}
			//获取文件类型
			if(gst_tag_list_get_string(tags,GST_TAG_AUDIO_CODEC,&codec))
			{
				if(codec!=NULL)
				{
					if(md->codec)
						g_free(md->codec);
					md->codec = g_malloc(128);
					md->codec = g_utf8_strncpy(md->codec,codec,127);
				}
				g_free(codec);
			}
			gst_tag_list_free(tags);
		}
		default:
			break;
	}
	return TRUE;
}

//********************************************************************************
//*	
//*	FUNCTION:		co_new
//*	DISCRIPTION:	create a new linnya playing core
//*
//********************************************************************************
gboolean co_new()
{	
	//创建_gl_core ＊类型的播放核心gl_core
	gl_core = (_gl_core*) g_malloc(sizeof(_gl_core));

	GstBus		*bus = NULL;
	GstElement	*play = NULL;
	GstElement	*equalizer = NULL ,
				*convert = NULL,
				*spectrum= NULL,
				*vol = NULL,
				*audiosink = NULL ;
	GstElement	*audio_bin;
	GstPad		*pad1;
	
	//配置gl_core的基本信息
	play = gst_element_factory_make("playbin", "play");
	bus	 = gst_pipeline_get_bus(GST_PIPELINE(play));
	gst_element_set_state(play, GST_STATE_NULL);
	gst_bus_add_watch(bus, (GstBusFunc)co_callback_core_bus, gl_core);
	gst_object_unref(bus);
	
	equalizer	= gst_element_factory_make("equalizer-10bands", "equalizer");
	convert		= gst_element_factory_make("audioconvert", "convert");
	spectrum	= gst_element_factory_make ("spectrum", "spectrum");
	vol 		= gst_element_factory_make("volume","vol");
	audiosink	= gst_element_factory_make("autoaudiosink","audiosink");
	
	audio_bin= gst_bin_new("audio-bin");
	gst_bin_add_many(GST_BIN(audio_bin), equalizer, convert, spectrum, vol, audiosink, NULL);
	gst_element_link_many(equalizer, convert, spectrum, vol, audiosink,NULL);
	pad1 = gst_element_get_static_pad(equalizer, "sink");
	gst_element_add_pad(audio_bin, gst_ghost_pad_new(NULL,pad1));
	g_object_set(G_OBJECT(play), "audio-sink", audio_bin, NULL);
	g_object_set (G_OBJECT (spectrum), "bands", 40, "threshold", -100, "message", TRUE, "message-phase", TRUE, NULL);
	
	gl_core->play = play ;
	gl_core->equalizer = equalizer ;
	gl_core->vol=vol;
	gl_core->audiosink = audiosink ;
	
	eq_read_equalizer();
	eq_set_equalizer();

	g_object_set(G_OBJECT(gl_core->vol), "volume", gl_conf->volume/100.0, NULL);

	return TRUE;
}

gboolean co_play()
{
	GstState state;
	GstClockTime timeout=0;
	gst_element_get_state(gl_core->play,&state,NULL,timeout);

	//始终播放gl_playing所指向的音乐
	if(!gl_playing)
	{
		de_print(__DE_ERROR,__FUNCTION__,"gl_playing is null, no music to be played.");
		return FALSE;
	}
	_gl_playinglist *pgl=gl_playing->data;
	g_object_set(G_OBJECT(gl_core->play), "uri", pgl->md->uri, NULL);
	
	if(state!=GST_STATE_PAUSED)
	{
		gst_element_set_state(gl_core->play,GST_STATE_NULL);
		gst_element_set_state(gl_core->play,GST_STATE_PAUSED);
		if(	g_strcmp0(pgl->md->start,"00:00:000"))
		{
			sleep(1);
			co_set_position(0);
		}
	}
	if(!gst_element_set_state(gl_core->play,GST_STATE_PLAYING))
	{
		return FALSE;
	}
	if(gl_lyrics==NULL)
	{
		gui_lrc_read_lyrics();
	}
	return TRUE;
}
gboolean co_pause()
{
	if(!gl_playing)
	{
		de_print(__DE_ERROR,__FUNCTION__,"gl_playing is null, no music to be played.");
		return FALSE;
	}
	if(!gst_element_set_state(gl_core->play,GST_STATE_PAUSED))
		return FALSE;
	return TRUE;
}
gboolean co_stop()
{
	if(!gl_playing)
	{
		de_print(__DE_ERROR,__FUNCTION__,"gl_playing is null, no music to be played.");
		return FALSE;
	}
	if(!gst_element_set_state(gl_core->play,GST_STATE_READY))	
		return FALSE;
	gl_free_list_lyrics(gl_lyrics);
	gl_lyrics=NULL;
	return TRUE;
}
gboolean co_set_next()
{
	if(!gl_playing)
	{
		de_print(__DE_ERROR,__FUNCTION__,"gl_playing is null, no music to be played.");
		return FALSE;
	}
	gl_played=pl_find_pl_by_playing();
	
	gl_free_list_lyrics(gl_lyrics);
	gl_lyrics=NULL;
	int plnum;
	
	if(gl_conf->random==1)
	{//如果随机播放模式
		srand((int)time(0));
		plnum=rand()%(g_list_length(gl_playinglist));
	}
	else
	{//如果非随机模式
		if(gl_conf->single==0)	//如果列表模式
			plnum=(g_list_index(gl_playinglist,gl_playing->data)+g_list_length(gl_playinglist)+1)%g_list_length(gl_playinglist);
		else					//如果是单曲模式
			plnum=g_list_index(gl_playinglist,gl_playing->data);
	}
	gl_playing=g_list_nth(gl_playinglist,plnum);
	return TRUE;
}
gboolean co_set_prev()
{
	if(!gl_playing)
	{
		de_print(__DE_ERROR,__FUNCTION__,"gl_playing is null, no music to be played.");
		return FALSE;
	}
	gl_played=pl_find_pl_by_playing();
	
	gl_free_list_lyrics(gl_lyrics);
	gl_lyrics=NULL;
	int plnum=0;
	
	if(gl_conf->random==1)
	{//如果随机播放模式
		srand((int)time(0));
		plnum=rand()%(g_list_length(gl_playinglist));
	}
	else
	{//如果非随机模式
		if(gl_conf->single==0)
		{//如果列表模式
			plnum=(g_list_index(gl_playinglist,gl_playing->data)+g_list_length(gl_playinglist)-1)%g_list_length(gl_playinglist);
		}
		else
		{//如果是单曲模式
			plnum=g_list_index(gl_playinglist,gl_playing->data);
		}
	}
	gl_playing=g_list_nth(gl_playinglist,plnum);
	return TRUE;
}
gboolean co_quit()
{
	gst_element_set_state(gl_core->play,GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(gl_core->play));
	return TRUE;
}

gdouble co_get_position()
{
	if(!gl_playing)
	{
		return 0;
	}
	
	_gl_playinglist *pgl;
	GstFormat fmt=GST_FORMAT_TIME;
	gint64 start=0;
	gint64 dura=0;
	gint64 pos=0;
	gdouble position=0;
	
	gint min;
	gint sec;
	gint frm;
	
	pgl=gl_playing->data;
	
	sscanf(pgl->md->duration,"%d:%d:%d",&min,&sec,&frm);
	dura=((gint64)((min*60+sec)*100+frm)*10000000);
	sscanf(pgl->md->start,"%d:%d:%d",&min,&sec,&frm);
	start=((gint64)((min*60+sec)*100+frm)*10000000);
	
	if(gst_element_query_position(gl_core->play, &fmt, &pos))
	{
		if(pos>0)
		{
			pos=pos-start;
			position=(gdouble)(pos/(gdouble)dura);
		}
	}
	return position;
}
gboolean co_set_position(gdouble percentage)
{
	if(!gl_playing)
	{
		de_print(__DE_ERROR,__FUNCTION__,"gl_playing is null, no music to be played.");
		return FALSE;
	}
	_gl_playinglist *pgl;
	gint64 start;
	gint64 dura;
	gint64 pos;
	gint min;
	gint sec;
	gint frm;
	
	pgl=gl_playing->data;
	
	sscanf(pgl->md->duration,"%d:%d:%d",&min,&sec,&frm);
	dura=((gint64)((min*60+sec)*100+frm)*10000000);
	sscanf(pgl->md->start,"%d:%d:%d",&min,&sec,&frm);
	start=((gint64)((min*60+sec)*100+frm)*10000000);
	
	pos=(dura*percentage)+start;
	
	if(!gst_element_seek(gl_core->play, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET, pos, GST_SEEK_TYPE_SET, start+dura))
	{
		de_print(__DE_WARNNING,__FUNCTION__,_("Failed to seek to desired position"));
		return FALSE;
	}
	return TRUE;
}
gdouble co_get_volume()
{
	gdouble percentage;
	g_object_get(G_OBJECT(gl_core->vol),"volume",&percentage,NULL);
	return percentage;
}
gboolean co_set_volume(gdouble percentage)
{
	gl_conf->volume=(gint)(percentage*100);
	g_object_set(G_OBJECT(gl_core->vol),"volume",percentage,NULL);
	return TRUE;
}
GstState co_get_state()
{
	GstState state;
	GstClockTime timeout=0;
	gst_element_get_state(gl_core->play,&state,NULL,timeout);
	return state;
}