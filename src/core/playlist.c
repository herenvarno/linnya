#include "playlist.h"

gboolean	ly_playlist_init				(void)
{
	return TRUE;
}
gboolean	ly_playlist_finalize			(void)
{
	return TRUE;
}

gboolean	ly_playlist_import_from_file	(char *path, char *name)
{
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		ly_msg_put("error", "core:playlist", _("playlist file doesn't exist!"));
		return FALSE;
	}
	char *s = ly_global_get_suffix(path);
	GString *string=g_string_new(s);
	string=g_string_ascii_down(string);
	if(g_str_equal(string->str, ".cue"))
	{
		ly_playlist_import_from_cue(path, name);
	}
	else if(g_str_equal(string->str, ".m3u"))
	{
		ly_playlist_import_from_m3u(path, name);
	}
	else
	{
		ly_msg_put("error", "core:playlist", _("Illegal playlist file type!"));
		g_string_free(string, TRUE);
		return FALSE;
	}
	g_string_free(string, TRUE);
	return TRUE;
}
	
	
	
gboolean	ly_playlist_import_from_cue		(char *path, char *name)
{
	char *contents=NULL;
	g_file_get_contents(path, &contents, NULL, NULL);
	if(!contents)
	{
		return FALSE;
	}
	return TRUE;
}


