/*
 * thm.c
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
#include "thm.h"

/*
 * FUNCTIONS
 */
gboolean ly_thm_init()
{	
	gboolean rt=FALSE;
	ly_log_put(_("[info] Initial ui module: THEME ..."));
	
	char path[1024]="";
	g_snprintf(path, sizeof(path), "%spictures/", LY_GLA_HOMEDIR);
	if(!ly_reg_get("thm_sssbg", "%s", path))
	{
		ly_reg_set("thm_sssbg", "%s", path);
	}
	
	g_snprintf(path, sizeof(path), "%sui/theme", LY_GLA_USERDIR);
	mkdir(path,0755);
	
	rt=ly_thm_table_new();
	
	return rt;
}

gboolean ly_thm_fina()
{
	ly_log_put(_("[info] Finalize ui moudle: THEME ..."));
	return TRUE;
}

GList* ly_thm_get_list()
{
	char path[1024]="";
	GList *list0=NULL;
	GList *list1=NULL;
	g_snprintf(path,sizeof(path),"%sui/theme/",LY_GLA_USERDIR);
	list0=ly_gla_traverse_dir(path, 1, FALSE);
	
	g_snprintf(path,sizeof(path),"%sui/theme/",LY_GLA_PROGDIR);
	list1=ly_gla_traverse_dir(path, 1, FALSE);
	
	GList *p=list1;
	while(p)
	{
		if(!g_list_find(list0, p->data))
		{
			list0=g_list_append(list0, p->data);
			p=p->next;
		}
		else
		{
			p=p->next;
			list1=g_list_delete_link(list1, p->prev);
		}
	}
	return list0;
}

gboolean ly_thm_table_new()
{
	if(ly_thm_table)
	{
		ly_thm_table_destroy();
	}
	ly_thm_table=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	
	char theme_name[128]="default";
	if(ly_reg_get("theme","%127s",theme_name))
	{
		ly_reg_set("theme","%s",theme_name);
	}
	
	gchar path[128]="";
	gchar path_dir[128]="";
	g_snprintf(path,sizeof(path),"%sui/theme/%s/index.xml",LY_GLA_USERDIR,theme_name);
	g_snprintf(path_dir,sizeof(path_dir),"%sui/theme/%s/",LY_GLA_USERDIR,theme_name);
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		g_snprintf(path,sizeof(path),"%sui/theme/%s/index.xml",LY_GLA_PROGDIR,theme_name);
		g_snprintf(path_dir,sizeof(path_dir),"%sui/theme/%s/",LY_GLA_PROGDIR,theme_name);
		if(!g_file_test(path, G_FILE_TEST_EXISTS))
		{
			if(!g_strrstr(path,"default"))
			{
				ly_msg_put("warning", "ui:theme", "Theme not found!");
			}
			return FALSE;
		}
	}
	
	GMarkupParser parser=
	{
		.start_element = ly_thm_start_cb,
		.end_element = NULL,
		.text = NULL,
		.passthrough = NULL,
		.error = NULL
	};
	
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;

	g_file_get_contents(path, &buf, &length, NULL);
	context = g_markup_parse_context_new(&parser, 0, path_dir, NULL);
	
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		ly_msg_put("warning","ui:theme", _("Read theme configure file error!"));
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
void ly_thm_start_cb(	GMarkupParseContext *context,
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
		
		LyThmItem *theme=(LyThmItem *)g_malloc(sizeof(LyThmItem));
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
			else if(g_str_equal(*name_cursor, "bg"))
			{
				gchar tmp[1024]="";
				sscanf((*value_cursor),"%s",tmp);
				if(tmp[0]=='/')
				{
					g_snprintf(theme->bg, sizeof(theme->bg), "%s", tmp);
				}
				else
				{
					g_snprintf(theme->bg, sizeof(theme->bg), "%s%s", (gchar*)data, tmp);
				}
			}
				
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
		g_hash_table_replace(ly_thm_table,name,theme);
		g_free(value);
	}
}

gboolean ly_thm_table_destroy()
{
	if(ly_thm_table)
	{
		g_hash_table_destroy(ly_thm_table);
	}
	ly_thm_table=NULL;
	return TRUE;
}

/*
 * NAME:	ly_ui_key_set
 * VARS:	[gchar*]name	the name of conf.
 * 			[gchar*]format	the formate.
 * 			[...]
 * RETN:	[gbooelan]		TRUE for success, FALSE for fail.
 * DESC:	set or put a conf.
 */
gboolean ly_thm_set(gchar *name, LyThmItem *theme)
{
	gchar *namestr=NULL;
	namestr=g_strconcat(name,NULL);
	g_hash_table_replace(ly_thm_table, namestr, theme);
	
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
LyThmItem* ly_thm_get(gchar *name)
{
	LyThmItem *value=NULL;
	value=g_hash_table_lookup(ly_thm_table, name);
	return value;
}
