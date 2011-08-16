#include "audio.h"

/* *
 *  callback
 * */

/* *
 * NAME:	ly_func_audio_callback
 * VARS:	bus[GstBus*] the bus;message[GstMessage*] the message;data[gpointer] the data;
 * RETN:	[boolean]		
 * DESC:	call to process.
 * */
gboolean ly_audio_callback(GstBus *bus,GstMessage *message,gpointer data)
{
	GstTagList* tags;
	
	switch (message->type)
	{
		case GST_MESSAGE_EOS:
			ly_audio_next();
			break;
		case GST_MESSAGE_TAG:
		{
			gchar* genre=NULL;
			
			gst_message_parse_tag(message,&tags);
			
			if(gst_tag_list_get_string(tags,GST_TAG_GENRE,&genre))
			{
				ly_msg_put("genre","core:audio",genre);
			}
			break;
			
		}
		default:
		break;
	}	
	return TRUE;
}

/* *
 *  module audio init
 * */

/* *
 * NAME:	ly_func_audio_init
 * VARS:	
 * RETN:	[boolean]		
 * DESC:	initalize module audio.
 * */
gboolean ly_audio_init()
{
	ly_global_debug("info","Initial core module: AUDIO ...\n");
	ly_audio_core=(lyAudioCore*)g_malloc(sizeof(lyAudioCore));
	GstElement* play=NULL;
	GstElement* equalizer=NULL;
	GstElement* convert=NULL;
	GstElement* spectrum=NULL;
	GstElement* volume=NULL;
	GstElement* audiosink=NULL;
	GstElement* audio_bin;
	GstPad*     mpad;
	GstBus* bus=NULL;


	play=gst_element_factory_make("playbin","play");
	bus=gst_pipeline_get_bus(GST_PIPELINE(play));
	gst_element_set_state(play,GST_STATE_NULL);
	gst_bus_add_watch(bus,(GstBusFunc)ly_audio_callback,ly_audio_core);
	gst_object_unref(bus);
	equalizer=gst_element_factory_make("equalizer-10bands","equalizer");
	volume= gst_element_factory_make("volume","volume");
	convert=gst_element_factory_make("audioconvert","convert");
	spectrum=gst_element_factory_make("spectrum","spectrum");
	audiosink=gst_element_factory_make("autoaudiosink","audiosink");

	audio_bin=gst_bin_new("audio-bin");
	gst_bin_add_many(GST_BIN(audio_bin),equalizer,convert,volume,audiosink,NULL);
	gst_element_link_many(equalizer,convert,volume,audiosink,NULL);
	mpad = gst_element_get_static_pad(equalizer, "sink");
	gst_element_add_pad(audio_bin, gst_ghost_pad_new(NULL,mpad));
	g_object_set(G_OBJECT(play),"audio-sink",audio_bin,NULL);
	g_object_set(G_OBJECT(spectrum),"bands",40,"threshold",-100,"message",TRUE,"message-phase",TRUE,NULL);

	gdouble vol=0.3;
	if(!ly_conf_get("audio_volume","%lf",&vol))
	{
		ly_conf_set("audio_volume","%lf",vol);
	}
	g_object_set(G_OBJECT(volume), "volume", vol, NULL);

	ly_audio_meta=NULL;
	gint mid=0;
	if(!ly_conf_get("audio_playing","%d",&mid))
		ly_conf_set("audio_playing","%d",mid);
	ly_audio_meta=ly_db_get_metadata_by_id(mid);
	
	ly_audio_core->play=play;
	ly_audio_core->spectrum=spectrum;
	ly_audio_core->volume=volume;
	ly_audio_core->equalizer=equalizer;
	return TRUE;

}


gboolean ly_audio_set_meta(gint id)
{
	if(ly_audio_meta)
		ly_db_free_metadata(ly_audio_meta);
	ly_audio_meta=ly_db_get_metadata_by_id(id);
	ly_msg_put("meta_changed","core:audio",NULL);
	return TRUE;
}
/* *
 *  previous && next
 * */