gboolean	ly_playlist_import_from_m3u		(char *path, char *name)
{
	if(name==NULL)
	{
		name=ly_global_get_filename(path);
	}
	if(!ly_playlist_add(name))
	{
		return FALSE;
	}
	int pid=ly_db_get_last_insert_rowid();
	
	FILE *fp=NULL;
	fp=fopen(path, "r");
	if(!fp)
	{
		ly_msg_put("error", "core:playing", _("Cannot open playlist file!"));
		return FALSE;
	}
	
	char line[1024]="";
	char uri[1024]="";
	char tmp[1024]="";
	int i=0;
	int mid=-1;
	lyDbMetadata *md=NULL;
	char sql[1024]="";
	
	ly_db_exec("begin", NULL, NULL);
	while(fgets(line, sizeof(line), fp))
	{
		line[strlen(line)-1]='\0';	//去掉'\n'
		for(i=0;i<strlen(line);i++)
		{
			if(line[i]!='#')
			{
				if(line[i]!=' '&&line[i]!='\t'&&line[i]!='\n')
				{
					sscanf(&line[i], "%1024[^#\n]%*[#\n\ ]$", tmp);
					g_snprintf(uri, sizeof(uri), "file://%s", tmp);
					ly_global_replace_str(uri,sizeof(uri),"'","''");
					g_snprintf(sql, sizeof(sql), "SELECT * FROM metadatas WHERE start='00:00.00' AND uri='%s'", uri);
					md=ly_db_get_metadata(sql);
					if(md)
					{
						mid=md->id;
						ly_playlist_add_metadata(pid, mid);
					}
					else
					{
						if(g_file_test(uri+7, G_FILE_TEST_EXISTS))
						{
							md=ly_db_read_metadata(uri);
							ly_db_add_metadata(md);
							mid=ly_db_get_last_insert_rowid();
							ly_playlist_add_metadata(pid, mid);
						}
						else
						{
							ly_msg_put("warning", "core:playlist", _("File is missing!"));
						}
					}
					if(md)
					{
						ly_db_free_metadata(md);
						md=NULL;
					}
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	ly_db_exec("commit", NULL, NULL);
	fclose(fp);
	return TRUE;
}

gboolean ly_playlist_test_is_exist(char *name)
{
	char sql[1024]="";
	g_snprintf(sql, sizeof(sql), "SELECT count(*) FROM connections WHERE name=%s", name);
	int count=ly_db_get_count(sql);
	if(count>0)
		return 	TRUE;
	return FALSE;
}
gboolean ly_playlist_test_is_empty (int pid)
{
	char sql[1024]="";
	g_snprintf(sql, sizeof(sql), "SELECT count(*) FROM connections WHERE pid=%d", pid);
	int count=ly_db_get_count(sql);
	if(count>0)
		return 	TRUE;
	return FALSE;
}

gboolean ly_playlist_add (const char *name)
{
	if(!name || g_str_equal(name ,""))
	{
		ly_msg_put("error", "core:playlist", _("Illegal playlist name!"));
		return FALSE;
	}

	char sql[1024]="";
	char tmp[1024]="";
	g_strlcpy(tmp, name, sizeof(tmp));
	ly_global_replace_str(tmp, sizeof(tmp), "'", "''");
	g_snprintf(sql,sizeof(sql),"INSERT INTO playlists (name, num) VALUES ('%s', ifnull((SELECT MAX(num) FROM playlists),0)+1)", tmp);
	if(!ly_db_exec(sql, NULL, NULL))
	{
		ly_msg_put("error", "core:playlist", _("Playlist already exist!"));
		return FALSE;
	}
	return TRUE;
}
gboolean ly_playlist_delete (int pid)
{
	char sql[1024]="";
	if(pid<0)
	{
		g_snprintf(sql,sizeof(sql),"DELETE FROM playlists");
	}
	else
	{
		g_snprintf(sql,sizeof(sql),"DELETE FROM playlists WHERE id=%d", pid);
	}
	if(!ly_db_exec(sql, NULL, NULL))
	{
		return FALSE;
	}
	return TRUE;
}
gboolean	ly_playlist_add_metadata		(int pid, int mid)
{
	char sql[1024]="";
	g_snprintf(sql,sizeof(sql),"SELECT count(*) FROM metadatas WHERE id=%d", mid);
	if(ly_db_exec(sql, NULL, NULL)<0)
	{
		return FALSE;
	}
	g_snprintf(sql,sizeof(sql),"SELECT count(*) FROM playlists WHERE id=%d", pid);
	if(ly_db_exec(sql, NULL, NULL)<0)
	{
		return FALSE;
	}
	g_snprintf(sql, sizeof(sql), "INSERT INTO connections (pid, mid, num) VALUES (%d, %d, ifnull((select max(num) from connections where pid=%d), 0)+1)", pid, mid, pid);
		
	return ly_db_exec(sql,NULL,NULL);
}

gboolean	ly_playlist_get_metadatas		(int pid, char *where, int limit, int offset, gpointer func, gpointer data)
{
	char sql[10240]="";
	if(where)
	{
		if(pid>=0)
		{
			g_snprintf(sql,sizeof(sql),"SELECT id,title,artist,album FROM metadatas tm, connections tc ORDER BY tc.num LIMIT %d, OFFSET %d WHERE tc.mid=tm.id AND pid=%d AND (%s)", limit, offset, pid, where);
		}
		else
		{
			g_snprintf(sql,sizeof(sql),"SELECT id,title,artist,album FROM metadatas tm, connections tc ORDER BY tc.num LIMIT %d, OFFSET %d WHERE tc.mid=tm.id AND (%s)", limit, offset, where);
		}
	}
	else
	{
		if(pid>=0)
		{
			g_snprintf(sql,sizeof(sql),"SELECT id,title,artist,album FROM metadatas tm, connections tc ORDER BY tc.num LIMIT %d, OFFSET %d WHERE tc.mid=tm.id AND pid=%d", limit, offset, pid);
		}
		else
		{
			g_snprintf(sql,sizeof(sql),"SELECT id,title,artist,album FROM metadatas tm, connections tc ORDER BY tc.num LIMIT %d, OFFSET %d WHERE tc.mid=tm.id", limit, offset);
		}
	}
	ly_global_replace_str(sql,sizeof(sql),"'","''");
	return ly_db_exec(sql, func, data);
	return TRUE;
}
gboolean	ly_playlist_get_playlists		(char *where, int limit, int offset,  gpointer func, gpointer data)
{
	char sql[10240]="";
	
	
	if(where)
	{
		g_snprintf(sql,sizeof(sql),"SELECT * FROM playlists LIMIT %d OFFSET %d ORDER BY num WHERE %s", limit, offset, where);
	}
	else
	{
		g_snprintf(sql,sizeof(sql),"SELECT * FROM connections LIMIT %d OFFSET %d ORDER BY num", limit, offset);
	}
	ly_global_replace_str(sql,sizeof(sql),"'","''");
	return ly_db_exec(sql, func, data);
}

