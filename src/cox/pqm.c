/*
 * pqm.h
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
#include "pqm.h"

/*
 * TYPES
 */


/*
 * VARIABLES
 */
LyMdhMetadata *ly_pqm_md=NULL;

/*
 * FUNCTIONS
 */

gboolean	ly_pqm_get_md_cb	(gpointer stmt, gpointer data);
 
/**
 * ly_pqm_init:
 *
 * Initialize the pqm module, it will be called by #ly_cox_init.
 */
void	ly_pqm_init	()
{
	int mid=1;
	if(!ly_reg_get("pqm_playing","%d",&mid))
	{
		ly_reg_set("pqm_playing","%d",mid);
	}
	ly_pqm_md=ly_pqm_get_md(mid);
}

/**
 * ly_pqm_fina:
 *
 * Initialize the pqm module, it will be called by #ly_cox_init.
 */
void	ly_pqm_fina	()
{
	if(ly_pqm_md)
	{
		ly_reg_set("pqm_playing","%d", ly_pqm_md->id);
	}
}

void		ly_pqm_add_md			(int id)
{
	char sql[1024]="";
	g_snprintf(sql,sizeof(sql),"UPDATE metadatas SET playing=ifnull((SELECT MAX(playing) FROM plist),0)+1 WHERE id=%d", id);
	ly_dbm_exec(sql, NULL, NULL);
}
void		ly_pqm_add_md_by_where_from_lib	(char *where)
{
	char *sql=NULL;
	char tmp[1024]="";
	g_snprintf(tmp,sizeof(tmp),"UPDATE metadatas SET playing=(ifnull((SELECT MAX(playing) FROM plist),0)+num)");
	if(where)
	{
		sql=g_strconcat(tmp, " WHERE ", where, NULL);
		ly_dbm_exec(sql, NULL, NULL);
		g_free(sql);
	}
	else
	{
		ly_dbm_exec(tmp, NULL, NULL);
	}
}
void		ly_pqm_add_md_by_where_from_plm	(int pid, char *where)
{
	char sql[1024]="";
	char tmp[1024]="";
	if(pid>0)
	{
		g_snprintf(tmp,sizeof(tmp),"UPDATE metadatas SET playing=(ifnull((SELECT MAX(playing) FROM plist),0)+(ifnull((SELECT connections.num FROM connections WHERE mid=metadatas.id AND pid=%d),1))) WHERE (id IN (SELECT mid FROM connections WHERE pid=%d))", pid, pid);
	}
	else
	{
		g_snprintf(tmp,sizeof(tmp),"UPDATE metadatas SET playing=(ifnull((SELECT MAX(playing) FROM plist),0)+1) WHERE (id IN (SELECT mid FROM connections))");
	}
		
	if(where)
	{
		g_snprintf(sql, sizeof(sql), "%s AND (%s)", tmp, where);
	}
	else
	{
		g_strlcpy(sql, tmp, sizeof(sql));
	}
	ly_dbm_exec(sql, NULL, NULL);
}
void		ly_pqm_del_md			(int id)
{
	char sql[1024]="";
	g_snprintf(sql,sizeof(sql),"UPDATE metadatas SET playing=0 WHERE id=%d", id);
	ly_dbm_exec(sql, NULL, NULL);
}
void		ly_pqm_del_md_by_where	(char *where)
{
	char *sql=NULL;
	char tmp[1024]="";
	g_snprintf(tmp,sizeof(tmp),"UPDATE metadatas SET playing=0");
	if(where)
	{
		sql=g_strconcat(tmp, " WHERE ", where, NULL);
		ly_dbm_exec(sql, NULL, NULL);
		g_free(sql);
	}
	else
	{
		ly_dbm_exec(tmp, NULL, NULL);
	}
}
void		ly_pqm_clear_md	()
{
	char sql[1024]="";
	g_snprintf(sql,sizeof(sql),"UPDATE metadatas SET playing=0");
	ly_dbm_exec(sql, NULL, NULL);
}
LyMdhMetadata*	ly_pqm_get_md			(int id)
{
	LyMdhMetadata *md=ly_mdh_new();
	if(!md)
		return NULL;
	char sql[10240]="";
	g_snprintf(sql, sizeof(sql), "SELECT * FROM plist WHERE id=%d", id);
	if(ly_dbm_exec(sql, ly_pqm_get_md_cb, md)>0)
	{
		return md;
	}
	ly_mdh_free(md);
	return NULL;
}

