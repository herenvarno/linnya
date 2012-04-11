/*
 * eql.h
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
#include "eql.h"

/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */

gboolean	ly_eql_on_equalizer_changed_cb(gpointer message, gpointer data);
gboolean	ly_eql_on_meta_update_cb(gpointer message, gpointer data);
gboolean	ly_eql_new_from_database_cb(gpointer stmt, gpointer data);
gboolean	ly_eql_conf_set_by_genre_cb(gpointer stmt, gpointer data);

/**
 * ly_eql_init:
 *
 * Initialize the eql module, it will be called by #ly_cox_init.
 */
void	ly_eql_init	()
{
	ly_log_put(_("[info] Init COX module: EQL"));
	/*
	 * check database
	 */
	ly_dbm_exec("begin", NULL, NULL);
	/*
	 * eq: default
	 */
	ly_dbm_exec(" "\
				"INSERT OR IGNORE "\
				"INTO equalizers ( "\
				"name, "\
				"band0, band1, band2, band3, band4, "\
				"band5, band6, band7, band8, band9) "\
				"VALUES ( "\
				"'default', "\
				"0, 0, 0, 0, 0, "\
				"0, 0, 0, 0, 0)",
				NULL, NULL);
	/*
	 * eq: blues
	 */
	ly_dbm_exec(" "\
				"INSERT OR IGNORE "\
				"INTO equalizers ( "\
				"name, "\
				"band0, band1, band2, band3, band4, "\
				"band5, band6, band7, band8, band9) "\
				"VALUES ( "\
				"'blues', "\
				"-2, 0, 2, 1, 0, "\
				"0, 0, 0, -2, -4)",
				NULL, NULL);
	/*
	 * eq: classic
	 */
	ly_dbm_exec(" "\
				"INSERT OR IGNORE "\
				"INTO equalizers ( "\
				"name, "\
				"band0, band1, band2, band3, band4, "\
				"band5, band6, band7, band8, band9) "\
				"VALUES ( "\
				"'classic', "\
				"0, 8, 8, 4, 0, "\
				"0, 0, 0, 2, 2)",
				NULL, NULL);
	/*
	 * eq: country
	 */
	ly_dbm_exec(" "\
				"INSERT OR IGNORE "\
				"INTO equalizers ( "\
				"name, "\
				"band0, band1, band2, band3, band4, "\
				"band5, band6, band7, band8, band9) "\
				"VALUES ( "\
				"'country', "\
				"-2, 0, 0, 2, 2, "\
				"0, 0, 0, 4, 4)",
				NULL, NULL);
	/*
	 * eq: dance
	 */
	ly_dbm_exec(" "\
				"INSERT OR IGNORE "\
				"INTO equalizers ( "\
				"name, "\
				"band0, band1, band2, band3, band4, "\
				"band5, band6, band7, band8, band9) "\
				"VALUES ( "\
				"'dance', "\
				"-2, 3, 4, 1, -2, "\
				"-2, 0, 0, 4, 4)",
				NULL, NULL);
	/*
	 * eq: jazz
	 */
	ly_dbm_exec(" "\
				"INSERT OR IGNORE "\
				"INTO equalizers ( "\
				"name, "\
				"band0, band1, band2, band3, band4, "\
				"band5, band6, band7, band8, band9) "\
				"VALUES ( "\
				"'jazz', "\
				"0, 0, 0, 4, 4, "\
				"4, 0, 2, 3, 4)",
				NULL, NULL);
	/*
	 * eq: metal
	 */
	ly_dbm_exec(" "\
				"INSERT OR IGNORE "\
				"INTO equalizers ( "\
				"name, "\
				"band0, band1, band2, band3, band4, "\
				"band5, band6, band7, band8, band9) "\
				"VALUES ( "\
				"'country', "\
				"-6, 0, 0, 0, 0, "\
				"0, 4, 0, 4, 0)",
				NULL, NULL);
	/*
	 * eq: pop
	 */
	ly_dbm_exec(" "\
				"INSERT OR IGNORE "\
				"INTO equalizers ( "\
				"name, "\
				"band0, band1, band2, band3, band4, "\
				"band5, band6, band7, band8, band9) "\
				"VALUES ( "\
				"'pop', "\
				"3, 1, 0, -2, -4, "\
				"-4, -2, 0, 1, 2)",
				NULL, NULL);
	/*
	 * eq: rock
	 */
	ly_dbm_exec(" "\
				"INSERT OR IGNORE "\
				"INTO equalizers ( "\
				"name, "\
				"band0, band1, band2, band3, band4, "\
				"band5, band6, band7, band8, band9) "\
				"VALUES ( "\
				"'rock', "\
				"-2, 0, 2, 4, -2, "\
				"-2, 0, 0, 4, 4)",
				NULL, NULL);
	ly_dbm_exec("commit", NULL, NULL);
	/*
	 * set equalizer
	 */
	int eql_auto=1;
	if(!ly_reg_get("eql_auto", "%d", &eql_auto))
	{
		ly_reg_set("eql_auto", "%d", eql_auto);
	}
	char eq_name[1024]="default";
	if(!ly_reg_get("equalizer", "%s", eq_name))
	{
		ly_reg_set("equalizer", "%s", eq_name);
	}
	LyEqlEqualizer *eq=ly_eql_new_by_conf();
	ly_eql_set_eq(eq);
	ly_eql_free(eq);

	/*
	 * bind message
	 */
	ly_msg_bind("reg_equalizer_changed", "core:reg", ly_eql_on_equalizer_changed_cb, NULL);
	ly_msg_bind("meta_update", "core:ppl", ly_eql_on_meta_update_cb, NULL);
}

