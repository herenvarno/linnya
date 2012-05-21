/*
 * dbm.c
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
#include "dbm.h"

/*
 * VARIABLES
 */
sqlite3		*ly_dbm_conn = NULL;
char		ly_dbm_file[1024] = "";

/*
 * FUNCTIONS
 */

gboolean ly_dbm_get_count_cb(gpointer stmt, gpointer data);

/**
 * ly_dbm_init:
 *
 * Initialize the dbm module, it will be called by #ly_cox_init.
 */
void	ly_dbm_init	()
{

	/*
	 * open sqlite3
	 */
	g_snprintf(ly_dbm_file, sizeof(ly_dbm_file), "%slinnya.db", LY_GLA_USERDIR);
	int rc=0;
	rc=sqlite3_open(ly_dbm_file, &ly_dbm_conn);
	if(rc != SQLITE_OK)
	{
		g_error(_("Cannot connect to database. Abort ..."));
	}

	ly_dbm_exec("begin", NULL, NULL);
	/*
	 * CREATE TABLE metadatas
	 */
	ly_dbm_exec(" "\
				"CREATE TABLE IF NOT EXISTS "\
				"metadatas( "\
				"id INTEGER PRIMARY KEY, "\
				"title TEXT, "\
				"artist TEXT, "\
				"album TEXT, "\
				"start TEXT NOT NULL, "\
				"duration TEXT NOT NULL, "\
				"uri TEXT NOT NULL, "\
				"playing INTEGER NOT NULL, "\
				"num INTEGER NOT NULL, "\
				"flag INTEGER NOT NULL, "\
				"tmpflag INTEGER, "\
				"codec TEXT, "\
				"genre TEXT, "\
				"date TEXT, "\
				"comment TEXT, "\
				"diskid TEXT)",
				NULL,NULL);
	/*
	 * CREATE TABLE playlists
	 */
	ly_dbm_exec(" "\
				"CREATE TABLE IF NOT EXISTS "\
				"playlists( "\
				"id INTEGER PRIMARY KEY, "\
				"name TEXT UNIQUE NOT NULL, "\
				"num INTEGER NOT NULL)",
				NULL,NULL);
	/*
	 * CREATE TABLE connections
	 */
	ly_dbm_exec(" "\
				"CREATE TABLE IF NOT EXISTS "\
				"connections( "\
				"pid INTEGER, "\
				"mid INTEGER, "\
				"num INTEGER, "\
				"FOREIGN KEY(mid) REFERENCES metadatas(id), "\
				"FOREIGN KEY(pid) REFERENCES playlists(id), "\
				"PRIMARY KEY(pid, mid))",
				NULL,NULL);
	/*
	 * CREATE TABLE equalizers
	 */
	ly_dbm_exec(" "\
				"CREATE TABLE IF NOT EXISTS "\
				"equalizers( "\
				"name TEXT PRIMARY KEY, "\
				"band0 REAL, "\
				"band1 REAL, "\
				"band2 REAL, "\
				"band3 REAL, "\
				"band4 REAL, "\
				"band5 REAL, "\
				"band6 REAL, "\
				"band7 REAL, "\
				"band8 REAL, "\
				"band9 REAL)",
				NULL, NULL);
	/*
	 * CREATE VIEW plist
	 */
	ly_dbm_exec(" "\
				"CREATE VIEW IF NOT EXISTS "\
				"plist AS "\
				"SELECT * FROM metadatas "\
				"WHERE playing>0 "\
				"ORDER BY playing",
				NULL,NULL);
	/*
	 * CREATE TRIGGER before_metadatas_delete
	 */
	ly_dbm_exec(" "\
				"CREATE TRIGGER IF NOT EXISTS "\
				"before_metadatas_delete "\
				"BEFORE DELETE ON metadatas "\
				"FOR EACH ROW "\
				"BEGIN "\
				"DELETE FROM connections WHERE mid=OLD.id; "\
				"END;",
				NULL,NULL);
	/*
	 * CREATE TRIGGER before_metadatas_insert
	 */
	ly_dbm_exec(" "\
				"CREATE TRIGGER IF NOT EXISTS "\
				"before_metadatas_insert "\
				"BEFORE INSERT ON metadatas "\
				"FOR EACH ROW "\
				"WHEN EXISTS( "\
				"SELECT 1 FROM metadatas "\
				"WHERE uri=NEW.uri AND start=NEW.start AND flag=NEW.flag) "\
				"BEGIN "\
				"SELECT RAISE(ROLLBACK,'A same metadata is exist!'); "\
				"END;",
				NULL,NULL);
	/*
	 * CREATE TRIGGER before_metadatas_update
	 */
	ly_dbm_exec(" "\
				"CREATE TRIGGER IF NOT EXISTS "\
				"before_metadatas_update "\
				"BEFORE UPDATE ON metadatas "\
				"FOR EACH ROW "\
				"WHEN EXISTS ( "\
				"SELECT 1 FROM metadatas "\
				"WHERE uri=NEW.uri AND start=NEW.start AND id!=NEW.id) "\
				"BEGIN "\
				"SELECT RAISE(ROLLBACK,'A same metadata is exist!'); "\
				"END;",
				NULL,NULL);
	/*
	 * CREATE TRIGGER before_playlists_delete
	 */
	ly_dbm_exec(" "\
				"CREATE TRIGGER IF NOT EXISTS "\
				"before_playlists_delete "\
				"BEFORE DELETE ON playlists "\
				"FOR EACH ROW "\
				"BEGIN "\
				"DELETE FROM connections WHERE pid=OLD.id; "\
				"END;",
				NULL,NULL);

	/*
	 * Set case sensitive
	 */
	ly_dbm_exec("PRAGMA case_sensitive_like = 1",NULL,NULL);

	ly_dbm_exec("commit", NULL, NULL);
}

