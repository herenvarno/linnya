#include "msg.h"

/*
 * FUNCTIONS	[PUBLIC]
 */

/*
 * NAME:	ly_msg_dispatch_cb
 * VARS:	data[gpointer]	the data.
 * RETN:	[gboolean]		TRUE for success, FALSE for fail.
 * DESC:	Check function for building source.
 */
gboolean ly_msg_dispatch_cb(gpointer data)
{
	if(g_queue_is_empty(ly_msg_messages))
		return TRUE;
	
	lyMsgMessage *message;
	LY_GLOBAL_CALLBACK(func);
	GList *p=NULL;
	lyMsgConn *pdata=NULL;

	while(!g_queue_is_empty(ly_msg_messages))
	{
		message=g_queue_pop_head(ly_msg_messages);
		p=ly_msg_conns;
		while(p)
		{
			pdata=p->data;
			if(g_str_equal(pdata->type,message->type))
			{
				if(g_str_has_prefix(message->from,pdata->from))
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

/*
 * NAME:	ly_msg_init
 * VARS:	[void]
 * RETN:	[gboolean]	TRUE for success, FALSE for fail.
 * DESC:	Initialize the msg module.
 */
gboolean ly_msg_init()
{
	ly_global_debug("info",_("Initial core module: MSG ...\n"));
	ly_msg_messages=NULL;
	ly_msg_messages=g_queue_new();
	if(ly_msg_messages==NULL)
	{
		ly_global_debug("error",_("Linnya ERROR: Building the message bus failed!\n"));
		exit(0);
		return FALSE;
	}

	ly_msg_conns=NULL;
	
	g_timeout_add(100,ly_msg_dispatch_cb,NULL);
	
	return TRUE;
}

/*
 * NAME:	ly_msg_put
 * VARS:	type[gchar*]	the type of message.
 *			from[gchar*]	the module that post the message.
 *			msg[gchar*]		the indeed information.
 * RETN:	[gboolean]		TRUE for success, FALSE for fail.
 * DESC:	Put a message to the message bus.
 */
gboolean ly_msg_put(gchar *type, gchar *from, gchar *msg)
{
	if(type==NULL)
	{
		return FALSE;
	}
	lyMsgMessage *message=(lyMsgMessage*)g_malloc(sizeof(lyMsgMessage));
	g_snprintf(message->type, sizeof(message->type), "%s", type);
	g_snprintf(message->from, sizeof(message->from), "%s", from);
	g_snprintf(message->msg, sizeof(message->msg), "%s", msg);
	g_queue_push_tail(ly_msg_messages,message);
	return TRUE;
}

/*
 * NAME:	ly_msg_bind
 * VARS:	type[gchar*]	the type of message.
 * VARS:	from[gchar*]	the from of message. (prefix)
 *			func[gpointer]	the callback function.
 * RETN:	[gboolean]		always TRUE.
 * DESC:	Bind a function and a message.
 */
gboolean ly_msg_bind(gchar *type, gchar *from, LY_GLOBAL_CALLBACK(func),gpointer data)
{
	lyMsgConn *conn=(lyMsgConn*)g_malloc(sizeof(lyMsgConn));
	g_snprintf(conn->type, sizeof(conn->type), "%s", type);
	g_snprintf(conn->from, sizeof(conn->from), "%s", from);
	conn->func=func;
	conn->data=data;
	ly_msg_conns=g_list_append(ly_msg_conns,conn);
	return TRUE;
}

/*
 * NAME:	ly_msg_unbind
 * VARS:	type[gchar*]	the type of message.
 * VARS:	from[gchar*]	the from of message. (prefix)
 *			func[gpointer]	the callback function.
 * RETN:	[gboolean]		always TRUE.
 * DESC:	Unbind a function and a message.
 */
gboolean ly_msg_unbind(gchar *type, gchar *from, LY_GLOBAL_CALLBACK(func))
{	
	GList *p=NULL;
	GList *pnext=NULL;
	lyMsgConn *pdata=NULL;
	
	p=ly_msg_conns;
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
			ly_msg_conns=g_list_delete_link(ly_msg_conns,p);
		}
		p=pnext;
	}
	return TRUE;
}

/*
 * NAME:	ly_msg_finalize
 * VARS:	[void]
 * RETN:	[gboolean]		always TRUE.
 * DESC:	finalize the msg module.
 */
gboolean ly_msg_finalize()
{
	ly_global_debug("info",_("Finalize core module: MSG ...\n"));

	/*
	 * free messages
	 */
	lyMsgMessage *message;
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
	lyMsgConn *pdata=NULL;
	p=ly_msg_conns;
	while(p)
	{
		pdata=p->data;
		if(pdata)
			g_free(pdata);
		p=p->next;
	}
	g_list_free(ly_msg_conns);
	return TRUE;
}
