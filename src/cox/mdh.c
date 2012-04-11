/*
 * mdh.c
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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
#include "mdh.h"

/*
 * VARIABLES
 */
GstElement *ly_mdh_pipeline;
gboolean ly_mdh_md_eos;



/*
 * FUNCTIONS
 */

gboolean	ly_mdh_new_with_uri_bus_cb	(GstBus *bus, GstMessage *message, gpointer data);
gboolean	ly_mdh_new_with_uri_full_bus_cb	(GstBus *bus, GstMessage *message, gpointer data);
void		ly_mdh_new_with_uri_loop_cb	(LyMdhMetadata *md, GstElement *element, gboolean block);
void		ly_mdh_new_with_uri_full_loop_cb	(LyMdhMetadata *md, GstElement *element, gboolean block);
void		ly_mdh_new_with_uri_pipe_cb	(GstElement *decodebin,GstPad *pad, gboolean last, gpointer data);

/**
 * ly_mdh_init:
 *
 * Initialize the mdh module, it will be called by #ly_cox_init
 */
void			ly_mdh_init				()
{
	ly_log_put(_("[info] Init COX module: MDH"));

	char extra_encoding[1024]="GB18030";
	if(!ly_reg_get("dbm_extra_encoding", "%*[^\n(](%1023[^\n)]", extra_encoding))
	{
		ly_reg_set("dbm_extra_encoding", "Chinese Simplified (GB18030)");
	}
	char encoding[1024]="";
	g_snprintf(encoding, sizeof(encoding), "%s:UTF-8", extra_encoding);
	g_setenv("GST_ID3_TAG_ENCODING", encoding, TRUE);
}
/**
 * ly_mdh_fina:
 *
 * Finalize the mdh module, it will be called by #ly_cox_fina
 */
void			ly_mdh_fina				()
{
	ly_log_put(_("[info] Fina COX module: MDH"));
}

/**
 * ly_gla_fina:
 *
 * Create a new allocate LyMdhMetadata type structure.
 *
 * Returns:		a newly allocated metadata struct.
 */
LyMdhMetadata*	ly_mdh_new				()
{
	LyMdhMetadata *md = NULL;
	md=(LyMdhMetadata*)g_malloc(sizeof(LyMdhMetadata));
	if(!md)
		return NULL;

	md->id=0;
	md->playing=0;
	md->num=0;
	md->track=0;
	md->bitrate=0;
	md->flag=0;
	md->cover=NULL;
	g_strlcpy(md->title, "unknown", sizeof(md->title));
	g_strlcpy(md->artist, "unknown", sizeof(md->artist));
	g_strlcpy(md->album, "unknown", sizeof(md->album));
	g_strlcpy(md->comment, "unknown", sizeof(md->comment));
	g_strlcpy(md->genre, "unknown", sizeof(md->genre));
	g_strlcpy(md->codec, "unknown", sizeof(md->codec));
	g_strlcpy(md->start, "00:00.00", sizeof(md->start));
	g_strlcpy(md->duration, "00:00.00", sizeof(md->duration));
	g_strlcpy(md->uri, "unknown", sizeof(md->uri));

	return md;
}

/**
 * ly_gla_fina:
 * @md:			the metadata struct to be free.
 *
 * Free a LyMdhMetadata type structure.
 */
void			ly_mdh_free				(LyMdhMetadata* md)
{
	if(!md)
		return;
	if(md->cover)
	{
		gst_buffer_unref(md->cover);
	}
	md->cover=NULL;
	g_free(md);
}

/**
 * ly_gla_fina:
 *
 * Create a new allocate LyMdhMetadata type structure based on a uri. It can only
 * fill the title, artist and album fields in metadata struct.
 *
 * Returns:		a newly allocated metadata struct.
 */