/**
 * ly_dbm_fina:
 *
 * Finalize the dbm module, it will be called by #ly_cox_fina.
 */
void		ly_dbm_fina		()
{
	if(ly_dbm_conn)
		sqlite3_close(ly_dbm_conn);
}

/**
 * ly_dbm_exec:
 * @sql:	the sql to be exec.
 * @func:	the #LY_GLA_CALLBACK fromat callback function.
 * @data:	data that passed into the callback.
 *
 * Exec a sql to manage database.
 *
 * Returns:	the number of result rows. A negtive number means error ocurred.
 */
int			ly_dbm_exec		(char *sql, gpointer func, gpointer data)
{
	char str[10240]="";
	int				rt=0;
	sqlite3_stmt	*stmt=0;
	int				r=0;
	int				count=0;
	char			*error;
	char			log[10240]="";

	if(func==NULL)
	{
		rt=sqlite3_exec(ly_dbm_conn,sql,0,0, (char **)(&error));
		if( rt != SQLITE_OK )
		{
			ly_log_put_with_flag(G_LOG_LEVEL_WARNING, "Cannot exec SQL: %s, for %s.", sql, error);
			sqlite3_free(error);
			return -1;
		}
		sqlite3_free(error);
		return 0;
	}

	rt=sqlite3_prepare(ly_dbm_conn, sql, strlen(sql), &stmt, (const char **)(&error));
	if( rt != SQLITE_OK )
	{
		ly_log_put_with_flag(G_LOG_LEVEL_WARNING, "Cannot exec SQL: %s, for %s.", sql, error);
		return -1;
	}
	r=sqlite3_step(stmt);
	while(r==SQLITE_ROW)
	{
		count++;
		LY_GLA_CALLBACK(f);
		f=func;
		if(f(stmt, data))
			break;
		r=sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	return count;
}

int			ly_dbm_get_count(char *sql)
{
	int count=0;
	ly_dbm_exec(sql , ly_dbm_get_count_cb, &count);
	return count;
}


void		ly_dbm_replace_str	(char *string, int length)
{
	gchar *pi, *po, *p;
	gint nSrcLen, nDstLen, nLen;
	
	gchar *pOutput=(gchar *)g_malloc(512*sizeof(gchar));
	
	pi = string;
	po = pOutput;
	nSrcLen = strlen("'");
	nDstLen = strlen("''");
	
	p = strstr(pi, "'");
	if(p)
	{
		while(p)
		{
			nLen = (gint)(p - pi);
			memcpy(po, pi, nLen);
			memcpy(po + nLen, "''", nDstLen);
			pi = p + nSrcLen;
			po = po + nLen + nDstLen;
			p = strstr(pi, "'");
		}
		strcpy(po, pi);
	}
	else
	{
		strcpy(po, pi);
	}
	g_strlcpy(string,(const gchar *)pOutput,length);
	g_free(pOutput);
}

int			ly_dbm_get_last_insert_rowid()
{
	return sqlite3_last_insert_rowid(ly_dbm_conn);
}


gboolean ly_dbm_get_count_cb(gpointer stmt, gpointer data)
{
	if(data==NULL||stmt==NULL)
		return TRUE;
	
	gint *count=data;
	*count=sqlite3_column_int(stmt,0);
	return TRUE;
}
