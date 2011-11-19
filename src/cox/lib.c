/*
 * lib.c
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
#include "lib.h"

/*
 * VARIABLES
 */
const char *ly_lib_audio_table[]={
	"mp3",
	"wma",
	"flac",
	"cda",
	"mid",
	"midi",
	"ogg",
	"wav",
	"acc",
	"ape"};
enum{
	AUDIO_MP3,
	AUDIO_WMA,
	AUDIO_FLAC,
	AUDIO_CDA,
	AUDIO_MID,
	AUDIO_MIDI,
	AUDIO_OGG,
	AUDIO_WAV,
	AUDIO_ACC,
	AUDIO_APE,
	AUDIO_COUNT};
const char *ly_lib_video_table[]={
	"avi",
	"wmv",
	"rmvb",
	"rm",
	"mkv"};
enum{
	VIDEO_AVI,
	VIDEO_WMV,
	VIDEO_RMVB,
	VIDEO_RM,
	VIDEO_MKV,
	VIDEO_COUNT};

/*
 * FUNCTIONS
 */
gboolean	ly_lib_get_md_cb	(gpointer stmt, gpointer data);
gboolean ly_lib_check_library_check_file_cb(gpointer stmt, gpointer data);
gboolean ly_lib_check_library_check_newfile_cb(gpointer stmt, gpointer data);

void	ly_lib_init()
{
	/*
	 * library setting
	 */
	char path[10240]="";
	g_snprintf(path,sizeof(path),"%smusic/", LY_GLA_HOMEDIR);
	
	if(!ly_reg_get("lib_path","%s",path))
	{
		ly_reg_set("lib_path","%s",path);
	}
	
		
	/*
	 * check library
	 */
	int autocheck=0;
	if(!ly_reg_get("lib_autocheck", "%d", &autocheck))
	{
		ly_reg_set("lib_autocheck", "%d", autocheck);
	}
	if(autocheck)
	{
		ly_lib_check_library();
	}

}

void	ly_lib_fina()
{
}

void	ly_lib_add_md(LyMdhMetadata *md)
{
	ly_dbm_replace_str(md->title, sizeof(md->title));
	ly_dbm_replace_str(md->artist, sizeof(md->artist));
	ly_dbm_replace_str(md->album, sizeof(md->album));
	ly_dbm_replace_str(md->uri, sizeof(md->uri));

	char sql[10240]="";
	g_snprintf(sql, sizeof(sql), "INSERT OR IGNORE INTO metadatas (title, artist, album, start, duration, uri, playing, num, flag, tmpflag) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', 0, IFNULL((SELECT MAX(num) FROM metadatas),0)+1, 0, 1)", md->title, md->artist, md->album, md->start, md->duration, md->uri);
	ly_dbm_exec(sql, NULL, NULL);
}

void	ly_lib_del_md(int id)
{
	char sql[10240];
	g_snprintf(sql, sizeof(sql), "DELETE FROM metadatas WHERE id=%d", id);
	ly_dbm_exec(sql, NULL, NULL);
}

void	ly_lib_del_md_by_where(char *where)
{
	char sql[1024]="";
	if(where)
		g_snprintf(sql,sizeof(sql),"DELETE FROM metadatas WHERE %s", where);
	else
		g_snprintf(sql,sizeof(sql),"DELETE FROM metadatas");
	ly_dbm_exec(sql, NULL, NULL);
}


LyMdhMetadata*	ly_lib_get_md	(int id)
{
	LyMdhMetadata *md=ly_mdh_new();
	if(!md)
		return NULL;

	char sql[10240]="";
	g_snprintf(sql, sizeof(sql), "SELECT * FROM metadatas WHERE id=%d", id);
	if(ly_dbm_exec(sql, ly_lib_get_md_cb, md)>0);
	{
		return md;
	}
	ly_mdh_free(md);
	return NULL;
}

LyMdhMetadata*	ly_lib_get_md_by_sql	(char *sql)
{
	if(!sql)
		return NULL;
	LyMdhMetadata *md=ly_mdh_new();
	if(!md)
		return NULL;

	if(ly_dbm_exec(sql, ly_lib_get_md_cb, md)>0);
	{
		return md;
	}
	ly_mdh_free(md);
	return NULL;
}

gboolean	ly_lib_get_md_cb	(gpointer stmt, gpointer data)
{
	LyMdhMetadata *md=(LyMdhMetadata *)data;
	g_strlcpy(md->title,(const gchar *)sqlite3_column_text(stmt, 1),sizeof(md->title));
	g_strlcpy(md->artist,(const gchar *)sqlite3_column_text(stmt, 2),sizeof(md->artist));
	g_strlcpy(md->album,(const gchar *)sqlite3_column_text(stmt, 3),sizeof(md->album));
	g_strlcpy(md->start,(const gchar *)sqlite3_column_text(stmt, 4),sizeof(md->start));
	g_strlcpy(md->duration,(const gchar *)sqlite3_column_text(stmt, 5),sizeof(md->duration));
	g_strlcpy(md->uri,(const gchar *)sqlite3_column_text(stmt, 6),sizeof(md->uri));
	return FALSE;
}

