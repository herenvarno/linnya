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
	buf=g_markup_printf_escaped ("\t<keybind name=\"%s\" key=\"%s\" mask0=\"%s\" mask1=\"%s\"/>\n", (gchar*)name, keybind->key, keybind->mask0, keybind->mask1);
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
gboolean ly_ui_key_set(gchar *name, lyUiKeyKeybind *keybind)
{
	lyUiKeyKeybind *key=(lyUiKeyKeybind *)g_malloc(sizeof(lyUiKeyKeybind));
	g_strlcpy(key->key,keybind->key,sizeof(key->key));
	g_strlcpy(key->key,keybind->mask0,sizeof(key->mask0));
	g_strlcpy(key->key,keybind->mask1,sizeof(key->mask1));
	gchar *namestr=g_strconcat(name,NULL);
	g_hash_table_replace(ly_ui_key_keybinds, namestr, key);
	
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


gboolean ly_ui_key_bind_signal(gchar *name, GtkWidget* widget, gchar *signal)
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
	gtk_widget_add_accelerator(widget, signal, ly_ui_key_accel, key[0], key[1]|key[2], GTK_ACCEL_VISIBLE);
	return TRUE;
}

gboolean ly_ui_key_bind_callback(gchar *name, gpointer f, gpointer data)
{
	lyUiKeyKeybind *keybind=NULL;
	keybind=ly_ui_key_get(name);
	if(!keybind)
		return FALSE;

	guint key[3]={0};
	key[0]=gdk_keyval_from_name(keybind->key);
	key[1]=ly_ui_key_get_mask(keybind->mask0);
	key[2]=ly_ui_key_get_mask(keybind->mask1);
	GClosure *closure;
	closure=g_cclosure_new(G_CALLBACK(f), data, NULL);
	gtk_accel_group_connect (ly_ui_key_accel, key[0], key[1]|key[2], GTK_ACCEL_VISIBLE, closure);
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