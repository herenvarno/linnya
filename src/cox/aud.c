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
	ly_mbs_put("meta_changed", "core:aud", NULL);
	ly_mbs_bind("ppl_eos", "core:ppl", ly_aud_on_ppl_eos_cb, NULL);
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

/**
 * ly_aud_next:
 *
 * play the next song.
 *
 * Returns:	TRUE for success, others FALSE.
 */
gboolean ly_aud_next()
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
	if(state==GST_STATE_PLAYING)
	{
		ly_aud_play();
	}
	return TRUE;
}

/**
 * ly_aud_prev:
 *
 * play the prev song.
 *
 * Returns:	TRUE for success, others FALSE.
 */
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

	if(state==GST_STATE_PLAYING)
	{
		ly_aud_play();
	}
	return TRUE;
}

/**
 * ly_aud_get_state:
 *
 * Get the state of playbin.
 *
 * Returns:	The GStreamer state of playbin.
 */
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

/**
 * ly_aud_play:
 *
 * Set pipeline to be PLAYING.
 *
 * Returns:	TRUE for success, others FALSE.
 */
gboolean ly_aud_play()
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
	GstElement *play=ly_ppl_get_playbin();
	if(!play)
		return FALSE;
	
	GstState state;
	state=ly_aud_get_state();
	if(state!=GST_STATE_PAUSED)
	{
		g_object_set(G_OBJECT(play),"uri",md->uri,NULL);
		if(!gst_element_set_state(play,GST_STATE_PLAYING))
		{
			ly_mbs_put("file_missed", "core:aud", NULL);
			return FALSE;
		}
		if(md->flag==1)	//cue play
		{
			g_usleep(0.1*G_USEC_PER_SEC);
			ly_aud_set_position(0);
		}
	}
	else
	{
		if(!gst_element_set_state(play,GST_STATE_PLAYING))
		{
			ly_mbs_put("file_missed", "core:aud", NULL);
			return FALSE;
		}
	}
	ly_mbs_put("play", "core:aud", NULL);
	return TRUE;
}

/**
 * ly_aud_pause:
 *
 * Set pipeline to be PAUSED.
 *
 * Returns:	TRUE for success, others FALSE.
 */
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
			ly_log_put_with_flag(G_LOG_LEVEL_WARNING, _("Gstreamer state wrong!"));
			return FALSE;
		}
	}
	ly_mbs_put("pause","core:aud",NULL);
	return TRUE;
}

/**
 * ly_aud_stop:
 *
 * Set pipeline to be STOPED.
 *
 * Returns:	TRUE for success, others FALSE.
 */
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
		ly_log_put_with_flag(G_LOG_LEVEL_WARNING, _("Gstreamer state wrong!"));
		return FALSE;
	}
	if(!gst_element_set_state(play,GST_STATE_READY))
	{
		ly_log_put_with_flag(G_LOG_LEVEL_WARNING, _("Gstreamer state wrong!"));
		return FALSE;
	}
	return TRUE;
}


/**
 * ly_aud_set_volume:
 *
 * @volume: The gdouble format of volume between 0 and 1.
 *
 * Set volume.
 *
 * Returns:	TRUE for success, others FALSE.
 */
gboolean ly_aud_set_volume(gdouble volume)
{
	GstElement *ele=ly_ppl_audio_get_element("volume");
	if(!ele)
		return FALSE;
	g_object_set(G_OBJECT(ele),"volume",volume,NULL);
	ly_reg_set("aud_volume","%lf",volume);

	return TRUE;


}

/**
 * ly_aud_get_volume:
 *
 * Get volume.
 *
 * Returns:	The gdouble format volume value.
 */
gdouble ly_aud_get_volume()
{
	gdouble volume;
	GstElement *ele=ly_ppl_audio_get_element("volume");
	if(!ele)
		return FALSE;
	g_object_get(G_OBJECT(ele),"volume",&volume,NULL);
	if(!volume)
	{
		ly_log_put_with_flag(G_LOG_LEVEL_WARNING, _("Volume wrong!"));
	}
	return volume;
}

/**
 * ly_aud_set_position:
 * @percent: The desire position you want to seek, it should be between 0 and 1.
 *
 * Seek.
 *
 * Returns:	TRUE for success, others FALSE.
 */
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
		g_critical(_("Cannot seek to desired position!"));
		return FALSE;
	}

	return TRUE;	
}

/**
 * ly_aud_get_position:
 *
 * Get the current postion in gdouble format.
 *
 * Returns:	The current position of pipeline.
 */
gdouble ly_aud_get_position()
{
	gint64 pos=0;
	pos=ly_aud_get_position_abs();
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return 0;
	GstElement *play=ly_ppl_get_playbin();
	if(!play)
		return 0;
		
	gint64 dura=0;
	gdouble position;
	
	dura=ly_mdh_time_str2int(md->duration);
	if(dura<=0)
		return 0;
	
	position=pos/(double)dura;
	return position;
}


/**
 * ly_aud_get_position_abs:
 *
 * Get the current position by GStreamer inner clock which is in a format of gint64. 
 *
 * Returns: The abusolute position time.
 */
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
	{
		gst_element_query_duration(play, &fmt, &dura);
		dura-=start;
		gchar *str=ly_mdh_time_int2str(dura);
		gchar sql[128]="";
		g_snprintf(sql, sizeof(sql), "UPDATE metadatas SET duration='%s' WHERE id=%d", str, md->id);
		g_free(str);
		ly_dbm_exec(sql, NULL, NULL);
		ly_pqm_set_current_md(md->id);
	}
	
	if(!gst_element_query_position(play, &fmt, &pos))
	{
		ly_log_put_with_flag(G_LOG_LEVEL_DEBUG, _("Position wrong!"));
		return 0;
	}
	if(pos-start<-60000000000)
	{
		ly_log_put_with_flag(G_LOG_LEVEL_DEBUG, _("Position wrong!"));
		return 0;
	}
	pos=pos-start;
	if(pos<0)
		pos=0;
	else if(pos>dura)
		pos=dura;
	return pos;
}

/**
 * ly_aud_on_ppl_eos_cb:
 *
 * A callback function when pipeline EOS message occurs. 
 *
 * Returns: FALSE
 */
gboolean		ly_aud_on_ppl_eos_cb(gpointer message, gpointer data)
{
	ly_aud_next();
	return FALSE;
}
