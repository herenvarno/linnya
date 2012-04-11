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

gboolean	ly_dbg_show_info_cb		(gpointer message, gpointer data);
gboolean	ly_dbg_show_warning_cb	(gpointer message, gpointer data);
gboolean	ly_dbg_show_error_cb	(gpointer message, gpointer data);
gboolean	ly_dbg_show_fatal_cb	(gpointer message, gpointer data);

void		ly_dbg_init()
{
	int v[DBG_COUNT]={1,1,1,1};
	if(!ly_reg_get("dbg_visible", "%d:%d:%d:%d", &v[DBG_INFO], &v[DBG_WARNING], &v[DBG_ERROR], &v[DBG_FATAL]))
	{
		ly_reg_set("dbg_visible", "%d:%d:%d:%d", v[DBG_INFO], v[DBG_WARNING], v[DBG_ERROR], v[DBG_FATAL]);
	}
	
	ly_msg_bind("info", "", ly_dbg_show_info_cb, NULL);
	ly_msg_bind("warning", "", ly_dbg_show_warning_cb, NULL);
	ly_msg_bind("error", "", ly_dbg_show_error_cb, NULL);
	ly_msg_bind("fatal", "", ly_dbg_show_fatal_cb, NULL);
	
	return TRUE;
}

void	 ly_dbg_fina()
{
}

gboolean ly_dbg_get_visible(int type)
{
	if(type>DBG_FATAL||type<DBG_INFO)
	{
		ly_msg_put("error", "ui:dbg", "Debug type error!");
		return FALSE;
	}
	int v[DBG_COUNT]={1,1,1,1};
	ly_reg_get("dbg_visible", "%d:%d:%d:%d", &v[DBG_INFO], &v[DBG_WARNING], &v[DBG_ERROR], &v[DBG_FATAL]);
	if(v[type])
	{
		return TRUE;
	}
	return FALSE;
}

gboolean ly_dbg_set_visible(int type, gboolean visible)
{
	if(type>DBG_FATAL||type<DBG_INFO)
	{
		ly_msg_put("error", "ui:dbg", "Debug type error!");
		return FALSE;
	}
	int v[DBG_COUNT]={1,1,1,1};
	ly_reg_get("debug_visible", "%d:%d:%d:%d", &v[DBG_INFO], &v[DBG_WARNING], &v[DBG_ERROR], &v[DBG_FATAL]);
	if(visible)
	{
		v[type]=1;
	}
	else
	{
		v[type]=0;
	}

	ly_reg_set("debug_visible", "%d:%d:%d:%d", v[DBG_INFO], v[DBG_WARNING], v[DBG_ERROR], v[DBG_FATAL]);
	return TRUE;
}

gboolean ly_dbg_show_info_cb(gpointer message, gpointer data)
{
	LyMsgMsg *m=(LyMsgMsg*)message;
	if(!ly_dbg_get_visible(DBG_INFO))
		return TRUE;

	GtkWidget *dialog=gtk_message_dialog_new_with_markup(
							NULL,
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_INFO,
							GTK_BUTTONS_OK,
							_("<b>INFO!</b>\n%s"), m->msg);
	if(!dialog)
	{
		return FALSE;
	}
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return TRUE;
}

gboolean ly_dbg_show_warning_cb(gpointer message, gpointer data)
{

	LyMsgMsg *m=(LyMsgMsg*)message;
	if(!ly_dbg_get_visible(DBG_WARNING))
		return TRUE;
		
	GtkWidget *dialog=gtk_message_dialog_new_with_markup(
							NULL,
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_OK,
							_("<b>WARNING!</b>\n%s"), m->msg);
	if(!dialog)
	{
		return FALSE;
	}
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return TRUE;
}

gboolean ly_dbg_show_error_cb(gpointer message, gpointer data)
{
	LyMsgMsg *m=(LyMsgMsg*)message;
	if(!ly_dbg_get_visible(DBG_ERROR))
		return TRUE;
	GtkWidget *dialog=gtk_message_dialog_new_with_markup(
							NULL,
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_OK,
							_("<b>ERROR!</b>\n%s"), m->msg);
	if(!dialog)
	{
		return FALSE;
	}
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return TRUE;
}
gboolean ly_dbg_show_fatal_cb(gpointer message, gpointer data)
{
	LyMsgMsg *m=(LyMsgMsg*)message;
	if(!ly_dbg_get_visible(DBG_FATAL))
		return TRUE;
	GtkWidget *dialog=gtk_message_dialog_new_with_markup(
							NULL,
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_OK,
							_("<b>FATAL ERROR!</b>\n%s"), m->msg);
	if(!dialog)
	{
		return FALSE;
	}
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return TRUE;
}


			


