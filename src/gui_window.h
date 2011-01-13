

#ifndef GUI_H
#define GUI_H

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

//VARIABLES
//gui_window
GtkWidget 	*gui_window;
GtkWidget	*gui_window_notebook;
GtkWidget	*gui_window_hscale_seek;
GtkWidget	*gui_window_button_play,
			*gui_window_button_stop,
			*gui_window_button_prev,
			*gui_window_button_next,
			*gui_window_button_conf,
			*gui_window_button_volume,
			*gui_window_button_min,
			*gui_window_button_close;
GtkWidget	*gui_window_image_cd,
			*gui_window_image_icon,
			*gui_window_image_bg;
GtkWidget	*gui_window_label_title,
			*gui_window_label_artist,
			*gui_window_label_album,
			*gui_window_label_duration;
			
GtkWidget	*gui_window_tree_left,
			*gui_window_tree_right;

GtkWidget	*gui_window_window_lrc;
GtkWidget	*gui_window_eventbox_lrc;

GtkTreeStore		*gui_window_store_left,
					*gui_window_store_right;
GtkTreeSelection	*gui_window_selection_left,
					*gui_window_selection_right;
					
GdkPixbuf *gui_window_pixbuf_playingicon;

//FUNCTIONS
gboolean	gui_window_new();
gboolean	gui_window_expose(GtkWidget * widget,GdkEventExpose * event, gpointer data);
gboolean	gui_window_quit (GtkWidget *widget, gpointer data);
gboolean	gui_window_min(GtkWidget *widget, gpointer data);
gboolean	gui_window_drag(GtkWidget * widget,GdkEventButton * event, gpointer data);

gboolean	gui_window_play (GtkWidget *widget, gpointer data);
gboolean	gui_window_stop (GtkWidget *widget, gpointer data);
gboolean	gui_window_next (GtkWidget *widget, gpointer data);
gboolean	gui_window_prev (GtkWidget *widget, gpointer data);

gboolean	gui_window_seek (GtkWidget * widget,GdkEventButton * event, gpointer data);
gboolean	gui_window_update_hscale(gpointer data);	
gboolean	gui_window_set_volume(GtkScaleButton *button,gdouble  value,gpointer data);
gboolean	gui_window_update_display();

gboolean	gui_window_change_visibal(GtkWidget *widget, gpointer data);
gboolean	gui_window_tray_icon_popup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu);

gboolean	gui_window_draw_text (cairo_t *cr, gchar *text, gchar *font);
gboolean	gui_window_draw_text_midx (cairo_t *cr, gchar *text, gchar *font, gint width_x, gint height_y);

extern		gboolean gui_dialog_new();
#endif
