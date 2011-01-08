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
gboolean ly_msg_finalize();
gboolean ly_msg_put(gchar *type, gchar *from, gchar *msg);
gboolean ly_msg_bind(gchar *type, gpointer func);
gboolean ly_msg_unbind(gchar *type, gpointer func);

/*
 * FUNCTIONS	[PRIVATE]
 */
gboolean ly_msg_free_message(lyMsgMessage *message);
gboolean ly_msg_free_conn(lyMsgConn *conn);
gboolean ly_msg_prepare_cb(GSource *source, gint *timeout);
gboolean ly_msg_check_cb(GSource *source);
gboolean ly_msg_dispatch_cb(GSource *source, GSourceFunc callback, gpointer data);
void ly_msg_finalize_cb(GSource *source);

#endif