/**
 * ly_eql_fina:
 *
 * Initialize the eql module, it will be called by #ly_cox_init.
 */
void	ly_eql_fina	()
{
	ly_log_put(_("[info] Fina COX module: EQL"));
	ly_msg_unbind("reg_equalizer_changed", "core:reg", ly_eql_on_equalizer_changed_cb);
	ly_msg_unbind("meta_update", "core:ppl", ly_eql_on_meta_update_cb);
}

/**
 * ly_eql_new:
 *
 * Create a new eq container.
 *
 * Returns: The LyEqlEqualizer* format eq container pointer.
 */
LyEqlEqualizer*	ly_eql_new				()
{
	LyEqlEqualizer* eq=(LyEqlEqualizer*)g_malloc(sizeof(LyEqlEqualizer));
	return eq;
}

/**
 * ly_eql_new_from_database:
 * @name: the eq name.
 *
 * Create a new eq container.
 *
 * Returns: The LyEqlEqualizer* format eq container pointer.
 */
LyEqlEqualizer*	ly_eql_new_from_database	(char *name)
{
	if(!name)
		return NULL;

	LyEqlEqualizer* eq=NULL;
	eq=ly_eql_new();
	if(!eq)
		return NULL;

	g_strlcpy(eq->name, name, sizeof(eq->name));
	char sql[1024]="";
	g_snprintf(sql, sizeof(sql), "SELECT * FROM equalizers WHERE name='%s'", eq->name);
	if(ly_dbm_exec(sql, ly_eql_new_from_database_cb, eq)>0)
	{
		return eq;
	}
	ly_msg_put("warning", "core:eql", "Cannot find particular equalizer record!");
	g_free(eq);
	return NULL;
}

/**
 * ly_eql_new_by_conf:
 *
 * Create a new eq container whose name is current actived (in reg hashtable).
 *
 * Returns: The LyEqlEqualizer* format eq container pointer.
 */
LyEqlEqualizer* ly_eql_new_by_conf	()
{
	char eq_name[1024]="default";
	ly_reg_get("equalizer", "%s", eq_name);
	LyEqlEqualizer *eq=NULL;
	eq=ly_eql_new_from_database(eq_name);
	return eq;
}

/**
 * ly_eql_new_by_genre:
 *
 * Set the equalizer setting whose name is similar to the genre of the current song
 *
 * Returns: TRUE for success, others FALSE.
 */
void ly_eql_conf_set_by_genre	()
{
	gchar name[1024]="default";
	if((ly_pqm_get_current_md())->genre!=NULL || !g_str_equal((ly_pqm_get_current_md())->genre, ""))
	{
		ly_dbm_exec("SELECT * FROM equalizers", ly_eql_conf_set_by_genre_cb, name);
	}
	ly_reg_set("equalizer", "%s", name);
}


/**
 * ly_eql_free:
 * @eq: The eq that to be freed.
 *
 * Free a eq container.
 */
void			ly_eql_free				(LyEqlEqualizer *eq)
{
	if(eq)
	{
		g_free(eq);
	}
}

/**
 * ly_eql_set_eq:
 * @eq: the equalizer to be set to the current eq.
 *
 * Set a eq to be the current eq. which modify the config of a GStreamer
 * element nameed "equalizer".
 *
 * Returns: The LyEqlEqualizer* format eq container pointer.
 */
