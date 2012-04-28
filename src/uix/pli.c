/*
 * pl.c
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
#include "pli.h"

/*
 * VARIABLES
 */
GHashTable	*ly_pli_plugins;
GList		*ly_pli_depends;
gchar		*ly_pli_element_name;	/* PRIVATE */

/*
 * FUNCTIONS
 */
void ly_pli_init()
{
	ly_pli_plugins=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	ly_pli_depends=NULL;
	
	if(!g_module_supported())
	{
		ly_log_put(_("[fatal] Your system does not support plugin!"));
		printf(_("[fatal] Your system does not support plugin!\n"));
		exit(0);
	}
	
	GList *list=NULL;
	list=ly_gla_get_subdirs(LY_GLB_USER_PLXDIR, FALSE);

	GList *p=list;
	gchar *pdata=NULL;
	while(p)
	{
		pdata=p->data;
		ly_pli_new(LY_GLB_USER_PLXDIR, pdata);
		g_free(pdata);
		p=p->next;
	}
	g_list_free(list);

	list=ly_gla_get_subdirs(LY_GLB_PROG_PLXDIR, FALSE);
	p=list;
	pdata=NULL;
	while(p)
	{
		pdata=p->data;
		ly_pli_new(LY_GLB_PROG_PLXDIR, pdata);
		g_free(pdata);
		p=p->next;
	}
	g_list_free(list);
}

void	ly_pli_fina()
{
}


void	ly_pli_set(LyPliPlugin* plugin)
{
	gchar *namestr=g_strconcat(plugin->name,NULL);
	g_hash_table_replace(ly_pli_plugins, namestr, plugin);
}

LyPliPlugin* ly_pli_get(gchar *name)
{
	LyPliPlugin *plugin=NULL;
	plugin=g_hash_table_lookup(ly_pli_plugins, name);
	return plugin;
}

void ly_pli_del(gchar *name)
{
	LyPliPlugin *plugin=NULL;
	plugin=ly_pli_get(name);
	if(!plugin)
		return;
	if(plugin->module)
		g_module_close(plugin->module);
	g_hash_table_remove(ly_pli_plugins, name);
}

LyPliPlugin* ly_pli_new(const char *dir, char *filename)
{
	if(!filename)
		return NULL;
	gchar path[1024];
	g_snprintf(path, sizeof(path),"%s%s/plugin.xml",dir, filename);
	
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		return NULL;
	}

	GMarkupParser parser=
	{
		.start_element = ly_pli_new_start_cb,
		.end_element = ly_pli_new_end_cb,
		.text = ly_pli_new_text_cb,
		.passthrough = NULL,
		.error = NULL
	};
	
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	LyPliPlugin *plugin=(LyPliPlugin *)g_malloc(sizeof(LyPliPlugin));
	g_strlcpy(plugin->name,filename,sizeof(plugin->name));
	
	g_file_get_contents(path, &buf, &length, NULL);
	context = g_markup_parse_context_new(&parser, 0, plugin, NULL);
	
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		ly_log_put(_("[warning] Read configuration file error."));
		g_free(plugin);
		return NULL;
	}

	g_markup_parse_context_free(context);
	plugin->widget=NULL;
	if(g_str_equal(plugin->name,""))
	{
		g_free(plugin);
		plugin=NULL;
		return NULL;
	}
	if(!g_str_equal(plugin->logo,""))
	{
		g_snprintf(path, sizeof(path),"%s%s/%s",dir,filename,plugin->logo);
		g_strlcpy(plugin->logo,path,sizeof(plugin->logo));
	}
	
	g_snprintf(path, sizeof(path),"%s%s/lib%s.so",dir,filename,filename);
	GModule *module=NULL;
	
	char lock_path[1024]="";
	g_snprintf(lock_path, sizeof(lock_path),"%s%s.lock",LY_GLB_USER_PLXDIR,filename);
	if(!g_file_test(lock_path, G_FILE_TEST_EXISTS))
	{
		module=g_module_open(path,G_MODULE_BIND_LAZY);
	}
	plugin->module=module;

	if(g_str_equal(plugin->alias,""))
	{
		g_strlcpy(plugin->alias, plugin->name, sizeof(plugin->alias));
	}
	ly_pli_set(plugin);
	return plugin;
}