LyMdhMetadata*	ly_mdh_new_with_uri		(char *uri)
{
	/*
	 * test file
	 */
	char *prefix=ly_gla_uri_get_prefix(uri);
	char *path=ly_gla_uri_get_path(uri);
	if(!prefix || !path)
		return NULL;
	
	if(!g_str_equal(prefix, "file://"))
	{
		g_free(prefix);
		ly_log_put(_("[warning] Cannot open file: %s. unsupported protocol!"), uri);
		return NULL;
	}
	g_free(prefix);
	
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		g_free(path);
		ly_log_put(_("[warning] Cannot open file: %s. file not found!"), uri);
		return NULL;
	}
	g_free(path);

	ly_mdh_pipeline=NULL;
	ly_mdh_md_eos=FALSE;

	/*
	 * create and initial metadata
	 */
	LyMdhMetadata *metadata=ly_mdh_new();
	g_strlcpy(metadata->uri, uri, sizeof(metadata->uri));

	/*
	 * build the pipeline
	 */
	GstFormat fmt=GST_FORMAT_TIME;
	GstElement *urisrc;
	GstElement *decodebin;
	GstElement *fakesink;
	GstBus *bus=NULL;
	gint changeTimeout = 0;
	GstStateChangeReturn rt;
	GstMessage *msg;

	ly_mdh_pipeline=gst_pipeline_new("pipeline");
	urisrc=gst_element_make_from_uri(GST_URI_SRC, uri,"urisrc");
	decodebin=gst_element_factory_make("decodebin","decodebin");
	fakesink=gst_element_factory_make("fakesink","fakesink");

	gst_bin_add_many(GST_BIN(ly_mdh_pipeline),urisrc,decodebin,fakesink,NULL);
	gst_element_link(urisrc,decodebin);

	g_signal_connect_object(G_OBJECT(decodebin),"new-decoded-pad",G_CALLBACK(ly_mdh_new_with_uri_pipe_cb),fakesink,0);

	bus = gst_pipeline_get_bus(GST_PIPELINE(ly_mdh_pipeline));
	gst_element_set_state(ly_mdh_pipeline,GST_STATE_NULL);
	gst_element_set_state(ly_mdh_pipeline,GST_STATE_READY);
	rt = gst_element_set_state(ly_mdh_pipeline,GST_STATE_PAUSED);
	if(!rt)
	{
		if(ly_mdh_pipeline!=NULL)
			gst_object_unref(GST_OBJECT(ly_mdh_pipeline));
		ly_mdh_md_eos=FALSE;
		ly_mdh_pipeline=NULL;
		return FALSE;
	}
	while(rt==GST_STATE_CHANGE_ASYNC && !ly_mdh_md_eos && changeTimeout < 5)
	{
		msg = gst_bus_timed_pop(bus, 1 * GST_SECOND);
		if(msg!=NULL)
		{
			ly_mdh_new_with_uri_bus_cb(bus, msg, metadata);
			gst_message_unref(msg);
			changeTimeout = 0;
		}
		else
			changeTimeout++;
		rt = gst_element_get_state(ly_mdh_pipeline, NULL, NULL, 0);
	}
	gst_object_unref(bus);
	ly_mdh_new_with_uri_loop_cb(metadata, ly_mdh_pipeline, FALSE);
	if(rt!=GST_STATE_CHANGE_SUCCESS)
	{
		gst_element_set_state(ly_mdh_pipeline,GST_STATE_NULL);
		return FALSE;
	}

	/*
	 * get duration
	 */
	gint64 dura=0;
	gst_element_query_duration(ly_mdh_pipeline,&fmt,&dura);
	char *duration=ly_mdh_time_int2str(dura);
	g_strlcpy(metadata->duration,duration,sizeof(metadata->duration));
	g_free(duration);

	rt = gst_element_set_state(ly_mdh_pipeline,GST_STATE_NULL);
	gst_object_unref(ly_mdh_pipeline);

	char *str;
	if(g_str_equal(metadata->title,"")||g_str_equal(metadata->title,"unknown"))
	{
		str=ly_gla_uri_get_filename(uri);
		g_strlcpy(metadata->title,str,sizeof(metadata->title));
		g_free(str);
	}

	ly_mdh_pipeline=NULL;
	ly_mdh_md_eos=FALSE;
	return metadata;
}

/**
 * ly_gla_fina:
 *
 * Create a new allocate LyMdhMetadata type structure based on a uri. It fills
 * all fields in metadata struct.
 *
 * Returns:		a newly allocated metadata struct.
 */
