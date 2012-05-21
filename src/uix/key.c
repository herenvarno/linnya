/*
 * key.c
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
#include "key.h"


/*
 * VARIABLES
 */
GHashTable *ly_key_keybinds;
GtkAccelGroup *ly_key_accel;

/*
 * FUNCTIONS
 */
gboolean	ly_key_read				(void);
void		ly_key_read_start_cb	(GMarkupParseContext *context,
									const gchar *element_name,
									const gchar **attribute_names,
									const gchar **attribute_values,
									gpointer data,
									GError **error);
gboolean	ly_key_write			(void);
guint		ly_key_get_mask			(gchar *mask);

/**
 *
 */
void		ly_key_init	()
{
	ly_key_accel=gtk_accel_group_new();
	ly_key_keybinds=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, ly_key_free);
	ly_key_read();
}

/**
 */
void		ly_key_fina	()
{
	ly_key_write();
	g_hash_table_destroy(ly_key_keybinds);
	g_object_unref(ly_key_accel);
}


LyKeyKeybind*	ly_key_new			(char *name)
{
	if((!name)||(g_str_equal(name, "")))
	{
		return NULL;
	}
	LyKeyKeybind *kb=(LyKeyKeybind*)g_malloc(sizeof(LyKeyKeybind));
	if(!kb)
	{
		return NULL;
	}
	g_strlcpy(kb->name, name, sizeof(kb->name));
	g_strlcpy(kb->mask0, "", sizeof(kb->mask0));
	g_strlcpy(kb->mask1, "", sizeof(kb->mask1));
	g_strlcpy(kb->key, "", sizeof(kb->key));
	kb->type=KEY_BIND_TYPE_UNDEFINED;
	kb->arg0=NULL;
	kb->arg1=NULL;
	return kb;
}

void			ly_key_free			(gpointer kb)
{
	if(kb)
		g_free(kb);
}

gboolean	ly_key_set		(char *name, char *mask0, char *mask1, char *key, int type, gpointer arg0, gpointer arg1)
{
	gboolean found=TRUE;
	LyKeyKeybind *kb=NULL;
	kb=ly_key_get(name);
	if(!kb)
	{
		found=FALSE;
		kb=ly_key_new(name);
		if(!kb)
		{
			return FALSE;
		}
	}
	
	if(mask0)
	{
		g_strlcpy(kb->mask0, mask0, sizeof(kb->mask0));
	}
	if(mask1)
	{
		g_strlcpy(kb->mask1, mask1, sizeof(kb->mask1));
	}
	if(key)
	{
		g_strlcpy(kb->key, key, sizeof(kb->key));
	}
	if(type>KEY_BIND_TYPE_UNDEFINED && type<=KEY_BIND_TYPE_CALLBACK)
	{
		kb->type=type;
	}
	if(arg0)
	{
		kb->arg0=arg0;
	}
	if(arg1)
	{
		kb->arg1=arg1;
	}
	if(!found)
	{
		gchar *namestr=g_strconcat(kb->name,NULL);
		g_hash_table_replace(ly_key_keybinds, namestr, kb);
	}
	return TRUE;
}

LyKeyKeybind *ly_key_get(char *name)
{
	if((!name)||(g_str_equal(name, "")))
		return NULL;

	LyKeyKeybind *kb=NULL;
	kb=g_hash_table_lookup(ly_key_keybinds, name);
	return kb;
}

/**
 */
void ly_key_del(gchar *name)
{
	g_hash_table_remove(ly_key_keybinds, name);
}

char *ly_key_get_conflict(char *except_name, char *mask0, char *mask1, char *key)
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
	g_hash_table_iter_init (&iter, ly_key_keybinds);
	
	LyKeyKeybind *k;
	while (g_hash_table_iter_next (&iter, &name, &value)) 
	{
		k=(LyKeyKeybind *)value;
		if(!g_str_equal(name, except_name) && g_str_equal(k->mask0, m0) && g_str_equal(k->mask1, m1) && g_str_equal(k->key, k0))
		{
			return (char *)name;
		}
	}
	return NULL;
}

