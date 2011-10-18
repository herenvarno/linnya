#ifndef LY_UI_DEBUG_H
#define LY_UI_DEBUG_H

/*
 * HEADERS
 */
#include "global.h"


/*
 * TYPES
 */
enum{
	DEBUG_SILENCE,
	DEBUG_INFO,
	DEBUG_WARNING,
	DEBUG_ERROR,
	DEBUG_FATAL,
	DEBUG_COUNT
};

/*
 * VARABLES
 */


/*
 * FUNCTIONS
 */
gboolean	ly_ui_debug_init			(void);
gboolean	ly_ui_debug_finalize		(void);

gboolean	ly_ui_debug_get_visible		(int type);
gboolean	ly_ui_debug_set_visible		(int type, gboolean visible);

void		ly_ui_debug_log_printf		(const char *fmt, ...);
gboolean	ly_ui_debug_show_silence_cb	(gpointer message, gpointer data);
gboolean	ly_ui_debug_show_info_cb	(gpointer message, gpointer data);
gboolean	ly_ui_debug_show_warning_cb	(gpointer message, gpointer data);
gboolean	ly_ui_debug_show_error_cb	(gpointer message, gpointer data);
gboolean	ly_ui_debug_show_fatal_cb	(gpointer message, gpointer data);

#endif
