/*
 * HEADERS
 */
#include "key.h"

/*
 * FUNCTIONS
 */

/*
 * NAME:	ly_ui_key_read
 * VARS:	[void]
 * RETN:	[gboolean]	TRUE for success, FALSE for fail.
 * DESC:	Read configurations from file.
 */
gboolean ly_ui_key_read(void)
{
	gchar path[128]="";
	g_snprintf(path,sizeof(path),"%sui/key.xml",LY_GLOBAL_USERDIR);
	
	GMarkupParser parser=
	{
		.start_element = ly_ui_key_start_cb,
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
		ly_global_debug("warnning",_("No configuration file found or read configurations error. \n"));
		
		g_hash_table_destroy(ly_ui_key_keybinds);
		ly_ui_key_keybinds=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
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
void ly_ui_key_start_cb(	GMarkupParseContext *context,
						const gchar *element_name,
						const gchar **attribute_names,
						const gchar **attribute_values,
						gpointer data,
						GError **error)
{
	if(g_str_equal(element_name,"keybind"))
	{
		gchar *name=NULL;
		
		const gchar **name_cursor = attribute_names;
		const gchar **value_cursor = attribute_values;
		
		lyUiKeyKeybind *keybind=(lyUiKeyKeybind *)g_malloc(sizeof(lyUiKeyKeybind));
		while (*name_cursor)
		{
			if (g_str_equal(*name_cursor, "name"))
				name = g_strdup (*value_cursor);
			else if (g_str_equal(*name_cursor, "key"))
				g_strlcpy(keybind->key,*value_cursor,sizeof(keybind->key));
			else if (g_str_equal(*name_cursor, "mask0"))
				g_strlcpy(keybind->mask0,*value_cursor,sizeof(keybind->mask0));
			else if (g_str_equal(*name_cursor, "mask1"))
				g_strlcpy(keybind->mask1,*value_cursor,sizeof(keybind->mask1));
			name_cursor++;
			value_cursor++;
		}
		
		if(!name)
		{
			if(name)
				g_free(name);
			if(keybind)
				g_free(keybind);
			return;
		}
		g_hash_table_replace(ly_ui_key_keybinds,name,keybind);
	}
}

/*
 * NAME:	ly_ui_key_write
 * VARS:	[void]
 * RETN:	[gboolean]	TRUE for success, FALSE for fail.
 * DESC:	Write configurations to file.
 */
gboolean ly_ui_key_write(void)
{
	gchar *path=g_strconcat(LY_GLOBAL_USERDIR,"ui/key.xml",NULL);
	
	FILE *fp=NULL;
	gchar *buf=NULL;
	
	if(!(fp=fopen(path,"w+")))
	{
		ly_global_debug("warnning",_("Cannot write keybinds to file!\n"));
		return FALSE;
	}
	
	buf=g_markup_printf_escaped ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<keybinds>\n");
	fputs(buf,fp);
	g_free(buf);
	
	g_hash_table_foreach(ly_ui_key_keybinds, ly_ui_key_write_each_cb, fp);
	
	buf=g_markup_printf_escaped ("</keybinds>");
	fputs(buf,fp);
	g_free(buf);
	
	fclose(fp);
	return TRUE;
}

/*
 * NAME:	ly_ui_key_write_each_cb
 * VARS:	[void]
 * RETN:	[void]
 * DESC:	Write each configuration to file. Called by ly_ui_key_write.
 */
void ly_ui_key_write_each_cb(gpointer name, gpointer value, gpointer data)
{
	FILE *fp=data;
	if(!fp)
	{
		ly_global_debug("warnning",_("Cannot write keybinds to file!\n"));
		return;
	}
	lyUiKeyKeybind *keybind=value;
	gchar *buf=NULL;
	buf=g_markup_printf_escaped ("\t<keybind name=\"%s\" mask0=\"%s\" mask1=\"%s\" key=\"%s\"/>\n", (gchar*)name, keybind->mask0, keybind->mask1, keybind->key);
	fputs(buf,fp);
	g_free(buf);
}

/*
 * NAME:	ly_ui_key_delete
 * VARS:	[gchar*]name	the name of conf.
 * RETN:	[void]
 * DESC:	delete a conf.
 */
void ly_ui_key_delete(gchar *name)
{
	g_hash_table_remove(ly_ui_key_keybinds, name);
}

/*
 * NAME:	ly_ui_key_init
 * VARS:	[gchar*]name	the name of conf.
 * 			[gchar*]value	the value of conf.
 * RETN:	[void]
 * DESC:	initialize the conf module.
 */
gboolean ly_ui_key_init(void)
{
	ly_global_debug("info",_("Initial ui module: KEY ...\n"));
	gboolean rt=FALSE;
	ly_ui_key_accel=gtk_accel_group_new();
	ly_ui_key_keybinds=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	rt=ly_ui_key_read();
	return rt;
}

/*
 * NAME:	ly_ui_key_finalize
 * VARS:	[gchar*]name	the name of conf.
 * 			[gchar*]value	the value of conf.
 * RETN:	[void]
 * DESC:	finalize the conf module.
 */
gboolean ly_ui_key_finalize(void)
{
	ly_global_debug("info",_("Finalize ui module: KEY ...\n"));
	gboolean rt=FALSE;
	rt=ly_ui_key_write();
	g_hash_table_destroy(ly_ui_key_keybinds);
	g_object_unref(ly_ui_key_accel);
	return rt;
}

/*
 * NAME:	ly_ui_key_set
 * VARS:	[gchar*]name	the name of conf.
 * 			[gchar*]format	the formate.
 * 			[...]
 * RETN:	[gbooelan]		TRUE for success, FALSE for fail.
 * DESC:	set or put a conf.
 */
gboolean ly_ui_key_set(gchar *name, gchar *mask0, gchar *mask1, gchar *key, int type, gpointer arg0, gpointer arg1)
{
	if(key==NULL)
		key="";
	if(mask0==NULL)
		mask0="";
	if(mask1==NULL)
		mask1="";
	
	lyUiKeyKeybind *k=(lyUiKeyKeybind *)g_malloc(sizeof(lyUiKeyKeybind));
	g_strlcpy(k->key,key,sizeof(k->key));
	g_strlcpy(k->mask0,mask0,sizeof(k->mask0));
	g_strlcpy(k->mask1,mask1,sizeof(k->mask1));
	k->type=type;
	k->arg0=arg0;
	k->arg1=arg1;
	
	gchar *namestr=g_strconcat(name,NULL);
	g_hash_table_replace(ly_ui_key_keybinds, namestr, k);
	
	return TRUE;
}

gboolean ly_ui_key_set_keys(gchar *name, gchar *mask0, gchar *mask1, gchar *key)
{
	if(key==NULL)
		key="";
	if(mask0==NULL)
		mask0="";
	if(mask1==NULL)
		mask1="";
	
	lyUiKeyKeybind *k=g_hash_table_lookup(ly_ui_key_keybinds, name);
	if(!k)
	{
		ly_ui_key_set(name, mask0, mask1, key, KEY_BIND_UNDEFINED, NULL, NULL);
	}
	g_strlcpy(k->key,key,sizeof(k->key));
	g_strlcpy(k->mask0,mask0,sizeof(k->mask0));
	g_strlcpy(k->mask1,mask1,sizeof(k->mask1));
	
	return TRUE;
}

gboolean ly_ui_key_set_args(char *name, int type, gpointer arg0, gpointer arg1)
{
	lyUiKeyKeybind *k=g_hash_table_lookup(ly_ui_key_keybinds, name);
	if(!k)
	{
		ly_ui_key_set(name, NULL, NULL, NULL, type, arg0, arg1);
	}
	k->type=type;
	k->arg0=arg0;
	k->arg1=arg1;
	
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
lyUiKeyKeybind *ly_ui_key_get(gchar *name)
{
	lyUiKeyKeybind *keybind=NULL;
	keybind=g_hash_table_lookup(ly_ui_key_keybinds, name);
	return keybind;
}

gboolean ly_ui_key_set_default_if_not_exist(gchar *name)
{
	if(g_hash_table_lookup(ly_ui_key_keybinds, name))
		return TRUE;
	ly_ui_key_set(name, NULL, NULL, NULL, KEY_BIND_UNDEFINED, NULL, NULL);
	return FALSE;
}

gboolean ly_ui_key_bind(gchar *name)
{
	lyUiKeyKeybind *keybind=NULL;
	keybind=ly_ui_key_get(name);
	if(keybind==NULL)
	{
		return FALSE;
	}
	
	guint key[3]={0};
	key[0]=gdk_keyval_from_name(keybind->key);
	key[1]=ly_ui_key_get_mask(keybind->mask0);
	key[2]=ly_ui_key_get_mask(keybind->mask1);
	if(key[0]<=0 && key[1]<=0 && key[2]<=0)
		return FALSE;

	GtkWidget *widget=NULL;
	char *signal=NULL;
	gpointer f=NULL;
	gpointer data=NULL;
	switch(keybind->type)
	{
		case KEY_BIND_SIGNAL:
			widget=(GtkWidget *)(keybind->arg0);
			signal=(char *)(keybind->arg1);
			gtk_widget_add_accelerator(widget, signal, ly_ui_key_accel, key[0], key[1]|key[2], GTK_ACCEL_VISIBLE);
			break;
		case KEY_BIND_CALLBACK:
			f=keybind->arg0;
			data= keybind->arg1;
			GClosure *closure;
			closure=g_cclosure_new(G_CALLBACK(f), data, NULL);
			gtk_accel_group_connect (ly_ui_key_accel, key[0], key[1]|key[2], GTK_ACCEL_VISIBLE, closure);
			break;
		default:
			break;
	}
	return TRUE;
}
gboolean ly_ui_key_unbind(char *name)
{
	lyUiKeyKeybind *keybind=NULL;
	keybind=ly_ui_key_get(name);
	if(keybind==NULL)
	{
		return FALSE;
	}
	
	guint key[3]={0};
	key[0]=ly_ui_key_get_mask(keybind->mask0);
	key[1]=ly_ui_key_get_mask(keybind->mask1);
	key[2]=gdk_keyval_from_name(keybind->key);
	if(key[0]<=0 && key[1]<=0 && key[2]<=0)
		return FALSE;

	GtkWidget *widget=NULL;
	switch(keybind->type)
	{
		case KEY_BIND_SIGNAL:
			widget=(GtkWidget *)(keybind->arg0);
			gtk_widget_remove_accelerator(widget, ly_ui_key_accel, key[2], key[0]|key[1]);
			break;
		case KEY_BIND_CALLBACK:
			gtk_accel_group_disconnect_key(ly_ui_key_accel, key[2], key[0]|key[1]);
			break;
		default:
			break;
	}
	
	return TRUE;
}
guint ly_ui_key_get_mask(gchar *mask)
{
	gchar *str=g_ascii_strdown(mask,-1);
	guint k=0;
	if(g_str_equal(str,"ctrl")||g_str_equal(str,"control"))
	{
		k=GDK_CONTROL_MASK;
	}
	else if(g_str_equal(str,"alt"))
	{
		k=GDK_MOD1_MASK;
	}
	else if(g_str_equal(str,"shift"))
	{
		k=GDK_SHIFT_MASK;
	}
	else if(g_str_equal(str,"super"))
	{
		k=GDK_SUPER_MASK;
	}
	else if(g_str_equal(str,"meta"))
	{
		k=GDK_META_MASK;
	}
	else if(g_str_equal(str,"hyper"))
	{
		k=GDK_HYPER_MASK;
	}
	g_free(str);
	return k;
}

char *ly_ui_key_get_conflict(char *except_name, char *mask0, char *mask1, char *key)
{
	char m0[1024]="";
	char m1[1024]="";
	char k0[1024]="";
	
	gboolean changed=FALSE;
	if(mask0!=NULL)
	{
		g_strlcpy(m0, mask0, sizeof(m0));
		changed=TRUE;
	}
	if(mask1!=NULL)
	{
		g_strlcpy(m1, mask1, sizeof(m1));
		changed=TRUE;
	}
	if(key!=NULL)
	{
		g_strlcpy(k0, key, sizeof(k0));
		changed=TRUE;
	}
	
	if(!changed)
	{
		return NULL;
	}
	
	GHashTableIter iter;
	gpointer name, value;
	g_hash_table_iter_init (&iter, ly_ui_key_keybinds);
	
	int i=1;
	lyUiKeyKeybind *k;
	while (g_hash_table_iter_next (&iter, &name, &value)) 
	{
		k=(lyUiKeyKeybind *)value;
		if(!g_str_equal(name, except_name) && g_str_equal(k->mask0, m0) && g_str_equal(k->mask1, m1) && g_str_equal(k->key, k0))
		{
			return (char *)name;
		}
		i++;
	}
	return NULL;
}