gboolean		ly_eql_set_eq			(LyEqlEqualizer *eq)
{
	if(!eq)
		return FALSE;

	GstElement *element=NULL;
	element=ly_ppl_audio_get_element("equalizer");
	if(!element)
		return FALSE;

	g_object_set(G_OBJECT(element),
				"band0", eq->band[0],
				"band1", eq->band[1],
				"band2", eq->band[2],
				"band3", eq->band[3],
				"band4", eq->band[4],
				"band5", eq->band[5],
				"band6", eq->band[6],
				"band7", eq->band[7],
				"band8", eq->band[8],
				"band9", eq->band[9],
				NULL);
	return TRUE;
}
/**
 * ly_eql_put:
 * @eq:		equalizer to be put into database.
 *
 * Put a eq into database.
 *
 * Returns:	The LyEqlEqualizer* format eq container pointer.
 */
gboolean		ly_eql_put			(LyEqlEqualizer *eq)
{
	if((!eq) ||(g_str_equal(eq->name, "")))
		return FALSE;

	ly_dbm_replace_str(eq->name, sizeof(eq->name));
	char sql[1024]="";
	g_snprintf(	sql,
				sizeof(sql),
				"INSERT OR REPLACE "\
				"INTO equalizers ( "\
				"name, "\
				"band0, band1, band2, band3, band4, "\
				"band5, band6, band7, band8, band9) "\
				"VALUES ( "\
				"'%s', "\
				"%lf, %lf, %lf, %lf, %lf, "\
				"%lf, %lf, %lf, %lf, %lf)",
				eq->name,
				eq->band[0], eq->band[1], eq->band[2], eq->band[3], eq->band[4],
				eq->band[5], eq->band[6], eq->band[7], eq->band[8], eq->band[9]);
	if(ly_dbm_exec(sql, NULL, NULL)<0)
		return FALSE;
	return TRUE;
}


/**
 * ly_eql_on_equalizer_changed_cb:
 * @message: the message struction.
 * @data: the data passed in by ly_msg_put.
 *
 * A callback function when equalizer_changed message occurs.
 *
 * Returns: FALSE.
 */
gboolean	ly_eql_on_equalizer_changed_cb(gpointer message, gpointer data)
{
	LyMsgMsg *m=(LyMsgMsg*)message;
	LyEqlEqualizer *eq=ly_eql_new_by_conf(m->msg);
	ly_eql_set_eq(eq);
	ly_eql_free(eq);
	return FALSE;
}

/**
 * ly_eql_on_meta_update_cb:
 * @message: the message struction.
 * @data: the data passed in by ly_msg_put.
 *
 * A callback function when meta_update message occurs.
 *
 * Returns: FALSE.
 */
gboolean	ly_eql_on_meta_update_cb(gpointer message, gpointer data)
{
	LyMsgMsg *m=(LyMsgMsg*)message;
	if(!g_str_equal(m->msg, "genre"))
		return FALSE;

	int eql_auto=1;
	ly_reg_get("eql_auto", "%d", &eql_auto);
	if(eql_auto)
	{
		ly_eql_conf_set_by_genre();
	}
	return FALSE;
}



/**
 * ly_eql_new_from_database_cb:
 * @stmt: The SQL query result.
 * @data: the data passed in by ly_dbm_exec.
 *
 * A callback function called by ly_eql_new_from_database
 *
 * Returns: TRUE.
 */
gboolean	ly_eql_new_from_database_cb(gpointer stmt, gpointer data)
{
	LyEqlEqualizer *eq= (LyEqlEqualizer *)data;
	int i=0;
	for(i=0;i<10;i++)
	{
		eq->band[i]=sqlite3_column_double(stmt, i+1);
	}
	return TRUE;
}

/**
 * ly_eql_conf_set_by_genre_cb:
 * @stmt: The SQL query result.
 * @data: the data passed in by ly_dbm_exec.
 *
 * A callback function called by ly_eql_new_from_genre
 *
 * Returns: TRUE.
 */
gboolean	ly_eql_conf_set_by_genre_cb(gpointer stmt, gpointer data)
{
	int i=0;
	char *name=g_utf8_strdown(sqlite3_column_text(stmt, 0), -1);
	char *genre=g_utf8_strdown((ly_pqm_get_current_md())->genre, -1);
	if(g_strcmp0(name, genre)==0)
	{
		g_strlcpy(data, name, 1024);
		g_free(name);
		g_free(genre);
		return TRUE;
	}
	else if(g_strrstr(name, genre)||g_strrstr(genre, name))
	{
		g_strlcpy(data, name, 1024);
	}
	g_free(name);
	g_free(genre);
	return FALSE;
}