/* *
 * NAME:	ly_func_audio_next
 * VARS:	
 * RETN:	[gboolean]	
 * DESC:	search 4 next song to play.
 * */
gboolean ly_audio_next()
{
	if(!ly_audio_meta)
		return FALSE;
	
	gchar sql[1024]="";
	gint random=0;
	gint repeat=1;
	gint single=0;
	
	GstState state;
	state=ly_audio_get_state();
	
	if(!ly_conf_get("audio_mode","%d:%d:%d", &random,&repeat,&single))
		ly_conf_set("audio_mode","%d:%d:%d",random, repeat, single);
	/*
	 * set to be NULL
	 */
	if(!gst_element_set_state(ly_audio_core->play,GST_STATE_NULL))
	{
		ly_msg_put("error","core:audio","Gstreamer States Error!");
		return FALSE;
	}
	
	/*
	 * set next
	 */
	if(random)
	{
		
		g_snprintf(sql,sizeof(sql),"SELECT * FROM plist ORDER BY RANDOM() LIMIT 1");
		ly_db_free_metadata(ly_audio_meta);
		ly_audio_meta=NULL;
		ly_audio_meta=ly_db_get_metadata(sql);
	}
	else
	{
		if(single)
		{
			if(!repeat)
			{
				state=GST_STATE_NULL;
			}
		}
		else
		{
			
			g_snprintf(sql,sizeof(sql),"SELECT * FROM plist WHERE playing>%d ORDER BY playing LIMIT 1",ly_audio_meta->playing);
			ly_db_free_metadata(ly_audio_meta);
			ly_audio_meta=NULL;
			ly_audio_meta=ly_db_get_metadata(sql);
		
			if(ly_audio_meta==NULL)
			{
				g_snprintf(sql,sizeof(sql),"SELECT * FROM plist ORDER BY playing LIMIT 1");
				ly_audio_meta=ly_db_get_metadata(sql);
				if(!repeat)
				{
					state=GST_STATE_NULL;
				}
			}
		}
	}
	ly_msg_put("meta_changed","core:audio",NULL);
	/*
	 * set to be READY
	 */
	if(!gst_element_set_state(ly_audio_core->play,GST_STATE_READY))
	{
		ly_msg_put("Error","core:audio","Gstreamer States Error!");
		return FALSE;
	}
	/*
	 * play if nessasary
	 */
	if(state==GST_STATE_PLAYING)
	{
		ly_audio_play();
	}
	return TRUE;
}

/* *
 * NAME:	ly_func_audio_prev
 * VARS:	
 * RETN:	[boolean] 		
 * DESC:	search 4 the previous to play.
 * */
gboolean ly_audio_prev()
{
	if(!ly_audio_meta)
		return FALSE;
	
	gchar sql[1024]="";
	gint random=0;
	gint repeat=1;
	gint single=0;
	
	GstState state;
	state=ly_audio_get_state();
	if(!ly_conf_get("audio_mode","%d:%d:%d",&random,&repeat,&single))
	{
		ly_conf_set("audio_mode","%d:%d:%d",random,repeat,single);
	}
	
	/*
	 * set to be NULL
	 */
	if(!gst_element_set_state(ly_audio_core->play,GST_STATE_NULL))
	{
		ly_msg_put("error","core:audio","Gstreamer States Error!");
		return FALSE;
	}
	
	/*
	 * set next
	 */
	if(random)
	{
		g_snprintf(sql,sizeof(sql),"SELECT * FROM plist ORDER BY RANDOM() LIMIT 1");
		ly_db_free_metadata(ly_audio_meta);
		ly_audio_meta=ly_db_get_metadata(sql);
	}
	else
	{
		if(single)
		{
			if(!repeat)
			{
				state=GST_STATE_NULL;
			}
		}
		else
		{
			g_snprintf(sql,sizeof(sql),"SELECT * FROM plist WHERE playing<%d ORDER BY playing DESC LIMIT 1",ly_audio_meta->playing);
			ly_db_free_metadata(ly_audio_meta);
			ly_audio_meta=ly_db_get_metadata(sql);
			
			if(ly_audio_meta==NULL)
			{
				g_snprintf(sql,sizeof(sql),"SELECT * FROM plist ORDER BY playing DESC LIMIT 1");
				ly_db_free_metadata(ly_audio_meta);
				ly_audio_meta=ly_db_get_metadata(sql);
				if(!repeat)
				{
					state=GST_STATE_NULL;
				}
			}
		}
	}
	ly_msg_put("meta_changed","core:audio",NULL);
	/*
	 * set to be READY
	 */
	if(!gst_element_set_state(ly_audio_core->play,GST_STATE_READY))
	{
		ly_msg_put("Error","core:audio","Gstreamer States Error!");
		return FALSE;
	}
	/*
	 * play if nessasary
	 */
	if(state==GST_STATE_PLAYING)
	{
		ly_audio_play();
	}
	return TRUE;
}

