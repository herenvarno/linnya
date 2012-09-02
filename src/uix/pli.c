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
GList *ly_pli_list;

/*
 * FUNCTIONS
 */
void ly_pli_init()
{
	// CHECK MODULE SUPPORT
	if(!g_module_supported())
	{
		g_error(_("Your system does not support plugin, abort ..."));
	}

	// LOAD CONFIG
	gchar pli_list[1024]="";
	gchar **list_cf=NULL;
	if(!ly_reg_get("pli_list", "%1023[^\n]", pli_list))
	{
		ly_reg_set("pli_list", "%s", pli_list);
	}
	if(pli_list)
	{
		list_cf=g_strsplit(pli_list, ":", 0);
	}

	// GET THE EXIST PLUGIN LIST
	GList *list_pl=NULL;
	GList *p=NULL;
	GList *r=NULL;
	list_pl=ly_gla_get_subdirs(LY_GLB_PROG_PIXDIR, FALSE);

	if(list_cf)
	{
		gint i=0;
		while(list_cf[i])
		{
			p=list_pl;
			while(p)
			{
				if(g_str_equal(list_cf[i], p->data))
				{
					r=p;
					p=p->next;
					list_pl=g_list_remove_link(list_pl, r);
					ly_pli_list=g_list_concat(ly_pli_list, r);
					break;
				}
				p=p->next;
			}
			i++;
		}
	}

	ly_pli_list=g_list_concat(ly_pli_list, list_pl);
	p=ly_pli_list;
	while(p)
	{
		ly_pli_add(p->data);
		p=p->next;
	}
}

void	ly_pli_fina()
{
	gchar *name=NULL;
	gchar *str=NULL;
	gchar *tmp=NULL;
	LyPliPlugin *pl;
	GList *p=ly_pli_list;
	while(p)
	{
		if(LY_PLI_PLUGIN_IS_PLUGIN(p->data))
		{
			pl=LY_PLI_PLUGIN(p->data);
			g_object_get(G_OBJECT(pl), "name", &name, NULL);
			if(str)
			{
				str=g_strconcat(tmp, ":", name, NULL);
				g_free(tmp);
				tmp=str;
				g_free(name);
			}
			else
			{
				str=name;
				tmp=str;
			}
			g_object_unref(p->data);
			p->data=NULL;
		}
		else
		{
			g_free(p->data);
		}
		p=p->next;
	}
	ly_reg_set("pli_list", "%s", str);
	g_free(str);
	g_list_free(ly_pli_list);
}


gboolean	ly_pli_add(gchar *name)
{
	LyPliPlugin *pl=ly_pli_plugin_new(name);
	if(!pl)
		return FALSE;

	gchar *depends;
	gchar **list_depends;
	g_object_get(G_OBJECT(pl), "depends", &depends, NULL);
	if(depends)
	{
		list_depends=g_strsplit(depends, ":", 0);
		if(list_depends)
		{
			gint i=0;
			while(list_depends[i])
			{
				if(!ly_pli_add(list_depends[i]))
				{
					g_free(depends);
					return FALSE;
				}
				i++;
			}
		}
	}
	GList *p=ly_pli_list;
	while(p)
	{
		if(g_str_equal(name, p->data))
		{
			g_free(p->data);
			p->data=pl;
			break;
		}
		p=p->next;
	}
	g_free(depends);
	g_strfreev(list_depends);
	return TRUE;
}

LyPliPlugin* ly_pli_get(gchar *name)
{
	gchar *name1=NULL;
	GList *p=ly_pli_list;
	while(p)
	{
		g_object_get(G_OBJECT(p->data), "name", &name1, NULL);
		if(g_str_equal(name, name1))
		{
			g_free(name1);
			return p->data;
		}
		p=p->next;
	}
	g_free(name1);
	return NULL;
}

gboolean ly_pli_lock(gchar *name)
{
/*	if(!name||g_str_equal(name,""))
		return FALSE;
	LyPliPlugin *pl=ly_pli_get(name);
	if(!pl)
		return FALSE;

	if(pl->locked)
		return TRUE;

	GList *list=NULL;
	GList *p=NULL;
	list=ly_pli_get_depend_bys(name);
	p=list;
	while(p)
	{
		ly_pli_lock(p->data);
		p=p->next;
	}
	ly_pli_plugin_lock(pl);*/
	return TRUE;
}

gboolean ly_pli_unlock(gchar *name)
{
/*	if(!name||g_str_equal(name,""))
		return FALSE;
	LyPliPlugin *pl=ly_pli_get(name);
	if(!pl)
		return FALSE;

	GList *list=NULL;
	GList *p=NULL;

	if(pl->locked)
		return TRUE;

	list=ly_pli_get_depend_bys(name);
	p=list;
	while(p)
	{
		ly_pli_lock(p->data);
		p=p->next;
	}
	ly_pli_plugin_lock(pl);*/
	return TRUE;
}


GList *ly_pli_get_list()
{
	return ly_pli_list;
}

/**
 * ly_pli_change_order:
 * @plname: the name of plugin to be changed
 * @pos: the name of plugin being in front of the replaced plugin after being
 * changed. NULL is allowed which means put the plname plugin to the end of list
 *
 * Changed a plugin's position.
 * [UNTESTED]
 */
void ly_pli_change_order(gchar *plname, gchar *pos)
{
	GList *p=NULL;
	GList *q=NULL;
	LyPliPlugin *pl;
	gchar *name;
	p=ly_pli_list;
	while(p)
	{
		if(LY_PLI_PLUGIN_IS_PLUGIN(p->data))
		{
			pl=LY_PLI_PLUGIN(p->data);
			g_object_get(G_OBJECT(pl), "name", &name, NULL);
			if(g_str_equal(name, plname))
			{
				g_free(name);
				q=p;
				break;
			}
		}
	}
	if(!p)
		return;
	if(!pos)
	{
		pl=q->data;
		ly_pli_list=g_list_delete_link(ly_pli_list, q);
		ly_pli_list=g_list_insert_before(ly_pli_list, NULL, pl);
		return;
	}

	p=ly_pli_list;
	while(p)
	{
		if(LY_PLI_PLUGIN_IS_PLUGIN(p->data))
		{
			pl=LY_PLI_PLUGIN(p->data);
			g_object_get(G_OBJECT(pl), "name", &name, NULL);
			if(g_str_equal(name, pos))
			{
				pl=q->data;
				ly_pli_list=g_list_delete_link(ly_pli_list, q);
				ly_pli_list=g_list_insert_before(ly_pli_list, p, pl);
				break;
			}
		}
	}
}