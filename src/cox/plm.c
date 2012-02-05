/*
 * plm.c
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
#include "plm.h"


/*
 * VARIABLES
 */
int ly_plm_import_pid=-1;

/*
 * FUNCTIONS
 */
gboolean	ly_plm_import_pl_check_exist_cb(gpointer stmt, gpointer data);
gboolean	ly_plm_get_id_cb	(gpointer stmt, gpointer data);

void			ly_plm_init				()
{
	
}
void			ly_plm_fina				()
{
	
}

int		ly_plm_add_pl			(char *name)
{
	char sql[1024]="";
	if(!name || g_str_equal(name , ""))
	{
		g_snprintf(sql,sizeof(sql),"INSERT INTO playlists (name, num) VALUES ('P-'||ABS(RANDOM()%1000), ifnull((SELECT MAX(num) FROM playlists),0)+1)");
		while(ly_dbm_exec(sql, NULL, NULL)<0)
		{
			;
		}
	}
	else
	{
		char tmp[1024]="";
		g_strlcpy(tmp, name, sizeof(tmp));
		ly_dbm_replace_str(tmp, sizeof(tmp));
		g_snprintf(sql,sizeof(sql),"INSERT INTO playlists (name, num) VALUES ('%s', ifnull((SELECT MAX(num) FROM playlists),0)+1)", tmp);
		if(ly_dbm_exec(sql, NULL, NULL)<0)
		{
			ly_msg_put("error", "core:plm", _("Playlist exist！"));
			return -1;
		}
	}
	ly_msg_put("plm_update", "core:plm", NULL);
	return ly_dbm_get_last_insert_rowid();
}
void		ly_plm_del_pl			(int pid)
{
	char sql[1024]="";
	g_snprintf(sql,sizeof(sql),"DELETE FROM playlists WHERE id=%d", pid);
	ly_dbm_exec(sql, NULL, NULL);
	ly_msg_put("plm_update", "core:plm", NULL);
}
void		ly_plm_del_pl_by_where	(char *where)
{
	char sql[1024]="";
	if(where)
		g_snprintf(sql,sizeof(sql),"DELETE FROM playlists WHERE %s", where);
	else
		g_snprintf(sql,sizeof(sql),"DELETE FROM playlists");
	ly_dbm_exec(sql, NULL, NULL);
	ly_msg_put("plm_update", "core:plm", NULL);
}

void		ly_plm_clear_pl			()
{
	char sql[1024]="";
	g_snprintf(sql,sizeof(sql),"DELETE FROM playlists");
	ly_dbm_exec(sql, NULL, NULL);
	ly_msg_put("plm_update", "core:plm", NULL);
}

void	ly_plm_rename_pl		(int pid, char *name)
{
	if((!name)||(g_str_equal(name,"")))
	{
		ly_msg_put("error", "core:plm", "Illegal playlist name. Please try again.");
		return;
	}
	char sql[1024]="";
	g_snprintf(sql,sizeof(sql),"UPDATE playlists SET name='%s' WHERE id=%d", name, pid);
	if(ly_dbm_exec(sql, NULL, NULL)<0)
	{
		ly_msg_put("error", "core:plm", "Cannot find particular playlist.");
		return;
	}
	ly_msg_put("plm_update", "core:plm", NULL);
}


gboolean		ly_plm_import_pl		(char *path)
{
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		ly_msg_put("error", "core:plm", _("playlist file doesn't exist!"));
		return FALSE;
	}
	int pid=-1;
	pid=ly_plm_add_pl(NULL);
	if(pid<=0)
		return FALSE;
	ly_plm_import_pid=pid;
	char *s = ly_gla_uri_get_suffix(path);
	if(g_ascii_strcasecmp(s, "cue")==0)
	{
		ly_plm_import_pl_from_cue(pid, path);
	}
	else if(g_ascii_strcasecmp(s, "m3u")==0)
	{
		ly_plm_import_pl_from_m3u(pid, path);
	}
	else
	{
		ly_msg_put("error", "core:plm", _("Illegal playlist file type!"));
		return FALSE;
	}
	g_free(s);
	ly_msg_put("plm_update", "core:plm", NULL);
	return TRUE;
}

gboolean		ly_plm_export_pl		(int pid, char *path)
{
	return TRUE;
}

