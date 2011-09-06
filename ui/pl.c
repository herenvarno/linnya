#include "pl.h"

gboolean ly_ui_pl_init()
{
	ly_global_debug("info",_("Initial plugin module ...\n"));
	
	ly_ui_pl_plugins=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	ly_ui_pl_depends=NULL;
	
	if(!g_module_supported())
		ly_global_debug("error",_("Your system does not support plugin!\n"));
	
	gchar path[256]={0};
	g_snprintf(path,sizeof(path),"%splugin/",LY_GLOBAL_PROGDIR);
	
	GList *list=NULL;
	list=ly_global_get_subdirs(path,FALSE);

	GList *p=list;
	gchar *pdata=NULL;
	while(p)
	{
		pdata=p->data;
		ly_ui_pl_new(pdata);
		p=p->next;
	}
	return TRUE;
}

gboolean ly_ui_pl_finalize()
{
	ly_global_debug("info",_("Finalize plugin module ...\n"));
	return TRUE;
}

gboolean ly_ui_pl_set(lyUiPlPlugin* plugin)
{
	gchar *namestr=g_strconcat(plugin->name,NULL);
	g_hash_table_replace(ly_ui_pl_plugins, namestr, plugin);
	
	return TRUE;
}
lyUiPlPlugin* ly_ui_pl_get(gchar *name)
{
	lyUiPlPlugin *plugin=NULL;
	plugin=g_hash_table_lookup(ly_ui_pl_plugins, name);
	return plugin;
}
gboolean ly_ui_pl_delete(gchar *name)
{
	lyUiPlPlugin *plugin=NULL;
	plugin=ly_ui_pl_get(name);
	if(!plugin)
		return FALSE;
	if(plugin->module)
		g_module_close(plugin->module);
	g_hash_table_remove(ly_ui_pl_plugins, name);
	return TRUE;
}

lyUiPlPlugin* ly_ui_pl_new(gchar *filename)
{
	gchar path[1024];
	g_snprintf(path, sizeof(path),"%splugin/%s/plugin.xml",LY_GLOBAL_PROGDIR,filename);
	
	GMarkupParser parser=
	{
		.start_element = ly_ui_pl_new_start_cb,
		.end_element = ly_ui_pl_new_end_cb,
		.text = ly_ui_pl_new_text_cb,
		.passthrough = NULL,
		.error = NULL
	};
	
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	lyUiPlPlugin *plugin=(lyUiPlPlugin *)g_malloc(sizeof(lyUiPlPlugin));
	g_strlcpy(plugin->name,filename,sizeof(plugin->name));
	
	g_file_get_contents(path, &buf, &length, NULL);
	context = g_markup_parse_context_new(&parser, 0, plugin, NULL);
	
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		ly_global_debug("warnning",_("Read configuration file (%s) error.\n"),path);
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
		g_snprintf(path, sizeof(path),"%splugin/%s/%s",LY_GLOBAL_PROGDIR,filename,plugin->logo);
		g_strlcpy(plugin->logo,path,sizeof(plugin->logo));
	}
	
	g_snprintf(path, sizeof(path),"%splugin/%s/lib%s.so",LY_GLOBAL_PROGDIR,filename,filename);
	GModule *module=NULL;
	if(plugin->name[0]!='!')
	{
		module=g_module_open(path,G_MODULE_BIND_LAZY);
	}
	else
	{
		char tmp[1024]="";
		g_strlcpy(tmp, plugin->name+1, sizeof(tmp));
		g_strlcpy(plugin->name, tmp, sizeof(plugin->name));
	}
	plugin->module=module;
	
	if(g_str_equal(plugin->alias,""))
	{
		g_strlcpy(plugin->alias, plugin->name, sizeof(plugin->alias));
	}
	ly_ui_pl_set(plugin);
	return plugin;
}

