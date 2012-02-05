/*
 * sss.h
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



#ifndef LY_SSS_H
#define LY_SSS_H

/*
 * HEADERS
 */
#include "glb.h"
#include "key.h"
#include "win.h"
#include "pli.h"

/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */
void		ly_sss_init		();
void		ly_sss_fina		();

gboolean 	ly_sss_create	(gchar *name, GtkWidget *widget);
gboolean 	ly_sss_refresh	();
gboolean 	ly_sss_destroy	(GtkWidget *widget);

gboolean	ly_sss_tab_add_init		();
gboolean	ly_sss_tab_add_fina		();
gboolean	ly_sss_tab_add_create	();
gboolean	ly_sss_tab_add_refresh	();
gboolean	ly_sss_tab_add_destroy	();

GdkPixbuf* ly_sss_alloc_bg(char *bg);

gboolean ly_sss_on_open_cb(GtkIconView *iconview,GtkTreePath *path,gpointer data);
gboolean ly_sss_on_close_cb(GtkWidget *widget, gpointer data);
#endif
