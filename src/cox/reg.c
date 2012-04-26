/*
 * reg.c
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
#include "reg.h"

/*
 * VARIABLES
 */
char			ly_reg_file[1024]="";
GHashTable		*ly_reg_table=NULL;

/*
 * FUNCTIONS
 */

gboolean	ly_reg_write			();
gboolean	ly_reg_read				();
void		ly_reg_read_start_cb	(GMarkupParseContext *context,
									const gchar *element_name,
									const gchar **attribute_names,
									const gchar **attribute_values,
									gpointer data,
									GError **error);

/**
 * ly_reg_init:
 *
 * Initialize the reg module, it will be called by #ly_cox_init
 */
void		ly_reg_init		()
{
	g_snprintf(ly_reg_file, sizeof(ly_reg_file), "%sconf.xml", LY_GLA_USERDIR);
	ly_reg_table=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	if(!ly_reg_read())
	{
		printf("[fatal] Cannot load reg file. Abort...\n");
		ly_log_put("[fatal] Cannot load reg file. Abort...");
		exit(0);
	}
}

/**
 * ly_reg_fina:
 *
 * Finalize the reg module, it will be called by #ly_cox_fina
 */
void		ly_reg_fina		()
{
	ly_reg_write();
	g_hash_table_destroy(ly_reg_table);
}

/**
 * ly_reg_get:
 * @name:		the record name.
 * @format:		a standard printf() format string, but notice string precision pitfalls.
 * @...:		the arguments to insert in the output.
 *
 * Get a reg record.
 *
 * Returns:		TRUE for success, FALSE for fail.
 */
gboolean	ly_reg_get(char *name, const char *format, ...)
{
	char *str=NULL;
	str=g_hash_table_lookup(ly_reg_table, name);

	if(str==NULL)
		return FALSE;
	va_list argp;
	va_start(argp, format);
	vsscanf(str, format, argp);
	va_end(argp);

	return TRUE;
}

/**
 * ly_reg_get:
 * @name:		the record name.
 * @format:		a standard printf() format string, but notice string precision pitfalls.
 * @...:		the arguments to insert in the output.
 *
 * Set or add a reg record.
 *
 * Returns:		TRUE for success, FALSE for fail.
 */
gboolean	ly_reg_set(char *name, const char *format, ...)
{
	if(!ly_reg_table)
		return FALSE;

	char *str=NULL;
	va_list argp;
	va_start(argp, format);
	str=g_strdup_vprintf(format, argp);
	va_end(argp);

	char *namestr=NULL;
	namestr=g_strconcat(name,NULL);
	g_hash_table_replace(ly_reg_table, namestr, str);

	char s[1024]="";
	g_snprintf(s, sizeof(s), "reg_%s_changed", namestr);
	ly_msg_put(s, "core:reg", str);
	return TRUE;
}

/**
 * ly_reg_del:
 * @name:		the record name.
 *
 * Delete a reg record.
 */
void		ly_reg_del(char *name)
{
	g_hash_table_remove(ly_reg_table, name);
}

/**
 * ly_reg_write:
 *
 * Write all reg records to file.
 *
 * Returns:		TRUE for success, FALSE for fail.
 */
gboolean	ly_reg_write			()
{
	FILE *fp=NULL;
	if(!(fp=fopen(ly_reg_file,"w+")))
	{
		ly_msg_put("warning", "core:reg", _("Cannot write reg file!"));
		return FALSE;
	}

	char *buf=NULL;
	buf=g_markup_printf_escaped ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<configurations>\n");
	fputs(buf,fp);
	g_free(buf);

	GHashTableIter iter;
	gpointer key, value;
	g_hash_table_iter_init (&iter, ly_reg_table);
	while (g_hash_table_iter_next (&iter, &key, &value))
	{
		buf=g_markup_printf_escaped ("\t<conf name=\"%s\" value=\"%s\" />\n", (gchar*)key, (gchar*)value);
		fputs(buf,fp);
		g_free(buf);
	}

	buf=g_markup_printf_escaped ("</configurations>");
	fputs(buf,fp);
	g_free(buf);

	fclose(fp);
	return TRUE;
}

/**
 * ly_reg_read:
 *
 * Read all reg records to memery and stroed in a hash table.
 *
 * Returns:		TRUE for success, FALSE for fail.
 */
gboolean	ly_reg_read			()
{
	if(!g_file_test(ly_reg_file, G_FILE_TEST_EXISTS))
	{
		ly_log_put(_("[warning] No configuration file found!"));
		ly_reg_table=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		ly_reg_set("version", "%lf", LY_GLA_VERSION_NUM);
		return TRUE;
	}

	LY_GLA_FIRST_FLAG=FALSE;
	GMarkupParser parser=
	{
		.start_element = ly_reg_read_start_cb,
		.end_element = NULL,
		.text = NULL,
		.passthrough = NULL,
		.error = NULL
	};

	gchar *buf;
	gsize length;
	GMarkupParseContext *context;

	g_file_get_contents(ly_reg_file, &buf, &length, NULL);
	context = g_markup_parse_context_new(&parser, 0,NULL, NULL);

	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		ly_log_put(_("[error] Read reg file error!"));
		g_hash_table_destroy(ly_reg_table);
		ly_reg_table=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		ly_reg_set("version","%lf", LY_GLA_VERSION_NUM);
	}
	else
	{
		/*
		 * check the version of configuration file.
		 */
		double version=0;
		if(ly_reg_get("version", "%lf", &version))
		{
			if(version>=LY_GLA_VERSION_NUM)
			{
				g_markup_parse_context_free(context);
				return TRUE;
			}
		}
		ly_log_put(_("[warning] Version of reg file is too low!"));
	}

	g_markup_parse_context_free(context);
	return TRUE;
}

/**
 * ly_reg_read_start_cb:
 *
 * The callback function of ly_reg_read.
 */
void ly_reg_read_start_cb	(	GMarkupParseContext *context,
								const gchar *element_name,
								const gchar **attribute_names,
								const gchar **attribute_values,
								gpointer data,
								GError **error)
{
	if(g_str_equal(element_name,"conf"))
	{
		gchar *name=NULL;
		gchar *value=NULL;

		const gchar **name_cursor = attribute_names;
		const gchar **value_cursor = attribute_values;

		while (*name_cursor)
		{
			if (g_str_equal(*name_cursor, "name"))
				name = g_strdup (*value_cursor);
			else if (g_str_equal(*name_cursor, "value"))
				value = g_strdup (*value_cursor);
			name_cursor++;
			value_cursor++;
		}

		if(!name || !value)
		{
			if(name)
				g_free(name);
			if(value)
				g_free(value);
			return;
		}
		g_hash_table_replace(ly_reg_table,name,value);
	}
}