void ly_pli_new_start_cb(GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer data, GError **error)
{
	if(ly_pli_element_name)
	{
		g_free(ly_pli_element_name);
		ly_pli_element_name=NULL;
	}
	ly_pli_element_name=g_strdup (element_name);
}
void ly_pli_new_text_cb(GMarkupParseContext * context, const gchar *text, gsize text_len, gpointer data, GError **error) 
{
	LyPliPlugin *plugin=data;
	if(!ly_pli_element_name||!text)
		return;
	
	else if(g_str_equal(ly_pli_element_name,"alias"))
		g_strlcpy(plugin->alias, text, sizeof(plugin->alias));
	else if(g_str_equal(ly_pli_element_name,"version"))
		g_strlcpy(plugin->version, text, sizeof(plugin->version));
	else if(g_str_equal(ly_pli_element_name,"author"))
		g_strlcpy(plugin->author, text, sizeof(plugin->author));
	else if(g_str_equal(ly_pli_element_name,"license"))
		g_strlcpy(plugin->license, text, sizeof(plugin->license));
	else if(g_str_equal(ly_pli_element_name,"logo"))
		g_strlcpy(plugin->logo, text, sizeof(plugin->logo));
	else if(g_str_equal(ly_pli_element_name,"comment"))
		g_strlcpy(plugin->comment, text, sizeof(plugin->comment));
	else if(g_str_equal(ly_pli_element_name,"website"))
		g_strlcpy(plugin->website, text, sizeof(plugin->website));
	else if(g_str_equal(ly_pli_element_name,"depends"))
	{
		if(g_str_equal(plugin->name,""))
			return;
		gchar depends[1024];
		g_strlcpy(depends, text, sizeof(depends));
		if(g_str_equal(depends,""))
			return;
		
		gchar **plugins=NULL;
		plugins=g_strsplit(depends,":",-1);
		gint i=0;
		for(i=0; plugins[i]!=NULL; i++)
		{
			ly_pli_set_depend(plugin->name, plugins[i]);
			if(ly_pli_get(plugins[i]))
				continue;
			if(!(ly_pli_new(LY_GLB_USER_PLXDIR, plugins[i])))
			{
				if(!(ly_pli_new(LY_GLB_PROG_PLXDIR, plugins[i])))
				{
					continue;
				}
			}
		}
	}
	else if(g_str_equal(ly_pli_element_name,"daemon"))
		sscanf(text,"%d",&(plugin->daemon));
	else if(g_str_equal(ly_pli_element_name,"create_symbol"))
		g_strlcpy(plugin->create_symbol, text, sizeof(plugin->create_symbol));
	else if(g_str_equal(ly_pli_element_name,"refresh_symbol"))
		g_strlcpy(plugin->refresh_symbol, text, sizeof(plugin->refresh_symbol));
	else if(g_str_equal(ly_pli_element_name,"destroy_symbol"))
		g_strlcpy(plugin->destroy_symbol, text, sizeof(plugin->destroy_symbol));
	else if(g_str_equal(ly_pli_element_name,"config_symbol"))
		g_strlcpy(plugin->config_symbol, text, sizeof(plugin->config_symbol));
}
void ly_pli_new_end_cb(GMarkupParseContext *context, const gchar *element_name, gpointer data, GError **error)
{
	if(ly_pli_element_name)
	{
		g_free(ly_pli_element_name);
		ly_pli_element_name=NULL;
	}
}

GList *ly_pli_get_depends(gchar *name)
{
	GList *depends=NULL;
	GList *p=ly_pli_depends;
	LyPliDepend *pdata;
	while(p)
	{
		pdata=p->data;
		if(pdata&&g_str_equal(pdata->name,name))
		{
			depends=g_list_append(depends,g_strconcat(pdata->depend,NULL));
		}
		p=p->next;
	}
	return depends;
}

GList *ly_pli_get_depend_bys(gchar *name)
{
	GList *depends=NULL;
	GList *p=ly_pli_depends;
	LyPliDepend *pdata;
	while(p)
	{
		pdata=p->data;
		if(pdata&&g_str_equal(pdata->depend,name))
		{
			depends=g_list_append(depends,g_strconcat(pdata->name,NULL));
		}
		p=p->next;
	}
	return depends;
}

gboolean ly_pli_set_depend(gchar *name, gchar *depend)
{
	if(name==NULL || depend==NULL)
		return FALSE;
	if(g_str_equal(name,"") || g_str_equal(depend, "") || g_str_equal(name, depend))
		return FALSE;
	
	LyPliDepend *d=(LyPliDepend *)g_malloc(sizeof(LyPliDepend));
	g_strlcpy(d->name,name,sizeof(d->name));
	g_strlcpy(d->depend,depend,sizeof(d->depend));
	
	ly_pli_depends=g_list_append(ly_pli_depends, d);
	return TRUE;
}

gboolean ly_pli_set_active(gchar *name, gboolean active)
{
	if(!name||g_str_equal(name,""))
		return FALSE;
	LyPliPlugin *pl=ly_pli_get(name);
	if(!pl)
		return FALSE;
	
	char lock_path[1024]="";
	GList *list=NULL;
	GList *p=NULL;
	
	if(active)
	{
		if(pl->module)
			return TRUE;
		list=ly_pli_get_depends(name);
		p=list;
		while(p)
		{
			if(!ly_pli_set_active((gchar*)(p->data), TRUE))
				return FALSE;
			p=p->next;
		}
		char path[1024]="";
		g_snprintf(path, sizeof(path),"%splugin/%s/lib%s.so",LY_GLA_PROGDIR, pl->name, pl->name);
		pl->module=g_module_open(path,G_MODULE_BIND_LAZY);
		g_snprintf(lock_path, sizeof(path),"%splugin/%s.lock",LY_GLA_USERDIR,pl->name);
		if(g_file_test(lock_path, G_FILE_TEST_EXISTS))
		{
			remove(lock_path);
		}
	}
	else
	{
		if(!(pl->module))
			return TRUE;
		
		list=ly_pli_get_depend_bys(name);
		p=list;
		while(p)
		{
			if(!ly_pli_set_active((gchar*)(p->data), FALSE))
				return FALSE;
			p=p->next;
		}
		g_module_close(pl->module);
		pl->module=NULL;
		g_snprintf(lock_path, sizeof(lock_path),"%splugin/%s.lock",LY_GLA_USERDIR,pl->name);
		if(!g_file_test(lock_path, G_FILE_TEST_EXISTS))
		{
			g_file_set_contents(lock_path, "== THIS IS A LOCK FILE FOR PLUGINS, DO NOT DELETE IT ==", -1, NULL);
		}
	}
	
	return TRUE;
}

GHashTable*		ly_pli_get_plugins		()
{
	return ly_pli_plugins;
}