/* *
 *  playstates
 * */

/* *
 * NAME:	ly_func_audio_get_state
 * VARS:	
 * RETN:	[GstState] state		
 * DESC:	ensure the state now.
 * */
GstState ly_audio_get_state()
{
	GstState     state;
	GstClockTime timeout=0;
	gst_element_get_state(ly_audio_core->play,&state,NULL,timeout);

	return state;
}

/* *
 * NAME:	ly_func_audio_play
 * VARS:	
 * RETN:	[gboolean] 		
 * DESC:	play a song.
 * */
gboolean ly_audio_play()
{	
	if(!((ly_audio_meta)&&(ly_audio_meta->uri)))
	{
		ly_msg_put("meta_missed","core:audio",NULL);
		return FALSE;
	}
	
	g_object_set(G_OBJECT(ly_audio_core->play),"uri",ly_audio_meta->uri,NULL);
	
	if(ly_audio_meta->flag==1)	//cue play
	{
		if(!ly_audio_set_position(0))
		{
			ly_msg_put("Error","ly_func_audio_play","Position not reachable");
		}
	}
	

	if(!gst_element_set_state(ly_audio_core->play,GST_STATE_PLAYING))
	{
		ly_msg_put("file_missed","core:audio",NULL);
		return FALSE;
	}

	ly_msg_put("play","core:audio", NULL);
	return TRUE;



}

/* *
 * NAME:	ly_func_audio_pause
 * VARS:	
 * RETN:	[gboolean] 		
 * DESC:	pause a song.
 * */
gboolean ly_audio_pause()
{
	GstState state;
	state=ly_audio_get_state();
	

	if((state==GST_STATE_PAUSED)||(state==GST_STATE_READY))
	{
		return FALSE;
	}
	if(state==GST_STATE_PLAYING)
	{
		if(!gst_element_set_state(ly_audio_core->play,GST_STATE_PAUSED))
		{
			ly_msg_put("warnning","core:audio","Gstreamer States Error!");
			return FALSE;
		}
		ly_msg_put("pause","core:audio",NULL);
		return TRUE;	
	}

	ly_msg_put("warnning","core:audio","Gstreamer States Error!");
	return FALSE;


}

/* *
 * NAME:	ly_func_audio_stop
 * VARS:	
 * RETN:	[gboolean] 		
 * DESC:	stop a song.
 * */
gboolean ly_audio_stop()
{
	GstState state;
	state=ly_audio_get_state();

	if((state==GST_STATE_PLAYING)||(state==GST_STATE_PAUSED)||(state==GST_STATE_NULL))
	{
		if(!gst_element_set_state(ly_audio_core->play,GST_STATE_NULL))
		{
			ly_msg_put("warnning","core:audio","Gstreamer States Error!");
			return FALSE;
		}
		if(!gst_element_set_state(ly_audio_core->play,GST_STATE_READY))
		{
			ly_msg_put("warnning","core:audio","Gstreamer States Error!");
			return FALSE;
		}
	}
	if(state==GST_STATE_READY)
	{
		ly_msg_put("stop","core:audio",NULL);
		return FALSE;
	}

	ly_msg_put("warnning","core:audio","Gstreamer States Error!");
	return FALSE;
}