LyMdhMetadata*	ly_mdh_new_with_uri_full	(char *uri)
{
	/*
	 * test file
	 */
	char *prefix=ly_gla_uri_get_prefix(uri);
	char *path=ly_gla_uri_get_path(uri);
	if(!g_str_equal(prefix, "file://"))
	{
		g_free(prefix);
		ly_log_put(_("[warning] Cannot open file: %s. unsupported protocol!"), uri);
		return NULL;
	}
	g_free(prefix);
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		g_free(path);
		ly_log_put(_("[warning] Cannot open file: %s. file not found!"), uri);
		return NULL;
	}
	g_free(path);

	ly_mdh_pipeline=NULL;
	ly_mdh_md_eos=FALSE;

	/*
	 * create and initial metadata
	 */
	LyMdhMetadata *metadata=ly_mdh_new();
	g_strlcpy(metadata->uri, uri, sizeof(metadata->uri));

	/*
	 * build the pipeline
	 */
	GstFormat fmt=GST_FORMAT_TIME;
	GstElement *urisrc;
	GstElement *decodebin;
	GstElement *fakesink;
	GstBus *bus=NULL;
	gint changeTimeout = 0;
	GstStateChangeReturn rt;
	GstMessage *msg;

	ly_mdh_pipeline=gst_pipeline_new("pipeline");
	urisrc=gst_element_make_from_uri(GST_URI_SRC,metadata->uri,"urisrc");
	decodebin=gst_element_factory_make("decodebin","decodebin");
	fakesink=gst_element_factory_make("fakesink","fakesink");

	gst_bin_add_many(GST_BIN(ly_mdh_pipeline),urisrc,decodebin,fakesink,NULL);
	gst_element_link(urisrc,decodebin);

	g_signal_connect_object(G_OBJECT(decodebin),"new-decoded-pad",G_CALLBACK(ly_mdh_new_with_uri_pipe_cb),fakesink,0);

	bus = gst_pipeline_get_bus(GST_PIPELINE(ly_mdh_pipeline));
	gst_element_set_state(ly_mdh_pipeline,GST_STATE_NULL);
	gst_element_set_state(ly_mdh_pipeline,GST_STATE_READY);
	rt = gst_element_set_state(ly_mdh_pipeline,GST_STATE_PAUSED);
	if(!rt)
	{
		if(ly_mdh_pipeline!=NULL)
			gst_object_unref(GST_OBJECT(ly_mdh_pipeline));
		ly_mdh_md_eos=FALSE;
		ly_mdh_pipeline=NULL;
		return FALSE;
	}
	while(rt==GST_STATE_CHANGE_ASYNC && !ly_mdh_md_eos && changeTimeout < 5)
	{
		msg = gst_bus_timed_pop(bus, 1 * GST_SECOND);
		if(msg!=NULL)
		{
			ly_mdh_new_with_uri_full_bus_cb(bus, msg, metadata);
			gst_message_unref(msg);
			changeTimeout = 0;
		}
		else
			changeTimeout++;
		rt = gst_element_get_state(ly_mdh_pipeline, NULL, NULL, 0);
	}
	gst_object_unref(bus);
	ly_mdh_new_with_uri_full_loop_cb(metadata, ly_mdh_pipeline, FALSE);
	if(rt!=GST_STATE_CHANGE_SUCCESS)
	{
		gst_element_set_state(ly_mdh_pipeline,GST_STATE_NULL);
		return FALSE;
	}

	/*
	 * get duration
	 */
	gint64 dura=0;
	gst_element_query_duration(ly_mdh_pipeline,&fmt,&dura);
	char *duration=ly_mdh_time_int2str(dura);
	g_strlcpy(metadata->duration,duration,sizeof(metadata->duration));
	g_free(duration);

	rt = gst_element_set_state(ly_mdh_pipeline,GST_STATE_NULL);
	gst_object_unref(ly_mdh_pipeline);

	char *str;
	if(g_str_equal(metadata->title,"")||g_str_equal(metadata->title,"unknown"))
	{
		str=ly_gla_uri_get_filename(uri);
		g_strlcpy(metadata->title,str,sizeof(metadata->title));
		g_free(str);
	}

	ly_mdh_pipeline=NULL;
	ly_mdh_md_eos=FALSE;
	return metadata;
}