gboolean		ly_lib_check_is_audio	(char *uri)
{
	gint	i=0;
	gchar	*p=NULL;
	p=ly_gla_uri_get_suffix(uri);
	
	if(!p)
		return FALSE;
	
	for(i=0;i<AUDIO_COUNT;i++)
	{
		if(g_str_equal(p, ly_lib_audio_table[i]))
		{
			return TRUE;
		}
	}
	return FALSE;
}
gboolean		ly_lib_check_is_video	(char *uri)
{
	gint	i=0;
	gchar	*p=NULL;
	p=ly_gla_uri_get_suffix(uri);
	
	if(!p)
		return FALSE;
	
	for(i=0;i<VIDEO_COUNT;i++)
	{
		if(g_str_equal(p, ly_lib_video_table[i]))
		{
			return TRUE;
		}
	}
	return FALSE;
}




void			ly_lib_check_library		()
{
	/*
	 * get library path
	 */
	gchar path[10240]="";
	gchar tmppath[10240]="";
	
	g_snprintf(path,sizeof(path),"%smusic/", LY_GLA_HOMEDIR);
	ly_reg_get("lib_path","%s",path);
	g_strlcpy(tmppath, path, sizeof(tmppath));
	
	/*
	 * get filenames
	 */
	ly_dbm_exec("begin",NULL,NULL);
	gchar tmpstr[1024]="";
	GList *list=ly_gla_traverse_dir(path, 5, TRUE);
	GList *p=list;
	gchar *tmpsql=NULL;
	gchar *sql=NULL;
	ly_dbm_replace_str(tmppath,sizeof(tmppath));
	sql=g_strconcat("UPDATE metadatas SET tmpflag=1 WHERE uri like 'file://",tmppath,"%' AND ( uri='0'",NULL);
	while(p)
	{
		if(!ly_lib_check_is_audio(p->data))
		{
			p=p->next;
			continue;
		}
		g_strlcpy(tmpstr,(gchar *)(p->data),sizeof(tmpstr));
		ly_dbm_replace_str(tmpstr,sizeof(tmpstr));
		tmpsql=g_strconcat(sql,"OR uri='",tmpstr,"' ",NULL);
		g_free(sql);
		sql=tmpsql;
		p=p->next;
	}
	
	tmpsql=g_strconcat(sql,")",NULL);
	g_free(sql);
	sql=tmpsql;
	ly_dbm_exec(sql,NULL,NULL);
	g_free(sql);
	tmpsql=NULL;
	
	sql=g_strconcat("DELETE FROM metadatas WHERE uri like 'file://",tmppath,"%' AND tmpflag=0",NULL);
	ly_dbm_exec(sql,NULL,NULL);
	g_free(sql);
	
	tmpsql=NULL;
	sql=g_strconcat("SELECT uri FROM metadatas WHERE tmpflag=0",NULL);
	ly_dbm_exec(sql, ly_lib_check_library_check_file_cb, &tmpsql);
	g_free(sql);
	if(tmpsql)
	{
		ly_dbm_exec(tmpsql,NULL,NULL);
		g_free(tmpsql);
		tmpsql=NULL;
	}
	sql=g_strconcat("UPDATE metadatas SET tmpflag=0",NULL);
	ly_dbm_exec(sql,NULL,NULL);
	g_free(sql);
	
	
	/*
	 * add new music
	 */
	sql=g_strconcat("SELECT uri FROM metadatas WHERE uri LIKE 'file://",tmppath,"%'",NULL);
	ly_dbm_exec(sql,ly_lib_check_library_check_newfile_cb,&list);
	g_free(sql);
	
	LyMdhMetadata *md=NULL;
	p=list;
	while(p)
	{
		if(!ly_lib_check_is_audio((gchar *)(p->data)))
		{
			g_free(p->data);
			p->data=NULL;
			p=p->next;
			continue;
		}
		md=ly_mdh_new_with_uri((gchar *)(p->data));
		if(md)
		{
			ly_lib_add_md(md);
		}
		ly_mdh_free(md);
		md=NULL;
		g_free(p->data);
		p->data=NULL;
		p=p->next;
	}
	g_list_free(list);
	ly_dbm_exec("commit",NULL,NULL);
}

gboolean ly_lib_check_library_check_file_cb(gpointer stmt, gpointer data)
{
	gchar uri[1024]="";
	gchar *sql=*(gchar **)data;
	g_strlcpy(uri,(const gchar *)sqlite3_column_text(stmt, 0),sizeof(uri));
	
	if(!g_file_test(uri+7,G_FILE_TEST_EXISTS))
	{
		ly_dbm_replace_str(uri,sizeof(uri));
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

gboolean ly_lib_check_library_check_newfile_cb(gpointer stmt, gpointer data)
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
