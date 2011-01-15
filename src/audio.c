#include "audio.h"

/* * 
 *  standard playmodes
 * */

/* *
 * NAME:	ly_func_audio_rand
 * VARS:	listlength[int]	the listlength
 * RETN:	[int] id		
 * DESC:	take out a song id by rand.
 * */
gint ly_func_audio_rand(gint listlength)
{
	return rand()%listlength;
}

/* *
 * NAME:	ly_func_audio_singleloop
 * VARS:	position[int]	the position
 * RETN:	[int] id		
 * DESC:	take out a song id by singleloop.
 * */
gint ly_func_audio_singleloop(gint position)
{
	return position;
}

/* *
 * NAME:	ly_func_audio_nlistloop
 * VARS:	position[int]	the position;listlength[int] the listlength
 * RETN:	[int] id		
 * DESC:	take out a song id by nlistloop.
 * */
gint ly_func_audio_nlistloop(gint position,gint listlength)
{
	int addposition=0;
	addposition=position+1;
	if(addposition>listlength)
	{
		addposition=addposition%listlength;
	}

	return addposition;
}

/* *
 * NAME:	ly_func_audio_plistloop
 * VARS:	position[int]	the position; listlength[int] the listlength;
 * RETN:	[int] id		
 * DESC:	take out a song id by plistloop.
 * */
gint ly_func_audio_plistloop(gint position,gint listlength)
{
	gint addposition=0;
	addposition=position-1;
	if(addposition<1)
	{
		addposition=listlength;
	}

	return addposition;
}

/* *
 *  set playmode
 * */

/* *
 * NAME:	ly_func_audio_setrand
 * VARS:	
 * RETN:	[boolean] 		
 * DESC:	set playmode by rand.
 * */
gboolean ly_func_audio_setrand()
{
	if(!audio_playmod)
	{
		audio_playmod=(ly_type_audio_playmod*)g_malloc(sizeof(ly_type_audio_playmod));
		if(!audio_playmod)
		{
			ly_msg_put("Error","ly_func_audio_setrand","Playmode Error!");
			
			return FALSE;
		}	
	}
	audio_playmod->random=1;
	
	return TRUE;
}

/* *
 * NAME:	ly_func_audio_setsingle
 * VARS:	
 * RETN:	[boolean]		
 * DESC:	set playmode by single.
 * */
gboolean ly_func_audio_setsingle()
{
	if(!audio_playmod)
	{
		audio_playmod=(ly_type_audio_playmod*)g_malloc(sizeof(ly_type_audio_playmod));
		if(!audio_playmod)
		{
			ly_msg_put("Error","ly_func_audio_setrand","Playmode Error!");
			
			return FALSE;
		}	
	}
	audio_playmod->single=1;
	
	return TRUE;
}

/* *
 * NAME:	ly_func_audio_setsingleloop
 * VARS:	
 * RETN:	[boolean]		
 * DESC:	set playmode by singleloop.
 * */
gboolean ly_func_audio_setsingleloop()
{
	if(!audio_playmod)
	{
		audio_playmod=(ly_type_audio_playmod*)g_malloc(sizeof(ly_type_audio_playmod));
		if(!audio_playmod)
		{
			ly_msg_put("Error","ly_func_audio_setrand","Playmode Error!");
			
			return FALSE;
		}	
	}
	audio_playmod->singleloop=1;
	
	return TRUE;
}

/* *
 * NAME:	ly_func_audio_setlist
 * VARS:	
 * RETN:	[boolean]		
 * DESC:	set playmode by list.
 * */
gboolean ly_func_audio_setlist()
{
	if(!audio_playmod)
	{
		audio_playmod=(ly_type_audio_playmod*)g_malloc(sizeof(ly_type_audio_playmod));
		if(!audio_playmod)
		{
			ly_msg_put("Error","ly_func_audio_setrand","Playmode Error!");
			
			return FALSE;
		}	
	}
	audio_playmod->list=1;
	
	return TRUE;
}

/* *
 * NAME:	ly_func_audio_setlistloop
 * VARS:	
 * RETN:	[boolean]		
 * DESC:	set playmode by listloop.
 * */
gboolean ly_func_audio_setlistloop()
{
	if(!audio_playmod)
	{
		audio_playmod=(ly_type_audio_playmod*)g_malloc(sizeof(ly_type_audio_playmod));
		if(!audio_playmod)
		{
			ly_msg_put("Error","ly_func_audio_setrand","Playmode Error!");
			
			return FALSE;
		}	
	}
	audio_playmod->listloop=1;
	
	return TRUE;
}

