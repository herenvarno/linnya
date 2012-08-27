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

GstElement *ly_mdh_put_pipeline;
GMutex *ly_mdh_put_mutex;



/*
 * FUNCTIONS
 */

gboolean	ly_mdh_new_with_uri_bus_cb	(GstBus *bus, GstMessage *message, gpointer data);
gboolean	ly_mdh_new_with_uri_full_bus_cb	(GstBus *bus, GstMessage *message, gpointer data);
void		ly_mdh_new_with_uri_loop_cb	(LyMdhMetadata *md, GstElement *element, gboolean block);
void		ly_mdh_new_with_uri_full_loop_cb	(LyMdhMetadata *md, GstElement *element, gboolean block);
void		ly_mdh_new_with_uri_pipe_cb	(GstElement *decodebin,GstPad *pad, gboolean last, gpointer data);
int			ly_mdh_push_handler_cb(GstBus *bus, GstMessage *msg, gpointer data);
void		ly_mdh_push_add_ogg_pad_cb(GstElement *demux, GstPad *pad,GstElement *tagger);
void		ly_mdh_push_add_id3_pad_cb(GstElement *demux, GstPad *pad,GstElement *tagger);
void		ly_mdh_push_move_file_cb();

/**
 * ly_mdh_init:
 *
 * Initialize the mdh module, it will be called by #ly_cox_init
 */
void			ly_mdh_init				()
{
	char extra_encoding[1024]="GB18030";
	if(!ly_reg_get("dbm_extra_encoding", "%*[^\n(](%1023[^\n)]", extra_encoding))
	{
		ly_reg_set("dbm_extra_encoding", "Chinese Simplified (GB18030)");
	}
	char encoding[1024]="";
	g_snprintf(encoding, sizeof(encoding), "%s:UTF-8", extra_encoding);
	g_setenv("GST_ID3_TAG_ENCODING", encoding, TRUE);

	ly_mdh_put_mutex=g_mutex_new();
}
/**
 * ly_mdh_fina:
 *
 * Finalize the mdh module, it will be called by #ly_cox_fina
 */
void			ly_mdh_fina				()
{
	g_mutex_free(ly_mdh_put_mutex);
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
		ly_log_put_with_flag(G_LOG_LEVEL_WARNING, _("Cannot open file: %s. unsupported protocol!"), uri);
		return NULL;
	}
	g_free(prefix);

	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		g_free(path);
		ly_log_put_with_flag(G_LOG_LEVEL_WARNING, _("Cannot open file: %s. file not found!"), uri);
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
	urisrc=gst_element_make_from_uri(GST_URI_SRC, uri, "urisrc");
	decodebin=gst_element_factory_make("decodebin", "decodebin");
	fakesink=gst_element_factory_make("fakesink", "fakesink");

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
		if(ly_mdh_pipeline!=NULL)
			gst_object_unref(GST_OBJECT(ly_mdh_pipeline));
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
		ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  _("Cannot open file: %s. unsupported protocol!"), uri);
		return NULL;
	}
	g_free(prefix);
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		g_free(path);
		ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  _("Cannot open file: %s. file not found!"), uri);
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
		if(ly_mdh_pipeline!=NULL)
			gst_object_unref(GST_OBJECT(ly_mdh_pipeline));
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
	if(!(gst_caps_is_empty(caps) || gst_caps_is_any(caps)))
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

