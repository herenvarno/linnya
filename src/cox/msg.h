/*
 * msg.h
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

/**
 * SECTION:		msg
 * @short_description:	communication between modules
 * @stability:	stable
 * @include:	cox.h
 *
 * This module is a module to make communications between modules more easier.
 * It provides a standerad method to create, recive messages, and atomatic exec
 * functions that have binded to such particular message.
 */

#ifndef LY_1MSG_H
#define LY_1MSG_H

/*
 * HEADERS
 */
#include "gla.h"

/*
 * TYPES
 */
/**
 * LyMsgMsg:
 * @type:		the message name.
 * @from:		the message creator which use ":" to seperate.
 * @msg:		the message contents.
 *
 * The message contents container structure that used in communications.
 */
typedef struct _LyMsgMsg{
	char type[64];
	char from[64];
	char msg[512];
}LyMsgMsg;

/**
 * LyMsgConn:
 * @type:		the message name to bind.
 * @from:		the message creator.
 * @func:		the callback function to exec, when the message occurs.
 * @data:		the gpointer format data that pass into the callback func.
 *
 * The message connection structure that store the bind information.
 */
typedef struct _LyMsgConn{
	char type[64];
	char from[64];
	gpointer func;
	gpointer data;
}LyMsgConn;

/*
 * FUNCTIONS
 */
void		ly_msg_init			();
void		ly_msg_fina			();	
void		ly_msg_free			(LyMsgMsg *message);
void		ly_msg_clear		();
gboolean	ly_msg_put			(char *type, char *from, char *msg);
gboolean	ly_msg_bind			(char *type, char *from, gpointer func, gpointer data);
gboolean	ly_msg_unbind		(char *type, char *from, gpointer func);

#endif
