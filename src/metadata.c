

//HEADERS
#include "metadata.h"

//VARIABLES
GstElement *pipeline=FALSE;
gboolean md_eos=FALSE;


//GLOBAL VARIABLES
#define extra_encoding gl_conf->encode


//FUNCTIONS
//*****************************************************************************************************
//*
//*	FUNCTION:	tag_get_id3v1
//*	DISCRIPTION:	get ID3v1 tag, serve for funcion get_tag
//*
//*****************************************************************************************************
gchar **tag_get_id3v1(FILE *file) 
{ 
	gchar *b; 
	gchar *temp; 
	static gchar a[130], *tag[3]; 
	gsize r,w; 
	gint i,n; 
	tag[0] = NULL; 
	tag[1] = NULL; 
	tag[2] = NULL; 
	if(fseek(file, -128, 2)==-1) 
	{ 
		return NULL; 
	} 
	if(!fread(a, 128, 1, file)) 
	{ 
		return NULL; 
	} 
	b=a; 
	if(*b=='T' && *(b+1)=='A' && *(b+2)=='G') 
	{
		b+=3; 
		for(n=0;n<3;n++) 
		{ 
			if(n==0)
				tag[2] = g_strndup(b, 30); 
			else if(n==1)
				tag[0] = g_strndup(b, 30); 
			else if(n==2)
				tag[1] = g_strndup(b, 30); 
			b+=30; 
			for(i=1;!(*(b-i)>=33 && *(b-i)<=122);i++)
				if(*(b-i)==0)
					break; 
				else if(*(b-i)==32)
					*(b-i)=0; 
		}
		for(i=0;i<3;i++) 
		{ 
			if(!g_utf8_validate(tag[i],-1,NULL)) 
			{ 
				temp = g_convert(tag[i], -1, "UTF-8", extra_encoding, &r, &w, NULL); 
				if(tag[i]!=NULL) g_free(tag[i]); 
					tag[i] = temp; 
			} 
			else 
			{ 
				g_free(tag[i]); 
				tag[i] = NULL; 
			} 
		}
		return tag; 
	}
	return NULL; 
} 
 
