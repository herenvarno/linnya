/*
 * sss.obj.c
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
#include "sss.obj.h"

/*
 * MACROS
 */
#define LY_SSS_SESSION_GET_PRIVATE(obj) \
G_TYPE_INSTANCE_GET_PRIVATE((obj), LY_SSS_SESSION_TYPE, LySssSessionPrivate)


/*
 * TYPES
 */
typedef struct _LySssSessionPrivate LySssSessionPrivate;

struct _LySssSessionPrivate{
	GtkWidget *nav;
	GtkWidget *content;
};

/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */
static void	ly_sss_session_init			(LySssSession *obj);
static void	ly_sss_session_fina			(LySssSession *obj);
static void	ly_sss_session_class_init	(LySssSessionClass *klass);

GType ly_sss_session_get_type()
{
	static GType ly_sss_session_type = 0;
	if(!ly_sss_session_type)
	{
		static const GTypeInfo ly_sss_session_info = {
			sizeof(LySssSessionClass),
			NULL,
			NULL,
			(GClassInitFunc)ly_sss_session_class_init,
			NULL,
			NULL,
			sizeof(LySssSession),
			0,
			(GInstanceInitFunc)ly_sss_session_init,
			NULL
		};
		ly_sss_session_type = g_type_register_static(GTK_TYPE_WIDGET,"LySssSession",&ly_sss_session_info,0);
	}
	return ly_sss_session_type;
}
static void ly_sss_session_init(LySssSession *obj)
{
}
static void ly_sss_session_fina(LySssSession *obj)
{
}
static void ly_cfg_dialog_class_init(LyCfgDialogClass *klass)
{
}
GtkWidget *ly_sss_session_new(gchar *name)
{
	if(!name)
		return NULL;
	
	LyPliPlugin *plugin=NULL;
	plugin = ly_pli_get(name);
	if(!plugin)
		return NULL;
	
	gboolean daemon=TRUE;
	GtkWidget *widget=NULL;
	g_object_get(G_OBJECT(session), "daemon", &daemon, "widget", &widget, NULL);
	if(daemon)
		return NULL;
	if(!widget)
	{
		widget=ly_pli_plugin_create(session);
		if(widget==NULL)
			return NULL;
	}
	
	
	GtkWidget *session;
	sesseion = g_object_new(LY_SSS_SESSION_TYPE, NULL);
	if(!session)
		return NULL;
	
	
	
	
	
	LySssSessionPrivate *priv=LY_SSS_SESSION_GET_PRIVATE(LY_SSS_SESSION(session));
	priv->nav=gtk_grid_new();
	priv->content=widget;
	
	return session;
}

#endif