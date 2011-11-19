/*
 * aud.c
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya. If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * HEADERS
 */
#include "aud.h"

/*
 * FUNCTIONS
 */

gboolean		ly_aud_on_ppl_eos_cb(gpointer message, gpointer data);

/**
 * ly_aud_init:
 *
 * Initialize the aud module, it will be called by #ly_cox_init.
 */
void		ly_aud_init()
{
	/*
	 * mode
	 */
	gint random=0;
	gint repeat=1;
	gint single=0;
	
	GstState state;
	state=ly_aud_get_state();
	
	if(!ly_reg_get("aud_mode","%d:%d:%d", &random,&repeat,&single))
		ly_reg_set("aud_mode","%d:%d:%d",random, repeat, single);

	/*
	 * volume
	 */
	double vol=0.3;
	if(!ly_reg_get("aud_volume","%lf",&vol))
	{
		ly_reg_set("aud_volume","%lf",vol);
	}
	GstElement *ele=ly_ppl_audio_get_element("volume");
	if(ele)
	{
		g_object_set(G_OBJECT(ele), "volume", vol, NULL);
	}
	
	/*
	 * autoplay
	 */
	int autoplay=0;
	if(!ly_reg_get("aud_autoplay", "%d", &autoplay))
		ly_reg_set("aud_autoplay", "%d", autoplay);
	
	if(autoplay)
	{
		ly_aud_play();
	}
	
	ly_msg_bind("ppl_eos", "core:ppl", ly_aud_on_ppl_eos_cb, NULL);
}

/**
 * ly_aud_fina:
 *
 * Finalize the aud module, it will be called by #ly_cox_fina.
 */
void		ly_aud_fina()
{
	ly_aud_stop();
}

/* *
 * NAME:	ly_func_audio_next
 * VARS:	
 * RETN:	[gboolean]	
 * DESC:	search 4 next song to play.
 * */
gboolean ly_aud_next()
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
	
	ly_msg_clear();
	GstState state;
	state=ly_aud_get_state();
	ly_aud_stop();

	gint random=0;
	gint repeat=1;
	gint single=0;
	ly_reg_get("aud_mode", "%d:%d:%d", &random, &repeat, &single);
	if(random)
	{
		ly_pqm_set_rand();
	}
	else
	{
		if(single)
		{
			if(repeat)
			{
				state=GST_STATE_READY;
			}
		}
		else
		{
			ly_pqm_set_next();
		}
	}

	ly_aud_play();
	if(state!=GST_STATE_PLAYING)
	{
		ly_aud_pause();
	}
	
	return TRUE;
}

/* *
 * NAME:	ly_func_audio_prev
 * VARS:	
 * RETN:	[boolean] 		
 * DESC:	search 4 the previous to play.
 * */
gboolean ly_aud_prev()
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;

	GstState state;
	state=ly_aud_get_state();
	ly_aud_stop();
	
	gint random=0;
	gint repeat=1;
	gint single=0;
	ly_reg_get("aud_mode","%d:%d:%d", &random,&repeat,&single);

	if(random)
	{
		ly_pqm_set_rand();
	}
	else
	{
		if(single)
		{
			if(!repeat)
			{
				state=GST_STATE_READY;
			}
		}
		else
		{
			
			ly_pqm_set_prev();
		}
	}

	ly_aud_play();
	if(state!=GST_STATE_PLAYING)
	{
		ly_aud_pause();
	}
	return TRUE;
}

/* *
 * NAME:	ly_func_audio_get_state
 * VARS:	
 * RETN:	[GstState] state		
 * DESC:	ensure the state now.
 * */
GstState ly_aud_get_state()
{
	GstState     state;
	GstClockTime timeout=0;
	GstElement *play=ly_ppl_get_playbin();
	if(!play)
		return 0;
	gst_element_get_state(play,&state,NULL,timeout);
	return state;
}

/* *
 * NAME:	ly_func_audio_play
 * VARS:	
 * RETN:	[gboolean] 		
 * DESC:	play a song.
 * */
gboolean ly_aud_play()
{	
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
	GstElement *play=ly_ppl_get_playbin();
	if(!play)
		return FALSE;
	
	g_object_set(G_OBJECT(play),"uri",md->uri,NULL);
	if(md->flag==1)	//cue play
	{
		ly_aud_set_position(0);
	}
	
	GstState state;
	state=ly_aud_get_state();
	
	if(!gst_element_set_state(play,GST_STATE_PLAYING))
	{
		ly_msg_put("file_missed", "core:aud", NULL);
		return FALSE;
	}
	ly_msg_put("play","core:aud", NULL);
	return TRUE;
}

/* *
 * NAME:	ly_func_audio_pause
 * VARS:	
 * RETN:	[gboolean] 		
 * DESC:	pause a song.
 * */
