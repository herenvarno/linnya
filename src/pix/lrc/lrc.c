/*
 * lrc.c
 * This file is part of linnya:plugin
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya:plugin is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya:plugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya:plugin. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * HEADERS
 */
#include "lrc.h"


/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */
const gchar* g_module_check_init(GModule *module)
{
	gchar title_font[1024]="Sans Regular 20";
	if(!ly_reg_get("3lrc_title_font", "%1024[^\n]", title_font))
		ly_reg_set("3lrc_title_font", "%s", title_font);
	gchar normal_font[1024]="Sans Regular 10";
	if(!ly_reg_get("3lrc_normal_font", "%1024[^\n]", normal_font))
		ly_reg_set("3lrc_normal_font", "%s", normal_font);
	gchar desktop_font[1024]="Sans Regular 25";
	if(!ly_reg_get("3lrc_desktop_font", "%1024[^\n]", desktop_font))
		ly_reg_set("3lrc_desktop_font", "%s", desktop_font);

	ly_3lrc_desktop_init();
	return NULL;
}
void g_module_unload(GModule *module)
{
	ly_3lrc_desktop_fina();
}

GtkWidget *ly_3lrc_create()
{
	return ly_3lrc_widget_create();
}
void ly_3lrc_refresh()
{
	ly_3lrc_widget_refresh();
}

void ly_3lrc_destroy()
{
	ly_3lrc_widget_destroy();
}
