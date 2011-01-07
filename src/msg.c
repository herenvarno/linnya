#include "msg.h"

/*
 * FUNCTIONS	[PUBLIC]
 */

/*
 * NAME:	mm_msg_prepare_cb
 * VARS:	source[GSource]	the source.
 *			timeout[gint]	the timeout length.
 * RETN:	[gboolean]		always FALSE.
 * DESC:	Prepare function for building source.
 */
gboolean mm_msg_prepare_cb(GSource *source, gint *timeout)
{
	puts(__FUNCTION__);
	*timeout=1000;
	return FALSE;
}

/*
 * NAME:	mm_msg_check_cb
 * VARS:	source[GSource]	the source.
 * RETN:	[gboolean]		TRUE for success, FALSE for fail.
 * DESC:	Check function for building source.
 */
gboolean mm_msg_check_cb(GSource *source)
{
	puts(__FUNCTION__);
	if(g_queue_is_empty(mm_msg_messages))
		return FALSE;
	return TRUE;
}

/*
 * NAME:	mm_msg_dispatch_cb
 * VARS:	source[GSource]	the source.
 * RETN:	[gboolean]		TRUE for success, FALSE for fail.
 * DESC:	Check function for building source.
 */
gboolean mm_msg_dispatch_cb(GSource *source, GSourceFunc callback, gpointer data)
{
 	puts(__FUNCTION__);

	mmMsgMessage *message;
	gboolean (*func)(mmMsgMessage *m);
	GList *p=NULL;
	mmMsgConn *pdata=NULL;

	while(!g_queue_is_empty(mm_msg_messages))
	{
		message=g_queue_pop_head(mm_msg_messages);
		p=mm_msg_conns;
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
		mm_msg_message_free(message);
	}
	return TRUE;
}

/*
 * NAME:	mm_msg_finalize_cb
 * VARS:	source[GSource]	the source.
 * RETN:	[void]
 * DESC:	Finalize function for building source.
 */
void mm_msg_finalize_cb(GSource *source)
{
	puts(__FUNCTION__);
}

/*
 * NAME:	mm_msg_init
 * VARS:	[void]
 * RETN:	[gboolean]	TRUE for success, FALSE for fail.
 * DESC:	Initialize the msg module.
 */
gboolean mm_msg_init()
{
	puts(__FUNCTION__);
	mm_msg_messages=NULL;
	mm_msg_messages=g_queue_new();
	if(mm_msg_messages==NULL)
	{
		printf(_("MusicMe ERROR: Building the message bus failed!\n"));
		exit(0);
		return FALSE;
	}

	mm_msg_conns=NULL;
	
	GSourceFuncs *sourcefuncs=NULL;
	sourcefuncs=(GSourceFuncs *)g_malloc(sizeof(GSourceFuncs));
	sourcefuncs->prepare=mm_msg_prepare_cb;
	sourcefuncs->check=mm_msg_check_cb;
	sourcefuncs->dispatch=mm_msg_dispatch_cb;
	sourcefuncs->finalize=mm_msg_finalize_cb;
	mm_msg_source=g_source_new(sourcefuncs,sizeof(GSource));
	GMainContext *maincontext=NULL;
	maincontext = g_main_loop_get_context(mm_gl_mainloop);
	g_source_attach(mm_msg_source, maincontext);
	
	return TRUE;
}

/*
 * NAME:	mm_msg_put
 * VARS:	type[gchar*]	the type of message.
 *			from[gchar*]	the module that post the message.
 *			msg[gchar*]		the indeed information.
 * RETN:	[gboolean]		TRUE for success, FALSE for fail.
 * DESC:	Put a message to the message bus.
 */
gboolean mm_msg_put(gchar *type, gchar *from, gchar *msg)
{
	puts(__FUNCTION__);
	if(type&&msg==0)
	{
		return FALSE;
	}
	mmMsgMessage *message=(mmMsgMessage*)g_malloc(sizeof(mmMsgMessage));
	message->type=g_strconcat(type,NULL);
	message->from=g_strconcat(from,NULL);
	message->msg=g_strconcat(msg,NULL);
	g_queue_push_tail(mm_msg_messages,message);
	return TRUE;
}

/*
 * NAME:	mm_msg_bind
 * VARS:	type[gchar*]	the type of message.
 *			func[gpointer]	the callback function.
 * RETN:	[gboolean]		always TRUE.
 * DESC:	Bind a function and a message.
 */
gboolean mm_msg_bind(gchar *type, gpointer func)
{
	puts(__FUNCTION__);
	mmMsgConn *conn=(mmMsgConn*)g_malloc(sizeof(mmMsgConn));
	conn->type=g_strconcat(type,NULL);
	conn->func=func;
	mm_msg_conns=g_list_append(mm_msg_conns,conn);
	return TRUE;
}

/*
 * NAME:	mm_msg_message_free
 * VARS:	message[mmMsgMessage*]	the message
 * RETN:	[gboolean]		always TRUE.
 * DESC:	Free a message.
 */
gboolean mm_msg_message_free(mmMsgMessage *message)
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