void		ly_plm_add_md			(int pid, int mid)
{
	char sql[10240]="";
	g_snprintf(sql, sizeof(sql),
				" "\
				"INSERT OR IGNORE "\
				"INTO connections (pid, mid, num) "\
				"VALUES (%d, %d, IFNULL((SELECT MAX(num) FROM connections WHERE pid=%d),0)+1)",
				pid, mid, pid);
	ly_dbm_exec(sql, NULL, NULL);
}

void		ly_plm_add_md_in_order	(int pid, int mid, int num)
{
	char sql[10240]="";
	g_snprintf(sql, sizeof(sql),
				" "\
				"INSERT OR IGNORE "\
				"INTO connections (pid, mid, num) "\
				"VALUES (%d, %d, %d)",
				pid, mid, num);
	ly_dbm_exec(sql, NULL, NULL);
}

void
ly_plm_del_md			(int pid, int mid)
{
	char sql[10240]="";
	g_snprintf(sql, sizeof(sql),
				" "\
				"DELETE "\
				"FROM connections "\
				"WHERE pid=%s AND mid=%s",
				pid, mid);
	ly_dbm_exec(sql, NULL, NULL);
}
void
ly_plm_del_md_by_where	(int pid, char *where)
{
	char *sql=NULL;
	char *tmpsql=NULL;
	char tmp[1024]="";
	if(pid>0)
		g_snprintf(tmp, sizeof(tmp),"DELETE FROM connections WHERE pid=%d", pid);
	else
		g_snprintf(tmp, sizeof(tmp),"DELETE FROM connections WHERE 1");
	if(where)
	{
		tmpsql=g_strconcat(tmp, " AND(", where, NULL);
		sql=g_strconcat(tmpsql, ")", NULL);
		g_free(tmpsql);
		ly_dbm_exec(sql, NULL, NULL);
		g_free(sql);
	}
	else
	{
		ly_dbm_exec(tmp, NULL, NULL);
	}
}
void
ly_plm_clear_md			(int pid)
{
	char sql[10240]="";
	if(pid>0)
	{
		g_snprintf(sql, sizeof(sql),"DELETE FROM connections WHERE pid=%d", pid);
	}
	else
	{
		g_snprintf(sql, sizeof(sql),"DELETE FROM connections");
	}
	ly_dbm_exec(sql, NULL, NULL);
}