/* *
 *  volume
 * */

/* *
 * NAME:	ly_audio_setvolume
 * VARS:	volume[gdouble] the volume;
 * RETN:	[gboolean] 		
 * DESC:	set volume.
 * */
gboolean ly_audio_set_volume(gdouble volume)
{
	g_object_set(G_OBJECT(ly_audio_core->volume),"volume",volume,NULL);
	ly_conf_set("audio_volume","%lf",volume);

	return TRUE;


}

/* *
 * NAME:	ly_audio_get_volume
 * VARS:	
 * RETN:	[gdouble] volume		
 * DESC:	take out volume in double.
 * */
gdouble ly_audio_get_volume()
{
	gdouble volume;

	g_object_get(G_OBJECT(ly_audio_core->volume),"volume",&volume,NULL);
	
	if(!volume)
	{
		ly_msg_put("Error","ly_func_audio_getvolumn"," Volumn Set Failure");
	}
	
	return volume;

}

/* *
 *  seek position
 * */

/* *
 * NAME:	ly_func_audio_setposition
 * VARS:	percent[gdouble] the position
 * RETN:	[gboolean] 		
 * DESC:	set position to play.
 * */
gboolean ly_audio_set_position(gdouble percent)
{
	
	gint64 position=0;
	gint64 start=0;
	gint64 duration=0;
	gint min=0;
	gint second=0;
	gint mis=0;


	if(!ly_audio_meta||!(ly_audio_meta->uri))
	{
		ly_msg_put("meta_missed","core:audio",NULL);
		return FALSE;
	}

	sscanf(ly_audio_meta->start,"%d:%d:%d",&min,&second,&mis);
	start=((gint64)((min*60+second)*100+mis)*10000000);
	sscanf(ly_audio_meta->duration,"%d:%d:%d",&min,&second,&mis);
	duration=((gint64)((min*60+second)*100+mis)*10000000);
	position=(duration*percent)+start;
	if(!gst_element_seek(ly_audio_core->play,1.0,GST_FORMAT_TIME,GST_SEEK_FLAG_FLUSH,GST_SEEK_TYPE_SET,position,GST_SEEK_TYPE_SET,start+duration))
	{
		ly_msg_put("error","core:audio","Fail to seek to desired position");
		return FALSE;
	}

	return TRUE;	
}

/* *
 * NAME:	ly_func_audio_get_position
 * VARS:	
 * RETN:	[gdouble] position		
 * DESC:	take out play position in gdouble.
 * */
gdouble ly_audio_get_position()
{
	gint64 dura=0;
	gint64 pos=0;
	gdouble position;

	if(!ly_audio_meta||!(ly_audio_meta->uri))
	{
		ly_msg_put("meta_missed","core:audio",NULL);
		return FALSE;
	}
	
	dura=ly_db_duration2dura(ly_audio_meta->duration);
	if(dura<=0)
		return FALSE;
	pos=ly_audio_get_position_abs();
	position=pos/(double)dura;
	return position;
}


/* *
 * NAME:	ly_audio_get_position_abs
 * VARS:	
 * RETN:	[gint] position		
 * DESC:	take out play position in gint.
 * */
gint64 ly_audio_get_position_abs()
{
	GstFormat fmt=GST_FORMAT_TIME;
	gint64 start=0;
	gint64 dura=0;
	gint64 pos=0;

	if(!ly_audio_meta||!(ly_audio_meta->uri))
	{
		ly_msg_put("meta_missed","core:audio",NULL);
		return FALSE;
	}

	start=ly_db_duration2dura(ly_audio_meta->start);
	dura=ly_db_duration2dura(ly_audio_meta->duration);
	if(dura<=0)
		return 0;
	if(!gst_element_query_position(ly_audio_core->play,&fmt,&pos))
	{
		ly_msg_put("warnning","core:audio", "Position Error!");
		return 0;
	}
	if(pos-start<-60000000000||dura-pos<-60000000000)
	{
		ly_msg_put("warnning","core:audio", "Position Error!");
		return 0;
	}
	pos=pos-start;
	if(pos<start)
		pos=start;
	else if(pos>dura)
		pos=dura;
	
	return pos;
}