void ly_ui_pl_new_start_cb(GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer data, GError **error)
{
	if(ly_ui_pl_element_name)
	{
		g_free(ly_ui_pl_element_name);
		ly_ui_pl_element_name=NULL;
	}
	ly_ui_pl_element_name=g_strdup (element_name);
}
void ly_ui_pl_new_text_cb(GMarkupParseContext * context, const gchar *text, gsize text_len, gpointer data, GError **error) 
{
	lyUiPlPlugin *plugin=data;
	if(!ly_ui_pl_element_name||!text)
		return;
	
	else if(g_str_equal(ly_ui_pl_element_name,"alias"))
		g_strlcpy(plugin->alias, text, sizeof(plugin->alias));
	else if(g_str_equal(ly_ui_pl_element_name,"version"))
		g_strlcpy(plugin->version, text, sizeof(plugin->version));
	else if(g_str_equal(ly_ui_pl_element_name,"author"))
		g_strlcpy(plugin->author, text, sizeof(plugin->author));
	else if(g_str_equal(ly_ui_pl_element_name,"license"))
		g_strlcpy(plugin->license, text, sizeof(plugin->license));
	else if(g_str_equal(ly_ui_pl_element_name,"logo"))
		g_strlcpy(plugin->logo, text, sizeof(plugin->logo));
	else if(g_str_equal(ly_ui_pl_element_name,"comment"))
		g_strlcpy(plugin->comment, text, sizeof(plugin->comment));
	else if(g_str_equal(ly_ui_pl_element_name,"website"))
		g_strlcpy(plugin->website, text, sizeof(plugin->website));
	else if(g_str_equal(ly_ui_pl_element_name,"depends"))
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
		lyUiPlPlugin *plugin=NULL;
		for(i=0;plugins[i]!=NULL;i++)
		{
			if(ly_ui_pl_get(plugins[i]))
				continue;
			if(!(plugin=ly_ui_pl_new(plugins[i])))
				continue;
			ly_ui_pl_set_depend(plugin->name,plugins[i]);
		}
	}
	else if(g_str_equal(ly_ui_pl_element_name,"deamon"))
		sscanf(text,"%d",&(plugin->deamon));
	else if(g_str_equal(ly_ui_pl_element_name,"create_symbol"))
		g_strlcpy(plugin->create_symbol, text, sizeof(plugin->create_symbol));
	else if(g_str_equal(ly_ui_pl_element_name,"refresh_symbol"))
		g_strlcpy(plugin->refresh_symbol, text, sizeof(plugin->refresh_symbol));
	else if(g_str_equal(ly_ui_pl_element_name,"destroy_symbol"))
		g_strlcpy(plugin->destroy_symbol, text, sizeof(plugin->destroy_symbol));
	else if(g_str_equal(ly_ui_pl_element_name,"config_symbol"))
		g_strlcpy(plugin->config_symbol, text, sizeof(plugin->config_symbol));
}
void ly_ui_pl_new_end_cb(GMarkupParseContext *context, const gchar *element_name, gpointer data, GError **error)
{
	if(ly_ui_pl_element_name)
	{
		g_free(ly_ui_pl_element_name);
		ly_ui_pl_element_name=NULL;
	}
}

GList *ly_ui_pl_get_depends(gchar *name)
{
	GList *depends=NULL;
	GList *p=ly_ui_pl_depends;
	lyUiPlDepend *pdata;
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
gboolean ly_ui_pl_set_depend(gchar *name, gchar *depend)
{
	lyUiPlDepend *d=(lyUiPlDepend *)g_malloc(sizeof(lyUiPlDepend));
	g_strlcpy(d->name,name,sizeof(d->name));
	g_strlcpy(d->depend,depend,sizeof(d->depend));
	
	ly_ui_pl_depends=g_list_append(ly_ui_pl_depends, d);
	return TRUE;
}

gboolean ly_ui_pl_set_active(gchar *name, gboolean active)
{
	if(!name||g_str_equal(name,""))
		return FALSE;
	lyUiPlPlugin *pl=ly_ui_pl_get(name);
	if(!pl)
		return FALSE;
	
	if(active)
	{
		if(pl->module)
			return TRUE;
		
		GList *list=ly_ui_pl_get_depends(name);
		GList *p=list;
		while(p)
		{
			if(!ly_ui_pl_set_active((gchar*)(p->data), TRUE))
				return FALSE;
			p=p->next;
		}
		char path[1024]="";
		g_snprintf(path, sizeof(path),"%splugin/%s/lib%s.so",LY_GLOBAL_PROGDIR, pl->name, pl->name);
		pl->module=g_module_open(path,G_MODULE_BIND_LAZY);
		if(!(pl->module))
		{
			return FALSE;
		}
	}
	else
	{
		if(!(pl->module))
			return TRUE;
	}
	
	return TRUE;
}