gboolean
ly_plm_import_pl_from_cue(int pid, char *path)
{
	if(pid<=0)
		return FALSE;
	gchar *buffer=NULL;
	g_file_get_contents(path, &buffer, NULL, NULL);
	if(!buffer)
	{
		ly_msg_put("error", "core:plm", _("Cannot open playlsit file!"));
	}
    
    char *sql=NULL;
    char *tmpsql=NULL;
    char tmp[1024]="";    
    char uri[1024]="";
    char *dir=NULL;
    int i;
    gchar *word=NULL; 
    LyMdhMetadata *md=NULL;
    int mid=-1;
    GList *list=NULL;
    GList *p=NULL;
    
    char genre[1024]="";
    char diskid[128]="";
    char artist[1024]="";
    char album[1024]="";
    char date[1024]="";
    char comment[1024]="";
    
    
    GRegex* regex;
    GMatchInfo *match_info;
    GError *error = NULL;
    
    /*
     * GENRE
     */
    regex = g_regex_new("REM GENRE \"[^\n\"]*\"", 0, 0, &error);
    g_regex_match(regex, buffer, 0, &match_info);
    if (g_match_info_matches(match_info))
    {
        gint count = g_match_info_get_match_count( match_info );
        word = g_match_info_fetch(match_info, 0);
		g_strlcpy(genre, word, sizeof(genre));
		g_free(word);
    }
    g_match_info_free(match_info);
    g_regex_unref(regex);
    
    /*
     * DATE
     */
    regex = g_regex_new("REM DATE [^\n]*", 0, 0, &error);
    g_regex_match(regex, buffer, 0, &match_info);
    if (g_match_info_matches(match_info))
    {
        gint count = g_match_info_get_match_count( match_info );
        word = g_match_info_fetch(match_info, 0);
		g_strlcpy(genre, word, sizeof(genre));
		g_free(word);
    }
    g_match_info_free(match_info);
    g_regex_unref(regex);
    
    /*
     * DISCID
     */
    regex = g_regex_new("REM DISCID [^\n]*", 0, 0, &error);
    g_regex_match(regex, buffer, 0, &match_info);
    if (g_match_info_matches(match_info))
    {
        gint count = g_match_info_get_match_count( match_info );
        word = g_match_info_fetch(match_info, 0);
		g_strlcpy(genre, word, sizeof(genre));
		g_free(word);
    }
    g_match_info_free(match_info);
    g_regex_unref(regex);
    
    /*
     * COMMENT
     */
    regex = g_regex_new("REM COMMENT \"[^\n\"]*\"", 0, 0, &error);
    g_regex_match(regex, buffer, 0, &match_info);
    if (g_match_info_matches(match_info))
    {
        gint count = g_match_info_get_match_count( match_info );
        word = g_match_info_fetch(match_info, 0);
		g_strlcpy(genre, word, sizeof(genre));
		g_free(word);
    }
    g_match_info_free(match_info);
    g_regex_unref(regex);
    
    /*
     * ARTIST
     */
    regex = g_regex_new("PERFORMER \"[^\n\"]*\"", 0, 0, &error);
    g_regex_match(regex, buffer, 0, &match_info);
    if (g_match_info_matches(match_info))
    {
        gint count = g_match_info_get_match_count( match_info );
        word = g_match_info_fetch(match_info, 0);
		g_strlcpy(genre, word, sizeof(genre));
		g_free(word);
    }
    g_match_info_free(match_info);
    g_regex_unref(regex);
    
    /*
     * ALBUM
     */
    regex = g_regex_new("TITLE \"[^\n\"]*\"", 0, 0, &error);
    g_regex_match(regex, buffer, 0, &match_info);
    if (g_match_info_matches(match_info))
    {
        gint count = g_match_info_get_match_count( match_info );
        word = g_match_info_fetch(match_info, 0);
		g_strlcpy(genre, word, sizeof(genre));
		g_free(word);
    }
    g_match_info_free(match_info);
    g_regex_unref(regex);

    /*
     * URI
     */
    regex = g_regex_new("FILE \"[^\n\"]*\"", 0, 0, &error);
    g_regex_match(regex, buffer, 0, &match_info);
    if (g_match_info_matches(match_info))
    {
        gint count = g_match_info_get_match_count( match_info );
        word = g_match_info_fetch(match_info, 0);
        if(*word!='/')
        {
       		dir = ly_gla_uri_get_dir(path);
			g_snprintf(uri, sizeof(uri), "file://%s%s", dir, word);
			g_free(dir);
		}
		else
		{
			g_snprintf(uri, sizeof(uri), "file://%s", word);
		}
		g_free(word);
    }
    g_match_info_free(match_info);
    g_regex_unref(regex);
    
    /*
     * TRACKS
     */
    //TODO
    
    
    g_free(buffer);
    
    if(!list)
    	return TRUE;
    ly_dbm_exec("begin",NULL,NULL);
    
	sql=g_strconcat("SELECT id, uri FROM metadatas WHERE flag=0 AND (", NULL);
	g_strlcpy(tmp, (char *)(list->data), sizeof(tmp));
	ly_dbm_replace_str(tmp, sizeof(tmp));
	tmpsql=g_strconcat(sql, " uri='", tmp,"'", NULL);
	g_free(sql);
	sql=tmpsql;
	p=list->next;
	while(p)
	{
		g_strlcpy(tmp, (char *)(p->data), sizeof(tmp));
		ly_dbm_replace_str(tmp, sizeof(tmp));
		tmpsql=g_strconcat(sql, " OR uri='", tmp,"'", NULL);
		g_free(sql);
		sql=tmpsql;
		p=p->next;
	}
	tmpsql=g_strconcat(sql, ")", NULL);
	g_free(sql);
	sql=tmpsql;
	ly_dbm_exec(sql, ly_plm_import_pl_check_exist_cb, &list);
	g_free(sql);
	p=list;
	while(p)
	{
		if(!p->data)
		{
			p=p->next;
			continue;
		}

		md=ly_mdh_new_with_uri((char *)(p->data));
		g_free(p->data);
		p->data=NULL;
		mid=ly_lib_add_md(md);
		if(mid<=0)
		{
			mid=ly_lib_get_id(md);
		}
		if(mid<=0)
		{
			continue;
		}
		ly_plm_add_md_in_order(pid, mid, g_list_position(list, p)+1);
	}
	g_list_free(list);
	ly_dbm_exec("commit",NULL,NULL);
	return TRUE;
}

