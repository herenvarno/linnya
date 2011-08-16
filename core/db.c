/*
 * HEADERS
 */
#include "db.h"

/*
 * VARIABLES
 */
gchar *musicfile[]={
	".mp3",".MP3",
	".wma",".WMA",
	".flac",".FLAC",
	".cda",".CDA",
	".mid",".MID",
	".midi",".MIDI",
	".ogg",".OGG",
	".wav",".WAV",
	".acc",".ACC",
	".ape",".APE"};

/*
 * FUNCTIONS [PUBLIC]
 */
/*
 * NAME:	ly_db_init
 * VARS:	[void]
 * RETN:	[gboolean]		always TRUE.
 * DESC:	Initial db module.
 */
gboolean ly_db_init()
{
	ly_global_debug("info",_("Initial core module: DB ...\n"));
	/*
	 * get extra_encoding
	 */
	gchar extra_encoding[512]="GB18030";
	if(!ly_conf_get("db_extra_encoding","%s", extra_encoding))
	{
		ly_conf_set("db_extra_encoding","%s", extra_encoding);
	}
	
	/*
	 * open sqlite3
	 */
	gchar path[10240]="";
	g_snprintf(path, sizeof(path), "%slinnya.db", LY_GLOBAL_USERDIR);
	if(!ly_conf_get("db_path","%s",path))
	{
		ly_conf_set("db_path","%s",path);
	}
	
	int rc=0;
	rc=sqlite3_open(path,&ly_db_conn);
	if(rc!=SQLITE_OK)
	{
		ly_global_debug("warnning",_("Failed to open database.\n"));
		exit(0);
	}
	ly_db_check_database();
	ly_db_check_library();

	return TRUE;
}

/*
 * NAME:	ly_db_finalize
 * VARS:	[void]
 * RETN:	[gboolean]		always TRUE.
 * DESC:	Finalize db module.
 */
gboolean ly_db_finalize()
{
	ly_global_debug("info",_("Finalize core module: DB ...\n"));
	gchar *sql=NULL;
	sql=g_strconcat("UPDATE metadatas SET tmpflag=0",NULL);
	ly_db_exec(sql,NULL,NULL);
	g_free(sql);
	sqlite3_close(ly_db_conn);
	return TRUE;
}


/*
 * NAME:	ly_db_check_file_exists
 * VARS:	[gpointer]stmt	the querying data
 * 			[gpointer]data	user data
 * RETN:	[gboolean]		always FALSE.
 * DESC:	The callback of check if the files which do not belongs to library is existed.
 */
gboolean ly_db_check_file_exists(gpointer stmt, gpointer data)
{
	gchar uri[1024]="";
	gchar *sql=*(gchar **)data;
	g_strlcpy(uri,(const gchar *)sqlite3_column_text(stmt, 0),sizeof(uri));
	
	if(!g_file_test(uri+7,G_FILE_TEST_EXISTS))
	{
		ly_global_replace_str(uri,sizeof(uri),"'","''");
		if(sql==NULL)
		{
			*(gchar **)data=g_strconcat("DELETE FROM metadatas WHERE uri='",uri,"' ",NULL);
		}
		else
		{
			gchar *tmpsql=sql;
			*(gchar **)data=g_strconcat(tmpsql,"OR uri='",uri,"' ",NULL);
			g_free(tmpsql);
		}
	}
	return FALSE;
}

/*
 * NAME:	ly_db_check_newfile
 * VARS:	[gpointer]stmt	the querying data
 * 			[gpointer]data	user data
 * RETN:	[gboolean]		always FALSE.
 * DESC:	The callback of check if there are new music in library.
 */
