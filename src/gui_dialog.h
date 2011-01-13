

#ifndef GUI_DIALOG_H
#define GUI_DIALOG_H

//HEADERS
#include "global.h"
#include "debug.h"
#include "common.h"
#include "configure.h"
#include "metadata.h"
#include "library.h"
#include "playlist.h"
#include "core.h"
#include "theme.h"
#include "gui_window.h"
#include "gui_treeview.h"
#include "gui_lrc.h"

//VARIABLES
GtkWidget	*gui_dialog;

GtkWidget	*gui_dialog_tree_view;
GtkWidget	*gui_dialog_notebook;

GtkWidget	*gui_dialog_play_check_window_mode;
GtkWidget	*gui_dialog_lib_entry_bind,
			*gui_dialog_lib_button_bind,
			*gui_dialog_lib_check_autoload,
			*gui_dialog_lib_combo_sublib,
			*gui_dialog_lib_combo_encode;
GtkWidget	*gui_dialog_eq_check_autoeq,
			*gui_dialog_eq_combo_eq,
			*gui_dialog_eq_button_save,
			*gui_dialog_eq_button_delete,
			*gui_dialog_eq_vscale_band[10];
GtkWidget	*gui_dialog_lrc_check_desktop_show,
			*gui_dialog_lrc_check_desktop_fix,
			*gui_dialog_lrc_check_lrc_location,
			*gui_dialog_lrc_entry_lrc_location,
			*gui_dialog_lrc_button_lrc_location,
			*gui_dialog_lrc_check_custom,
			*gui_dialog_lrc_font_highlight,
			*gui_dialog_lrc_font_normal,
			*gui_dialog_lrc_font_desktop,
			*gui_dialog_lrc_color_highlight,
			*gui_dialog_lrc_color_normal,
			*gui_dialog_lrc_color_spectrum,
			*gui_dialog_lrc_color_desktop_bg1,
			*gui_dialog_lrc_color_desktop_bg2,
			*gui_dialog_lrc_color_desktop_fg1,
			*gui_dialog_lrc_color_desktop_fg2;
GtkWidget	*gui_dialog_th_combo_theme,
			*gui_dialog_th_image_preview;

GtkTreeStore		*gui_dialog_store,
					*gui_dialog_store_theme;
GtkTreeSelection	*gui_dialog_selection,
					*gui_dialog_selection_theme;

//FUNCTIONS
gboolean	gui_dialog_new();
GtkWidget*	gui_dialog_set_title(GtkWidget *widget, gchar *title, gchar *icon);
gboolean	gui_dialog_set_index(GtkWidget *widget, gchar *str);
gboolean	gui_dialog_quit(GtkWidget *widget,gpointer data);
gboolean	gui_dialog_show_about(GtkWidget *widget,gpointer data);
gboolean	gui_dialog_change_selection(GtkWidget *widget,gpointer data);
gboolean	gui_dialog_set_play_mode(GtkWidget *widget,gpointer data);
gboolean	gui_dialog_bind_lib(GtkWidget *widget,gpointer data);
gboolean	gui_dialog_change_sublib(GtkWidget *widget,gpointer data);
gboolean	gui_dialog_bool_change(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_int_change(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_char_change(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_font_change(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_color_change(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_change_sensitive_eq(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_select_eq(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_set_eq(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_save_eq(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_delete_eq(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_change_sensitive_lrc_location(GtkWidget *widget, gpointer data);
gboolean	gui_dialog_bind_lrc_location(GtkWidget *widget,gpointer data);
gboolean	gui_dialog_change_sensitive_lrc(GtkWidget *widget, gpointer data);
GdkColor*	gui_dialog_color_new_from_string(gchar *string);
gboolean	gui_dialog_select_theme(GtkWidget *widget, gpointer data);

#endif
