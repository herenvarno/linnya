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
gboolean	ly_plm_get_id_cb	(gpointer stmt, gpointer data);
gboolean	ly_plm_export_pl_cb	(gpointer stmt, gpointer data);

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
			ly_msg_put("error", "core:plm", _("Playlist exist!"));
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
	FILE *fp=fopen(path, "w+");
	if(!fp)
	{
		ly_msg_put("error", "core:plm", _("Cannot open a file to save playlist!"));
		return FALSE;
	}
	
	char sql[1024]="";
	g_snprintf(sql, sizeof(sql), "SELECT title, uri FROM metadatas, connections WHERE connections.pid=%d AND metadatas.id=connections.mid ORDER BY connections.num", pid);
	ly_dbm_exec(sql, ly_plm_export_pl_cb, fp);
	fclose(fp);
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
	gchar *buffer_tmp=NULL;
	g_file_get_contents(path, &buffer, NULL, NULL);
	if(!buffer)
	{
		ly_msg_put("error", "core:plm", _("Cannot open playlsit file!"));
		return FALSE;
	}
	
	char extra_encoding[1024]="GB18030";
	if(!ly_reg_get("dbm_extra_encoding", "%*[^\n(](%1023[^\n)]", extra_encoding))
	{
		ly_reg_set("dbm_extra_encoding", "Chinese Simplified (GB18030)");
	}
	if(!g_utf8_validate(buffer,-1,NULL))
	{
		buffer_tmp = g_convert(buffer, -1, "UTF-8", extra_encoding, NULL, NULL, NULL);
		g_free(buffer);
		buffer = buffer_tmp;
	}
	
	
	char *dir=ly_gla_uri_get_dir(path);
    
    char tmp[1024]="";
    LyMdhMetadata *md=NULL;
    LyMdhMetadata *md_prev=NULL;
    int mid=-1;
	char *time_str=NULL;
	
	char title[512]="";
	char artist[128]="";
	char comment[1024]="";
	char genre[128]="";
	char codec[128]="";
	int date=0;
	int track=0;
	char file[1024]="";
	char file_tmp[1024]="";
	
	int min=0;
	int sec=0;
	int frm=0;
	char temp[64]="";
    
    GRegex* regex;
    GError *error = NULL;
    regex = g_regex_new("#.*$", G_REGEX_MULTILINE, 0, &error);
	buffer_tmp=g_regex_replace(regex, buffer, strlen(buffer), 0, "",  0, NULL);
	g_free(buffer);
    g_regex_unref(regex);
    regex = g_regex_new("^[ \t]*", G_REGEX_MULTILINE, 0, &error);
	buffer=g_regex_replace(regex, buffer_tmp, strlen(buffer_tmp), 0, "",  0, NULL);
	g_free(buffer_tmp);
    g_regex_unref(regex);
	
	ly_dbm_exec("begin", NULL, NULL);
	
	char **array=NULL;
	int i=0;
	array=g_strsplit(buffer,"\n",-1);
	for(i=0; i<g_strv_length(array); i++)
	{
		if(sscanf(array[i], "%*[Tt]%*[Rr]%*[Aa]%*[Cc]%*[Kk]%*[ \t]%d", &track))
		{
			if(md!=NULL)
			{
				if(md_prev)
				{
					//duration
					if(g_str_equal(md_prev->duration, "00:00.00"))
					{
						time_str=ly_mdh_time_int2str(ly_mdh_time_str2int(md->start)-ly_mdh_time_str2int(md_prev->start));
						g_strlcpy(md_prev->duration, time_str, sizeof(md_prev->duration));
					}
					//title
					if(g_str_equal(md_prev->title, "unknown"))
					{
						g_snprintf(md_prev->title, sizeof(md_prev->title), "%s %d", title, md_prev->track);
					}
					//artist
					if(g_str_equal(md_prev->artist, "unknown")&&(!g_str_equal(artist, "")))
					{
						g_snprintf(md_prev->artist, sizeof(md_prev->title), "%s %d", md_prev->title, md_prev->track);
					}
					//album
					if(g_str_equal(md_prev->album, "unknown")&&(!g_str_equal(title, "")))
					{
						g_strlcpy(md_prev->album, title, sizeof(md_prev->album));
					}
					//comment
					if(g_str_equal(md_prev->comment, "unknown")&&(!g_str_equal(comment, "")))
					{
						g_strlcpy(md_prev->comment, comment, sizeof(md_prev->comment));
					}
					//genre
					if(g_str_equal(md_prev->genre, "unknown")&&(!g_str_equal(genre, "")))
					{
						g_strlcpy(md_prev->genre, genre, sizeof(md_prev->genre));
					}
					//uri
					if(g_str_equal(md_prev->uri, "unknown")&&(!g_str_equal(file, "")))
					{
						if(g_str_has_prefix(file, "/"))
						{
							g_snprintf(md_prev->uri, sizeof(md_prev->uri), "file://%s", file);
						}
						else
						{
							g_snprintf(md_prev->uri, sizeof(md_prev->uri), "file://%s%s", dir, file);
						}
					}
					//date
					md_prev->date=date;
					mid=ly_lib_add_md(md_prev);
					if(mid<=0)
					{
						mid=ly_lib_get_id(md_prev);
					}
					if(mid>0)
					{
						puts(md_prev->title);
						ly_plm_add_md(pid, mid);
					}
					ly_mdh_free(md_prev);
				}
				md_prev=md;
			}
			md=ly_mdh_new();
			if(!md)
			{
				return FALSE;
			}
			md->track=track;
			md->flag=1;
			i++;
			for(; i<g_strv_length(array); i++)
			{
				if(sscanf(array[i], "%*[Tt]%*[Rr]%*[Aa]%*[Cc]%*[Kk]%*[ \t]%d", &track))
				{
					i--;
					break;
				}
				if(sscanf(array[i], "%*[Tt]%*[Ii]%*[Tt]%*[Ll]%*[Ee]%*[ \t]\"%[^\"\n]\"", md->title))
				{
					continue;
				}
				if(sscanf(array[i], "%*[Pp]%*[Ee]%*[Rr]%*[Ff]%*[Oo]%*[Rr]%*[Mm]%*[Ee]%*[Rr]%*[ \t]\"%[^\"\n]\"", md->artist))
				{
					continue;
				}
				if(sscanf(array[i], "%*[Ff]%*[Ii]%*[Ll]%*[Ee]%*[ \t]\"%[^\"\n]\"%*[ \t]%[^\n]", file_tmp, md->codec))
				{
					if(g_str_has_prefix(file_tmp, "/"))
					{
						g_snprintf(md->uri, sizeof(md->uri), "file://%s", file_tmp);
					}
					else
					{
						g_snprintf(md->uri, sizeof(md->uri), "file://%s%s", dir, file_tmp);
					}
					continue;
				}
				if(sscanf(array[i], "%*[Ii]%*[Nn]%*[Dd]%*[Ee]%*[Xx]%*[ \t]00%*[ \t]%d:%d:%d", &min, &sec, &frm))
				{
					if(!md_prev)
					{
						continue;
					}
					g_snprintf(temp, sizeof(temp), "%d:%d.%d", min, sec, frm);
					time_str=ly_mdh_time_int2str(ly_mdh_time_str2int(temp)-ly_mdh_time_str2int(md_prev->start));
					g_strlcpy(md_prev->duration, time_str, sizeof(md_prev->duration));
					g_free(time_str);
					continue;
				}
				if(sscanf(array[i], "%*[Ii]%*[Nn]%*[Dd]%*[Ee]%*[Xx]%*[ \t]01%*[ \t]%d:%d:%d", &min, &sec, &frm))
				{
					g_snprintf(md->start, sizeof(md->start), "%d:%d.%d", min, sec, frm);
					continue;
				}
			}
			continue;
		}
		if(sscanf(array[i], "%*[Rr]%*[Ee]%*[Mm]%*[ \t]%*[Gg]%*[Ee]%*[Nn]%*[Rr]%*[Ee]%*[ \t]%[^\n]", genre))
		{
			continue;
		}
		if(sscanf(array[i], "%*[Rr]%*[Ee]%*[Mm]%*[ \t]%*[Dd]%*[Aa]%*[Tt]%*[Ee]%*[ \t]%d", &date))
		{
			continue;
		}
		if(sscanf(array[i], "%*[Rr]%*[Ee]%*[Mm]%*[ \t]%*[Cc]%*[Oo]%*[Mm]%*[Mm]%*[Ee]%*[Nn]%*[Tt]%*[ \t]\"%[^\"\n]\"", comment))
		{
			continue;
		}
		if(sscanf(array[i], "%*[Pp]%*[Ee]%*[Rr]%*[Ff]%*[Oo]%*[Rr]%*[Mm]%*[Ee]%*[Rr]%*[ \t]\"%[^\"\n]\"", artist))
		{
			continue;
		}
		if(sscanf(array[i], "%*[Tt]%*[Ii]%*[Tt]%*[Ll]%*[Ee]%*[ \t]\"%[^\"\n]\"", title))
		{
			continue;
		}
		if(sscanf(array[i], "%*[Ff]%*[Ii]%*[Ll]%*[Ee]%*[ \t]\"%[^\"\n]\"%*[ \t]%[^\n]", file, codec))
		{
			continue;
		}
	}

	if(md)
	{
		if(md_prev)
		{
			//duration
			if(g_str_equal(md_prev->duration, "00:00.00"))
			{
				time_str=ly_mdh_time_int2str(ly_mdh_time_str2int(md->start)-ly_mdh_time_str2int(md_prev->start));
				g_strlcpy(md_prev->duration, time_str, sizeof(md_prev->duration));
			}
			//title
			if(g_str_equal(md_prev->title, "unknown"))
			{
				g_snprintf(md_prev->title, sizeof(md_prev->title), "%s %d", title, md_prev->track);
			}
			//artist
			if(g_str_equal(md_prev->artist, "unknown")&&(!g_str_equal(artist, "")))
			{
				g_snprintf(md_prev->artist, sizeof(md_prev->title), "%s %d", md_prev->title, md_prev->track);
			}
			//album
			if(g_str_equal(md_prev->album, "unknown")&&(!g_str_equal(title, "")))
			{
				g_strlcpy(md_prev->album, title, sizeof(md_prev->album));
			}
			//comment
			if(g_str_equal(md_prev->comment, "unknown")&&(!g_str_equal(comment, "")))
			{
				g_strlcpy(md_prev->comment, comment, sizeof(md_prev->comment));
			}
			//genre
			if(g_str_equal(md_prev->genre, "unknown")&&(!g_str_equal(genre, "")))
			{
				g_strlcpy(md_prev->genre, genre, sizeof(md_prev->genre));
			}
			//uri
			if(g_str_equal(md_prev->uri, "unknown")&&(!g_str_equal(file, "")))
			{
				if(g_str_has_prefix(file, "/"))
				{
					g_snprintf(md_prev->uri, sizeof(md_prev->uri), "file://%s", file);
				}
				else
				{
					g_snprintf(md_prev->uri, sizeof(md_prev->uri), "file://%s%s", dir, file);
				}
			}
			//date
			md_prev->date=date;
			mid=ly_lib_add_md(md_prev);
			if(mid<=0)
			{
				mid=ly_lib_get_id(md_prev);
			}
			if(mid>0)
			{
				ly_plm_add_md(pid, mid);
			}
			ly_mdh_free(md_prev);
		}
		ly_mdh_free(md);
	}
	g_strfreev(array);
	ly_dbm_exec("commit", NULL, NULL);
	return TRUE;
}






