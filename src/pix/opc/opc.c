/*
 * opc.c
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
#include "opc.h"


/*
 * VARIABLES
 */
gchar sql_right[1024];
int limit_right=10;
int offset_right=0;

const gchar* g_module_check_init(GModule *module)
{
	int offset=0;
	int limit=-1;
	int index0=0;
	int index1=0;
	int id=0;
	char name[1024]="unknown";
	
	if(!ly_reg_get("3opc_limit", "%d:%d", &offset, &limit))
	{
		ly_reg_set("3opc_limit", "%d:%d", offset, limit);
	}
	if(!ly_reg_get("3opc_select", "%d:%d:%d:%s", &index0, &index1, &id, name))
	{
		ly_reg_set("3opc_select", "%d:%d:%d:%s", index0, index1, id, name);
	}
	
	ly_3opc_left_init();
	ly_3opc_right_init();
	return NULL;
}
void g_module_unload(GModule *module)
{
	ly_3opc_right_fina();
	ly_3opc_left_fina();
}

GtkWidget *ly_3opc_create()
{
	widget=gtk_event_box_new();
	hpaned=gtk_hpaned_new();
	gtk_paned_set_position (GTK_PANED(hpaned),180);
	gtk_container_add(GTK_CONTAINER(widget),hpaned);
	
	GtkWidget *w_l=ly_3opc_left_create();
	GtkWidget *w_r=ly_3opc_right_create();
	gtk_container_add(GTK_CONTAINER(hpaned), w_l);
	gtk_container_add(GTK_CONTAINER(hpaned), w_r);

	gtk_widget_set_name(widget, "ly_pl_treeview_manager");
	return widget;
}

void ly_3opc_destroy()
{
	ly_3opc_right_destroy();
	ly_3opc_left_destroy();
}

