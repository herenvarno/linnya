/*
 * HEADERS
 */
#include "mdh.obj.h"

/*
 * FUNCTIONS
 */
GType ndl_mdh_meta_get_type ()
{
}
LyMdhMeta* ly_mdh_meta_new ()
{
}
LyMdhMeta* ly_mdh_meta_new_from_uri(gchar *uri)
{
}
LyMdhMeta* ly_mdh_meta_new_from_uri_async(gchar *uri)
{
}
gboolean ly_mdh_meta_set_id(LyMdhMeta *meta, gint id)
{
}
gboolean ly_mdh_meta_set_title(LyMdhMeta *meta, gchar *title)
{
	g_return_var_if_fail(meta!=NULL, FALSE);
	if(meta->title)
	{
		g_free(meta->title);
	}
	meta->title=g_strdup(title);
}
gboolean ly_mdh_meta_set_artist(LyMdhMeta *meta, gchar *artist)
{
	g_return_var_if_fail(meta!=NULL, FALSE);
	if(meta->artist)
	{
		g_free(meta->artist);
	}
	meta->artist=g_strdup(artist);
}
gboolean ly_mdh_meta_set_album(LyMdhMeta *meta, gchar *album);
gboolean ly_mdh_meta_set_comment(LyMdhMeta *meta, gchar *comment);
gboolean ly_mdh_meta_set_lrc(LyMdhMeta *meta, gchar *lrc);
gboolean ly_mdh_meta_set_codec(LyMdhMeta *meta, gchar *codec);
gboolean ly_mdh_meta_set_genre(LyMdhMeta *meta, gchar *genre);
gboolean ly_mdh_meta_set_biterate(LyMdhMeta *meta, guint biterate);
gboolean ly_mdh_meta_set_track(LyMdhMeta *meta, guint track);
gboolean ly_mdh_meta_set_start(LyMdhMeta *meta, gchar *start);
gboolean ly_mdh_meta_set_duration(LyMdhMeta *meta, gchar *duration);
gboolean ly_mdh_meta_set_uri(LyMdhMeta *meta, gchar *uri);
gboolean ly_mdh_meta_set_date(LyMdhMeta *meta, gint date);
gboolean ly_mdh_meta_set_playing(LyMdhMeta *meta, gint playing);
gboolean ly_mdh_meta_set_num(LyMdhMeta *meta, gint num);
gboolean ly_mdh_meta_set_flag(LyMdhMeta *meta, gint flag);
gboolean ly_mdh_meta_set_tmpflag(LyMdhMeta *meta, gint tmpflag);
gboolean ly_mdh_meta_set_cover(LyMdhMeta *meta, GstBuffer *buffer);
gint ly_mdh_meta_get_id(LyMdhMeta *meta);
gchar* ly_mdh_meta_get_title(LyMdhMeta *meta);
gchar* ly_mdh_meta_get_artist(LyMdhMeta *meta);
gchar* ly_mdh_meta_get_album(LyMdhMeta *meta);
gchar* ly_mdh_meta_get_comment(LyMdhMeta *meta);
gchar* ly_mdh_meta_get_lrc(LyMdhMeta *meta);
gchar* ly_mdh_meta_get_codec(LyMdhMeta *meta);
gchar* ly_mdh_meta_get_genre(LyMdhMeta *meta);
guint ly_mdh_meta_get_biterate(LyMdhMeta *meta);
guint ly_mdh_meta_get_track(LyMdhMeta *meta);
gchar* ly_mdh_meta_get_start(LyMdhMeta *meta);
gchar* ly_mdh_meta_get_duration(LyMdhMeta *meta);
gchar* ly_mdh_meta_get_uri(LyMdhMeta *meta);
gint ly_mdh_meta_get_date(LyMdhMeta *meta);
gint ly_mdh_meta_get_playing(LyMdhMeta *meta);
gint ly_mdh_meta_get_num(LyMdhMeta *meta);
gint ly_mdh_meta_get_flag(LyMdhMeta *meta);
gint ly_mdh_meta_get_tmpflag(LyMdhMeta *meta);
GstBuffer* y_mdh_meta_get_cover(LyMdhMeta *meta);