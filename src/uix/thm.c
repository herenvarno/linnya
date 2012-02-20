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
void	ly_thm_init()
{
	ly_log_put(_("[info] Initial ui module: THEME ..."));
	
	/*
	 * REGIST CONF: thm_theme
	 */
	char theme[128]="default";
	if(!ly_reg_get("thm_theme", "%128[^\n]", theme))
	{
		ly_reg_set("thm_theme", "%s", theme);
	}
	/*
	 * REGIST CONF: thm_decorated
	 */
	int decorated=1;
	if(!ly_reg_get("thm_decorated", "%d", &decorated))
	{
		ly_reg_set("thm_decorated", "%d", decorated);
	}
	/*
	 * REGIST CONF: thm_custom_winbg
	 */
	int custom_winbg=0;
	if(!ly_reg_get("thm_custom_winbg", "%d", &custom_winbg))
	{
		ly_reg_set("thm_custom_winbg", "%d", custom_winbg);
	}
	/*
	 * REGIST CONF: thm_custom_sssbg
	 */
	int custom_sssbg=0;
	if(!ly_reg_get("thm_custom_sssbg", "%d", &custom_sssbg))
	{
		ly_reg_set("thm_custom_sssbg", "%d", custom_sssbg);
	}
	/*
	 * REGIST CONF: thm_winbg
	 */
	guint winbg[4]={0, 0, 0, 65535};
	if(!ly_reg_get("thm_winbg", "%d:%d:%d:%d", &winbg[0], &winbg[1], &winbg[2], &winbg[3]))
	{
		ly_reg_set("thm_winbg", "%d:%d:%d:%d", winbg[0], winbg[1], winbg[2], winbg[3]);
	}
	/*
	 * REGIST CONF: thm_sssbg
	 */
	char sssbg[512]="";
	if(!ly_reg_get("thm_sssbg", "%512[^\n]", sssbg))
	{
		ly_reg_set("thm_sssbg", "%s", sssbg);
	}
	
	/*
	 * build user theme directory
	 */
	gchar path[1024]="";
	g_snprintf(path, sizeof(path), "%sui/theme", LY_GLA_USERDIR);
	mkdir(path,0755);
	
	/*
	 * Load themes
	 */
	ly_thm_item_new_from_conf();
}

void		ly_thm_fina()
{

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

LyThmItem* ly_thm_item_new()
{
	LyThmItem *item=NULL;
	item=(LyThmItem*)g_malloc(sizeof(LyThmItem));
	if(!item)
		return NULL;
	g_strlcpy(item->name, "", sizeof(item->name));
	g_strlcpy(item->sssbg, "", sizeof(item->sssbg));
	g_strlcpy(item->style, "", sizeof(item->style));
	return item;
}

LyThmItem* ly_thm_item_new_with_name(gchar *name)
{
	if(!name || g_str_equal(name, ""))
		return NULL;
	gchar path[512]="";
	gchar dir[512]="";
	g_snprintf(dir,sizeof(dir),"%sui/theme/%s/",LY_GLA_USERDIR, name);
	if(!g_file_test(dir, G_FILE_TEST_IS_DIR))
	{
		g_snprintf(dir,sizeof(dir),"%sui/theme/%s/",LY_GLA_PROGDIR, name);
		if(!g_file_test(dir, G_FILE_TEST_IS_DIR))
		{
			ly_msg_put("warning", "ui:theme", "Theme not found!");
			return NULL;
		}
	}
	
	LyThmItem *item=ly_thm_item_new();
	if(!item)
		return NULL;
	g_strlcpy(item->name, name, sizeof(item->name));
	g_snprintf(path, sizeof(path), "%ssssbg/", dir);
	if(g_file_test(path, G_FILE_TEST_IS_DIR))
	{
		g_strlcpy(item->sssbg, path, sizeof(item->sssbg));
	}
	g_snprintf(path, sizeof(path), "%sgtk-3.0/gtk.css", dir);
	if(g_file_test(path, G_FILE_TEST_EXISTS))
	{
		g_strlcpy(item->style, path, sizeof(item->style));
	}
	
	return item;
}

LyThmItem* ly_thm_item_new_from_conf()
{
	char theme[128]="default";
	ly_reg_get("thm_theme", "%128[^\n]", theme);
	LyThmItem *item=ly_thm_item_new_with_name(theme);
	return item;
}

void ly_thm_item_free(LyThmItem* item)
{
	g_free(item);
}
