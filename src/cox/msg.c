/*
 * msg.c
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
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
#include "msg.h"

/*
 * VARIABLES
 */
GQueue		*ly_msg_messages;
GList		*ly_msg_connections;

/*
 * FUNCTIONS
 */
gboolean	ly_msg_dispatch_cb		(gpointer data);
void		ly_msg_on_clear_cb		(gpointer message, gpointer data);

/**
 * ly_msg_init:
 *
 * Initialize the msg module, it will be called by #ly_cox_init
 */
void	 ly_msg_init()
{
	ly_msg_messages=NULL;
	ly_msg_messages=g_queue_new();
	if(ly_msg_messages==NULL)
	{
		printf(_("[ERROR] Linnya ERROR: Building the message bus failed!\n"));
		exit(0);
	}

	ly_msg_connections=NULL;

	g_timeout_add(100,ly_msg_dispatch_cb,NULL);
}
/**
 * ly_msg_fina:
 *
 * Finalize the msg module, it will be called by #ly_cox_fina
 */
void ly_msg_fina()
{
	ly_log_put(_("[info] Finalize core module: MSG ..."));
	/*
	 * free messages
	 */
	LyMsgMsg *message;
	while(!g_queue_is_empty(ly_msg_messages))
	{
		message=g_queue_pop_head(ly_msg_messages);
		if(message)
			g_free(message);
	}
	g_queue_free(ly_msg_messages);

	/*
	 * free connections
	 */
	GList *p=NULL;
	LyMsgConn *pdata=NULL;
	p=ly_msg_connections;
	while(p)
	{
		pdata=p->data;
		if(pdata)
			g_free(pdata);
		p=p->next;
	}
	g_list_free(ly_msg_connections);
}

/**
 * ly_msg_put:
 * @type:		the type of message.
 * @from:		the module that post the message.
 * @msg:		the indeed information.
 *
 * Put a message to the message bus.
 *
 * Returns:		TRUE for success, FALSE for fail.
 */
gboolean ly_msg_put(char *type, char *from, char *msg)
{
	if(type==NULL)
	{
		return FALSE;
	}
	LyMsgMsg *message=(LyMsgMsg*)g_malloc(sizeof(LyMsgMsg));
	g_snprintf(message->type, sizeof(message->type), "%s", type);
	g_snprintf(message->from, sizeof(message->from), "%s", from);
	g_snprintf(message->msg, sizeof(message->msg), "%s", msg);
	g_queue_push_tail(ly_msg_messages,message);
	return TRUE;
}

/**
 * ly_msg_bind:
 * @type:		the type of message.
 * @from:		the poster of message. (prefix)
 * @func:		the callback function that will atomatically exec.
 * @data:		the data that pass to func.
 *
 * Bind a function and a message.
 *
 * Returns:		TRUE for success, FALSE for fail.
 */
gboolean ly_msg_bind(char *type, char *from, gpointer func, gpointer data)
{
	LyMsgConn *conn=(LyMsgConn*)g_malloc(sizeof(LyMsgConn));
	g_snprintf(conn->type, sizeof(conn->type), "%s", type);
	g_snprintf(conn->from, sizeof(conn->from), "%s", from);
	conn->func=func;
	conn->data=data;
	ly_msg_connections=g_list_append(ly_msg_connections,conn);
	if(ly_msg_connections)
		return TRUE;
	return FALSE;
}

/**
 * ly_msg_unbind:
 * @type:		the type of message.
 * @from:		the poster of message. (prefix)
 * @func:		the callback function.
 *
 * Unbind a function and a message.
 *
 * Returns:		always TRUE.
 */
gboolean ly_msg_unbind(char *type, char *from, gpointer func)
{
	GList *p=NULL;
	GList *pnext=NULL;
	LyMsgConn *pdata=NULL;

	p=ly_msg_connections;
	while(p)
	{
		pdata=p->data;
		pnext=p->next;
		if((g_str_equal(pdata->type,type)) &&
			((from&&g_str_has_prefix(pdata->from,from))||from==NULL) &&
			(func==pdata->func)
		)
		{
			g_free(pdata);
			pdata=NULL;
			p->data=NULL;
			ly_msg_connections=g_list_delete_link(ly_msg_connections,p);
		}
		p=pnext;
	}
	return TRUE;
}

void	ly_msg_clear()
{
	 g_queue_foreach(ly_msg_messages,ly_msg_on_clear_cb,NULL);
}

void	ly_msg_on_clear_cb(gpointer message, gpointer data)
{
	ly_msg_free(message);
	g_queue_remove(ly_msg_messages, message);
}

void	ly_msg_free(LyMsgMsg *message)
{
	if(message)
	{
		g_free(message);
	}
}

/**
 * ly_msg_dispatch_cb:
 * @data:		not used.
 *
 * Pop all message in message queue, and exec the binded function on by on based
 * on the connection list.
 *
 * Returns:		always TRUE
 */
gboolean ly_msg_dispatch_cb(gpointer data)
{
	if(g_queue_is_empty(ly_msg_messages))
		return TRUE;

	LyMsgMsg *message;
	LY_GLA_CALLBACK(func);
	GList *p=NULL;
	LyMsgConn *pdata=NULL;

	while(!g_queue_is_empty(ly_msg_messages))
	{
		message=g_queue_pop_head(ly_msg_messages);
		p=ly_msg_connections;
		while(p)
		{
			pdata=p->data;
			if(g_str_equal(pdata->type,message->type))
			{
				if(g_str_has_prefix(message->from, pdata->from))
				{
					func=pdata->func;
					func(message,pdata->data);
				}
			}
			p=p->next;
		}
		g_free(message);
	}
	return TRUE;
}