//*****************************************************************************************************
//*
//*	FUNCTION:	tag_get_id3v2
//*	DISCRIPTION:	get ID3v2 tag, serve for fuction get_tag
//*
//*****************************************************************************************************
static char **tag_get_id3v2(FILE *file) 
{
	gchar *a, *c, b[5], *d; 
	static gchar *tag[3]; 
	gint j, header_size, version; 
	glong i; 
	gint size[4], tag_type; 
	gsize r, w; 
	tag[0] = NULL; 
	tag[1] = NULL; 
	tag[2] = NULL; 
	a = g_malloc(10);
	int output;
	output=fread(a, 10, 1, file);
	/* Judge if it is an ID3v2 tag. */ 
	if(*a=='I' && *(a+1)=='D' && *(a+2)=='3') 
	{	
		version = *(a+3); 
		size[0] = *(a+6); 
		if(size[0]<0) size[0]+=256; 
		size[1] = *(a+7); 
		if(size[1]<0) size[1]+=256; 
		size[2] = *(a+8); 
		if(size[2]<0) size[2]+=256; 
		size[3] = *(a+9); 
		if(size[3]<0) size[3]+=256; 
		i = (size[0]&0x7F)*0x200000 + (size[1]&0x7F)*0x4000 +  (size[2]&0x7F)*0x80 + (size[3]&0x7F); 
		header_size = i; 
		g_free(a); 
		a = g_malloc(i); 
		if(!fread(a,i,1,file)) 
		{
			g_free(a);
			return NULL; 
		}; 
		c = a;
		
		/* If it is an ID3v2.4 tag. */
		if(version==4)
		{
			g_free(a);
			return NULL;
		}
 		/* If it is an ID3v2.3 tag. */
		else if(version==3)
		{
			/* Read each tag in the loop. */
			for(;c!=a+header_size;)
			{
				if(tag[0]!=NULL && tag[1]!=NULL && tag[2]!=NULL)
				{
					g_free(a);
					return tag;
				}
				c[4]='\0';
				strcpy(b,c);
				b[4] = 0;
				tag_type = 0;
				size[0] = *(c+4);
				if(size[0]<0) size[0]+=256;
				size[1] = *(c+5);
				if(size[1]<0) size[1]+=256;
				size[2]=*(c+6);
				if(size[2]<0) size[2]+=256;
				size[3]=*(c+7);
				if(size[3]<0) size[3]+=256;
				i = size[0]*0x1000000+size[1]*0x10000+size[2]*0x100+size[3];
				if(i<0) i+=256;
				if((c+i)>(a+header_size)) break;
				if(i==0) break;
				tag_type = 0;
				if(!strcmp(b, "TPE1")) tag_type = 1;
				if(!strcmp(b, "TALB")) tag_type = 2;
				if(!strcmp(b, "TIT2")) tag_type = 3;
				if(!tag_type)
				{
					c+=i+10;
					continue;
				}
				c+=10;
				d = g_malloc(i+1);
				if(d==NULL)
				{
					g_free(a);
					return NULL;
				}
				for(j=0;i!=0;i--)
				{
					if(*c!=0)
					{
						d[j] = *c;
						j++;
					}
					c++;
				}
				d[j]=0;
				if(g_utf8_validate(d,-1,NULL))
				tag[tag_type-1] = NULL;
				else
				{
					tag[tag_type-1]=g_convert(d, -1, "UTF-8", extra_encoding, &r, &w, NULL);
				}
				g_free(d);
			}
			if(tag[0]==NULL && tag[1]==NULL && tag[2]==NULL)
			{
				g_free(a);
				return NULL;
			}
		}
		/* If it is an ID3v2.2 tag. */
		else if(version==2)
		{
			/* Read each tag in the loop. */
			for(;c<a+header_size;)
			{
				if(tag[0]!=NULL && tag[1]!=NULL && tag[2]!=NULL)
				{
					g_free(a);
					return tag;
				}
				c[3]='\0';
				strcpy(b,c);
				b[3] = 0;
				size[0] = *(c+3);
				if(size[0]<0) size[0]+=256;
				size[1] = *(c+4);
				if(size[1]<0) size[1]+=256;
				size[2] = *(c+5);
				if(size[2]<0) size[2]+=256;
				i = size[0]*0x10000 + size[1]*0x100 + size[2];
				if(i<0) i+=256;
				if((c+i)>(a+header_size)) break;
				if(i==0) break;
				tag_type=0;
				if(!strcmp(b,"TP1")) tag_type = 1;
				if(!strcmp(b,"TAL")) tag_type = 2;
				if(!strcmp(b,"TT2")) tag_type = 3;
				if(!tag_type)
				{
					c+=i+6;
					continue;
				}
				c+=6;
				d = g_malloc(i);
				if(d==NULL)
				{
					g_free(a);
					return NULL;
				}
				for(j=0;i!=0;i--)
				{
					if(*c==0)
					{
						c++;
						continue;
					}
					d[j] = *c;
					j++;
					c++;
				}
				d[j] = 0;
				if(g_utf8_validate(d,-1,NULL))
				{
					tag[tag_type-1] = NULL;
				}
				else
				{
					tag[tag_type-1] = g_convert(d, -1, "UTF-8",  extra_encoding, &r, &w, NULL);
				}
				g_free(d);
			}
			if(tag[0]==NULL && tag[1]==NULL && tag[2]==NULL)
			{
				g_free(a);
				return NULL;
			}
		}
	}
	g_free(a);
	return NULL;
} 
 
//*****************************************************************************************************
//*
//*	FUNCTION:		md_get_mp3_tag
//*	DISCRIPTION:	get ID3 tag
//*
//*****************************************************************************************************
gchar **md_get_mp3_tag(gchar *filename)  
{ 
    FILE *file; 
    static gchar **tag; 

    if(filename==NULL || *filename==0)
    { 
        g_printf("Get tag: Invaild filename.\n"); 
        return NULL; 
    } 
    file=fopen(filename, "rb"); 
    if(file==NULL)
    {
        g_printf("Get tag: Can't open file %s\n", filename);
        return NULL; 
    }
    /* Get ID3v2 tag, if it returns NULL, then try to get ID3v1 tag. */
    tag=tag_get_id3v2(file);
    if(tag==NULL)
    {
        rewind(file);
        tag=tag_get_id3v1(file);
    }
    fclose(file);
    return tag; 
} 