LyMdhMetadata*	ly_pqm_get_md_by_sql	(char *sql)
{
	if(!sql)
		return NULL;
	LyMdhMetadata *md=ly_mdh_new();
	if(!md)
		return NULL;

	if(ly_dbm_exec(sql, ly_pqm_get_md_cb, md)>0)
	{
		return md;
	}
	ly_mdh_free(md);
	return NULL;
}
gboolean	ly_pqm_get_md_cb	(gpointer stmt, gpointer data)
{
	LyMdhMetadata *md=(LyMdhMetadata *)data;
	md->id=sqlite3_column_int(stmt, 0);
	g_strlcpy(md->title,(const gchar *)sqlite3_column_text(stmt, 1),sizeof(md->title));
	g_strlcpy(md->artist,(const gchar *)sqlite3_column_text(stmt, 2),sizeof(md->artist));
	g_strlcpy(md->album,(const gchar *)sqlite3_column_text(stmt, 3),sizeof(md->album));
	g_strlcpy(md->start,(const gchar *)sqlite3_column_text(stmt, 4),sizeof(md->start));
	g_strlcpy(md->duration,(const gchar *)sqlite3_column_text(stmt, 5),sizeof(md->duration));
	g_strlcpy(md->uri,(const gchar *)sqlite3_column_text(stmt, 6),sizeof(md->uri));
	md->playing=sqlite3_column_int(stmt, 7);
	md->num=sqlite3_column_int(stmt, 8);
	md->flag=sqlite3_column_int(stmt, 9);
	md->tmpflag=sqlite3_column_int(stmt, 10);
	
	return FALSE;
}


LyMdhMetadata*	ly_pqm_get_current_md	()
{
	return ly_pqm_md;
}
void			ly_pqm_set_current_md	(int id)
{
	LyMdhMetadata *md=NULL;
	md=ly_pqm_get_md(id);
	if(!md)
	{
		ly_pqm_add_md(id);
		md=ly_pqm_get_md(id);
		if(!md)
			return;
	}
	ly_mdh_free(ly_pqm_md);
	ly_pqm_md=md;
	ly_msg_put("meta_changed", "core:pqm", NULL);
}
void		ly_pqm_set_rand			()
{
	if(!ly_pqm_md)
		return;
		
	char sql[1024]="";
	char where[1024]="";
	
	int flag_min=0;
	int flag_max=0;
	flag_min=(ly_pqm_md->flag/10)*10;
	flag_max=(ly_pqm_md->flag/10)*10+9;
	
	g_snprintf(where,sizeof(where),"(flag>=%d AND flag<=%d AND playing>0)", flag_min, flag_max);
	g_snprintf(sql,sizeof(sql),"SELECT * FROM plist WHERE %s ORDER BY RANDOM() LIMIT 1", where);
	ly_mdh_free(ly_pqm_md);
	ly_pqm_md=NULL;
	ly_pqm_md=ly_pqm_get_md_by_sql(sql);
	ly_msg_put("meta_changed","core:pqm", NULL);
}
void		ly_pqm_set_next			()
{
	if(!ly_pqm_md)
		return;
		
	char sql[1024]="";
	char where[1024]="";
	
	int flag_min=0;
	int flag_max=0;
	flag_min=(ly_pqm_md->flag/10)*10;
	flag_max=(ly_pqm_md->flag/10)*10+9;
	
	g_snprintf(where,sizeof(where),"(flag>=%d AND flag<=%d)", flag_min, flag_max);
	if(ly_pqm_md)
	{
		g_snprintf(sql,sizeof(sql),"SELECT * FROM plist WHERE playing>%d AND %s ORDER BY playing LIMIT 1",ly_pqm_md->playing, where);
		ly_mdh_free(ly_pqm_md);
		ly_pqm_md=NULL;
		ly_pqm_md=ly_pqm_get_md_by_sql(sql);
	}
	if(!ly_pqm_md)
	{
		g_snprintf(sql,sizeof(sql),"SELECT * FROM plist WHERE %s ORDER BY playing LIMIT 1", where);
		ly_pqm_md=ly_pqm_get_md_by_sql(sql);
	}
	ly_msg_put("meta_changed","core:pqm",NULL);
	return;
}
void		ly_pqm_set_prev			()
{
	if(!ly_pqm_md)
		return;
		
	char sql[1024]="";
	char where[1024]="";
	
	int flag_min=0;
	int flag_max=0;
	flag_min=(ly_pqm_md->flag/10)*10;
	flag_max=(ly_pqm_md->flag/10)*10+9;
	
	g_snprintf(where,sizeof(where),"(flag>=%d AND flag<=%d)", flag_min, flag_max);
	if(ly_pqm_md)
	{
		g_snprintf(sql,sizeof(sql),"SELECT * FROM plist WHERE playing<%d AND playing>0 AND %s ORDER BY playing DESC LIMIT 1",ly_pqm_md->playing, where);
		ly_mdh_free(ly_pqm_md);
		ly_pqm_md=NULL;
		ly_pqm_md=ly_pqm_get_md_by_sql(sql);
	}
	if(!ly_pqm_md)
	{
		g_snprintf(sql,sizeof(sql),"SELECT * FROM plist WHERE %s ORDER BY playing DESC LIMIT 1", where);
		ly_pqm_md=ly_pqm_get_md_by_sql(sql);
	}
	ly_msg_put("meta_changed","core:pqm",NULL);
	return;
}






