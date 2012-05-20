/*
 * dbg.c
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya. If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * HEADERS
 */
#include "dbg.h"

/*
 * FUNCTIONS
 */

void ly_dbg_show_message_cb	(const gchar *log_domain, \
	GLogLevelFlags log_level, const gchar *message, gpointer data);
void ly_dbg_show_warning_cb	(const gchar *log_domain, \
	GLogLevelFlags log_level, const gchar *message, gpointer data);
void ly_dbg_show_critical_cb	(const gchar *log_domain, \
	GLogLevelFlags log_level, const gchar *message, gpointer data);
gboolean ly_dbg_show_message_dialog_cb(gpointer data);
gboolean ly_dbg_show_warning_dialog_cb(gpointer data);
gboolean ly_dbg_show_critical_dialog_cb(gpointer data);

void		ly_dbg_init()
{
	g_log_set_handler(NULL, G_LOG_LEVEL_MESSAGE, ly_dbg_show_message_cb, NULL);
	g_log_set_handler(NULL, G_LOG_LEVEL_WARNING, ly_dbg_show_warning_cb, NULL);
	g_log_set_handler(NULL, G_LOG_LEVEL_CRITICAL, ly_dbg_show_critical_cb, NULL);
	return TRUE;
}
void	 ly_dbg_fina()
{
	
}
void ly_dbg_show_message_cb	(const gchar *log_domain, \
	GLogLevelFlags log_level, const gchar *message, gpointer data)
{
	ly_log_put_with_flag(log_level, message);
	g_idle_add(ly_dbg_show_message_dialog_cb, g_strconcat(message, NULL));
}
gboolean	ly_dbg_show_message_dialog_cb(gpointer data)
{
	GtkWidget *dialog=gtk_message_dialog_new_with_markup(
							NULL,
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_INFO,
							GTK_BUTTONS_OK,
							_("<b>MESSAGE!</b>\n\n%s"), data);
	g_free(data);
	g_return_if_fail(dialog!=NULL);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return FALSE;
}
void ly_dbg_show_warning_cb	(const gchar *log_domain, \
	GLogLevelFlags log_level, const gchar *message, gpointer data)
{
	ly_log_put_with_flag(log_level, message);
	g_idle_add(ly_dbg_show_warning_dialog_cb, g_strconcat(message, NULL));
}
gboolean	ly_dbg_show_warning_dialog_cb(gpointer data)
{
	GtkWidget *dialog=gtk_message_dialog_new_with_markup(
							NULL,
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_OK,
							_("<b>WARNING!</b>\n\n%s"), data);
	g_free(data);
	g_return_if_fail(dialog!=NULL);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
void ly_dbg_show_critical_cb	(const gchar *log_domain, \
	GLogLevelFlags log_level, const gchar *message, gpointer data)
{
	ly_log_put_with_flag(log_level, message);
	g_idle_add(ly_dbg_show_critical_dialog_cb, g_strconcat(message, NULL));
}
gboolean	ly_dbg_show_critical_dialog_cb(gpointer data)
{
	GtkWidget *dialog=gtk_message_dialog_new_with_markup(
							NULL,
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_OK,
							_("<b>CRITICAL!</b>\n\n%s"), data);
	g_free(data);
	g_return_if_fail(dialog!=NULL);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}


			


