/*
 * mbs.obj.c
 * This file is part of linnya
 *
 * Copyright (C) 2010-2012 - Edward
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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
#include "mbs.obj.h"

/*
 * MACROS
 */
#define LY_MBS_MESSAGE_GET_PRIVATE(obj) \
	G_TYPE_INSTANCE_GET_PRIVATE((obj), LY_MBS_MESSAGE_TYPE, LyMbsMessagePrivate)
#define LY_MBS_FILTER_GET_PRIVATE(obj) \
	G_TYPE_INSTANCE_GET_PRIVATE((obj), LY_MBS_FILTER_TYPE, LyMbsFilterPrivate)
#define LY_MBS_BUS_GET_PRIVATE(obj) \
	G_TYPE_INSTANCE_GET_PRIVATE((obj), LY_MBS_BUS_TYPE, LyMbsBusPrivate)

/*
 * TYPES
 */
typedef struct _LyMbsMessagePrivate LyMbsMessagePrivate;
typedef struct _LyMbsFilterPrivate LyMbsFilterPrivate;
typedef struct _LyMbsBusPrivate LyMbsBusPrivate;

struct _LyMbsMessagePrivate{
	gchar *title;
	gchar *from;
	gchar *body;
};
struct _LyMbsFilterPrivate{
	gchar *title;
	gchar *from;
	LyMbsFilterBindFunc func;
	gpointer data;
};
struct _LyMbsBusPrivate{
	GQueue *queue;
	GList *table;
};

/*
 * FUNCTIONS
 */
static void ly_mbs_message_init(LyMbsMessage *obj);
static void ly_mbs_message_final(GObject *obj);
static void ly_mbs_message_class_init(LyMbsMessageClass *klass);
static void ly_mbs_filter_init(LyMbsFilter *obj);
static void ly_mbs_filter_final(GObject *obj);
static void ly_mbs_filter_class_init(LyMbsFilterClass *klass);
static void ly_mbs_bus_init(LyMbsBus *obj);
static void ly_mbs_bus_final(GObject *obj);
static void ly_mbs_bus_class_init(LyMbsBusClass *klass);

GType ly_mbs_message_get_type()
{
	static GType ly_mbs_message_type = 0;
	if(!ly_mbs_message_type)
	{
		static const GTypeInfo ly_mbs_message_info={
			sizeof(LyMbsMessageClass),
			NULL,
			NULL,
			(GClassInitFunc)ly_mbs_message_class_init,
			NULL,
			NULL,
			sizeof(LyMbsMessage),
			0,
			(GInstanceInitFunc)ly_mbs_message_init,
			NULL
		};
		ly_mbs_message_type=g_type_register_static(G_TYPE_OBJECT, "LyMbsMessage", &ly_mbs_message_info, 0);
	}
	
	return ly_mbs_message_type;
}
static void ly_mbs_message_init(LyMbsMessage *obj)
{

}
static void ly_mbs_message_final(GObject *obj)
{
	LyMbsMessagePrivate *priv=LY_MBS_MESSAGE_GET_PRIVATE(LY_MBS_MESSAGE(obj));
	g_free(priv->title);
	g_free(priv->from);
	g_free(priv->body);
}
static void ly_mbs_message_class_init(LyMbsMessageClass *klass)
{
	GObjectClass *object_class=G_OBJECT_CLASS(klass);
	object_class->finalize=ly_mbs_message_final;
	g_type_class_add_private(klass, sizeof(LyMbsMessagePrivate));
}
LyMbsMessage* ly_mbs_message_new(gchar *title, gchar *from, gchar *body)
{
	g_return_val_if_fail((title!=NULL && from!=NULL), NULL);
	LyMbsMessage *message;
	message=g_object_new(LY_MBS_MESSAGE_TYPE, NULL);
	LyMbsMessagePrivate *priv=LY_MBS_MESSAGE_GET_PRIVATE(LY_MBS_MESSAGE(message));
	priv->title=g_strconcat(title, NULL);
	priv->from=g_strconcat(from, NULL);
	priv->body=g_strconcat(body, NULL);
	return message;
}
const gchar *ly_mbs_message_get_title(LyMbsMessage *message)
{
	LyMbsMessagePrivate *priv=LY_MBS_MESSAGE_GET_PRIVATE(LY_MBS_MESSAGE(message));
	return priv->title;
}
const gchar *ly_mbs_message_get_from(LyMbsMessage *message)
{
	LyMbsMessagePrivate *priv=LY_MBS_MESSAGE_GET_PRIVATE(LY_MBS_MESSAGE(message));
	return priv->from;
}
const gchar *ly_mbs_message_get_body(LyMbsMessage *message)
{
	LyMbsMessagePrivate *priv=LY_MBS_MESSAGE_GET_PRIVATE(LY_MBS_MESSAGE(message));
	return priv->body;
}