gboolean ly_key_bind(gchar *name)
{
	LyKeyKeybind *keybind=NULL;
	keybind=ly_key_get(name);
	if(keybind==NULL)
	{
		return FALSE;
	}
	
	guint key[3]={0};
	key[0]=gdk_keyval_from_name(keybind->key);
	key[1]=ly_key_get_mask(keybind->mask0);
	key[2]=ly_key_get_mask(keybind->mask1);
	if(key[0]<=0 && key[1]<=0 && key[2]<=0)
		return FALSE;

	GtkWidget *widget=NULL;
	char *signal=NULL;
	gpointer f=NULL;
	gpointer data=NULL;
	switch(keybind->type)
	{
		case KEY_BIND_TYPE_SIGNAL:
			widget=(GtkWidget *)(keybind->arg0);
			signal=(char *)(keybind->arg1);
			gtk_widget_add_accelerator(widget, signal, ly_key_accel, key[0], key[1]|key[2], GTK_ACCEL_VISIBLE);
			break;
		case KEY_BIND_TYPE_CALLBACK:
			f=keybind->arg0;
			data= keybind->arg1;
			GClosure *closure;
			closure=g_cclosure_new(G_CALLBACK(f), data, NULL);
			gtk_accel_group_connect (ly_key_accel, key[0], key[1]|key[2], GTK_ACCEL_VISIBLE, closure);
			break;
		default:
			break;
	}
	return TRUE;
}
gboolean ly_key_unbind(char *name)
{
	LyKeyKeybind *keybind=NULL;
	keybind=ly_key_get(name);
	if(keybind==NULL)
	{
		return FALSE;
	}
	
	guint key[3]={0};
	key[0]=ly_key_get_mask(keybind->mask0);
	key[1]=ly_key_get_mask(keybind->mask1);
	key[2]=gdk_keyval_from_name(keybind->key);
	if(key[0]<=0 && key[1]<=0 && key[2]<=0)
		return FALSE;

	GtkWidget *widget=NULL;
	switch(keybind->type)
	{
		case KEY_BIND_TYPE_SIGNAL:
			widget=(GtkWidget *)(keybind->arg0);
			gtk_widget_remove_accelerator(widget, ly_key_accel, key[2], key[0]|key[1]);
			break;
		case KEY_BIND_TYPE_CALLBACK:
			gtk_accel_group_disconnect_key(ly_key_accel, key[2], key[0]|key[1]);
			break;
		default:
			break;
	}
	
	return TRUE;
}






gboolean ly_key_read(void)
{
	gchar path[1024]="";
	g_snprintf(path,sizeof(path),"%skey.xml",LY_GLB_USER_UIXDIR);
	
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		ly_log_put_with_flag(G_LOG_LEVEL_WARNING, _("Key defination file does not exist, linnya will create a new one when exit!"));
		return TRUE;
	}
	
	GMarkupParser parser=
	{
		.start_element = ly_key_read_start_cb,
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
		ly_log_put_with_flag(G_LOG_LEVEL_WARNING, _("Configuration file error!"));
		
		g_hash_table_destroy(ly_key_keybinds);
		ly_key_keybinds=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	}
	
	g_markup_parse_context_free(context);
	return TRUE;
}

/*
 * NAME:	ly_key_start_cb
 * VARS:	[...] ...
 * RETN:	[void]
 * DESC:	Callback for ly_key_read
 */
void ly_key_read_start_cb(	GMarkupParseContext *context,
						const gchar *element_name,
						const gchar **attribute_names,
						const gchar **attribute_values,
						gpointer data,
						GError **error)
{
	if(g_str_equal(element_name,"keybind"))
	{
		char name[1024]="";
		char key[1024]="";
		char mask0[1024]="";
		char mask1[1024]="";
		
		const gchar **name_cursor = attribute_names;
		const gchar **value_cursor = attribute_values;
		
		while (*name_cursor)
		{
			if (g_str_equal(*name_cursor, "name"))
				g_strlcpy(name,*value_cursor,sizeof(name));
			else if (g_str_equal(*name_cursor, "key"))
				g_strlcpy(key,*value_cursor,sizeof(key));
			else if (g_str_equal(*name_cursor, "mask0"))
				g_strlcpy(mask0,*value_cursor,sizeof(mask0));
			else if (g_str_equal(*name_cursor, "mask1"))
				g_strlcpy(mask1,*value_cursor,sizeof(mask1));
			name_cursor++;
			value_cursor++;
		}
		
		if(!g_str_equal(name, ""))
		{
			ly_key_set(name, mask0, mask1, key, KEY_BIND_TYPE_UNDEFINED, NULL, NULL);
		}
	}
}

/*
 * NAME:	ly_key_write
 * VARS:	[void]
 * RETN:	[gboolean]	TRUE for success, FALSE for fail.
 * DESC:	Write configurations to file.
 */
gboolean ly_key_write(void)
{
	gchar path[1024]="";
	g_snprintf(path,sizeof(path),"%skey.xml",LY_GLB_USER_UIXDIR);
	
	FILE *fp=NULL;
	gchar *buf=NULL;
	
	if(!(fp=fopen(path,"w+")))
	{
		ly_log_put_with_flag(G_LOG_LEVEL_WARNING, _("Cannot write keybinds to file!"));
		return FALSE;
	}
	
	buf=g_markup_printf_escaped ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<keybinds>\n");
	fputs(buf,fp);
	g_free(buf);
	
	GHashTableIter iter;
	gpointer name, value;
	g_hash_table_iter_init (&iter, ly_key_keybinds);
	
	LyKeyKeybind *k;
	while (g_hash_table_iter_next (&iter, &name, &value)) 
	{
		k=(LyKeyKeybind *)value;
		buf=g_markup_printf_escaped ("\t<keybind name=\"%s\" mask0=\"%s\" mask1=\"%s\" key=\"%s\"/>\n", (gchar*)name, k->mask0, k->mask1, k->key);
		fputs(buf,fp);
		g_free(buf);
	}
	
	buf=g_markup_printf_escaped ("</keybinds>");
	fputs(buf,fp);
	g_free(buf);
	
	fclose(fp);
	return TRUE;
}

guint ly_key_get_mask(gchar *mask)
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

GtkAccelGroup *ly_key_get_accel()
{
	return ly_key_accel;
}
GHashTable*		ly_key_get_keybinds	()
{
	return ly_key_keybinds;
}

