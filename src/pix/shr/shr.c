/*
 * shr.c
 * This file is part of linnya
 *
 * Copyright (C) 2012 - Edward
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
#include "shr.h"

/*
 * MACROS
 */

/*
 * VARIABLES
 */

/*
 * FUNCTIONS
 */
const gchar*	g_module_check_init		(GModule *module)
{
	return NULL;
}

void			g_module_unload			(GModule *module)
{

}

GtkWidget*		ly_3shr_create			()
{
	GtkWidget *widget;
	ClutterActor *stage;
	widget=gtk_clutter_embed_new();
	stage=gtk_clutter_embed_get_stage(GTK_CLUTTER_EMBED(widget));

	ClutterActor *list;
	list=mx_list_view_new();


}

void			ly_3shr_destroy			()
{

}

void			ly_3shr_refresh			()
{

}