GType ly_mbs_filter_get_type()
{
	static GType ly_mbs_filter_type = 0;
	if(!ly_mbs_filter_type)
	{
		static const GTypeInfo ly_mbs_filter_info={
			sizeof(LyMbsFilterClass),
			NULL,
			NULL,
			(GClassInitFunc)ly_mbs_filter_class_init,
			NULL,
			NULL,
			sizeof(LyMbsFilter),
			0,
			(GInstanceInitFunc)ly_mbs_filter_init,
			NULL
		};
		ly_mbs_filter_type=g_type_register_static(G_TYPE_OBJECT, "LyMbsFilter", &ly_mbs_filter_info, 0);
	}
	return ly_mbs_filter_type;
}
static void ly_mbs_filter_init(LyMbsFilter *obj)
{

}
static void ly_mbs_filter_final(GObject *obj)
{
	LyMbsFilterPrivate *priv=LY_MBS_FILTER_GET_PRIVATE(LY_MBS_FILTER(obj));
	g_free(priv->title);
	g_free(priv->from);
}
static void ly_mbs_filter_class_init(LyMbsFilterClass *klass)
{
	GObjectClass *object_class=G_OBJECT_CLASS(klass);
	object_class->finalize=ly_mbs_filter_final;
	g_type_class_add_private(klass, sizeof(LyMbsFilterPrivate));
}
LyMbsFilter* ly_mbs_filter_new(gchar *title, gchar *from, LyMbsFilterBindFunc func, gpointer data)
{
	g_return_val_if_fail((title!=NULL), NULL);
	LyMbsFilter *filter;
	filter=g_object_new(LY_MBS_FILTER_TYPE, NULL);
	LyMbsFilterPrivate *priv=LY_MBS_FILTER_GET_PRIVATE(LY_MBS_FILTER(filter));
	priv->title=g_strconcat(title, NULL);
	priv->from=g_strconcat(from, NULL);
	priv->func=func;
	priv->data=data;
	return filter;
}

