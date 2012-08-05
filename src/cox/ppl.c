/*
 * ppl.c
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
#include "ppl.h"

/*
 * VARIABLES
 */
GstElement	*ly_ppl_playbin=NULL;
GstElement	*ly_ppl_audio_bin=NULL;
GstElement	*ly_ppl_video_bin=NULL;

/*
 * FUNCTIONS
 */

gboolean ly_ppl_bus_cb(GstBus *bus,GstMessage *message,gpointer data);

void		ly_ppl_init		()
{
	GstElement *playbin=NULL;
	GstElement *equalizer=NULL;
	GstElement *convert=NULL;
	GstElement *volume=NULL;
	GstElement *audiosink=NULL;
	GstElement *videosink=NULL;
	GstElement *defaultsrc=NULL;
	GstElement *fakesink=NULL;
	GstElement *outselector=NULL;
	GstElement *inselector=NULL;
	GstPad*     mpad;
	GstBus* bus=NULL;

	playbin=gst_element_factory_make("playbin", "playbin");
	equalizer=gst_element_factory_make("equalizer-10bands","equalizer");
	volume= gst_element_factory_make("volume","volume");
	convert=gst_element_factory_make("audioconvert","autoconvert");
	audiosink=gst_element_factory_make("autoaudiosink","autoaudiosink");
	videosink=gst_element_factory_make("ximagesink","autovideosink");
	fakesink=gst_element_factory_make("fakesink","fakesink");
	defaultsrc=gst_element_factory_make("filesrc", "defaultsrc");
	inselector=gst_element_factory_make("input-selector", "inselector");
	outselector=gst_element_factory_make("output-selector", "outselector");

	bus=gst_pipeline_get_bus(GST_PIPELINE(playbin));
	gst_element_set_state(playbin,GST_STATE_NULL);
	gst_bus_add_watch(bus,(GstBusFunc)ly_ppl_bus_cb, NULL);
	gst_object_unref(bus);

	ly_ppl_audio_bin=gst_bin_new("audio-bin");
	gst_bin_add_many(GST_BIN(ly_ppl_audio_bin),equalizer,convert,volume,audiosink,NULL);
	gst_element_link_many(equalizer,convert,volume,audiosink,NULL);
	mpad = gst_element_get_static_pad(equalizer, "sink");
	gst_element_add_pad(ly_ppl_audio_bin, gst_ghost_pad_new(NULL,mpad));
	g_object_set(G_OBJECT(playbin),"audio-sink",ly_ppl_audio_bin,NULL);

	ly_ppl_video_bin=gst_bin_new("video-bin");
	gst_bin_add_many(GST_BIN(ly_ppl_video_bin), videosink, NULL);
	mpad = gst_element_get_static_pad (videosink, "sink");
	gst_element_add_pad(ly_ppl_video_bin, gst_ghost_pad_new(NULL,mpad));
	g_object_set(G_OBJECT(playbin),"video-sink",ly_ppl_video_bin,NULL);
	gst_x_overlay_set_window_handle(GST_X_OVERLAY(videosink), 0);
	g_object_set(G_OBJECT(videosink),"force-aspect-ratio", TRUE, NULL);
//	g_object_set(G_OBJECT(outselector), "active-pad", gst_element_get_static_pad(fakesink, "pad_fake"), NULL);

	ly_ppl_playbin=playbin;
}
void		ly_ppl_fina		()
{
}
GstElement*			ly_ppl_get_playbin				()
{
	return ly_ppl_playbin;
}

GstElement*			ly_ppl_audio_get_element		(char *name)
{
	GstElement *ele=NULL;
	ele=gst_bin_get_by_name(GST_BIN(ly_ppl_audio_bin), name);
	return ele;
}
GstElement*			ly_ppl_video_get_element		(char *name)
{
	GstElement *ele=NULL;
	ele=gst_bin_get_by_name(GST_BIN(ly_ppl_video_bin), name);
	return ele;
}