/* *
 *  equalizer
 * */

/* *
 * NAME:	ly_audio_setequalizer
 * VARS:	bands of equalizer
 * RETN:	[gboolean] 		
 * DESC:	set equalizer.
 * */
gboolean ly_audio_set_equalizer(gdouble band0,gdouble band1,gdouble band2,gdouble band3,gdouble band4,gdouble band5,gdouble band6,gdouble band7,gdouble band8,gdouble band9)
{
	if(!(ly_audio_core->equalizer&&ly_audio_core))
	{
		ly_msg_put("warnning","core:audio","Equalizer Error!");
		return FALSE;
	}
	g_object_set(G_OBJECT(ly_audio_core->equalizer),"band0",band0,NULL);
	g_object_set(G_OBJECT(ly_audio_core->equalizer),"band1",band1,NULL);
	g_object_set(G_OBJECT(ly_audio_core->equalizer),"band2",band2,NULL);
	g_object_set(G_OBJECT(ly_audio_core->equalizer),"band3",band3,NULL);
	g_object_set(G_OBJECT(ly_audio_core->equalizer),"band4",band4,NULL);
	g_object_set(G_OBJECT(ly_audio_core->equalizer),"band5",band5,NULL);
	g_object_set(G_OBJECT(ly_audio_core->equalizer),"band6",band6,NULL);
	g_object_set(G_OBJECT(ly_audio_core->equalizer),"band7",band7,NULL);
	g_object_set(G_OBJECT(ly_audio_core->equalizer),"band8",band8,NULL);
	g_object_set(G_OBJECT(ly_audio_core->equalizer),"band9",band9,NULL);

	return TRUE;

}


/* *
 * NAME:	ly_audio_get_equalizer
 * VARS:	pointer of bands
 * RETN:	[gboolean] 		
 * DESC:	get equalizer.
 * */
gboolean ly_audio_get_equalizer(gdouble* band0,gdouble* band1,gdouble* band2,gdouble* band3,gdouble* band4,gdouble* band5,gdouble* band6,gdouble* band7,gdouble* band8,gdouble* band9)
{
	if(!(ly_audio_core->equalizer&&ly_audio_core))
	{
		ly_msg_put("warnning","core:audio","Equalizer Error!");
		return FALSE;
	}
	g_object_get(G_OBJECT(ly_audio_core->equalizer),"band0",band0,NULL);
	g_object_get(G_OBJECT(ly_audio_core->equalizer),"band1",band1,NULL);
	g_object_get(G_OBJECT(ly_audio_core->equalizer),"band2",band2,NULL);
	g_object_get(G_OBJECT(ly_audio_core->equalizer),"band3",band3,NULL);
	g_object_get(G_OBJECT(ly_audio_core->equalizer),"band4",band4,NULL);
	g_object_get(G_OBJECT(ly_audio_core->equalizer),"band5",band5,NULL);
	g_object_get(G_OBJECT(ly_audio_core->equalizer),"band6",band6,NULL);
	g_object_get(G_OBJECT(ly_audio_core->equalizer),"band7",band7,NULL);
	g_object_get(G_OBJECT(ly_audio_core->equalizer),"band8",band8,NULL);
	g_object_get(G_OBJECT(ly_audio_core->equalizer),"band9",band9,NULL);

	return TRUE;

}

/* *
 *  module audio end free resources
 * */

/* *
 * NAME:	ly_func_audio_finalize
 * VARS:	
 * RETN:	[gboolean] 		
 * DESC:	uninitalized module audio.
 * */
gboolean ly_audio_finalize()
{
	ly_global_debug("info","Finalize core module: AUDIO ...\n");
	if(ly_audio_meta)
	{
		ly_conf_set("audio_playing","%d",ly_audio_meta->id);
		ly_db_free_metadata(ly_audio_meta);
	}
	gst_element_set_state(ly_audio_core->play,GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(ly_audio_core->play));
	
	return TRUE;
}