GType ly_mbs_bus_get_type()
{
	static GType ly_mbs_bus_type = 0;
	if(!ly_mbs_bus_type)
	{
		static const GTypeInfo ly_mbs_bus_info={
			sizeof(LyMbsBusClass),
			NULL,
			NULL,
			(GClassInitFunc)ly_mbs_bus_class_init,
			NULL,
			NULL,
			sizeof(LyMbsBus),
			0,
			(GInstanceInitFunc)ly_mbs_bus_init,
			NULL
		};
		ly_mbs_bus_type=g_type_register_static(G_TYPE_OBJECT, "LyMbsBus", &ly_mbs_bus_info, 0);
	}
	return ly_mbs_bus_type;
}
static void ly_mbs_bus_init(LyMbsBus *obj)
{

}
static void ly_mbs_bus_final(GObject *obj)
{
	LyMbsBusPrivate *priv=LY_MBS_BUS_GET_PRIVATE(LY_MBS_BUS(obj));
	LyMbsMessage *message=NULL;
	while(!g_queue_is_empty(priv->queue))
	{
		message=LY_MBS_MESSAGE(g_queue_pop_head(priv->queue));
		g_object_unref(message);
	}
	g_queue_free(priv->queue);
	GList *p=NULL;
	LyMbsFilter *filter=NULL;
	p=priv->table;
	while(p)
	{
		filter=LY_MBS_FILTER(p->data);
		g_object_unref(filter);
		p=p->next;
	}
	g_list_free(priv->table);
}
static void ly_mbs_bus_class_init(LyMbsBusClass *klass)
{
	GObjectClass *object_class=G_OBJECT_CLASS(klass);
	object_class->finalize=ly_mbs_bus_final;
	g_type_class_add_private(klass, sizeof(LyMbsBusPrivate));
}
LyMbsBus* ly_mbs_bus_new()
{
	LyMbsBus *bus;
	bus=g_object_new(LY_MBS_BUS_TYPE, NULL);
	LyMbsBusPrivate *priv=LY_MBS_BUS_GET_PRIVATE(LY_MBS_BUS(bus));
	priv->queue=g_queue_new();
	return bus;
}
gboolean ly_mbs_bus_send_message(LyMbsBus *bus, LyMbsMessage *message)
{	
	LyMbsBusPrivate *priv=LY_MBS_BUS_GET_PRIVATE(LY_MBS_BUS(bus));
	g_queue_push_tail(priv->queue, message);
	ly_mbs_bus_distribute(bus);
	return TRUE;
}
gboolean ly_mbs_bus_bind_filter(LyMbsBus *bus, LyMbsFilter *filter)
{
	LyMbsBusPrivate *priv=LY_MBS_BUS_GET_PRIVATE(LY_MBS_BUS(bus));
	LyMbsFilterPrivate *priv_filter=LY_MBS_FILTER_GET_PRIVATE(LY_MBS_FILTER(filter));
	GList *p=priv->table;
	LyMbsFilterPrivate *priv_fi;
	while(p)
	{
		priv_fi=LY_MBS_FILTER_GET_PRIVATE(LY_MBS_FILTER(p->data));
		if(g_str_equal(priv_fi->title, priv_filter->title) && \
			g_str_equal(priv_fi->from, priv_filter->from) && \
			priv_fi->func==priv_filter->func)
		{
			g_message(_("Filter already exist!"));
			return FALSE;
		}
		p=p->next;
	}
	priv->table=g_list_append(priv->table, filter);
	return TRUE;
}
gboolean ly_mbs_bus_unbind_filter(LyMbsBus *bus, LyMbsFilter *filter)
{
	LyMbsBusPrivate *priv=LY_MBS_BUS_GET_PRIVATE(LY_MBS_BUS(bus));
	LyMbsFilterPrivate *priv_filter=LY_MBS_FILTER_GET_PRIVATE(LY_MBS_FILTER(filter));
	GList *p=priv->table;
	LyMbsFilterPrivate *priv_fi;
	while(p)
	{
		priv_fi=LY_MBS_FILTER_GET_PRIVATE(LY_MBS_FILTER(p->data));
		if(g_str_equal(priv_fi->title, priv_filter->title) && \
			g_str_equal(priv_fi->from, priv_filter->from) && \
			priv_fi->func==priv_filter->func)
		{
			g_object_unref(G_OBJECT(p->data));
			p->data=NULL;
			priv->table=g_list_delete_link(priv->table, p);
			return TRUE;
		}
		p=p->next;
	}
	g_message(_("Filter not found!"));
	return FALSE;
}

void ly_mbs_bus_distribute(LyMbsBus *bus)
{
	LyMbsBusPrivate *priv=LY_MBS_BUS_GET_PRIVATE(LY_MBS_BUS(bus));
	LyMbsMessage *message;
	LyMbsMessagePrivate *priv_message;
	LyMbsFilter *filter;
	LyMbsFilterPrivate *priv_filter;
	LyMbsFilterBindFunc f;
	GList *p=NULL;
	while(!g_queue_is_empty(priv->queue))
	{
		message=g_queue_pop_head(priv->queue);
		priv_message=LY_MBS_MESSAGE_GET_PRIVATE(LY_MBS_MESSAGE(message));
		p=priv->table;
		while(p)
		{
			filter=p->data;
			priv_filter=LY_MBS_FILTER_GET_PRIVATE(LY_MBS_FILTER(filter));
			if(g_str_equal(priv_filter->title, priv_message->title) && \
				g_str_has_prefix(priv_message->from, priv_filter->from))
			{
				f=priv_filter->func;
				f(message, priv_filter->data);
			}
			p=p->next;
		}
		g_object_unref(message);
	}
}




