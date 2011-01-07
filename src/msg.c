#include "msg.h"

/*
 * FUNCTIONS	[PUBLIC]
 */

/*
 * NAME:	ly_msg_prepare_cb
 * VARS:	source[GSource]	the source.
 *			timeout[gint]	the timeout length.
 * RETN:	[gboolean]		always FALSE.
 * DESC:	Prepare function for building source.
 */
gboolean ly_msg_prepare_cb(GSource *source, gint *timeout)
{
	puts(__FUNCTION__);
	*timeout=1000;
	return FALSE;
}

/*
 * NAME:	ly_msg_check_cb
 * VARS:	source[GSource]	the source.
 * RETN:	[gboolean]		TRUE for success, FALSE for fail.
 * DESC:	Check function for building source.
 */
gboolean ly_msg_check_cb(GSource *source)
{
	puts(__FUNCTION__);
	if(g_queue_is_empty(ly_msg_messages))
		return FALSE;
	return TRUE;
}

/*
 * NAME:	ly_msg_dispatch_cb
 * VARS:	source[GSource]	the source.
 * RETN:	[gboolean]		TRUE for success, FALSE for fail.
 * DESC:	Check function for building source.
 */
gboolean ly_msg_dispatch_cb(GSource *source, GSourceFunc callback, gpointer data)
{
 	puts(__FUNCTION__);

	lyMsgMessage *message;
	gboolean (*func)(lyMsgMessage *m);
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
				func=pdata->func;
				func(message);
			}
			p=p->next;
		}
		ly_msg_message_free(message);
	}
	return TRUE;
}

/*
 * NAME:	ly_msg_finalize_cb
 * VARS:	source[GSource]	the source.
 * RETN:	[void]
 * DESC:	Finalize function for building source.
 */
void ly_msg_finalize_cb(GSource *source)
{
	puts(__FUNCTION__);
}

/*
 * NAME:	ly_msg_init
 * VARS:	[void]
 * RETN:	[gboolean]	TRUE for success, FALSE for fail.
 * DESC:	Initialize the msg module.
 */
gboolean ly_msg_init()
{
	puts(__FUNCTION__);
	ly_msg_messages=NULL;
	ly_msg_messages=g_queue_new();
	if(ly_msg_messages==NULL)
	{
		printf(_("Linnya ERROR: Building the message bus failed!\n"));
		exit(0);
		return FALSE;
	}

	ly_msg_conns=NULL;
	
	GSourceFuncs *sourcefuncs=NULL;
	sourcefuncs=(GSourceFuncs *)g_malloc(sizeof(GSourceFuncs));
	sourcefuncs->prepare=ly_msg_prepare_cb;
	sourcefuncs->check=ly_msg_check_cb;
	sourcefuncs->dispatch=ly_msg_dispatch_cb;
	sourcefuncs->finalize=ly_msg_finalize_cb;
	mm_msg_source=g_source_new(sourcefuncs,sizeof(GSource));
	GMainContext *maincontext=NULL;
	maincontext = g_main_loop_get_context(ly_gl_mainloop);
	g_source_attach(ly_msg_source, maincontext);
	
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
	puts(__FUNCTION__);
	if(type&&msg==0)
	{
		return FALSE;
	}
	lyMsgMessage *message=(lyMsgMessage*)g_malloc(sizeof(lyMsgMessage));
	message->type=g_strconcat(type,NULL);
	message->from=g_strconcat(from,NULL);
	message->msg=g_strconcat(msg,NULL);
	g_queue_push_tail(ly_msg_messages,message);
	return TRUE;
}

/*
 * NAME:	ly_msg_bind
 * VARS:	type[gchar*]	the type of message.
 *			func[gpointer]	the callback function.
 * RETN:	[gboolean]		always TRUE.
 * DESC:	Bind a function and a message.
 */
gboolean ly_msg_bind(gchar *type, gpointer func)
{
	puts(__FUNCTION__);
	lyMsgConn *conn=(lyMsgConn*)g_malloc(sizeof(mlyMsgConn));
	conn->type=g_strconcat(type,NULL);
	conn->func=func;
	ly_msg_conns=g_list_append(ly_msg_conns,conn);
	return TRUE;
}

/*
 * NAME:	ly_msg_message_free
 * VARS:	message[lyMsgMessage*]	the message
 * RETN:	[gboolean]		always TRUE.
 * DESC:	Free a message.
 */
gboolean ly_msg_message_free(lyMsgMessage *message)
{
	puts(__FUNCTION__);
	if(message->type)
	{
		g_free(message->type);
		message->type=NULL;
	}
	if(message->from)
	{
		g_free(message->type);
		message->type=NULL;
	}
	if(message->msg)
	{
		g_free(message->type);
		message->type=NULL;
	}
	g_free(message);
	return TRUE;
}
