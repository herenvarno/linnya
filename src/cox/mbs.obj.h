/*
 * mbs.obj.h
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

#ifndef LY_MBS_OBJ_H
#define LY_MBS_OBJ_H

/*
 * HEADERS
 */
#include "gla.h"
#include "log.h"

G_BEGIN_DECLS

/*
 * MACROS
 */
#define LY_MBS_MESSAGE_TYPE \
	(ly_mbs_message_get_type())
#define LY_MBS_MESSAGE(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), LY_MBS_MESSAGE_TYPE, LyMbsMessage))
#define LY_MBS_MESSAGE_CLASS(klass) \
	(G_TYPE_CHECK_INSTANCE_CAST((klass), LY_MBS_MESSAGE_CLASS_TYPE, LyMbsMessageClass))
#define LY_MBS_MESSAGE_IS_MESSAGE(obj) \
	(G_TYPE_CHECK_VALUE_TYPE ((obj), LY_MBS_MESSAGE_TYPE))
#define LY_MBS_MESSAGE_IS_MESSAGE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), LY_MBS_MESSAGE_CLASS_TYPE))
#define LY_MBS_MESSAGE_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), LY_MBS_MESSAGE_TYPE, LyMbsMessageClass))

#define LY_MBS_FILTER_TYPE \
	(ly_mbs_filter_get_type())
#define LY_MBS_FILTER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), LY_MBS_FILTER_TYPE, LyMbsFilter))
#define LY_MBS_FILTER_CLASS(klass) \
	(G_TYPE_CHECK_INSTANCE_CAST((klass), LY_MBS_FILTER_CLASS_TYPE, LyMbsFilterClass))
#define LY_MBS_FILTER_IS_FILTER(obj) \
	(G_TYPE_CHECK_VALUE_TYPE ((obj), LY_MBS_FILTER_TYPE))
#define LY_MBS_FILTER_IS_FILTER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), LY_MBS_FILTER_CLASS_TYPE))
#define LY_MBS_FILTER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), LY_MBS_FILTER_TYPE, LyMbsFilterClass))

#define LY_MBS_BUS_TYPE \
	(ly_mbs_bus_get_type())
#define LY_MBS_BUS(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), LY_MBS_BUS_TYPE, LyMbsBus))
#define LY_MBS_BUS_CLASS(klass) \
	(G_TYPE_CHECK_INSTANCE_CAST((klass), LY_MBS_BUS_CLASS_TYPE, LyMbsBusClass))
#define LY_MBS_BUS_IS_BUS(obj) \
	(G_TYPE_CHECK_VALUE_TYPE ((obj), LY_MBS_BUS_TYPE))
#define LY_MBS_BUS_IS_BUS_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), LY_MBS_BUS_CLASS_TYPE))
#define LY_MBS_BUS_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), LY_MBS_BUS_TYPE, LyMbsBusClass))

#define LY_MBS_FILTER_BIND_FUNC(f) \
	void (*f) (LyMbsMessage *message, gpointer data)


/*
 * TYPES
 */
typedef struct _LyMbsMessage LyMbsMessage;
typedef struct _LyMbsMessageClass LyMbsMessageClass;
typedef struct _LyMbsFilter LyMbsFilter;
typedef struct _LyMbsFilterClass LyMbsFilterClass;
typedef struct _LyMbsBus LyMbsBus;
typedef struct _LyMbsBusClass LyMbsBusClass;
typedef void (*LyMbsFilterBindFunc) (LyMbsMessage *message, gpointer data);

/**
 * LyMbsMessage
 *
 * The #LyMbsMessage object is used for writing various kind of log to a
 * specific log file while program is running. The contents of the
 * #LyMbsMessage structure are private and should only be accessed via
 * provided APIs.
 */
struct _LyMbsMessage {
	/*<private>*/
	GObject parent;
};

/**
 * LyMbsMessage
 *
 * the class structure of #LyMbsMessage.
 */
struct _LyMbsMessageClass {
	/*<private>*/
	GObjectClass parent_class;
};


/**
 * LyMbsFilter
 *
 * The #LyMbsMessage object is used for writing various kind of log to a
 * specific log file while program is running. The contents of the
 * #LyMbsMessage structure are private and should only be accessed via
 * provided APIs.
 */
struct _LyMbsFilter {
	/*<private>*/
	GObject parent;
};

/**
 * LyMbsMessage
 *
 * the class structure of #LyMbsMessage.
 */
struct _LyMbsFilterClass {
	/*<private>*/
	GObjectClass parent_class;
	
	/*<private>*/
	void (*on_timeout)();
	void (*on_set_timer)();
};

/**
 * LyMbsBus
 *
 * The #LyMbsMessage object is used for writing various kind of log to a
 * specific log file while program is running. The contents of the
 * #LyMbsMessage structure are private and should only be accessed via
 * provided APIs.
 */
struct _LyMbsBus {
	/*<private>*/
	GObject parent;
};

/**
 * LyMbsMessage
 *
 * the class structure of #LyMbsMessage.
 */
struct _LyMbsBusClass {
	/*<private>*/
	GObjectClass parent_class;
};


/*
 * FUNCTIONS
 */
GType ly_mbs_message_get_type ();
LyMbsMessage *ly_mbs_message_new (gchar *title, gchar *from, gchar *body);
const gchar *ly_mbs_message_get_title(LyMbsMessage *message);
const gchar *ly_mbs_message_get_from(LyMbsMessage *message);
const gchar *ly_mbs_message_get_body(LyMbsMessage *message);

GType ly_mbs_filter_get_type ();
LyMbsFilter *ly_mbs_filter_new (gchar *title, gchar *from, LyMbsFilterBindFunc func, gpointer data);

GType ly_mbs_bus_get_type ();
LyMbsBus *ly_mbs_bus_new ();
gboolean ly_mbs_bus_send_message(LyMbsBus *bus, LyMbsMessage *message);
gboolean ly_mbs_bus_bind_filter(LyMbsBus *bus, LyMbsFilter *filter);
gboolean ly_mbs_bus_unbind_filter(LyMbsBus *bus, LyMbsFilter *filter);
void ly_mbs_bus_distribute(LyMbsBus *bus);

G_END_DECLS

#endif //LY_MBS_OBJ_H
