/*
 * pl.h
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



#ifndef LY_PLI_H
#define LY_PLI_H

/*
 * HEADERS
 */
#include "glb.h"
#include "key.h"
#include "win.h"

/*
 * TYPES
 */
typedef struct _LyPliPlugin
{
	gchar name[256];
	gchar alias[256];
	gchar version[256];
	gchar author[256];
	gchar license[10240];
	gchar logo[256];
	gchar comment[2048];
	gchar website[1024];
	
	GModule *module;
	gpointer widget;
	gint daemon;
	
	gchar create_symbol[256];
	gchar refresh_symbol[256];
	gchar destroy_symbol[256];
	gchar config_symbol[256];
}LyPliPlugin;

typedef struct _LyPliDepend
{
	gchar name[256];
	gchar depend[256];
}LyPliDepend;



/*
 * FUNCTIONS
 */
void		ly_pli_init			();
void		ly_pli_fina			();

LyPliPlugin*	ly_pli_new			(const char *dir, char *filename);
void			ly_pli_new_start_cb	(GMarkupParseContext *context,
										 const gchar *element_name,
										 const gchar **attribute_names,
										 const gchar **attribute_values,
										 gpointer data,
										 GError **error);
void			ly_pli_new_text_cb	(GMarkupParseContext * context,
										 const gchar * text,
										 gsize text_len,
										 gpointer data, 
										 GError **error);
void			ly_pli_new_end_cb		(GMarkupParseContext *context,
										 const gchar *element_name,
										 gpointer data,
										 GError **error);

void			ly_pli_set				(LyPliPlugin *plugin);
LyPliPlugin*	ly_pli_get				(gchar *name);
void			ly_pli_del				(gchar *name);

gboolean		ly_pli_set_active		(gchar *name, gboolean active);
gboolean		ly_pli_get_active		(gchar *name);
GList*			ly_pli_get_depends		(gchar *name);
gboolean		ly_pli_set_depend		(gchar *name, gchar *depend);

GHashTable*		ly_pli_get_plugins		();


#endif	//LY_PLI_H