gboolean	ly_mdh_push(LyMdhMetadata *md)
{
	if(!md||!g_str_has_prefix(md->uri, "file://"))
		return FALSE;

	if(!g_mutex_trylock(ly_mdh_put_mutex))
	{
		ly_log_put_with_flag(G_LOG_LEVEL_WARNING, _("An old task is running, Tag Failed!"));
		return FALSE;
	}
	/*
	 * BUILD
	 */
	GstElement *filesrc=NULL;
	GstElement *demux=NULL;
	GstElement *mux=NULL;
	GstElement *parse=NULL;
	GstElement *filesink=NULL;
	GstElement *tagger=NULL;
	GstBus *bus=NULL;

	const gchar *codec=NULL;
	ly_mdh_put_pipeline=gst_pipeline_new("pipeline");
	filesrc=gst_element_factory_make("filesrc","filesrc");
	filesink=gst_element_factory_make("filesink","filesink");
	if(!ly_mdh_put_pipeline||!filesrc||!filesink)
	{
		if(ly_mdh_put_pipeline);
			gst_object_unref(ly_mdh_put_pipeline);
		g_mutex_unlock(ly_mdh_put_mutex);
		return FALSE;
	}

	//MP3
	if(strstr(md->codec,"MP3")!=NULL)
	{
		demux=gst_element_factory_make("id3demux","demux");
		tagger=gst_element_factory_make("id3v2mux","tagger");
		codec = "LAME";
		if(!demux||!tagger)
		{
			gst_object_unref(ly_mdh_put_pipeline);
			g_mutex_unlock(ly_mdh_put_mutex);
			return FALSE;
		}
	}
	//OGG
	else if(strstr(md->codec,"Vorbis")!=NULL)
	{
		tagger = gst_element_factory_make("vorbistag", "tagger");
		demux=gst_element_factory_make("oggdemux","demux");
		mux=gst_element_factory_make("oggmux","mux");
		parse = gst_element_factory_make("vorbisparse", "parse");
		codec = "Vorbis";
		if(!demux||!mux||!tagger||!parse)
		{
			gst_object_unref(ly_mdh_put_pipeline);
			g_mutex_unlock(ly_mdh_put_mutex);
			return FALSE;
		}
	}
	//FLAC
	else if(strstr(md->codec,"FLAC")!=NULL)
	{
		tagger = gst_element_factory_make("flactag", "tagger");
		codec="FLAC";
		if(!tagger)
		{
			gst_object_unref(ly_mdh_put_pipeline);
			g_mutex_unlock(ly_mdh_put_mutex);
			return FALSE;
		}
	}
	//APE
	else if(strstr(md->codec,"Monkey's Audio")!=NULL)
	{
		demux=gst_element_factory_make("apedemux","demux");
		tagger=gst_element_factory_make("apev2mux","tagger");
		codec="LAME";
		if(!demux||!tagger)
		{
			gst_object_unref(ly_mdh_put_pipeline);
			g_mutex_unlock(ly_mdh_put_mutex);
			return FALSE;
		}
	}
	else
	{
		gst_object_unref(ly_mdh_put_pipeline);
		g_mutex_unlock(ly_mdh_put_mutex);
		return FALSE;
	}

	/*
	 * SET
	 */
	gchar location_i[1024]="";
	gchar location_o[1024]="";
	g_snprintf(location_i, sizeof(location_i), "%s", md->uri+7);
	g_snprintf(location_o, sizeof(location_o), "%s%s-%s.audio", LY_GLA_TEMPDIR, md->artist, md->title);
	g_object_set(G_OBJECT(filesrc), "location", location_i, NULL);
	g_object_set(G_OBJECT(filesink), "location", location_o, NULL);

	gst_tag_setter_add_tags(GST_TAG_SETTER(tagger),
							GST_TAG_MERGE_REPLACE_ALL,
							GST_TAG_TITLE, md->title,
							GST_TAG_ARTIST, md->artist,
							GST_TAG_ALBUM, md->album,
							GST_TAG_GENRE, md->genre,
							GST_TAG_TRACK_NUMBER, md->track,
							GST_TAG_ENCODER, "Linnya",
							GST_TAG_ENCODER_VERSION, 1,
							GST_TAG_CODEC,codec,
							NULL);

	/*
	 *LINK
	 */
	//MP3
	if(strstr(md->codec,"MP3")!=NULL)
	{
		gst_bin_add_many(GST_BIN(ly_mdh_put_pipeline), filesrc, demux,tagger,filesink, NULL);
		g_signal_connect(demux, "pad-added",G_CALLBACK(ly_mdh_push_add_id3_pad_cb), tagger);
		gst_element_link(filesrc, demux);
		gst_element_link(tagger, filesink);
	}
	//OGG
	else if(strstr(md->codec,"Vorbis")!=NULL)
	{
		gst_bin_add_many(GST_BIN(ly_mdh_put_pipeline), filesrc, demux, tagger, parse, mux, filesink, NULL);
		g_signal_connect(demux, "pad-added",G_CALLBACK(ly_mdh_push_add_ogg_pad_cb), tagger);
		gst_element_link(filesrc, demux);
		gst_element_link_many(tagger, parse, mux, filesink,NULL);
	}
	//FLAC
	else if(strstr(md->codec,"FLAC")!=NULL)
	{
		gst_bin_add_many(GST_BIN(ly_mdh_put_pipeline), filesrc, tagger, filesink, NULL);
		gst_element_link_many(filesrc, tagger, filesink, NULL);
	}
	//APE
	else if(strstr(md->codec,"Monkey's Audio")!=NULL)
	{
		gst_bin_add_many(GST_BIN(ly_mdh_put_pipeline), filesrc, demux,tagger,filesink, NULL);
		g_signal_connect(demux, "pad-added",G_CALLBACK(ly_mdh_push_add_id3_pad_cb), tagger);
		gst_element_link(filesrc, demux);
		gst_element_link(tagger, filesink);
	}
	else
	{
		gst_object_unref(ly_mdh_put_pipeline);
		g_mutex_unlock(ly_mdh_put_mutex);
		return FALSE;
	}

	bus = gst_pipeline_get_bus(GST_PIPELINE(ly_mdh_put_pipeline));
	gst_bus_add_watch(bus, (GstBusFunc)ly_mdh_push_handler_cb,  g_memdup(md,sizeof(LyMdhMetadata)));
	gst_object_unref(bus);
	gst_element_set_state(ly_mdh_put_pipeline, GST_STATE_NULL);
	gst_element_set_state(ly_mdh_put_pipeline, GST_STATE_READY);
	if(gst_element_set_state(ly_mdh_put_pipeline, GST_STATE_PLAYING)==GST_STATE_CHANGE_FAILURE)
	{
		gst_element_set_state(ly_mdh_put_pipeline, GST_STATE_NULL);
		gst_object_unref(ly_mdh_put_pipeline);
		g_mutex_unlock(ly_mdh_put_mutex);
		return FALSE;
	}
	return TRUE;
}