gboolean
ly_plm_import_pl_from_m3u(int pid, char *path)
{
	if(pid<=0)
		return FALSE;
	gchar *buffer=NULL;
	gchar *buffer_tmp=NULL;
	g_file_get_contents(path, &buffer, NULL, NULL);
	if(!buffer)
	{
		ly_msg_put("error", "core:plm", _("Cannot open playlsit file!"));
		return FALSE;
	}
	char extra_encoding[1024]="GB18030";
	if(!ly_reg_get("dbm_extra_encoding", "%*[^\n(](%1023[^\n)]", extra_encoding))
	{
		ly_reg_set("dbm_extra_encoding", "Chinese Simplified (GB18030)");
	}
	if(!g_utf8_validate(buffer,-1,NULL))
	{
		buffer_tmp = g_convert(buffer, -1, "UTF-8", extra_encoding, NULL, NULL, NULL);
		g_free(buffer);
		buffer = buffer_tmp;
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

gboolean	ly_plm_export_pl_cb	(gpointer stmt, gpointer data)
{
	FILE *fp=(FILE *)data;
	const char *title=NULL;
	const char *location=NULL;
	
	title=sqlite3_column_text(stmt, 0);
	location=sqlite3_column_text(stmt, 1);
	
	if(!g_str_has_prefix(location, "file://"))
	{
		return FALSE;
	}
	
	char text[1024]="";
	g_snprintf(text, sizeof(text), "#EXTM3U %s\n%s\n", title, location+7);
	fputs(text, fp);
	
	return FALSE;
}

