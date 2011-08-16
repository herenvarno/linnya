

//HEADERS
#include "global.h"
#include "debug.h"
#include "common.h"
#include "configure.h"
#include "metadata.h"
#include "library.h"
#include "playlist.h"
#include "equalizer.h"
#include "core.h"
#include "theme.h"
#include "gui_window.h"
#include "gui_treeview.h"

//FUNCTIONS
gboolean	gui_lrc_eventbox_expose( GtkWidget * widget, GdkEventExpose * event, gpointer data);
gboolean	gui_lrc_window_expose( GtkWidget * widget, GdkEventExpose * event, gpointer data);

gboolean	gui_lrc_read_lyrics();
gboolean	gui_lrc_analyse_lyrics(FILE *fp);
gboolean	gui_lrc_sort_lyrics();
gboolean	gui_lrc_update_lrc();
gboolean	gui_lrc_download_lyrics();
gboolean	gui_lrc_seek( GtkWidget * widget, GdkEventButton *event, gpointer data);
gboolean	gui_lrc_expose_desktop( GtkWidget * widget, GdkEventExpose * event, gpointer data);
gboolean	gui_lrc_move_desktop( GtkWidget * widget, GdkEvent *event, gpointer data);