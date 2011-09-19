#ifndef MSG_H
#define MSG_H

#include "global.h"

/*
 * TYPES
 */
typedef struct lyMsgMessage
{
	gchar from[64];
	gchar type[64];
	gchar msg[512];
}lyMsgMessage;

typedef struct lyMsgConn
{
	gchar type[64];
	gchar from[64];
	LY_GLOBAL_CALLBACK(func);
	gpointer data;
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
gboolean ly_msg_bind(gchar *type, gchar *from, LY_GLOBAL_CALLBACK(func),gpointer data);
gboolean ly_msg_unbind(gchar *type, gchar *from, LY_GLOBAL_CALLBACK(func));

/*
 * FUNCTIONS	[PRIVATE]
 */
gboolean ly_msg_dispatch_cb(gpointer data);

#endif
