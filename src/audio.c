#include "audio.h"

/* * 
 *  standard playmodes
 * */

int ly_func_audio_rand(int listlength)
{
	return rand()%listlength;
}

int ly_func_audio_singleloop(int position)
{
	return position;
}

int ly_func_audio_nlistloop(int position,int listlength)
{
	int addpositon=0;
	addposition=position+1;
	if(addposition>listlength)
	{
		addposition=addposition%listlength;
	}

	return addposition;
}

int ly_func_audio_plistloop(int position,int listlength)
{
	int addposition=0;
	addposition=position-1;
	if(addposition<1)
	{
		addposition=listlength;
	}

	return addposition;
}

/* *
 *  callback
 * */

gboolean ly_func_audio_callback()
{
	
}

/* *
 *  module audio init
 * */
gboolean ly_audio_init()
{
	/* gl_core create */
	audio_core=(ly_type_audio_core*)g_malloc(sizeof(ly_type_audio_core));
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
	bus=gst_popeline_get_bus(GST_PIPELINE(play));
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

gboolean ly_func_audio_next()
{
	gint next=0;
	gint position=0;
	gint listlength=0;
	position=audio_meta->id;
	listlength=      ;
	
	
	if(audio_playmod->random==1)
	{
		next=(gint)ly_func_audio_rand((int)listlength);

		return TRUE;
	}
	if(audio_playmod->singleloop==1)
	{
		next=(gint)ly_func_audio_singleloop((int)position);

		return TRUE;
	}
	if(audio_playmod->listloop==1)
	{
		next=(gint)ly_func_audio_nlistloop((int)position,(int)listlength);

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

		return TRUE;
	}

	ly_msg_put("Error","ly_func_audio_next","Playmod Error!");

}

gboolean ly_func_audio_previous()
{	
	gint previous=0;
	gint position=0;
	gint listlength=0;
	position=audio_meta->id;
	listlength=      ;

	
	if(audio_playmod->random==1)
	{
		previous=(gint)ly_func_audio_rand((int)listlength);

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

		return TRUE;
	}
	if(audio_playmod->singleloop==1)
	{
		previous=(gint)ly_func_audio_singleloop((int)position);
		return TRUE;
	}
	if(audio_playmod->list==1)
	{
		previous=(gint)ly_func_audio_plistloop((int)position,(int)listlength);
		if(audio_playmod->count==1&&next==listlength)
		{
			ly_func_audio_stop();

			return FALSE;
		}
		audio_playmod->count=1;

		return TRUE;
	}
	if(audio_playmod->listloop==1)
	{
		previous=(gint)ly_func_audio_plistloop((int)position,(int)listlength);

		return TRUE;
	}

	ly_msg_put("Error","ly_func_audio_next","Playmod Error!");
}

/* *
 *  playstates
 * */

GstState ly_func_audio_getstate()
{
	GstState     state;
	GstClockTime timeout=0;
	gst_element_get_state(audio_core->play,&state,NULL,timeout);

	return state;
}

gboolean ly_func_audio_play()
{
	if(!((audio_meta)||(audio_meta->uri)))
	{
		ly_msg_put("Error","ly_func_audio_play","Song doesn't exist!");
		return FALSE;
	}
	if(audio_meta->cueflag==0)	//normal play
	{
		if(!g_object_set(audio_core->play),"uri",audio_meta->uri,NULL)
		{
			ly_msg_put("Error","ly_func_audio_play","Song can't play!");
		}
	}
	if(audio_meta->cueflag==1)	//cue play
	{
		
	}
	if(!gst_element_set_state(ly_type_audio_core->play,GST_STATE_PLAYING))
	{
		ly_msg_put("Error","ly_func_audio_play","Gstreamer States Error!");
		return FALSE;
	}

	return TRUE;



}

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
		if(!gst_element_set_state(ly_type_audio_core->play,GST_STATE_PAUSED))
		{
			ly_msg_put("Error","ly_func_audio_pause","Gstreamer States Error!");
			return FALSE;
		}
		return TRUE;
			
	}

	ly_msg_put("Error","ly_func_audio_pause","Unknow Error!");
	return FALSE;


}

gboolean ly_func_audio_stop()
{
	GstState state;
	state=ly_func_audio_getstate();
	
	if((state==GST_STATE_PLAYING)||(state==GST_STATE_PAUSED))
	{
		if(!gst_element_set_state(ly_type_audio_core->play,GST_STATE_READY))
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

gboolean ly_func_audio_setvolumn(gdouble pvolumn)
{
	if(!g_object_set(G_OBJECT(ly_type_audio_core->volumn),"volumn",pvolumn,NULL))
	{
		ly_msg_put("Error","ly_func_audio_setvolumn","Volumn Set Failure!");
		return FALSE;
	}

	return TRUE;


}

gdouble ly_func_audio_getvolumn()
{
	gdouble pvolumn;

	if(!g_object_get(G_OBJECT(ly_type_audio_core->volumn),"volumn",&pvolumn,NULL))
	{
		ly_msg_put("Error","ly_func_audio_getvolumn","Volumn Get Failure!");
		return 1;
	}
	
	return pvolumn;

}

/* *
 *  seek position
 * */
gboolean ly_func_audio_seekposition()
{
	
}


/* *
 *  equalizer
 * */

gboolean ly_func_audio_setequalizer()
{
	
}

gboolean ly_func_audio_getequalizer()
{
	
}

/* *
 *  module audio end free resources
 * */
gboolean ly_func_audio_finalize()
{
	
}

