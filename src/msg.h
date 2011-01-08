#ifndef MSG_H
#define MSG_H

#include "global.h"

/*
 * TYPES
 */
typedef struct lyMsgMessage
{
	gchar *from;
	gchar *type; 
	gchar *msg;
}lyMsgMessage;

typedef struct lyMsgConn
{
	gchar *type;
	gboolean (*func)(lyMsgMessage* message);
}lyMsgConn;

/*
 * VARIABLES	[PRIVATE]
 */
GQueue *ly_msg_messages;
GList *ly_msg_conns;
GSource *ly_msg_source;

/*
 * FUNCTIONS	[PUBLIC]
 */
gboolean ly_msg_init();
gboolean ly_msg_put(gchar *type, gchar *from, gchar *msg);
gboolean ly_msg_bind(gchar *type, gpointer func);

gboolean ly_msg_message_free(lyMsgMessage *message);	/*[PRIVATE]*/
gboolean ly_msg_prepare_cb(GSource *source, gint *timeout);	/*[PRIVATE]*/
gboolean ly_msg_check_cb(GSource *source);	/*[PRIVATE]*/
gboolean ly_msg_dispatch_cb(GSource *source, GSourceFunc callback, gpointer data);	/*[PRIVATE]*/
void ly_msg_finalize_cb(GSource *source);	/*[PRIVATE]*/

#endif