int ly_mdh_push_handler_cb(GstBus *bus, GstMessage *msg, gpointer data)
{
	switch(GST_MESSAGE_TYPE(msg))
	{
		case GST_MESSAGE_EOS:
			ly_mdh_push_move_file_cb((LyMdhMetadata*)data);
			break;
		default:
			break;
	}
	return GST_BUS_ASYNC;
}

void ly_mdh_push_add_ogg_pad_cb(GstElement *demux, GstPad *pad,GstElement *tagger)
{
	GstPad *conn_pad = NULL;
	conn_pad = gst_element_get_compatible_pad(tagger, pad, NULL);
	gst_pad_link(pad, conn_pad);
	gst_object_unref(conn_pad);
}
void ly_mdh_push_add_id3_pad_cb(GstElement *demux, GstPad *pad,GstElement *tagger)
{
	GstPad *conn_pad = NULL;
	conn_pad = gst_element_get_compatible_pad(tagger, pad, NULL);
	gst_pad_link(pad, conn_pad);
	gst_object_unref(conn_pad);
}

void ly_mdh_push_move_file_cb(LyMdhMetadata* md)
{
	if(ly_mdh_put_pipeline)
	{
		gst_element_set_state(ly_mdh_put_pipeline, GST_STATE_NULL);
		gst_object_unref(ly_mdh_put_pipeline);
		ly_mdh_put_pipeline = NULL;

		gchar location_i[1024]="";
		gchar location_o[1024]="";
		g_snprintf(location_i, sizeof(location_i), "%s", md->uri+7);
		g_snprintf(location_o, sizeof(location_o), "%s%s-%s.audio", LY_GLA_TEMPDIR, md->artist, md->title);
		int output;
		char cmd[10240]="";
		g_snprintf(cmd, sizeof(cmd), "mv \"%s\" \"%s\"", location_o, location_i);
		output=system(cmd);
	}
	g_free(md);
	g_mutex_unlock(ly_mdh_put_mutex);
}
