
#ifndef	LIBRARY_H
#define	LIBRARY_H

//HEADERS
#include "global.h"
#include "debug.h"
#include "common.h"
#include "configure.h"
#include "metadata.h"

//VARIABLES
//这两个结构体慎用
typedef struct _lib_uri
{
	gchar *start;
	gchar *duration;
	gchar *uri;
}_lib_uri;
typedef struct _lib_playing
{
	gchar *start;
	gchar *duration;
	gchar *uri;
	gchar *name;
	gint flag;
}_lib_playing;


//FUNCTIONS
gboolean	lib_read_lib();
gboolean	lib_check_lib();
gboolean	lib_check_lib_first();
gboolean	lib_write_lib();
gboolean	lib_read_sublib();
gboolean	lib_check_sublib();
gboolean	lib_write_sublib();
gboolean	lib_refresh_sublib();

GList*		lib_find_pl_by_name(GList *list, gchar *name);
GList*		lib_find_pl_by_index(GList *list, gint index);
GList*		lib_find_md_by_uri(GList *list, gchar *start, gchar *duration, gchar *uri);
GList*		lib_find_md_by_index(GList *list, gint index);
GList*		lib_find_pgl_by_uri(GList *list, gchar *start, gchar *duration, gchar *uri, gchar *name, gint flag);
GList*		lib_find_pgl_by_index(GList *list, gint index);

gboolean	lib_add_md(_gl_metadata *md);
gboolean	lib_delete_md(GList *p);

GList*		lib_order_pl_by_name(GList *p);
GList*		lib_order_md_by_title(GList *q);
GList*		lib_order_md_by_artist(GList *q);
GList*		lib_order_md_by_album(GList *q);

#endif
