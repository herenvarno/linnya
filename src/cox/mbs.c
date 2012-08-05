/*
 * mbs.c
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
#include "mbs.h"

/*
 * VARIABLES
 */
LyMbsBus *ly_mbs_bus;

/*
 * FUNCTIONS
 */

/**
 * ly_mbs_init:
 *
 * Initialize the msg module, it will be called by #ly_cox_init
 */
void	 ly_mbs_init()
{
	ly_mbs_bus=ly_mbs_bus_new();
	if(!ly_mbs_bus)
		g_error(_("Cannot build message bus, abort ..."));
}
/**
 * ly_mbs_fina:
 *
 * Finalize the msg module, it will be called by #ly_cox_fina
 */
void ly_mbs_fina()
{
	g_object_unref(ly_mbs_bus);
}

/**
 * ly_mbs_put:
 * @type:		the type of message.
 * @from:		the module that post the message.
 * @msg:		the indeed information.
 *
 * Put a message to the message bus.
 *
 * Returns:		TRUE for success, FALSE for fail.
 */
gboolean ly_mbs_put(gchar *title, gchar *from, gchar *body)
{
	LyMbsMessage *message=ly_mbs_message_new(title, from, body);
	g_return_val_if_fail(message!=NULL, FALSE);
	return ly_mbs_bus_send_message(ly_mbs_bus, message);
}

/**
 * ly_mbs_bind:
 * @type:		the type of message.
 * @from:		the poster of message. (prefix)
 * @func:		the callback function that will atomatically exec.
 * @data:		the data that pass to func.
 *
 * Bind a function and a message.
 *
 * Returns:		TRUE for success, FALSE for fail.
 */
gboolean ly_mbs_bind(gchar *title, gchar *from, LyMbsFilterBindFunc func, gpointer data)
{
	LyMbsFilter *filter=ly_mbs_filter_new(title, from, func, data);
	g_return_val_if_fail(filter!=NULL, FALSE);
	return ly_mbs_bus_bind_filter(ly_mbs_bus, filter);
}

/**
 * ly_mbs_unbind:
 * @type:		the type of message.
 * @from:		the poster of message. (prefix)
 * @func:		the callback function.
 *
 * Unbind a function and a message.
 *
 * Returns:		always TRUE.
 */
gboolean ly_mbs_unbind(char *title, char *from, LyMbsFilterBindFunc func)
{
	LyMbsFilter *filter=ly_mbs_filter_new(title, from, func, NULL);
	g_return_val_if_fail(filter!=NULL, FALSE);
	return ly_mbs_bus_unbind_filter(ly_mbs_bus, filter);
}
