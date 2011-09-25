/*
 * HEADERS
 */
#include "conf.h"

/*
 * FUNCTIONS
 */

/*
 * NAME:	ly_conf_read
 * VARS:	[void]
 * RETN:	[gboolean]	TRUE for success, FALSE for fail.
 * DESC:	Read configurations from file.
 */
gboolean ly_conf_read(void)
{
	gchar *path=g_strconcat(LY_GLOBAL_USERDIR,"conf.xml",NULL);
	
	GMarkupParser parser=
	{
		.start_element = ly_conf_start_cb,
		.end_element = NULL,
		.text = NULL,
		.passthrough = NULL,
		.error = NULL
	};
	
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	
	g_file_get_contents(path, &buf, &length, NULL);
	context = g_markup_parse_context_new(&parser, 0,NULL, NULL);
	
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		/*
		 * broadcast the information.
		 */
		ly_global_debug("warnning",_("No configuration file found or read configurations error. Use default configurations!\n"));

		g_hash_table_destroy(ly_conf_configurations);
		ly_conf_configurations=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		ly_conf_set("version","%lf",LY_GLOBAL_VERSION_NUM);
	}
	else
	{
	/*
	 * check the version of configuration file.
	 */
		gdouble version=0;
		if(ly_conf_get("version","%lf",&version))
		{
			if(version>=LY_GLOBAL_VERSION_NUM)
			{
				g_markup_parse_context_free(context);
				return TRUE;
			}
		}
		/*
		* broadcast the information.
		*/
		ly_global_debug("warnning",_("The version of configuration file is too low. Use default configurations!\n"));
		
		g_hash_table_destroy(ly_conf_configurations);
		ly_conf_configurations=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		ly_conf_set("version","%lf",LY_GLOBAL_VERSION_NUM);
	}
	
	g_markup_parse_context_free(context);
	return TRUE;	
}

/*
 * NAME:	ly_conf_start_cb
 * VARS:	[...] ...
 * RETN:	[void]
 * DESC:	Callback for ly_conf_read
 */
void ly_conf_start_cb(	GMarkupParseContext *context,
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
		g_hash_table_replace(ly_conf_configurations,name,value);
	}
}

/*
 * NAME:	ly_conf_write
 * VARS:	[void]
 * RETN:	[gboolean]	TRUE for success, FALSE for fail.
 * DESC:	Write configurations to file.
 */
gboolean ly_conf_write(void)
{
	gchar *path=g_strconcat(LY_GLOBAL_USERDIR,"conf.xml",NULL);
	
	FILE *fp=NULL;
	gchar *buf=NULL;
	
	if(!(fp=fopen(path,"w")))
	{
		ly_global_debug("warnning",_("Cannot write configurations to file!\n"));
		return FALSE;
	}

	buf=g_markup_printf_escaped ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<configurations>\n");
	fputs(buf,fp);
	g_free(buf);
	
	g_hash_table_foreach(ly_conf_configurations, ly_conf_write_each_cb, fp);
	
	buf=g_markup_printf_escaped ("</configurations>");
	fputs(buf,fp);
	g_free(buf);
	
	fclose(fp);
	return TRUE;
}

/*
 * NAME:	ly_conf_write_each_cb
 * VARS:	[void]
 * RETN:	[void]
 * DESC:	Write each configuration to file. Called by ly_conf_write.
 */
void ly_conf_write_each_cb(gpointer key, gpointer value, gpointer data)
{
	FILE *fp=data;
	if(!fp)
	{
		ly_global_debug("warnning",_("Cannot write configurations to file!\n"));
		return;
	}

	gchar *buf=NULL;
	buf=g_markup_printf_escaped ("\t<conf name=\"%s\" value=\"%s\" />\n", (gchar*)key, (gchar*)value);
	fputs(buf,fp);
	g_free(buf);
}

/*
 * NAME:	ly_conf_delete
 * VARS:	[gchar*]name	the name of conf.
 * RETN:	[void]
 * DESC:	delete a conf.
 */
void ly_conf_delete(gchar *name)
{
	g_hash_table_remove(ly_conf_configurations, name);
}

/*
 * NAME:	ly_conf_init
 * VARS:	[gchar*]name	the name of conf.
 * 			[gchar*]value	the value of conf.
 * RETN:	[void]
 * DESC:	initialize the conf module.
 */
gboolean ly_conf_init(void)
{
	ly_global_debug("info",_("Initial core module: CONF ...\n"));
	gboolean rt=FALSE;
	ly_conf_configurations=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	rt=ly_conf_read();
	return rt;
}

/*
 * NAME:	ly_conf_finalize
 * VARS:	[gchar*]name	the name of conf.
 * 			[gchar*]value	the value of conf.
 * RETN:	[void]
 * DESC:	finalize the conf module.
 */
gboolean ly_conf_finalize(void)
{
	ly_global_debug("info",_("Finalize core module: CONF ...\n"));
	gboolean rt=FALSE;
	rt=ly_conf_write();
	g_hash_table_destroy(ly_conf_configurations);
	return rt;
}

/*
 * NAME:	ly_conf_set
 * VARS:	[gchar*]name	the name of conf.
 * 			[gchar*]format	the formate.
 * 			[...]
 * RETN:	[gbooelan]		TRUE for success, FALSE for fail.
 * DESC:	set or put a conf.
 */
gboolean ly_conf_set(gchar *name, const gchar *format, ...)
{
	gchar *str=NULL;

	va_list argp;
	va_start(argp, format);
	str=g_strdup_vprintf(format, argp);
	va_end(argp);
	
	gchar *namestr=NULL;
	namestr=g_strconcat(name,NULL);
	g_hash_table_replace(ly_conf_configurations, namestr, str);

	return TRUE;
}

/*
 * NAME:	ly_conf_get
 * VARS:	[gchar*]name	the name of conf.
 * 			[gchar*]format	the formate.
 * 			[...]
 * RETN:	[gbooelan]		TRUE for success, FALSE for fail.
 * DESC:	get a conf.
 */
gboolean ly_conf_get(gchar *name, const gchar *format, ...)
{
	gchar *str=NULL;
	str=g_hash_table_lookup(ly_conf_configurations, name);
	
	if(str==NULL)
		return FALSE;
	va_list argp;
	va_start(argp, format);
	vsscanf(str, format, argp);
	va_end(argp);
	
	return TRUE;
}
