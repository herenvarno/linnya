#ifndef MSG_H
#define MSG_H

#include "gl.h"

/*
 * TYPES
 */
typedef struct mmMsgMessage
{
	gchar *from;
	gchar *type; 
	gchar *msg;
}mmMsgMessage;

typedef struct mmMsgConn
{
	gchar *type;
	gboolean (*func)(mmMsgMessage* message);
}mmMsgConn;

/*
 * VARIABLES	[PRIVATE]
 */
GQueue *mm_msg_messages;
GList *mm_msg_conns;
GSource *mm_msg_source;

/*
 * FUNCTIONS	[PUBLIC]
 */
gboolean mm_msg_init();
gboolean mm_msg_put(gchar *type, gchar *from, gchar *msg);
gboolean mm_msg_bind(gchar *type, gpointer func);

gboolean mm_msg_message_free(mmMsgMessage *message);	/*[PRIVATE]*/
gboolean mm_msg_prepare_cb(GSource *source, gint *timeout);	/*[PRIVATE]*/
gboolean mm_msg_check_cb(GSource *source);	/*[PRIVATE]*/
gboolean mm_msg_dispatch_cb(GSource *source, GSourceFunc callback, gpointer data);	/*[PRIVATE]*/
void mm_msg_finalize_cb(GSource *source);	/*[PRIVATE]*/

#endif
