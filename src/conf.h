#ifndef CONF_H
#define CONF_H

/*
 * HEADERS
 */
#include "global.h"
#include "msg.h"

/*
 * VARIABLES	[PRIVATE]
 */
GHashTable *ly_conf_configurations;

/*
 * FUNCTIONS	[PUBLIC]
 */
gboolean ly_conf_init(void);
gboolean ly_conf_finalize(void);

gboolean ly_conf_get(gchar *name, const gchar *format, ...);	/*similar to scanf()*/
gboolean ly_conf_set(gchar *name, const gchar *format, ...);	/*similar to printf()*/
void ly_conf_delete(gchar *name);

/*
 * FUNCTIONS	[PRIVATE]
 */
gboolean ly_conf_read(void);
gboolean ly_conf_write(void);
void ly_conf_write_each_cb(gpointer key, gpointer value, gpointer data);
void ly_conf_start_cb(	GMarkupParseContext *context,
								const gchar *element_name,
								const gchar **attribute_names,
								const gchar **attribute_values,
								gpointer data,
								GError **error);
#endif