gboolean ly_aud_pause()
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
		GstElement *play=ly_ppl_get_playbin();
	if(!play)
		return FALSE;
		
	GstState state;
	state=ly_aud_get_state();

	if((state!=GST_STATE_PAUSED))
	{
		if(!gst_element_set_state(play,GST_STATE_PAUSED))
		{
			ly_msg_put("warning","core:aud","Gstreamer States Error!");
			return FALSE;
		}
	}
	ly_msg_put("pause","core:aud",NULL);
	return TRUE;
}

/* *
 * NAME:	ly_func_audio_stop
 * VARS:	
 * RETN:	[gboolean] 		
 * DESC:	stop a song.
 * */
gboolean ly_aud_stop()
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
	GstElement *play=ly_ppl_get_playbin();
	if(!play)
		return FALSE;
	
	GstState state;
	state=ly_aud_get_state();
	
	if(!gst_element_set_state(play,GST_STATE_NULL))
	{
		ly_msg_put("warning","core:aud","1Gstreamer States Error!");
		return FALSE;
	}
	if(!gst_element_set_state(play,GST_STATE_READY))
	{
		ly_msg_put("warning","core:aud","2Gstreamer States Error!");
		return FALSE;
	}
	ly_msg_put("stop","core:aud",NULL);
	return TRUE;
}

/* *
 *  volume
 * */

/* *
 * NAME:	ly_aud_setvolume
 * VARS:	volume[gdouble] the volume;
 * RETN:	[gboolean] 		
 * DESC:	set volume.
 * */
gboolean ly_aud_set_volume(gdouble volume)
{
	GstElement *ele=ly_ppl_audio_get_element("volume");
	if(!ele)
		return FALSE;
	g_object_set(G_OBJECT(ele),"volume",volume,NULL);
	ly_reg_set("aud_volume","%lf",volume);

	return TRUE;


}

/* *
 * NAME:	ly_aud_get_volume
 * VARS:	
 * RETN:	[gdouble] volume		
 * DESC:	take out volume in double.
 * */
gdouble ly_aud_get_volume()
{
	gdouble volume;
	GstElement *ele=ly_ppl_audio_get_element("volume");
	if(!ele)
		return FALSE;
	g_object_get(G_OBJECT(ele),"volume",&volume,NULL);
	if(!volume)
	{
		ly_msg_put("error","core:aud",	"Get volume error!");
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
gboolean ly_aud_set_position(gdouble percent)
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
		GstElement *play=ly_ppl_get_playbin();
	if(!play)
		return FALSE;
		
	gint64 position=0;
	gint64 start=0;
	gint64 duration=0;

	duration=ly_mdh_time_str2int(md->duration);
	if(duration<10)
	{
		return FALSE;
	}
	start=ly_mdh_time_str2int(md->start);
	
	position=(duration*percent)+start;
	if(!gst_element_seek(play,1.0,GST_FORMAT_TIME,GST_SEEK_FLAG_FLUSH,GST_SEEK_TYPE_SET,position,GST_SEEK_TYPE_SET,start+duration))
	{
		ly_msg_put("error","core:aud","Fail to seek to desired position");
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
gdouble ly_aud_get_position()
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return 0;
	GstElement *play=ly_ppl_get_playbin();
	if(!play)
		return 0;
		
	gint64 dura=0;
	gint64 pos=0;
	gdouble position;
	
	dura=ly_mdh_time_str2int(md->duration);
	if(dura<=0)
		return 0;
	pos=ly_aud_get_position_abs();
	position=pos/(double)dura;
	return position;
}


/* *
 * NAME:	ly_aud_get_position_abs
 * VARS:	
 * RETN:	[gint] position		
 * DESC:	take out play position in gint.
 * */
gint64 ly_aud_get_position_abs()
{
	GstState state;
	state=ly_aud_get_state();
	if((state!=GST_STATE_PLAYING)&&(state!=GST_STATE_PAUSED))
		return 0;
	
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return 0;
	GstElement *play=ly_ppl_get_playbin();
	if(!play)
		return 0;
		
	GstFormat fmt=GST_FORMAT_TIME;
	gint64 start=0;
	gint64 dura=0;
	gint64 pos=0;
	
	start=ly_mdh_time_str2int(md->start);
	dura=ly_mdh_time_str2int(md->duration);
	if(dura<=0)
		return 0;
	
	if(!gst_element_query_position(play, &fmt, &pos))
	{
		ly_msg_put("warning","core:aud", "Position Error!");
		return 0;
	}
	if(pos-start<-60000000000||dura-pos<-60000000000)
	{
		ly_msg_put("warning","core:aud", "Position Error!");
		return 0;
	}
	pos=pos-start;
	if(pos<start)
		pos=start;
	else if(pos>dura)
		pos=dura;
	
	return pos;
}

gboolean		ly_aud_on_ppl_eos_cb(gpointer message, gpointer data)
{
	ly_aud_next();
	return FALSE;
}