//*****************************************************************************************************
//*
//*	FUNCTION:		md_callback_handler
//*	DISCRIPTION:	the bus handler for getting metadata
//*
//*****************************************************************************************************
static gboolean md_callback_get_metadata(GstBus *bus, GstMessage *message, gpointer data)
{
	_gl_metadata *md=data;
	
	gchar *tag_codec=NULL;
	gchar *tag_title=NULL;
	gchar *tag_artist=NULL;
	gchar *tag_album=NULL;
	
	if(md==NULL) return TRUE;
	if(md->uri==NULL) return TRUE;

	switch(GST_MESSAGE_TYPE(message)) 
	{
		case GST_MESSAGE_EOS:
		{
			md_eos=TRUE;
			return TRUE;
		}
		case GST_MESSAGE_ERROR:
		{
			md_eos=TRUE;
			return TRUE;
		}
		case GST_MESSAGE_TAG:
		{
			GstTagList *tags;
			gst_message_parse_tag(message,&tags);
			//获取文件类型
			if(gst_tag_list_get_string(tags,GST_TAG_AUDIO_CODEC,&tag_codec))
			{
				if(tag_codec!=NULL)
					md->codec=tag_codec;
			}
			//获取歌曲名
			if(gst_tag_list_get_string(tags,GST_TAG_TITLE,&tag_title))
			{
				if(g_utf8_validate(tag_title,-1,NULL))
				{
					if(md->title!=NULL)
						g_free((gchar *)md->title);
					md->title = g_malloc(128);
					md->title = g_utf8_strncpy((gchar *)md->title,tag_title,128);
				}
				g_free(tag_title);
			}
			//获取艺术家
			if(gst_tag_list_get_string(tags,GST_TAG_ARTIST,&tag_artist))
			{
				if(g_utf8_validate(tag_artist,-1,NULL))
				{
					if(md->artist!=NULL) g_free((gchar *)md->artist);
					md->artist = g_malloc(128);
					md->artist = g_utf8_strncpy((gchar *)md->artist,tag_artist,128);
				}
				g_free(tag_artist);
			}
			//获取专辑名
			if(gst_tag_list_get_string(tags,GST_TAG_ALBUM,&tag_album))
			{
				if(g_utf8_validate(tag_album,-1,NULL))
				{
					if(md->album!=NULL) g_free((gchar *)md->album);
					md->album = g_malloc(128);
					md->album = g_utf8_strncpy((gchar *)md->album,tag_album,128);
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

//*****************************************************************************************************
//*
//*	FUNCTION:		md_get_meta
//*	DISCRIPTION:	get meta of paticular music by uri
//*
//*****************************************************************************************************
static void md_callback_event_loop(_gl_metadata *md, GstElement *element, gboolean block)
{
	GstBus *bus;
	GstMessage *message;
	gboolean done = FALSE;
	bus = gst_element_get_bus(element);
	g_return_if_fail(bus!=NULL);
	while(!done && !md_eos)
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
		done = md_callback_get_metadata(bus,message,md);
		gst_message_unref(message);
	}
	gst_object_unref(bus);
}
static void md_callback_build_pipeline(GstElement *decodebin,GstPad *pad, gboolean last, gpointer data)
{
	static GstCaps *caps;
	static GstPad *sink_pad;
	
	caps = gst_pad_get_caps(pad);
	if(gst_caps_is_empty(caps) || gst_caps_is_any(caps))
	{
		de_print(__DE_ERROR,__FUNCTION__,_("Decoded pad with no caps or any caps, boring."));
	}
	else
	{
		sink_pad = gst_element_get_static_pad(GST_ELEMENT(data), "sink");
		gst_pad_link(pad, sink_pad);
		gst_object_unref(sink_pad);
		//gst_element_set_state(pipeline,GST_STATE_PAUSED);	//加上这一句程序会假死，原因不详
	}
	gst_caps_unref (caps);
}
_gl_metadata*	md_get_metadata(gchar *uri)
{
	FILE *fp=fopen(uri+7,"r");
	if(!fp)
	{
		de_print(__DE_ERROR,__FUNCTION__,_("cannot open file"));
		return NULL;
	}
	fclose(fp);
	
	pipeline=NULL;
	md_eos=FALSE;
	
	_gl_metadata *md;
	md=(_gl_metadata*)g_malloc(sizeof(_gl_metadata));
	
	md->title=g_strconcat("unknown",NULL);
	md->artist=g_strconcat("unknown",NULL);
	md->album=g_strconcat("unknown",NULL);
	md->start=g_strconcat("00:00:000",NULL);
	md->duration=g_strconcat("00:00:00",NULL);
	md->codec=g_strconcat("unknown",NULL);
	md->uri=g_strconcat(uri,NULL);
	
	GstFormat fmt=GST_FORMAT_TIME;
	GstElement *urisrc;
	GstElement *decodebin;
	GstElement *fakesink;
	GstBus *bus=NULL;
	gint changeTimeout = 0;
	GstStateChangeReturn ret;
	GstMessage *msg;
	
	pipeline=gst_pipeline_new("pipeline");
	urisrc=gst_element_make_from_uri(GST_URI_SRC,md->uri,"urisrc");
	decodebin=gst_element_factory_make("decodebin","decodebin");
	fakesink=gst_element_factory_make("fakesink","fakesink");
	
	gst_bin_add_many(GST_BIN(pipeline),urisrc,decodebin,fakesink,NULL);
	gst_element_link(urisrc,decodebin);
	
	g_signal_connect_object(G_OBJECT(decodebin),"new-decoded-pad",G_CALLBACK(md_callback_build_pipeline),fakesink,0);
	
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_element_set_state(pipeline,GST_STATE_NULL);
	gst_element_set_state(pipeline,GST_STATE_READY);
	ret = gst_element_set_state(pipeline,GST_STATE_PAUSED);
	if(!ret)
	{
		if(pipeline!=NULL)
			gst_object_unref(GST_OBJECT(pipeline));
		md_eos=FALSE;
		pipeline=NULL;
		return FALSE;
	}
	while(ret==GST_STATE_CHANGE_ASYNC && !md_eos && changeTimeout < 5) 
	{
		msg = gst_bus_timed_pop(bus, 1 * GST_SECOND);
		if(msg!=NULL) 
		{
			md_callback_get_metadata(bus, msg, md);
			gst_message_unref(msg);
			changeTimeout = 0;
		}
		else
			changeTimeout++;
		ret = gst_element_get_state(pipeline, NULL, NULL, 0);
	}
	gst_object_unref(bus);
	md_callback_event_loop(md, pipeline, FALSE);
	if(ret!=GST_STATE_CHANGE_SUCCESS)
	{
		gst_element_set_state(pipeline,GST_STATE_NULL);
		return FALSE;
	}
	
	gint64 dura=0;
	gint64 duration=0;
	gint dura_min=0;
	gint dura_sec=0;
	gint dura_frm=0;	

	gst_element_query_duration(pipeline,&fmt,&dura);
	duration = dura/10000000;
	dura_frm=duration%100;
	duration=duration/100;
	dura_min = duration / 60;
	dura_sec = duration % 60;

	md->duration = (gchar *)g_malloc(128);
	g_snprintf(md->duration, 127, "%02d:%02d:%02d", dura_min, dura_sec, dura_frm);
	ret = gst_element_set_state(pipeline,GST_STATE_NULL);
	gst_object_unref(pipeline);

	gchar *str;
	gchar *c;
	if(md->codec==NULL||g_strcmp0(md->codec,"")==0||g_strcmp0(md->codec,"unknown")==0)
	{
		str=g_strconcat(md->uri,NULL);
		c=str+strlen(str);
		while(*c!='.')
			c--;
		if(g_strcmp0(c,".mp3")==0||g_strcmp0(c,".MP3")==0)
		{
			g_free(md->codec);
			md->codec=g_strconcat("MPEG 1 Audio, Layer 3 (MP3)",NULL);
		}
		g_free(str);
	}
	
	//for mp3, use custom function to read tags
	if(g_strcmp0(md->codec,"MPEG 1 Audio, Layer 3 (MP3)")==0);
	{
		gchar *location=g_filename_from_uri(md->uri,NULL,NULL);
		gchar **tag=NULL;
		tag=md_get_mp3_tag(location);
		g_free(location);
		
		if(tag!=NULL)
		{
			//title
			if(tag[2]!=NULL)
			{
				if(md->title!=NULL)
					g_free((gchar *)md->title);
				md->title = g_malloc(128);
				md->title = g_utf8_strncpy((gchar *)md->title,tag[2],128);
				g_free(tag[2]);
			}
			
			//artist
			if(tag[0]!=NULL)
			{
				if(md->artist!=NULL)
					g_free((gchar *)md->artist);
				md->artist = g_malloc(128);
				md->artist = g_utf8_strncpy((gchar *)md->artist,tag[0],128);
				g_free(tag[0]);
			}
			
			//album
			if(tag[1]!=NULL)
			{
				if(md->album!=NULL)
					g_free((gchar *)md->album);
				md->album = g_malloc(128);
				md->album = g_utf8_strncpy((gchar *)md->album,tag[1],128);
				g_free(tag[1]);
			}
		}
	}
	
	if(md->title==NULL||g_strcmp0(md->title,"")==0||g_strcmp0(md->title,"unknown")==0)
	{
		str=g_strconcat(md->uri,NULL);
		c=str+strlen(str);
		while(*c!='.')
			c--;
		*c='\0';
		while(*c!='/')
			c--;
		c++;
		md->title=g_strconcat(c,NULL);
		g_free(str);
	}
	pipeline=NULL;
	md_eos=FALSE;
	return md;
}



