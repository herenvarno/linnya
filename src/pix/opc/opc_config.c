/*
 * opc_config.c
 * This file is part of linnya:plugin
 *
 * Copyright (C) 2012 - Edward Yang
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
#include "opc_config.h"

/*
 * VARIABLES
 */

/*
 * FUNCTIONS
 */
gboolean ly_3opc_config_on_limit_changed_cb(GtkWidget *widget, gpointer data);

GtkWidget* ly_3opc_config()
{
	GtkWidget *page;
	GtkWidget *item;
	GtkWidget *hbox;
	GtkWidget *button;
	GtkWidget *label;

	gchar *name;
	gchar *alias;
	gchar *logo;
	LyPliPlugin *pl=ly_pli_get("opc");
	g_object_get(G_OBJECT(pl), "name", &name,"alias", &alias, "logo", &logo, NULL);
	page=ly_cfg_page_new(name, alias, logo);
	g_free(name);
	g_free(alias);
	g_free(logo);
	name=NULL;
	alias=NULL;
	logo=NULL;

	item=ly_cfg_item_new(_("Limitation of A Single Page"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	ly_cfg_item_append(LY_CFG_ITEM(item), hbox);

	gint limit=0;
	ly_reg_get("3opc_limit", "%*d:%d", &limit);
	button= gtk_spin_button_new_with_range(-1, 1000, 50);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(button), limit);
	gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE , 0);
	label=gtk_label_new(_("Attention: \"-1\" is for Unlimited!"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE , 0);
	g_signal_connect(G_OBJECT(button), "value-changed", G_CALLBACK(ly_3opc_config_on_limit_changed_cb), NULL);

	return page;
}

gboolean ly_3opc_config_on_limit_changed_cb(GtkWidget *widget, gpointer data)
{
	gint offset=0;
	gint limit=0;
	ly_reg_get("3opc_limit", "%d:%d", &offset, &limit);
	limit=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
	ly_reg_set("3opc_limit", "%d:%d", offset, limit);
	return FALSE;
}