gboolean	ly_mdh_new_with_uri_bus_cb	(GstBus *bus, GstMessage *message, gpointer data)
{
	LyMdhMetadata *md=data;

	char *tag_title=NULL;
	char *tag_artist=NULL;
	char *tag_album=NULL;

	if(md==NULL) return TRUE;
	if(md->uri==NULL) return TRUE;

	switch(GST_MESSAGE_TYPE(message))
	{
		case GST_MESSAGE_EOS:
		{
			ly_mdh_md_eos=TRUE;
			return TRUE;
		}
		case GST_MESSAGE_ERROR:
		{
			ly_mdh_md_eos=TRUE;
			return TRUE;
		}
		case GST_MESSAGE_TAG:
		{
			GstTagList *tags;
			gst_message_parse_tag(message,&tags);
			//title
			if(gst_tag_list_get_string(tags,GST_TAG_TITLE,&tag_title))
			{
				if(g_utf8_validate(tag_title,-1,NULL))
				{
					g_utf8_strncpy((char *)(md->title),tag_title, sizeof(md->title));
				}
				g_free(tag_title);
			}
			//artist
			if(gst_tag_list_get_string(tags,GST_TAG_ARTIST,&tag_artist))
			{
				if(g_utf8_validate(tag_artist,-1,NULL))
				{
					g_utf8_strncpy((char *)(md->artist), tag_artist, sizeof(md->artist));
				}
				g_free(tag_artist);
			}
			//album
			if(gst_tag_list_get_string(tags,GST_TAG_ALBUM,&tag_album))
			{
				if(g_utf8_validate(tag_album,-1,NULL))
				{
					g_utf8_strncpy((char *)(md->album), tag_album, sizeof(md->album));
				}
				g_free(tag_album);
			}
			gst_tag_list_free(tags);
			return TRUE;
			break;
		}
		case GST_MESSAGE_ELEMENT:
			break;
		default:
			break;
	}
	return FALSE;
}

gboolean	ly_mdh_new_with_uri_full_bus_cb	(GstBus *bus, GstMessage *message, gpointer data)
{
	LyMdhMetadata *md=data;

	char *tag_codec=NULL;
	char *tag_title=NULL;
	char *tag_artist=NULL;
	char *tag_album=NULL;
	char *tag_comment=NULL;
	char *tag_genre=NULL;
	guint tag_bitrate=0;
	guint tag_track=0;
	GstBuffer *tag_cover=NULL;

	if(md==NULL) return TRUE;
	if(md->uri==NULL) return TRUE;

	switch(GST_MESSAGE_TYPE(message))
	{
		case GST_MESSAGE_EOS:
		{
			ly_mdh_md_eos=TRUE;
			return TRUE;
		}
		case GST_MESSAGE_ERROR:
		{
			ly_mdh_md_eos=TRUE;
			return TRUE;
		}
		case GST_MESSAGE_TAG:
		{
			GstTagList *tags;
			gst_message_parse_tag(message,&tags);
			//codec
			if(gst_tag_list_get_string(tags,GST_TAG_AUDIO_CODEC,&tag_codec))
			{
				if(tag_codec!=NULL)
					g_strlcpy(md->codec, tag_codec, sizeof(md->codec));
			}
			//title
			if(gst_tag_list_get_string(tags,GST_TAG_TITLE,&tag_title))
			{
				if(g_utf8_validate(tag_title,-1,NULL))
				{
					g_utf8_strncpy((char *)(md->title),tag_title, sizeof(md->title));
				}
				g_free(tag_title);
			}
			//artist
			if(gst_tag_list_get_string(tags,GST_TAG_ARTIST,&tag_artist))
			{
				if(g_utf8_validate(tag_artist,-1,NULL))
				{
					g_utf8_strncpy((char *)(md->artist), tag_artist, sizeof(md->artist));
				}
				g_free(tag_artist);
			}
			//album
			if(gst_tag_list_get_string(tags,GST_TAG_ALBUM,&tag_album))
			{
				if(g_utf8_validate(tag_album,-1,NULL))
				{
					g_utf8_strncpy((char *)(md->album), tag_album, sizeof(md->album));
				}
				g_free(tag_album);
			}
			//comment
			if(gst_tag_list_get_string(tags,GST_TAG_COMMENT,&tag_comment))
			{
				if(g_utf8_validate(tag_comment,-1,NULL))
				{
					g_utf8_strncpy((char *)(md->comment), tag_comment, sizeof(md->comment));
				}
				g_free(tag_comment);
			}
			//bitrate
			if(gst_tag_list_get_uint(tags, GST_TAG_BITRATE, &tag_bitrate))
			{
				md->bitrate = tag_bitrate;
			}
			//track_number
			if(gst_tag_list_get_uint(tags, GST_TAG_TRACK_NUMBER, &tag_track))
			{
				md->track = tag_track;
			}
			//genre
			if(gst_tag_list_get_string(tags, GST_TAG_GENRE,&tag_genre))
			{
				if(g_utf8_validate(tag_genre,-1,NULL))
				{
					g_utf8_strncpy((char *)(md->genre), tag_genre, sizeof(md->genre));
				}
				g_free(tag_genre);
			}
			//album cover
 			if(gst_tag_list_get_buffer(tags, GST_TAG_IMAGE, &tag_cover))
 			{
 				if(md->cover!=NULL)
 					gst_buffer_unref(md->cover);
				md->cover=tag_cover;
			}
			gst_tag_list_free(tags);
			return TRUE;
			break;
		}
		case GST_MESSAGE_ELEMENT:
			break;
		default:
			break;
	}
	return FALSE;
}