gboolean
ly_plm_import_pl_from_m3u(int pid, char *path)
{
	if(pid<=0)
		return FALSE;
	gchar *buffer=NULL;
	g_file_get_contents(path, &buffer, NULL, NULL);
	if(!buffer)
	{
		ly_msg_put("error", "core:plm", _("Cannot open playlsit file!"));
	}
    
    char *sql=NULL;
    char *tmpsql=NULL;
    char tmp[1024]="";    
    int i;
    gchar *word=NULL; 
    LyMdhMetadata *md=NULL;
    int mid=-1;
    GList *list=NULL;
    GList *p=NULL;
    
    GRegex* regex;
    GMatchInfo *match_info;
    GError *error = NULL;
    regex = g_regex_new("/[^\n#]*", 0, 0, &error);
    g_regex_match(regex, buffer, 0, &match_info);
    while (g_match_info_matches(match_info))
    {
        gint count = g_match_info_get_match_count( match_info );
        for (i=0; i<count; i++ )
        {
            word = g_match_info_fetch(match_info, i);
            list=g_list_append(list, g_strconcat("file://", word, NULL));
            g_free(word);
        }
        g_match_info_next(match_info, NULL);
    }
    g_match_info_free(match_info);
    g_regex_unref(regex);
    g_free(buffer);
    
    if(!list)
    	return TRUE;
    ly_dbm_exec("begin",NULL,NULL);
    
	sql=g_strconcat("SELECT id, uri FROM metadatas WHERE flag=0 AND (", NULL);
	g_strlcpy(tmp, (char *)(list->data), sizeof(tmp));
	ly_dbm_replace_str(tmp, sizeof(tmp));
	tmpsql=g_strconcat(sql, " uri='", tmp,"'", NULL);
	g_free(sql);
	sql=tmpsql;
	p=list->next;
	while(p)
	{
		g_strlcpy(tmp, (char *)(p->data), sizeof(tmp));
		ly_dbm_replace_str(tmp, sizeof(tmp));
		tmpsql=g_strconcat(sql, " OR uri='", tmp,"'", NULL);
		g_free(sql);
		sql=tmpsql;
		p=p->next;
	}
	tmpsql=g_strconcat(sql, ")", NULL);
	g_free(sql);
	sql=tmpsql;
	ly_dbm_exec(sql, ly_plm_import_pl_check_exist_cb, &list);
	g_free(sql);
	p=list;
	while(p)
	{
		if(!p->data)
		{
			p=p->next;
			continue;
		}

		md=ly_mdh_new_with_uri((char *)(p->data));
		g_free(p->data);
		p->data=NULL;
		mid=ly_lib_add_md(md);
		if(mid<=0)
		{
			mid=ly_lib_get_id(md);
		}
		if(mid<=0)
		{
			continue;
		}
		ly_plm_add_md_in_order(pid, mid, g_list_position(list, p)+1);
	}
	g_list_free(list);
	ly_dbm_exec("commit",NULL,NULL);
	return TRUE;
}



gboolean
ly_plm_import_pl_check_exist_cb(gpointer stmt, gpointer data)
{
	gint mid=-1;
	gchar *uri=NULL;
	GList *list=*(GList**)data;
	GList *p=list;
	mid=(int)sqlite3_column_int(stmt, 0);
	uri=g_strconcat((const gchar *)sqlite3_column_text(stmt, 1),NULL);

	while(p)
	{
		if(!p->data)
		{
			p=p->next;
			continue;
		}
		if(g_str_equal(uri,p->data))
		{
			ly_plm_add_md_in_order(ly_plm_import_pid, mid, g_list_position(list, p)+1);
			g_free(p->data);
			p->data=NULL;
			break;
		}
		p=p->next;
	}
	g_free(uri);
	return FALSE;
}


int			ly_plm_get_id			(char *name)
{
	if((!name)||(g_str_equal(name, "")))
		return -1;
	char tmp[1024]="";
	g_strlcpy(tmp, name, sizeof(tmp));
	int id=-1;
	char sql[10240]="";
	ly_dbm_replace_str(tmp, sizeof(tmp));
	g_snprintf(sql, sizeof(sql), "SELECT id FROM playlists WHERE name='%d'", tmp);
	if(ly_dbm_exec(sql, ly_plm_get_id_cb, &id)>0);
	{
		return id;
	}
	return -1;
}

gboolean	ly_plm_get_id_cb	(gpointer stmt, gpointer data)
{
	int *id=data;
	*id=(int)sqlite3_column_int(stmt, 0);
	return FALSE;
}

