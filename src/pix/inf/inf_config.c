/*
 * inf_config.c
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
#include "inf_config.h"

/*
 * VARIABLES
 */

/*
 * FUNCTIONS
 */
gboolean ly_3inf_config_on_title_font_set_cb		(GtkWidget *widget, gpointer data);
gboolean ly_3inf_config_on_normal_font_set_cb		(GtkWidget *widget, gpointer data);

GtkWidget* ly_3inf_config()
{
	GtkWidget *page;
	GtkWidget *item;
	GtkWidget *button;
	GtkWidget *label;
	GtkWidget *table;
	char str[1024]="";

	gchar *name;
	gchar *alias;
	gchar *logo;
	LyPliPlugin *pl=ly_pli_get("inf");
	g_object_get(G_OBJECT(pl), "name", &name,"alias", &alias, "logo", &logo, NULL);
	page=ly_cfg_page_new(name, alias, logo);
	g_free(name);
	g_free(alias);
	g_free(logo);
	name=NULL;
	alias=NULL;
	logo=NULL;

	item=ly_cfg_item_new(_("Font"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	table=gtk_grid_new();
	ly_cfg_item_append(LY_CFG_ITEM(item), table);
	label=gtk_label_new(_("Title Font"));
	gtk_grid_attach(GTK_GRID(table), label, 0, 1, 0, 1);
	ly_reg_get("3inf_title_font", "%1024[^\n]", str);
	button=gtk_font_button_new_with_font(str);
	g_signal_connect(G_OBJECT(button), "font-set", G_CALLBACK(ly_3inf_config_on_title_font_set_cb), NULL);
	gtk_grid_attach(GTK_GRID(table), button, 1, 2, 0, 1);
	label=gtk_label_new(_("Normal Font"));
	gtk_grid_attach(GTK_GRID(table), label, 0, 1, 1, 2);
	ly_reg_get("3inf_normal_font", "%1024[^\n]", str);
	button=gtk_font_button_new_with_font(str);
	g_signal_connect(G_OBJECT(button), "font-set", G_CALLBACK(ly_3inf_config_on_normal_font_set_cb), NULL);
	gtk_grid_attach(GTK_GRID(table), button, 1, 2, 1, 2);

	return page;
}

gboolean ly_3inf_config_on_title_font_set_cb(GtkWidget *widget, gpointer data)
{
	gchar font[1024]="Sans Regular 18";
	g_strlcpy(font, gtk_font_button_get_font_name(GTK_FONT_BUTTON(widget)), sizeof(font));
	ly_reg_set("3inf_title_font", "%s", font);
	return FALSE;
}
gboolean ly_3inf_config_on_normal_font_set_cb(GtkWidget *widget, gpointer data)
{
	gchar font[1024]="Sans Regular 10";
	g_strlcpy(font, gtk_font_button_get_font_name(GTK_FONT_BUTTON(widget)), sizeof(font));
	ly_reg_set("3inf_normal_font", "%s", font);
	return FALSE;
}
