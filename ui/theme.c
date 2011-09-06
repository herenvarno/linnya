/*
 * HEADERS
 */
#include "theme.h"

/*
 * FUNCTIONS
 */
gboolean ly_ui_theme_init()
{
	ly_global_debug("info",_("Initial ui module: THEME ...\n"));
	gboolean rt=FALSE;
	ly_ui_theme_themes=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	rt=ly_ui_theme_read();
	
	return rt;
}

gboolean ly_ui_theme_finalize()
{
	ly_global_debug("info",_("Finalize ui moudle: THEME ...\n"));
	gboolean rt=FALSE;
	rt=ly_ui_theme_write();
	g_hash_table_destroy(ly_ui_theme_themes);
	return rt;
}

gboolean ly_ui_theme_read(void)
{
	gchar theme_name[128]="default";
	if(ly_conf_get("theme","%s",theme_name))
	{
		ly_conf_set("theme","%s",theme_name);
	}
	
	gchar path[128]="";
	g_snprintf(path,127,"%sui/default/themes/%s/index.xml",LY_GLOBAL_USERDIR,theme_name);
	
	GMarkupParser parser=
	{
		.start_element = ly_ui_theme_start_cb,
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
		ly_global_debug("warnning",_("No theme found. Use default theme.\n"));
		ly_conf_set("theme","%s","default");
		g_snprintf(path,127,"%sui/default/themes/default/",LY_GLOBAL_USERDIR);
		mkdir(path,0755);
		
		g_hash_table_destroy(ly_ui_theme_themes);
		ly_ui_theme_themes=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

		gboolean ly_ui_theme_read();
	}
	
	g_markup_parse_context_free(context);
	return TRUE;
}

/*
 * NAME:	ly_ui_key_start_cb
 * VARS:	[...] ...
 * RETN:	[void]
 * DESC:	Callback for ly_ui_key_read
 */
void ly_ui_theme_start_cb(	GMarkupParseContext *context,
							const gchar *element_name,
							const gchar **attribute_names,
							const gchar **attribute_values,
							gpointer data,
							GError **error)
{
	if(g_str_equal(element_name,"theme"))
	{
		gchar *name=NULL;
		gchar *value=NULL;
		
		const gchar **name_cursor = attribute_names;
		const gchar **value_cursor = attribute_values;
		
		lyUiThemeTheme *theme=(lyUiThemeTheme *)g_malloc(sizeof(lyUiThemeTheme));
		while (*name_cursor)
		{
			if (g_str_equal(*name_cursor, "name"))
				name = g_strdup (*value_cursor);
			else if(g_str_equal(*name_cursor, "pos"))
				sscanf((*value_cursor),"%d:%d",&(theme->pos[0]),&(theme->pos[1]));
			else if(g_str_equal(*name_cursor, "size"))
				sscanf((*value_cursor),"%d:%d",&(theme->size[0]),&(theme->size[1]));
			else if(g_str_equal(*name_cursor, "decorated"))
				sscanf((*value_cursor),"%d",&(theme->decorated));
			else if(g_str_equal(*name_cursor, "border"))
				sscanf((*value_cursor),"%d",&(theme->border));
			else if(g_str_equal(*name_cursor, "background"))
				sscanf((*value_cursor),"%s",theme->background);
			else if(g_str_equal(*name_cursor, "font"))
				sscanf((*value_cursor),"%s",theme->font);
			else if(g_str_equal(*name_cursor, "fg_color0"))
				sscanf((*value_cursor),"%lf:%lf:%lf:%lf",&(theme->fg_color0[0]),&(theme->fg_color0[1]),&(theme->fg_color0[2]),&(theme->fg_color0[3]));
			else if(g_str_equal(*name_cursor, "fg_color1"))
				sscanf((*value_cursor),"%lf:%lf:%lf:%lf",&(theme->fg_color1[0]),&(theme->fg_color1[1]),&(theme->fg_color1[2]),&(theme->fg_color1[3]));
			else if(g_str_equal(*name_cursor, "bg_color0"))
				sscanf((*value_cursor),"%lf:%lf:%lf:%lf",&(theme->bg_color0[0]),&(theme->bg_color0[1]),&(theme->bg_color0[2]),&(theme->bg_color0[3]));
			else if(g_str_equal(*name_cursor, "bg_color1"))
				sscanf((*value_cursor),"%lf:%lf:%lf:%lf",&(theme->bg_color1[0]),&(theme->bg_color1[1]),&(theme->bg_color1[2]),&(theme->bg_color1[3]));
				
			name_cursor++;
			value_cursor++;
		}
		
		if(!name)
		{
			if(name)
				g_free(name);
			if(value)
				g_free(theme);
			return;
		}
		g_hash_table_replace(ly_ui_theme_themes,name,theme);
		g_free(value);
	}
}