void		ly_mdh_new_with_uri_loop_cb	(LyMdhMetadata *md, GstElement *element, gboolean block)
{
	GstBus *bus;
	GstMessage *message;
	gboolean done = FALSE;
	bus = gst_element_get_bus(element);
	g_return_if_fail(bus!=NULL);
	while(!done && !ly_mdh_md_eos)
	{
		if(block)
			message = gst_bus_timed_pop(bus, GST_CLOCK_TIME_NONE);
		else
			message = gst_bus_timed_pop(bus, 0);
		if(message==NULL)
		{
			gst_object_unref(bus);
			return;
		}
		done = ly_mdh_new_with_uri_bus_cb(bus, message, md);
		gst_message_unref(message);
	}
	gst_object_unref(bus);
}

void		ly_mdh_new_with_uri_full_loop_cb	(LyMdhMetadata *md, GstElement *element, gboolean block)
{
	GstBus *bus;
	GstMessage *message;
	gboolean done = FALSE;
	bus = gst_element_get_bus(element);
	g_return_if_fail(bus!=NULL);
	while(!done && !ly_mdh_md_eos)
	{
		if(block)
			message = gst_bus_timed_pop(bus, GST_CLOCK_TIME_NONE);
		else
			message = gst_bus_timed_pop(bus, 0);
		if(message==NULL)
		{
			gst_object_unref(bus);
			return;
		}
		done = ly_mdh_new_with_uri_full_bus_cb(bus, message, md);
		gst_message_unref(message);
	}
	gst_object_unref(bus);
}

void		ly_mdh_new_with_uri_pipe_cb	(GstElement *decodebin,GstPad *pad, gboolean last, gpointer data)
{
	static GstCaps *caps;
	static GstPad *sink_pad;

	caps = gst_pad_get_caps(pad);
	if(gst_caps_is_empty(caps) || gst_caps_is_any(caps))
	{
		ly_log_put(_("[warning] Decoded pad with no caps or any caps, Abort..."));
	}
	else
	{
		sink_pad = gst_element_get_static_pad(GST_ELEMENT(data), "sink");
		gst_pad_link(pad, sink_pad);
		gst_object_unref(sink_pad);
	}
	gst_caps_unref (caps);
}

/**
 * ly_mdh_time_int2str:
 * @t_int:		the absolute time get from gstreamer pipeline whose format is gint64
 *
 * Covert the absolute gint64 time to string format.
 *
 * Returns:		the string format time
 */
char *ly_mdh_time_int2str(gint64 t_int)
{

	int min=0;
	int sec=0;
	int frm=0;

	t_int=t_int / 10000000;
	frm = t_int % 100;
	t_int=t_int / 100;
	min = t_int / 60;
	sec = t_int % 60;

	char *t_str = (char *)g_malloc(128);
	g_snprintf(t_str, 128, "%02d:%02d.%02d", min, sec, frm);
	return t_str;
}

/**
 * ly_mdh_time_str2int:
 * @t_str:		the string format time like "min:sec.frm"
 *
 * Covert the string time to absolute time in gint64 format.
 *
 * Returns:		the gint64 format time
 */
gint64 ly_mdh_time_str2int(gchar *t_str)
{
	if(!t_str)
		return 0;

	int min=0;
	int sec=0;
	int frm=0;
	gint64 t_int=0;

	sscanf(t_str, "%02d:%02d.%02d", &min, &sec, &frm);
	t_int=(min*60+sec)*100+frm;
	t_int=t_int*10000000;
	return t_int;
}











