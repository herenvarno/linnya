#ifndef PLUGIN_LRC
#define PLUGIN_LRC

/*
 * HEADERS
 */
#include "core.h"
#include "ui.h"

/*
 * TYPES
 */
typedef struct lyPluginLrcLyric
{
	gint64 time;
	gchar text[512];
}lyPluginLrcLyric;

/*
 * VARIABLES
 */
lyPluginLrcLyric *ly_plugin_lrc_lyrics_array[10240];
gint ly_plugin_lrc_lyrics_length;
gint ly_plugin_lrc_lyrics_index;

GtkWidget *window;
GtkWidget *ly_plugin_lrc_desktop;

/*
 * FUNCTIONS
 */
const gchar *g_module_check_init(GModule *module);
void g_module_unload(GModule *module);

GtkWidget *ly_plugin_lrc_create();
void ly_plugin_lrc_destroy();
GtkWidget *ly_plugin_lrc_config();

gboolean ly_plugin_lrc_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean ly_plugin_lrc_seek_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean ly_plugin_lrc_read_cb(gpointer message, gpointer data);
void ly_plugin_lrc_read_lyrics(FILE *fp);
void ly_plugin_lrc_sort_lyrics();
gboolean ly_plugin_lrc_update_index();
gboolean ly_plugin_lrc_desktop_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean ly_plugin_lrc_desktop_drag_cb( GtkWidget *widget, GdkEvent *event, gpointer data);
gboolean ly_plugin_lrc_desktop_update_cb(gpointer data);

void ly_plugin_lrc_draw_text (cairo_t *cr, gchar *text, gchar *font);
void ly_plugin_lrc_draw_text_midx (cairo_t *cr, gchar *text, gchar *font, gint width_x, gint height_y);

#endif