gboolean			ly_ppl_video_set_screen			(guintptr handle)
{
	GstElement *videosink;
//	GstElement *outselector;
//	outselector=ly_ppl_video_get_element("outselector");
	videosink=ly_ppl_video_get_element("autovideosink");
	gst_x_overlay_set_window_handle(GST_X_OVERLAY(videosink), handle);
/*	if(handle==0)
	{
		GstElement *fakesink;
		fakesink=ly_ppl_video_get_element("fakesink");
		g_object_set(G_OBJECT(outselector), "active-pad", gst_element_get_static_pad(fakesink, "pad_fake"), NULL);
	}
	else
	{
		g_object_set(G_OBJECT(outselector), "active-pad", gst_element_get_static_pad(videosink, "pad_video"), NULL);
	}*/
	return TRUE;
}

gboolean ly_ppl_bus_cb(GstBus *bus,GstMessage *message,gpointer data)
{
	char *tag_codec=NULL;
//	char *tag_title=NULL;
//	char *tag_artist=NULL;
//	char *tag_album=NULL;
	char *tag_comment=NULL;
	char *tag_genre=NULL;
	char *tag_lrc=NULL;
	guint tag_bitrate=0;
	guint tag_track=0;
	GstBuffer *tag_cover=NULL;
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return TRUE;

	GstTagList* tags;

	switch (message->type)
	{
		case GST_MESSAGE_EOS:
			ly_mbs_put("ppl_eos", "core:ppl", NULL);
			break;
		case GST_MESSAGE_TAG:
		{
			gst_message_parse_tag(message,&tags);

			//codec
			if(gst_tag_list_get_string(tags,GST_TAG_AUDIO_CODEC,&tag_codec))
			{
				if(tag_codec!=NULL)
					g_strlcpy(md->codec, tag_codec, sizeof(md->codec));
				ly_mbs_put("meta_update", "core:ppl", "codec");
			}
			//comment
			if(gst_tag_list_get_string(tags,GST_TAG_COMMENT,&tag_comment))
			{
				if(g_utf8_validate(tag_comment,-1,NULL))
				{
					g_utf8_strncpy((char *)(md->comment), tag_comment, sizeof(md->comment));
				}
				g_free(tag_comment);
				ly_mbs_put("meta_update", "core:ppl", "comment");
			}
			//bitrate
			if(gst_tag_list_get_uint(tags, GST_TAG_BITRATE, &tag_bitrate))
			{
				md->bitrate = tag_bitrate;
				ly_mbs_put("meta_update", "core:ppl", "bitrate");
			}
			//track_number
			if(gst_tag_list_get_uint(tags, GST_TAG_TRACK_NUMBER, &tag_track))
			{
				md->track = tag_track;
				ly_mbs_put("meta_update", "core:ppl", "track");
			}
			//genre
			if(gst_tag_list_get_string(tags,GST_TAG_GENRE, &tag_genre))
			{
				if(g_utf8_validate(tag_genre,-1,NULL))
				{
					g_utf8_strncpy((gchar *)(md->genre),tag_genre, sizeof(md->genre));
				}
				g_free(tag_genre);
				ly_mbs_put("meta_update", "core:ppl", "genre");
			}
			//album cover
 			if(gst_tag_list_get_buffer(tags,GST_TAG_IMAGE, &tag_cover))
 			{
 				if(md->cover)
 					gst_buffer_unref(md->cover);
 				md->cover=tag_cover;
 				ly_mbs_put("meta_update", "core:ppl", "cover");
			}
			//lyrics
			if(gst_tag_list_get_string(tags,GST_TAG_LYRICS, &tag_lrc))
			{
				if(g_utf8_validate(tag_lrc,-1,NULL))
				{
					g_utf8_strncpy((gchar *)(md->lrc), tag_lrc, sizeof(md->lrc));
				}
				g_free(tag_lrc);
				ly_mbs_put("meta_update", "core:ppl", "lrc");
			}
			break;
		}
		default:
		break;
	}
	return TRUE;
}