gboolean ly_db_check_newfile(gpointer stmt, gpointer data)
{
	gchar *uri=NULL;
	GList *list=*(GList**)data;
	GList *p=list;
	uri=g_strconcat((const gchar *)sqlite3_column_text(stmt, 0),NULL);
	while(p)
	{
		if(g_str_equal(uri,p->data))
		{
			g_free(p->data);
			p->data=NULL;
			*(GList**)data=g_list_delete_link(list,p);
			break;
		}
		p=p->next;
	}
	g_free(uri);
	return FALSE;
}

/*
 * NAME:	ly_db_check_isaudio
 * VARS:	[gchar]filename	the filename
 * RETN:	[gboolean]		True for audio, Others false.
 * DESC:	To check if file is an audio file.
 */
gboolean ly_db_check_isaudio(gchar *filename)
{
	gint	i=0;
	gchar	*p=NULL;
	
	//get position to the last '.'
	p=filename;
	p=p+strlen(filename);
	for(i=0;i<(strlen(filename));i++)
	{
		if(*p=='.')
			break;
		else
			p--;
	}
	
	if(i>=(strlen(filename)+1))
	{
		return FALSE;
	}
	//music
	for(i=0;i<20;i++)
	{
		if(g_str_equal(p,musicfile[i]))
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * NAME:	ly_db_check_database
 * VARS:	[void]
 * RETN:	[gboolean]		always TRUE.
 * DESC:	To check if the tables is in database. if not, create them.
 */
gboolean ly_db_check_database()
{
	/*
	 * CREATE TABLE metadatas
	 */
	if(!ly_db_get_count("SELECT count(*) FROM sqlite_master WHERE type='table' AND name='metadatas'"))
	{
		ly_db_exec("CREATE TABLE metadatas(id INTEGER PRIMARY KEY, title TEXT, artist TEXT, album TEXT, codec TEXT, start TEXT NOT NULL, duration TEXT NOT NULL, uri TEXT NOT NULL, playing INTEGER NOT NULL, num INTEGER NOT NULL, flag INTEGER NOT NULL, tmpflag INTEGER)",NULL,NULL);
	}
	/*
	 * CREATE TABLE playlists
	 */
	if(!ly_db_get_count("SELECT count(*) FROM sqlite_master WHERE type='table' AND name='playlists'"))
	{
		ly_db_exec("CREATE TABLE playlists(id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL, num INTEGER NOT NULL)",NULL,NULL);
	}
	/*
	 * CREATE TABLE connections
	 */
	if(!ly_db_get_count("SELECT count(*) FROM sqlite_master WHERE type='table' AND name='connections'"))
	{
		ly_db_exec("CREATE TABLE connections(pid INTEGER, mid INTEGER, num INTEGER, FOREIGN KEY(mid) REFERENCES metadatas(id), FOREIGN KEY(pid) REFERENCES playlists(id), PRIMARY KEY(pid, mid))",NULL,NULL);
	}
	/*
	 * CREATE VIEW plist
	 */
	if(!ly_db_get_count("SELECT count(*) FROM sqlite_master WHERE type='view' AND name='plist'"))
	{
		ly_db_exec("CREATE VIEW plist AS SELECT * FROM metadatas WHERE playing>0 ORDER BY playing",NULL,NULL);
	}
	/*
	 * CREATE TRIGGER before_metadatas_delete
	 */
	if(!ly_db_get_count("SELECT count(*) FROM sqlite_master WHERE type='trigger' AND name='before_metadatas_delete'"))
	{
		ly_db_exec("CREATE TRIGGER before_metadatas_delete BEFORE DELETE ON metadatas FOR EACH ROW BEGIN DELETE FROM connections WHERE mid=OLD.id; END;",NULL,NULL);
	}
	/*
	 * CREATE TRIGGER before_metadatas_insert
	 */
	if(!ly_db_get_count("SELECT count(*) FROM sqlite_master WHERE type='trigger' AND name='before_metadatas_insert'"))
	{
		ly_db_exec("CREATE TRIGGER before_metadatas_insert BEFORE INSERT ON metadatas FOR EACH ROW WHEN EXISTS (SELECT 1 FROM metadatas WHERE uri=NEW.uri AND start=NEW.start) BEGIN SELECT RAISE(ROLLBACK,'A same metadata is exist!'); END;",NULL,NULL);
	}
	/*
	 * CREATE TRIGGER before_metadatas_update
	 */
	if(!ly_db_get_count("SELECT count(*) FROM sqlite_master WHERE type='trigger' AND name='before_metadatas_update'"))
	{
		ly_db_exec("CREATE TRIGGER before_metadatas_update BEFORE UPDATE ON metadatas FOR EACH ROW WHEN EXISTS (SELECT 1 FROM metadatas WHERE uri=NEW.uri AND start=NEW.start AND id!=NEW.id) BEGIN SELECT RAISE(ROLLBACK,'A same metadata is exist!'); END;",NULL,NULL);
	}
	/*
	 * CREATE TRIGGER before_playlists_delete
	 */
	if(!ly_db_get_count("SELECT count(*) FROM sqlite_master WHERE type='trigger' AND name='before_playlists_delete'"))
	{
		ly_db_exec("CREATE TRIGGER before_playlists_delete BEFORE DELETE ON playlists FOR EACH ROW BEGIN DELETE FROM connections WHERE pid=OLD.id; END;",NULL,NULL);
	}
	
	return TRUE;
}

/*
 * NAME:	ly_db_check_library
 * VARS:	[void]
 * RETN:	[gboolean]	always TRUE.
 * DESC:	To check the library.
 */
gboolean ly_db_check_library()
{
	/*
	 * get library path
	 */
	gchar path[10240]="";
	gchar tmppath[10240]="";
	
	g_snprintf(path,sizeof(path),"%smusic/", LY_GLOBAL_HOMEDIR);
	if(!ly_conf_get("lib_path","%s",path))
	{
		ly_conf_set("lib_path","%s",path);
	}
	g_strlcpy(tmppath, path, sizeof(tmppath));
	
	/*
	 * get filenames
	 */
	ly_db_exec("begin",NULL,NULL);
	gchar tmpstr[1024]="";
	GList *list=ly_global_traverse_dir(path, 5, TRUE);
	GList *p=list;
	gchar *tmpsql=NULL;
	gchar *sql=NULL;
	ly_global_replace_str(tmppath,sizeof(tmppath),"'","''");
	sql=g_strconcat("UPDATE metadatas SET tmpflag=1 WHERE uri like 'file://",tmppath,"%' AND ( uri='0'",NULL);
	while(p)
	{
		if(!ly_db_check_isaudio(p->data))
		{
			p=p->next;
			continue;
		}
		g_strlcpy(tmpstr,(gchar *)(p->data),sizeof(tmpstr));
		ly_global_replace_str(tmpstr,sizeof(tmpstr),"'","''");
		tmpsql=g_strconcat(sql,"OR uri='",tmpstr,"' ",NULL);
		g_free(sql);
		sql=tmpsql;
		p=p->next;
	}
	
	tmpsql=g_strconcat(sql,")",NULL);
	g_free(sql);
	sql=tmpsql;
	ly_db_exec(sql,NULL,NULL);
	g_free(sql);
	tmpsql=NULL;
	
	sql=g_strconcat("DELETE FROM metadatas WHERE uri like 'file://",tmppath,"%' AND tmpflag=0",NULL);
	
	ly_db_exec(sql,NULL,NULL);
	g_free(sql);
	
	tmpsql=NULL;
	sql=g_strconcat("SELECT uri FROM metadatas WHERE tmpflag=0",NULL);
	ly_db_exec(sql,ly_db_check_file_exists,&tmpsql);
	g_free(sql);
	if(tmpsql)
	{
		ly_db_exec(tmpsql,NULL,NULL);
		g_free(tmpsql);
		tmpsql=NULL;
	}
	sql=g_strconcat("UPDATE metadatas SET tmpflag=0",NULL);
	ly_db_exec(sql,NULL,NULL);
	g_free(sql);
	
	
	/*
	 * add new music
	 */
	sql=g_strconcat("SELECT uri FROM metadatas WHERE uri LIKE 'file://",tmppath,"%'",NULL);
	ly_db_exec(sql,ly_db_check_newfile,&list);
	g_free(sql);
	
	lyDbMetadata *metadata=NULL;
	p=list;
	while(p)
	{
		if(!ly_db_check_isaudio((gchar *)(p->data)))
		{
			g_free(p->data);
			p->data=NULL;
			p=p->next;
			continue;
		}
		metadata=ly_db_read_metadata((gchar *)(p->data));
		if(metadata)
		{
			ly_global_replace_str(metadata->title,sizeof(metadata->title),"'","''");
			ly_global_replace_str(metadata->artist,sizeof(metadata->artist),"'","''");
			ly_global_replace_str(metadata->album,sizeof(metadata->album),"'","''");
			ly_global_replace_str(metadata->codec,sizeof(metadata->codec),"'","''");
			ly_global_replace_str(metadata->uri,sizeof(metadata->uri),"'","''");
			
			sql=g_strconcat("INSERT INTO metadatas(title, artist, album, codec, start, duration, uri, playing, num, flag, tmpflag) VALUES ('",metadata->title,"', '",metadata->artist,"', '",metadata->album,"', '",metadata->codec,"', '",metadata->start,"', '",metadata->duration,"', '",metadata->uri,"',0, ifnull((select max(num) from metadatas),0)+1, 0, 1)",NULL);
			ly_db_exec(sql,NULL,NULL);
			g_free(sql);
		}
		g_free(metadata);
		metadata=NULL;
		g_free(p->data);
		p->data=NULL;
		p=p->next;
	}
	g_list_free(list);
	ly_db_exec("commit",NULL,NULL);
	return TRUE;
}


/*
 * NAME:	ly_db_exec
 * VARS:	[gchar*]sql		the sql command
 * 		[LY_GLOBAL_CALLBACK]func	the callback function
 * 		[gpointer]data	user data
 * RETN:	[gboolean]		True for success, Others false.
 * DESC:	To execute the sql command, and use callback func to deal with each line of the data.
 */
gboolean ly_db_exec(gchar *sql, LY_GLOBAL_CALLBACK(func), gpointer data)
{
	gint rt=0;
	sqlite3_stmt *stmt = 0;
	gint r=0;
	gboolean count=FALSE;
	char *error;
	
	if(func==NULL)
	{
		rt=sqlite3_exec(ly_db_conn,sql,0,0, (char **)(&error));
		if( rt != SQLITE_OK )
		{
			ly_msg_put("warnning", "core:db", _("Cannot exec sql!"));
			ly_global_debug("warnning",_("CANNOT EXEC SQL COMMAND: %s, FOR %s.\n"), sql, error);
			
		}
		sqlite3_free(error);
		return TRUE;
	}
	
	rt=sqlite3_prepare(ly_db_conn, sql, strlen(sql), &stmt, (const char **)(&error));
	if( rt != SQLITE_OK )
	{
		ly_msg_put("warnning", "core:db", _("Cannot exec sql!"));
		ly_global_debug("warnning",_("CANNOT EXEC SQL COMMAND: %s, FOR %s.\n"), sql, error);
		return FALSE;
	}
	r=sqlite3_step(stmt);
	while(r==SQLITE_ROW)
	{
		count=TRUE;
		if(func(stmt,data))
			break;
		r=sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	return count;
}

/*
 * NAME:	ly_db_get_metadata_cb
 * VARS:	[gpointer]stmt	the querying data
 * 			[gpointer]data	user data
 * RETN:	[gboolean]		always TRUE.
 * DESC:	The callback of get metadata.
 */
gboolean ly_db_get_metadata_cb(gpointer stmt, gpointer data)
{
	if(data==NULL||stmt==NULL)
		return TRUE;

	lyDbMetadata *metadata=data;
	metadata->id=sqlite3_column_int(stmt, 0);
	
	g_strlcpy(metadata->title,(const gchar *)sqlite3_column_text(stmt, 1),127);
	g_strlcpy(metadata->artist,(const gchar *)sqlite3_column_text(stmt, 2),127);
	g_strlcpy(metadata->album,(const gchar *)sqlite3_column_text(stmt, 3),127);
	g_strlcpy(metadata->codec,(const gchar *)sqlite3_column_text(stmt, 4),127);
	g_strlcpy(metadata->start,(const gchar *)sqlite3_column_text(stmt, 5),63);
	g_strlcpy(metadata->duration,(const gchar *)sqlite3_column_text(stmt, 6),63);
	g_strlcpy(metadata->uri,(const gchar *)sqlite3_column_text(stmt, 7),1023);
	
	metadata->playing=sqlite3_column_int(stmt, 8);
	metadata->num=sqlite3_column_int(stmt, 9);
	metadata->flag=sqlite3_column_int(stmt, 10);
	metadata->tmpflag=sqlite3_column_int(stmt, 11);

	return TRUE;
}

/*
 * NAME:	ly_db_get_metadata
 * VARS:	[gchar*]sql		the sql command, must like "SELECT * FROM metadatas WHERE...."
 * RETN:	[lyDbMetadata]	a new lyDbMetadata. must be freed when no longer used.
 * DESC:	To get a metadata by sql.
 */
lyDbMetadata *ly_db_get_metadata(gchar *sql)
{
	gboolean rt=FALSE;
	lyDbMetadata *metadata=(lyDbMetadata *)g_malloc(sizeof(lyDbMetadata));
	
	rt=ly_db_exec(sql,ly_db_get_metadata_cb,metadata);
	if(rt)
	{
		return metadata;
	}
	g_free(metadata);
	return NULL;
}

/*
 * NAME:	ly_db_get_metadata_by_id
 * VARS:	[gint]id		the id of metadata.
 * RETN:	[lyDbMetadata]	a new lyDbMetadata. must be freed when no longer used.
 * DESC:	To get a metadata by id.
 */
lyDbMetadata *ly_db_get_metadata_by_id(gint id)
{
	lyDbMetadata *metadata=NULL;
	gchar sql[10240];
	g_snprintf(sql, sizeof(sql), "SELECT * FROM metadatas WHERE id=%d", id);
	metadata=ly_db_get_metadata(sql);
	return metadata;
}

/*
 * NAME:	ly_db_get_count_cb
 * VARS:	[gpointer]stmt	the querying data
 * 			[gpointer]data	user data
 * RETN:	[gboolean]		always FALSE.
 * DESC:	The callback of get count.
 */
gboolean ly_db_get_count_cb(gpointer stmt, gpointer data)
{
	if(data==NULL||stmt==NULL)
		return TRUE;
	
	gint *count=data;
	*count=sqlite3_column_int(stmt,0);
	return TRUE;
}

/*
 * NAME:	ly_db_get_count
 * VARS:	[gchar*]sql		the sql command, must like "SELECT count(*) FROM metadatas WHERE...."
 * RETN:	[gint]			the count.
 * DESC:	To get the count of a table by sql.
 */
gint ly_db_get_count(gchar *sql)
{
	gint count=0;
	ly_db_exec(sql , ly_db_get_count_cb, &count);
	return count;
}

/*
 * NAME:	ly_db_get_metadata_count
 * VARS:	[void]
 * RETN:	[gint]			the count.
 * DESC:	To get the count of a metadatas table.
 */
gint ly_db_get_metadata_count()
{
	gint count=0;
	gchar *sql="SELECT COUNT(*) FROM metadatas";
	count=ly_db_get_count(sql);
	return count;
}

/*
 * NAME:	ly_db_get_playlist_count
 * VARS:	[void]
 * RETN:	[gint]			the count.
 * DESC:	To get the count of a playlists table.
 */
gint ly_db_get_playlist_count()
{
	gint count=0;
	gchar *sql="SELECT COUNT(*) FROM playlists";
	count=ly_db_get_count(sql);
	return count;
}


/*
 * NAME:	ly_db_dura2duration
 * VARS:	[gint64]dura	the absolute dura time.
 * RETN:	[gchar*]		the duration string.
 * DESC:	covert the absolute dura to duration string.
 */
gchar *ly_db_dura2duration(gint64 dura)
{
	
	gint dura_min=0;
	gint dura_sec=0;
	gint dura_frm=0;
	
	dura = dura/10000000;
	dura_frm=dura%100;
	dura=dura/100;
	dura_min = dura / 60;
	dura_sec = dura % 60;
	gchar *duration = (gchar *)g_malloc(128);
	g_snprintf(duration, 128, "%02d:%02d.%02d", dura_min, dura_sec, dura_frm);
	return duration;
}

/*
 * NAME:	ly_db_duration2dura
 * VARS:	[gchar*]duration	the duration string.
 * RETN:	[gint64]	the absolute dura time.
 * DESC:	covert the duration string to absolute dura.
 */
gint64 ly_db_duration2dura(gchar *duration)
{
	if(!duration)
		return 0;
	
	gint dura_min=0;
	gint dura_sec=0;
	gint dura_frm=0;
	
	gint64 dura=0;
	
	sscanf(duration,"%02d:%02d.%02d", &dura_min, &dura_sec, &dura_frm);
	dura=(dura_min*60+dura_sec)*100+dura_frm;
	dura=dura*10000000;
	return dura;
}

/*
 * NAME:	ly_db_free_metadata
 * VARS:	[lyDbMetadata]metadata	the metadata to be freed.
 * RETN:	[gboolean]		Always TRUE.
 * DESC:	Free the metadata.
 */
void ly_db_free_metadata(lyDbMetadata *metadata)
{
	if(metadata)
		g_free(metadata);
}




/*
 * NAME:	ly_db_read_metadata
 * VARS:	[gchar*]uri		the uri of music.
 * RETN:	[lyDbMetadata*]	metadata. Must be freed while no longer use.
 * DESC:	To get metadata by uri.
 */
lyDbMetadata *ly_db_read_metadata(gchar *uri)
{
	/*
	 * test file
	 */
	if(!g_file_test(uri+7, G_FILE_TEST_EXISTS))
	{
		ly_global_debug("warnning",_("Cannot open file: %s\n"),uri);
		return NULL;
	}

	ly_db_pipeline=NULL;
	ly_db_md_eos=FALSE;

	/*
	 * create and initial metadata
	 */
	lyDbMetadata *metadata=(lyDbMetadata*)g_malloc(sizeof(lyDbMetadata));
	g_strlcpy(metadata->title,"unknown",127);
	g_strlcpy(metadata->artist,"unknown",127);
	g_strlcpy(metadata->album,"unknown",127);
	g_strlcpy(metadata->codec,"unknown",127);
	g_strlcpy(metadata->start,"00:00.00",63);
	g_strlcpy(metadata->duration,"00:00.00",63);
	g_strlcpy(metadata->uri,uri,1023);
	metadata->num=0;
	metadata->flag=0;
	metadata->tmpflag=1;

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

	ly_db_pipeline=gst_pipeline_new("pipeline");
	urisrc=gst_element_make_from_uri(GST_URI_SRC,metadata->uri,"urisrc");
	decodebin=gst_element_factory_make("decodebin","decodebin");
	fakesink=gst_element_factory_make("fakesink","fakesink");
	
	gst_bin_add_many(GST_BIN(ly_db_pipeline),urisrc,decodebin,fakesink,NULL);
	gst_element_link(urisrc,decodebin);
	
	g_signal_connect_object(G_OBJECT(decodebin),"new-decoded-pad",G_CALLBACK(ly_db_build_pipeline_cb),fakesink,0);
	
	bus = gst_pipeline_get_bus(GST_PIPELINE(ly_db_pipeline));
	gst_element_set_state(ly_db_pipeline,GST_STATE_NULL);
	gst_element_set_state(ly_db_pipeline,GST_STATE_READY);
	rt = gst_element_set_state(ly_db_pipeline,GST_STATE_PAUSED);
	if(!rt)
	{
		if(ly_db_pipeline!=NULL)
			gst_object_unref(GST_OBJECT(ly_db_pipeline));
		ly_db_md_eos=FALSE;
		ly_db_pipeline=NULL;
		return FALSE;
	}
	while(rt==GST_STATE_CHANGE_ASYNC && !ly_db_md_eos && changeTimeout < 5)
	{
		msg = gst_bus_timed_pop(bus, 1 * GST_SECOND);
		if(msg!=NULL)
		{
			ly_db_read_metadata_cb(bus, msg, metadata);
			gst_message_unref(msg);
			changeTimeout = 0;
		}
		else
			changeTimeout++;
		rt = gst_element_get_state(ly_db_pipeline, NULL, NULL, 0);
	}
	gst_object_unref(bus);
	ly_db_event_loop_cb(metadata, ly_db_pipeline, FALSE);
	if(rt!=GST_STATE_CHANGE_SUCCESS)
	{
		gst_element_set_state(ly_db_pipeline,GST_STATE_NULL);
		return FALSE;
	}

	/*
	 * get duration
	 */
	gint64 dura=0;
	gst_element_query_duration(ly_db_pipeline,&fmt,&dura);

	gchar *duration=ly_db_dura2duration(dura);
	g_strlcpy(metadata->duration,duration,sizeof(metadata->duration));
	g_free(duration);

	rt = gst_element_set_state(ly_db_pipeline,GST_STATE_NULL);
	gst_object_unref(ly_db_pipeline);

	gchar *str;
	str=ly_global_get_suffix(uri);
	puts(str);
	if(g_str_equal(metadata->codec,"")||g_str_equal(metadata->codec,"unknown"))
	{
		str=ly_global_get_suffix(uri);
		if(g_str_equal(str,".mp3")||g_str_equal(str,".MP3"))
		{
			g_strlcpy(metadata->codec,"MPEG 1 Audio, Layer 3 (MP3)",sizeof(metadata->codec));	
		}
		g_free(str);
	}
	
	/*
	 * for mp3, use custom function to read tags
	 */
	if(g_str_equal(metadata->codec,"MPEG 1 Audio, Layer 3 (MP3)"));
	{
		gchar *location=g_filename_from_uri(metadata->uri,NULL,NULL);
		gchar **tag=NULL;
		tag=ly_db_get_mp3_tag(location);
		g_free(location);
		
		if(tag!=NULL)
		{
			//title
			if(tag[2]!=NULL)
			{
				g_utf8_strncpy(metadata->title,tag[2],128);
				g_free(tag[2]);
			}
			
			//artist
			if(tag[0]!=NULL)
			{
				g_utf8_strncpy(metadata->artist,tag[0],128);
				g_free(tag[0]);
			}
			
			//album
			if(tag[1]!=NULL)
			{
				g_utf8_strncpy(metadata->album,tag[1],128);
				g_free(tag[1]);
			}
		}
	}
	
	if(g_str_equal(metadata->title,"")||g_str_equal(metadata->title,"unknown"))
	{
		str=ly_global_get_dir(uri);
		g_strlcpy(metadata->title,str,sizeof(metadata->title));
		g_free(str);
	}
	ly_db_pipeline=NULL;
	ly_db_md_eos=FALSE;
	puts(uri);
	return metadata;
}

gboolean ly_db_read_metadata_cb(GstBus *bus, GstMessage *message, gpointer data)
{
	lyDbMetadata *md=data;
	
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
			ly_db_md_eos=TRUE;
			return TRUE;
		}
		case GST_MESSAGE_ERROR:
		{
			ly_db_md_eos=TRUE;
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
					g_strlcpy(md->codec,tag_codec,127);
			}
			//title
			if(gst_tag_list_get_string(tags,GST_TAG_TITLE,&tag_title))
			{
				if(g_utf8_validate(tag_title,-1,NULL))
				{
					g_utf8_strncpy((gchar *)md->title,tag_title,128);
				}
				g_free(tag_title);
			}
			//artist
			if(gst_tag_list_get_string(tags,GST_TAG_ARTIST,&tag_artist))
			{
				if(g_utf8_validate(tag_artist,-1,NULL))
				{
					g_utf8_strncpy((gchar *)md->artist,tag_artist,128);
				}
				g_free(tag_artist);
			}
			//album
			if(gst_tag_list_get_string(tags,GST_TAG_ALBUM,&tag_album))
			{
				if(g_utf8_validate(tag_album,-1,NULL))
				{
					g_utf8_strncpy((gchar *)md->album,tag_album,128);
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

void ly_db_event_loop_cb(lyDbMetadata *md, GstElement *element, gboolean block)
{
	GstBus *bus;
	GstMessage *message;
	gboolean done = FALSE;
	bus = gst_element_get_bus(element);
	g_return_if_fail(bus!=NULL);
	while(!done && !ly_db_md_eos)
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
		done = ly_db_read_metadata_cb(bus,message,md);
		gst_message_unref(message);
	}
	gst_object_unref(bus);
}

void ly_db_build_pipeline_cb(GstElement *decodebin,GstPad *pad, gboolean last, gpointer data)
{
	static GstCaps *caps;
	static GstPad *sink_pad;
	
	caps = gst_pad_get_caps(pad);
	if(gst_caps_is_empty(caps) || gst_caps_is_any(caps))
	{
		ly_global_debug("warnning",_("Decoded pad with no caps or any caps, boring."));
	}
	else
	{
		sink_pad = gst_element_get_static_pad(GST_ELEMENT(data), "sink");
		gst_pad_link(pad, sink_pad);
		gst_object_unref(sink_pad);
	}
	gst_caps_unref (caps);
}

gchar **ly_db_get_mp3_tag_id3v1(FILE *file)
{
	gchar extra_encoding[128]="GB18030";
	if(!ly_conf_get("db_extra_encoding","%s",extra_encoding))
	{
		ly_conf_set("db_extra_encoding","%s",extra_encoding);
	}

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

gchar **ly_db_get_mp3_tag_id3v2(FILE *file)
{
	gchar extra_encoding[128]="GB18030";
	if(!ly_conf_get("db_extra_encoding","%s",extra_encoding))
	{
		ly_conf_set("db_extra_encoding","%s",extra_encoding);
	}
	
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
	fread(a, 10, 1, file);
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

gchar **ly_db_get_mp3_tag(gchar *filename)
{
	FILE *file;
	gchar **tag;
	
	if(filename==NULL || *filename==0)
	{
		ly_global_debug("warnning",_("Get tag: Invaild filename.\n"));
		return NULL;
	}
	file=fopen(filename, "rb");
	if(file==NULL)
	{
		ly_global_debug("warnning",_("Get tag: Can't open file %s\n"), filename);
		return NULL;
	}
	/* Get ID3v2 tag, if it returns NULL, then try to get ID3v1 tag. */
	tag=ly_db_get_mp3_tag_id3v2(file);
	if(tag==NULL)
	{
		rewind(file);
		tag=ly_db_get_mp3_tag_id3v1(file);
	}
	fclose(file);
	return tag;
}

gint64 ly_db_get_last_insert_rowid()
{
	return sqlite3_last_insert_rowid(ly_db_conn);
}
