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
 * FUNCTIONS
 */
void			ly_plm_init				()
{
	
}
void			ly_plm_fina				()
{
	
}

void		ly_plm_add_pl			(char *name)
{
	if(!name || g_str_equal(name , ""))
	{
		ly_msg_put("error", "core:plm", _("Illegal playlist name!"));
		return;
	}

	char sql[1024]="";
	char tmp[1024]="";
	g_strlcpy(tmp, name, sizeof(tmp));
	ly_dbm_replace_str(tmp, sizeof(tmp));
	g_snprintf(sql,sizeof(sql),"INSERT INTO playlists (name, num) VALUES ('%s', ifnull((SELECT MAX(num) FROM playlists),0)+1)", tmp);
	ly_dbm_exec(sql, NULL, NULL);
}
void		ly_plm_del_pl			(int pid)
{
	char sql[1024]="";
	g_snprintf(sql,sizeof(sql),"DELETE FROM playlists WHERE id=%d", pid);
	ly_dbm_exec(sql, NULL, NULL);
}
void		ly_plm_del_pl_by_where	(char *where)
{
	char sql[1024]="";
	if(where)
		g_snprintf(sql,sizeof(sql),"DELETE FROM playlists WHERE %s", where);
	else
		g_snprintf(sql,sizeof(sql),"DELETE FROM playlists");
	ly_dbm_exec(sql, NULL, NULL);
}

gboolean		ly_plm_import_pl		(char *name, char *path)
{
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		ly_msg_put("error", "core:plm", _("playlist file doesn't exist!"));
		return FALSE;
	}
	
	char *s = ly_gla_uri_get_suffix(path);
	if(g_ascii_strcasecmp(s, "cue")==0)
	{
		ly_plm_import_pl_from_cue(name, path);
	}
	else if(g_ascii_strcasecmp(s, "m3u")==0)
	{
		ly_plm_import_pl_from_m3u(name, path);
	}
	else
	{
		ly_msg_put("error", "core:plm", _("Illegal playlist file type!"));
		return FALSE;
	}
	g_free(s);
	return TRUE;
}

gboolean		ly_plm_export_pl		(char *name, char *path)
{
	return TRUE;
}

void		ly_plm_add_md			(int pid, int mid)
{
	char sql[10240]="";
	g_snprintf(sql, sizeof(sql),
				" "\
				"INSERT OR IGNORE "\
				"INTO connections (pid, mid) "\
				"VALUES (%d, %d)",
				pid, mid);
	ly_dbm_exec(sql, NULL, NULL);
}
void		ly_plm_del_md			(int pid, int mid)
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
void		ly_plm_del_md_by_where	(int pid, char *where)
{
	char sql[10240]="";
	if(where)
		g_snprintf(sql, sizeof(sql),"DELETE FROM connections WHERE %s", where);
	else
		g_snprintf(sql, sizeof(sql),"DELETE FROM connections");
	ly_dbm_exec(sql, NULL, NULL);
}



gboolean		ly_plm_import_pl_from_cue(char *name, char *path)
{
	return TRUE;
}
gboolean		ly_plm_import_pl_from_m3u(char *name, char *path)
{
	return TRUE;
}
