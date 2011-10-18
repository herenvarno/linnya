#ifndef DB_H
#define DB_H

/*
 * HEADERS
 */
#include "global.h"
#include "msg.h"
#include "conf.h"

/*
 * TYPES
 */
typedef struct lyDbMetadata
{
	gint id;
	gchar title[128];
	gchar artist[128];
	gchar album[128];
	gchar codec[128];
	gchar start[64];
	gchar duration[64];
	gchar uri[1024];
	gint playing;
	gint num;
	gint flag;
	gint tmpflag;
}lyDbMetadata;

/*
 * VARIABLES
 */
sqlite3 *ly_db_conn;
GstElement *ly_db_pipeline;
gboolean ly_db_md_eos;

/*
 * FUNCTIONS [PUBLIC]
 */
gboolean ly_db_init();
gboolean ly_db_finalize();

gboolean ly_db_exec(gchar *sql, LY_GLOBAL_CALLBACK(func), gpointer data);
lyDbMetadata *ly_db_get_metadata(gchar *sql);
lyDbMetadata *ly_db_get_metadata_by_id(gint id);
gint ly_db_get_count(gchar *sql);
gint ly_db_get_metadata_count();
gint ly_db_get_playlist_count();

lyDbMetadata *ly_db_read_metadata(gchar *uri);
gchar *ly_db_dura2duration(gint64 dura);
gint64 ly_db_duration2dura(gchar *duration);

gboolean ly_db_check_library();
gboolean ly_db_check_isaudio(gchar *filename);

void ly_db_free_metadata(lyDbMetadata *metadata);
gint64 ly_db_get_last_insert_rowid();

gboolean ly_db_add_metadata(lyDbMetadata *md);
/*
 * FUNCTIONS [PRIVATE]
 */
gboolean ly_db_read_metadata_cb(GstBus *bus, GstMessage *message, gpointer data);
void ly_db_event_loop_cb(lyDbMetadata *md, GstElement *element, gboolean block);
void ly_db_build_pipeline_cb(GstElement *decodebin,GstPad *pad, gboolean last, gpointer data);

gboolean ly_db_check_database();
gboolean ly_db_check_file_exists(gpointer stmt, gpointer data);
gboolean ly_db_check_newfile(gpointer stmt, gpointer data);

gboolean ly_db_get_count_cb(gpointer stmt, gpointer data);
gboolean ly_db_get_metadata_cb(gpointer stmt, gpointer data);


#endif
