#ifndef UI_DEFAULT_WINDOW_H
#define UI_DEFAULT_WINDOW_H

/*
 * HEADERS
 */
#include "global.h"
#include "key.h"
#include "theme.h"


/*
 * TYPES
 */
typedef struct lyUiWinWindow
{
	GtkWidget *win;
	GtkWidget *vbox_main;
	GtkWidget *hbox_title;
	GtkWidget *notebook_session;
	GtkWidget *hbox_control;
// 	GtkWidget *icon;
	GtkWidget *button_close;
 	GtkWidget *button_max;
	GtkWidget *button_min;
 	GtkWidget *button_mini;
	GtkWidget *fixed_control_left;
	GtkWidget *fixed_control_right;
	GtkWidget *button_play;
	GtkWidget *button_prev;
	GtkWidget *button_next;
	GtkWidget *button_config;
	GtkWidget *button_volume;
	GtkWidget *hscale_seek;
}lyUiWinWindow;

/*
 * VARIABLES
 */
lyUiWinWindow *ly_ui_win_window;
gboolean ly_ui_win_flag_seek;

/*
 * FUNCTIONS [PUBLIC]
 */
gboolean ly_ui_win_init();
gboolean ly_ui_win_finalize();

/*
 * FUNCTIONS [PRIVATE]
 */
gboolean ly_ui_win_quit_cb(GtkWidget *widget, gpointer data);
gboolean ly_ui_win_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean ly_ui_win_drag_cb(GtkWidget * widget,GdkEventButton * event, gpointer data);
gboolean ly_ui_win_play_cb(GtkWidget *widget, gpointer data);
gboolean ly_ui_win_prev_cb(GtkWidget *widget, gpointer data);
gboolean ly_ui_win_next_cb(GtkWidget *widget, gpointer data);
gboolean ly_ui_win_volume_cb(GtkScaleButton *button,gdouble  value,gpointer   data);
gboolean ly_ui_win_change_volume_cb (GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);
gboolean ly_ui_win_seek_cb(GtkWidget * widget, GdkEventButton * event, gpointer data);
gboolean ly_ui_win_change_seek_cb (GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);
gboolean ly_ui_win_update_hscale_cb(gpointer data);
gboolean ly_ui_win_update_meta_cb(gpointer message, gpointer data);
gboolean ly_ui_win_update_button_cb(gpointer message, gpointer data);
gboolean ly_ui_win_config_cb(GtkWidget *widget, gpointer data);
gboolean ly_ui_win_tray_popup_cb(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu);
gboolean ly_ui_win_change_visible_cb(GtkWidget *widget, gpointer data);

#endif