gboolean ly_ui_theme_write(void)
{
	gchar theme_name[128]="default";
	if(ly_conf_get("theme","%s",theme_name))
	{
		ly_conf_set("theme","%s",theme_name);
	}
	gchar path[128]="";
	g_snprintf(path,127,"%sui/default/themes/%s/index.xml",LY_GLOBAL_USERDIR,theme_name);
	
	FILE *fp=NULL;
	gchar *buf=NULL;
	
	if(!(fp=fopen(path,"w")))
	{
		ly_global_debug("warnning",_("Cannot write themes to file!\n"));
		return FALSE;
	}
	
	buf=g_markup_printf_escaped ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<themes>\n");
	fputs(buf,fp);
	g_free(buf);
	
	g_hash_table_foreach(ly_ui_theme_themes, ly_ui_theme_write_each_cb, fp);
	
	buf=g_markup_printf_escaped ("</themes>");
	fputs(buf,fp);
	g_free(buf);
	
	fclose(fp);
	return TRUE;
}

void ly_ui_theme_write_each_cb(gpointer key, gpointer value, gpointer data)
{
	FILE *fp=data;
	if(!fp)
	{
		ly_global_debug("warnning",_("Cannot write themes to file!\n"));
		return;
	}

	lyUiThemeTheme *theme=(lyUiThemeTheme *)value;
	gchar *buf=NULL;
	buf=g_markup_printf_escaped ("\t<theme name=\"%s\" pos=\"%d:%d\" size=\"%d:%d\" decorated=\"%d\" border=\"%d\" background=\"%s\" font=\"%s\" fg_color0=\"%lf:%lf:%lf:%lf\" fg_color1=\"%lf:%lf:%lf:%lf\" bg_color0=\"%lf:%lf:%lf:%lf\" bg_color1=\"%lf:%lf:%lf:%lf\" />\n", (gchar*)key, theme->pos[0], theme->pos[1], theme->size[0], theme->size[1], theme->decorated, theme->border, theme->background,theme->font, theme->fg_color0[0], theme->fg_color0[1], theme->fg_color0[2], theme->fg_color0[3],theme->fg_color1[0],theme->fg_color1[1],theme->fg_color1[2], theme->fg_color1[3],theme->bg_color0[0], theme->bg_color0[1], theme->bg_color0[2], theme->bg_color0[3],theme->bg_color1[0],theme->bg_color1[1],theme->bg_color1[2],theme->bg_color1[3]);
	fputs(buf,fp);
	g_free(buf);
}

/*
 * NAME:	ly_ui_key_set
 * VARS:	[gchar*]name	the name of conf.
 * 			[gchar*]format	the formate.
 * 			[...]
 * RETN:	[gbooelan]		TRUE for success, FALSE for fail.
 * DESC:	set or put a conf.
 */
gboolean ly_ui_theme_set(gchar *name, lyUiThemeTheme *theme)
{
	gchar *namestr=NULL;
	namestr=g_strconcat(name,NULL);
	g_hash_table_replace(ly_ui_theme_themes, namestr, theme);
	
	return TRUE;
}

/*
 * NAME:	ly_ui_key_get
 * VARS:	[gchar*]name	the name of conf.
 * 			[gchar*]format	the formate.
 * 			[...]
 * RETN:	[gbooelan]		TRUE for success, FALSE for fail.
 * DESC:	get a conf.
 */
lyUiThemeTheme* ly_ui_theme_get(gchar *name)
{
	lyUiThemeTheme *value=NULL;
	value=g_hash_table_lookup(ly_ui_theme_themes, name);
	return value;
}