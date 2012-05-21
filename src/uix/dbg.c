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

gboolean ly_dbg_message_cb(gpointer data);
gboolean ly_dbg_warning_cb(gpointer data);
gboolean ly_dbg_critical_cb(gpointer data);



void		ly_dbg_init()
{

}
void	 ly_dbg_fina()
{
	
}

void ly_dbg_message(const char *format, ...)
{
	gchar *message;
	va_list argp;
	va_start(argp, format);
	message=g_strdup_vprintf(format, argp);
	va_end(argp);

	g_idle_add(ly_dbg_message_cb, g_strconcat(message, NULL));
	g_free(message);
}
gboolean ly_dbg_message_cb(gpointer data)
{
	GtkWidget *dialog=gtk_message_dialog_new_with_markup(
							NULL,
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_INFO,
							GTK_BUTTONS_OK,
							_("<b>MESSAGE!</b>\n\n%s"), (gchar *)data);
	g_return_val_if_fail(dialog!=NULL, FALSE);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	g_free(data);
	return FALSE;
}

void ly_dbg_warning(const char *format, ...)
{
	gchar *message;
	va_list argp;
	va_start(argp, format);
	message=g_strdup_vprintf(format, argp);
	va_end(argp);

	ly_log_put_with_flag(G_LOG_LEVEL_WARNING, message);
	g_idle_add(ly_dbg_warning_cb, g_strconcat(message, NULL));
	g_free(message);
}
gboolean ly_dbg_warning_cb(gpointer data)
{
	GtkWidget *dialog=gtk_message_dialog_new_with_markup(
							NULL,
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_OK,
							_("<b>WARNING!</b>\n\n%s"), (gchar *)data);
	g_return_val_if_fail(dialog!=NULL, FALSE);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	g_free(data);
	return FALSE;
}

void ly_dbg_critical(const char *format, ...)
{
	gchar *message;
	va_list argp;
	va_start(argp, format);
	message=g_strdup_vprintf(format, argp);
	va_end(argp);
	
	ly_log_put_with_flag(G_LOG_LEVEL_CRITICAL, message);
	g_idle_add(ly_dbg_critical_cb, g_strconcat(message, NULL));
	g_free(message);
}
gboolean ly_dbg_critical_cb(gpointer data)
{
	GtkWidget *dialog=gtk_message_dialog_new_with_markup(
							NULL,
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_OK,
							_("<b>CRITICAL!</b>\n\n%s"), (gchar *)data);
	g_return_val_if_fail(dialog!=NULL, FALSE);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	g_free(data);
	return FALSE;
}


			


