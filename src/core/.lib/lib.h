/*
 * HEADERS
 */
#include "db.h"

/*
 * TYPES
 */
 enum{
 	METADATA_TYPE_UNDEFINED,
 	METADATA_TYPE_LOCAL_AUDIO,
 	METADATA_TYPE_LOCAL_VIDEO,
 	METADATA_TYPE_REMOTE_AUDIO,
 	METADATA_TYPE_REMOTE_VIDEO,
}

typedef struct _lyLibMetadata{
	gint	id;
	gchar	title[128];
	gchar	artist[128];
	gchar	album[128];
	gchar	codec[128];
	gchar	start[64];
	gchar	duration[64];
	gchar	uri[1024];
	gint	playing;
	gint	num;
	gint	type;
	gint	tmpflag;
}lyLibMetadata;

/*
 * FUNCTIONS
 */
gboolean		ly_lib_update			(void);

gboolean		ly_lib_get_meatdatas	(int base, int length, gpointer callback, gpointer data, const char *fmt, ...);
gboolean		ly_lib_get_artists		(int base, int length, gpointer callback, gpointer data, const char *fmt, ...);
gboolean		ly_lib_get_albums		(int base, int length, gpointer callback, gpointer data, const char *fmt, ...);

lyLibMetadata*	ly_lib_metadata_new		(char *uri);
gboolean		ly_lib_metadata_destroy	(lyLibMetadata *md);
gboolean		ly_lib_metadata_store	(lyLibMetadata *md);
gboolean		ly_lib_metadata_delete	(int mid);
lyLibMetadata*	ly_lib_metadata_get		(int mid);