/* *
 *  callback
 * */

/* *
 * NAME:	ly_func_audio_callback
 * VARS:	bus[GstBus*] the bus;message[GstMessage*] the message;data[gpointer] the data;
 * RETN:	[boolean]		
 * DESC:	call to process.
 * */
gboolean ly_func_audio_callback(GstBus *bus,GstMessage *message,gpointer data)
{
	GstTagList* tags;
	
	switch (message->type)
	{
		case GST_MESSAGE_EOS:
			ly_func_audio_next();
			ly_func_audio_play();
			break;
		case GST_MESSAGE_TAG:
		{
			gchar* genre=NULL;
			
			gst_message_parse_tag(message,&tags);
			
			if(gst_tag_list_get_string(tags,GST_TAG_GENRE,&genre))
			{
				ly_msg_put("Information","ly_func_audio_callback",genre);
			}
			break;
			
		}
		default:
		break;
	}	
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
	/* gl_core create */
	audio_core=(ly_type_audio_core*)g_malloc(sizeof(ly_type_audio_core));
	audio_playmod=(ly_type_audio_playmod*)g_malloc(sizeof(ly_type_audio_playmod));
	GstElement* play=NULL;
	GstElement* equalizer=NULL;
	GstElement* convert=NULL;
	GstElement* spectrum=NULL;
	GstElement* volumn=NULL;
	GstElement* audiosink=NULL;
	GstElement* audio_bin;
	GstPad*     mpad;
	GstBus* bus=NULL;


	/* gl_core init */
	play=gst_element_factory_make("playbin","play");
	bus=gst_pipeline_get_bus(GST_PIPELINE(play));
	gst_element_set_state(play,GST_STATE_NULL);
	gst_bus_add_watch(bus,(GstBusFunc)ly_func_audio_callback,audio_core);
	gst_object_unref(bus);
	equalizer=gst_element_factory_make("equalizer-10bands","equalizer");
	convert=gst_element_factory_make("audioconvert","convert");
	spectrum=gst_element_factory_make("spectrum","spectrum");
	audiosink=gst_element_factory_make("autoaudiosink","audiosink");

	audio_bin=gst_bin_new("audio-bin");
	gst_bin_add_many(GST_BIN(audio_bin),equalizer,convert,spectrum,volumn,audiosink,NULL);
	gst_element_link_many(equalizer,convert,spectrum,volumn,audiosink,NULL);
	mpad=gst_element_add_pad(audio_bin,gst_ghost_pad_new(NULL,mpad));
	g_object_set(G_OBJECT(play),"audio-sink",audio_bin,NULL);
	g_object_set(G_OBJECT(spectrum),"bands",40,"threshold",-100,"message",TRUE,"message-phase",TRUE,NULL);

	audio_core->play=play;
	audio_core->volumn=volumn;
	audio_core->equalizer=equalizer;
	

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
gboolean ly_func_audio_next()
{
	gint next=0;
	gint position=0;
	gint listlength=0;
	position=audio_meta->id;
	listlength=ly_db_get_playlist_count();
	
	
	if(audio_playmod->random==1)
	{
		next=(gint)ly_func_audio_rand((int)listlength);
		audio_meta=(ly_type_audio_playmeta*)ly_db_get_metadata_by_id(next);

		return TRUE;
	}
	if(audio_playmod->singleloop==1)
	{
		next=(gint)ly_func_audio_singleloop((int)position);
		audio_meta=(ly_type_audio_playmeta*)ly_db_get_metadata_by_id(next);

		return TRUE;
	}
	if(audio_playmod->listloop==1)
	{
		next=(gint)ly_func_audio_nlistloop((int)position,(int)listlength);
		audio_meta=(ly_type_audio_playmeta*)ly_db_get_metadata_by_id(next);

		return TRUE;
	}
	if(audio_playmod->list==1)
	{
		next=(gint)ly_func_audio_nlistloop((int)position,(int)listlength);
		if(audio_playmod->count==1&&next==listlength)
		{
			ly_func_audio_stop();
			return FALSE;
		}
		audio_playmod->count=1;
		audio_meta=(ly_type_audio_playmeta*)ly_db_get_metadata_by_id(next);
		return TRUE;


	}
	if(audio_playmod->single==1)
	{
		next=(gint)ly_func_audio_singleloop((int)position);
		if(audio_playmod->count==1)
		{
			ly_func_audio_stop();
			return FALSE;
		}
		audio_playmod->count=1;
		audio_meta=(ly_type_audio_playmeta*)ly_db_get_metadata_by_id(next);
		return TRUE;
	}

	ly_msg_put("Error","ly_func_audio_next","Playmod Error!");

}

/* *
 * NAME:	ly_func_audio_previous
 * VARS:	
 * RETN:	[boolean] 		
 * DESC:	search 4 the previous to play.
 * */
gboolean ly_func_audio_previous()
{	
	gint previous=0;
	gint position=0;
	gint listlength=0;
	position=audio_meta->id;
	listlength=ly_db_get_playlist_count();

	
	if(audio_playmod->random==1)
	{
		previous=(gint)ly_func_audio_rand((int)listlength);
		audio_meta=(ly_type_audio_playmeta*)ly_db_get_metadata_by_id(previous);
		return TRUE;
	}
	if(audio_playmod->single==1)
	{
		previous=(gint)ly_func_audio_singleloop((int)position);
		if(audio_playmod->count==1)
		{
			ly_func_audio_stop();

			return FALSE;
		}
		audio_playmod->count=1;
		audio_meta=(ly_type_audio_playmeta*)ly_db_get_metadata_by_id(previous);
		return TRUE;
	}
	if(audio_playmod->singleloop==1)
	{
		previous=(gint)ly_func_audio_singleloop((int)position);
		audio_meta=(ly_type_audio_playmeta*)ly_db_get_metadata_by_id(previous);
		return TRUE;
	}
	if(audio_playmod->list==1)
	{
		previous=(gint)ly_func_audio_plistloop((int)position,(int)listlength);
		if(audio_playmod->count==1&&previous==listlength)
		{
			ly_func_audio_stop();

			return FALSE;
		}
		audio_playmod->count=1;
		audio_meta=(ly_type_audio_playmeta*)ly_db_get_metadata_by_id(previous);
		return TRUE;
	}
	if(audio_playmod->listloop==1)
	{
		previous=(gint)ly_func_audio_plistloop((int)position,(int)listlength);
		audio_meta=(ly_type_audio_playmeta*)ly_db_get_metadata_by_id(previous);
		return TRUE;
	}

	ly_msg_put("Error","ly_func_audio_next","Playmod Error!");
}

/* *
 *  playstates
 * */

/* *
 * NAME:	ly_func_audio_getstate
 * VARS:	
 * RETN:	[GstState] state		
 * DESC:	ensure the state now.
 * */
GstState ly_func_audio_getstate()
{
	GstState     state;
	GstClockTime timeout=0;
	gst_element_get_state(audio_core->play,&state,NULL,timeout);

	return state;
}

/* *
 * NAME:	ly_func_audio_play
 * VARS:	
 * RETN:	[gboolean] 		
 * DESC:	play a song.
 * */
gboolean ly_func_audio_play()
{
	if(!((audio_meta)||(audio_meta->uri)))
	{
		ly_msg_put("Error","ly_func_audio_play","Song doesn't exist!");
		return FALSE;
	}
	
	if(!g_object_set(G_OBJECT(audio_core->play),"uri",audio_meta->uri,NULL))
	{
			ly_msg_put("Error","ly_func_audio_play","Song can't play!");		
	}
	if(audio_meta->cueflag==1)	//cue play
	{
		if(!ly_func_audio_setposition(0))
		{
			ly_msg_put("Error","ly_func_audio_play","Position not reachable");
		}
	}
	
	if(!gst_element_set_state(audio_core->play,GST_STATE_PLAYING))
	{
		ly_msg_put("Error","ly_func_audio_play","Gstreamer States Error!");
		return FALSE;
	}

	return TRUE;



}

/* *
 * NAME:	ly_func_audio_pause
 * VARS:	
 * RETN:	[gboolean] 		
 * DESC:	pause a song.
 * */
gboolean ly_func_audio_pause()
{
	GstState state;
	state=ly_func_audio_getstate();

	if((state==GST_STATE_PAUSED)||(state==GST_STATE_READY))
	{
		return FALSE;
	}
	if(state==GST_STATE_PLAYING)
	{
		if(!gst_element_set_state(audio_core->play,GST_STATE_PAUSED))
		{
			ly_msg_put("Error","ly_func_audio_pause","Gstreamer States Error!");
			return FALSE;
		}
		return TRUE;
			
	}

	ly_msg_put("Error","ly_func_audio_pause","Unknow Error!");
	return FALSE;


}

/* *
 * NAME:	ly_func_audio_stop
 * VARS:	
 * RETN:	[gboolean] 		
 * DESC:	stop a song.
 * */
gboolean ly_func_audio_stop()
{
	GstState state;
	state=ly_func_audio_getstate();
	
	if((state==GST_STATE_PLAYING)||(state==GST_STATE_PAUSED))
	{
		if(!gst_element_set_state(audio_core->play,GST_STATE_READY))
		{
			ly_msg_put("Error","ly_func_audio_stop","Gstreamer States Error!");
			return FALSE;
		}
		return TRUE;
	}
	if(state==GST_STATE_READY)
	{
		return FALSE;
	}

	ly_msg_put("Error","ly_func_audio_stop","Unknow Error!");
	return FALSE;
}

/* *
 *  volumn
 * */

/* *
 * NAME:	ly_func_audio_setvolumn
 * VARS:	pvolumn[gdouble] the volumn;
 * RETN:	[gboolean] 		
 * DESC:	set volumn.
 * */
gboolean ly_func_audio_setvolumn(gdouble pvolumn)
{
	if(!g_object_set(G_OBJECT(audio_core->volumn),"volumn",pvolumn,NULL))
	{
		ly_msg_put("Error","ly_func_audio_setvolumn","Volumn Set Failure!");
		return FALSE;
	}

	return TRUE;


}

/* *
 * NAME:	ly_func_audio_getvolumn
 * VARS:	
 * RETN:	[gdouble] volumn		
 * DESC:	take out volumn in double.
 * */
gdouble ly_func_audio_getvolumn()
{
	gdouble pvolumn;

	if(!g_object_get(G_OBJECT(audio_core->volumn),"volumn",&pvolumn,NULL))
	{
		ly_msg_put("Error","ly_func_audio_getvolumn","Volumn Get Failure!");
		return 1;
	}
	
	return pvolumn;

}

/* *
 *  seek position
 * */

/* *
 * NAME:	ly_func_audio_setposition
 * VARS:	playperc[gdouble] the position
 * RETN:	[gboolean] 		
 * DESC:	set position to play.
 * */
gboolean ly_func_audio_setposition(gdouble playperc)
{
	
	gint64 position=0;
	gint64 start=0;
	gint64 duration=0;
	gint min=0;
	gint second=0;
	gint mis=0;

	if(!(audio_meta->uri&&audio_meta))
	{
		ly_msg_put("Error","ly_func_audio_setposition","No song!");
		return FALSE;
	}

	sscanf(audio_meta->start,"%d:%d:%d",&min,&second,&mis);
	start=((gint64)((min*60+second)*100+mis)*10000000);
	sscanf(audio_meta->duration,"%d:%d:%d",&min,&second,&mis);
	duration=((gint64)((min*60+second)*100+mis)*10000000);
	position=(duration*playperc)+start;
	if(!gst_element_seek(audio_core->play,1.0,GST_FORMAT_TIME,GST_SEEK_FLAG_FLUSH,GST_SEEK_TYPE_SET,position,GST_SEEK_TYPE_SET,start+duration))
	{
		ly_msg_put("Error","ly_func_audio_setposition","Fail to seek to desired position");

		return FALSE;
	}

	return TRUE;


	
}

/* *
 * NAME:	ly_func_audio_getpositionp
 * VARS:	
 * RETN:	[gdouble] position		
 * DESC:	take out play position in gdouble.
 * */
gdouble ly_func_audio_getpositionp()
{
	gint64 duration=0;
	gint min=0;
	gint second=0;
	gint mis=0;
	
	sscanf(audio_meta->duration,"%d:%d:%d",&min,&second,&mis);
	duration=((gint64)((min*60+second)*100+mis)*10000000);

	
	return (gdouble)(ly_func_audio_getposition()/(gdouble)duration);
}


/* *
 * NAME:	ly_func_audio_getpositioni
 * VARS:	
 * RETN:	[gint] position		
 * DESC:	take out play position in gint.
 * */
gint ly_func_audio_getpositioni()
{
	GstFormat fmt=GST_FORMAT_TIME;
	gint64 start=0;
	gint64 duration=0;
	gint64 position=0;
	gdouble pos=0;
	gint min=0;
	gint second=0;
	gint mis=0;


	if(!(audio_meta->uri&&audio_meta))
	{
		ly_msg_put("Error","ly_func_audio_getposition","No song!");
	}

	sscanf(audio_meta->start,"%d:%d:%d",&min,&second,&mis);
	start=((gint64)((min*60+second)*100+mis)*10000000);
	sscanf(audio_meta->duration,"%d:%d:%d",&min,&second,&mis);
	duration=((gint64)((min*60+second)*100+mis)*10000000);
	if(!gst_element_query_position(audio_core->play,&fmt,&position))
	{
		ly_msg_put("Error","ly_func_audio_getposition","Position Error!");

		return 1;
	}
	if(position<=0)
	{
		ly_msg_put("Error","ly_func_audio_getposition","Position Error!");
	}
	pos=position-start;

	return pos;


}

/* *
 *  equalizer
 * */

/* *
 * NAME:	ly_func_audio_setequalizer
 * VARS:	bands of equalizer
 * RETN:	[gboolean] 		
 * DESC:	set equalizer.
 * */
gboolean ly_func_audio_setequalizer(gdouble band0,gdouble band1,gdouble band2,gdouble band3,gdouble band4,gdouble band5,gdouble band6,gdouble band7,gdouble band8,gdouble band9)
{
	if(!(audio_core->equalizer&&audio_core))
	{
		ly_msg_put("Error","ly_func_audio_setequalizer","Equalizer Error!");
		return FALSE;
	}
	g_object_set(G_OBJECT(audio_core->equalizer),"band0",band0,NULL);
	g_object_set(G_OBJECT(audio_core->equalizer),"band1",band1,NULL);
	g_object_set(G_OBJECT(audio_core->equalizer),"band2",band2,NULL);
	g_object_set(G_OBJECT(audio_core->equalizer),"band3",band3,NULL);
	g_object_set(G_OBJECT(audio_core->equalizer),"band4",band4,NULL);
	g_object_set(G_OBJECT(audio_core->equalizer),"band5",band5,NULL);
	g_object_set(G_OBJECT(audio_core->equalizer),"band6",band6,NULL);
	g_object_set(G_OBJECT(audio_core->equalizer),"band7",band7,NULL);
	g_object_set(G_OBJECT(audio_core->equalizer),"band8",band8,NULL);
	g_object_set(G_OBJECT(audio_core->equalizer),"band9",band9,NULL);

	return TRUE;

}


/* *
 * NAME:	ly_func_audio_getequalizer
 * VARS:	pointer of bands
 * RETN:	[gboolean] 		
 * DESC:	get equalizer.
 * */
gboolean ly_func_audio_getequalizer(gdouble* band0,gdouble* band1,gdouble* band2,gdouble* band3,gdouble* band4,gdouble* band5,gdouble* band6,gdouble* band7,gdouble* band8,gdouble* band9)
{
	if(!(audio_core->equalizer&&audio_core))
	{
		ly_msg_put("Error","ly_func_audio_setequalizer","Equalizer Error!");
		return FALSE;
	}
	g_object_get(G_OBJECT(audio_core->equalizer),"band0",band0,NULL);
	g_object_get(G_OBJECT(audio_core->equalizer),"band1",band1,NULL);
	g_object_get(G_OBJECT(audio_core->equalizer),"band2",band2,NULL);
	g_object_get(G_OBJECT(audio_core->equalizer),"band3",band3,NULL);
	g_object_get(G_OBJECT(audio_core->equalizer),"band4",band4,NULL);
	g_object_get(G_OBJECT(audio_core->equalizer),"band5",band5,NULL);
	g_object_get(G_OBJECT(audio_core->equalizer),"band6",band6,NULL);
	g_object_get(G_OBJECT(audio_core->equalizer),"band7",band7,NULL);
	g_object_get(G_OBJECT(audio_core->equalizer),"band8",band8,NULL);
	g_object_get(G_OBJECT(audio_core->equalizer),"band9",band9,NULL);

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
gboolean ly_func_audio_finalize()
{
	
	g_free(audio_playmod);
	gst_element_set_state(audio_core->play,GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(audio_core->play));
	gst_object_unref(GST_OBJECT(audio_core->equalizer));
	gst_object_unref(GST_OBJECT(audio_core->volumn));

}

