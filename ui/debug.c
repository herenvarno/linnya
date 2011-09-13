/**
 *
 */

/*
 * HEADERS
 */
#include "debug.h"

/*
 * FUNCTIONS
 */

gboolean ly_ui_debug_init()
{
	/*
	 * Open log file.
	 */
	char path[1024]="";
	g_snprintf(path, sizeof(path), "%s/ui/log", LY_GLOBAL_USERDIR);

	ly_ui_debug_log_fp=NULL;
	ly_ui_debug_log_fp=fopen(path,"a+");
	if(!ly_ui_debug_log_fp)
	{
		puts("FATAL ERROR: cannot open log file.");
		exit(0);
		return FALSE;
	}
	
	ly_msg_bind("info", "", ly_ui_debug_show_info_cb, NULL);
	ly_msg_bind("warning", "", ly_ui_debug_show_warning_cb, NULL);
	ly_msg_bind("error", "", ly_ui_debug_show_error_cb, NULL);
	ly_msg_bind("fatal error", "", ly_ui_debug_show_fatal_cb, NULL);
	
	return TRUE;
}

gboolean ly_ui_debug_finalize()
{
	if(ly_ui_debug_log_fp)
	{
		fclose(ly_ui_debug_log_fp);
	}
	return TRUE;
}

gboolean ly_ui_debug_get_visible(int type)
{
	if(type>DEBUG_FATAL||type<DEBUG_INFO)
	{
		ly_msg_put("error", "ui:debug", "Debug type error!");
		return FALSE;
	}
	int v[4]={0};
	if(ly_conf_get("debug_visible", "%d:%d:%d:%d", &v[0], &v[1], &v[2], &v[3]))
	{
		ly_conf_set("debug_visible", "%d:%d:%d:%d", v[0], v[1], v[2], v[3]);
	}
	if(v[type])
	{
		return TRUE;
	}
	return FALSE;
}

gboolean ly_ui_debug_set_visible(int type, gboolean visible)
{
	if(type>DEBUG_FATAL||type<DEBUG_INFO)
	{
		ly_msg_put("error", "ui:debug", "Debug type error!");
		return FALSE;
	}
	int v[4]={0};
	if(visible)
	{
		v[type]=1;
	}
	else
	{
		v[type]=0;
	}

	ly_conf_set("debug_visible", "%d:%d:%d:%d", v[0], v[1], v[2], v[3]);
	return TRUE;
}

void ly_ui_debug_log_printf(const char *format, ...)
{
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	gchar *str=NULL;
	va_list argp;
	va_start(argp, format);
	str=g_strdup_vprintf(format, argp);
	va_end(argp);
	
	fprintf(ly_ui_debug_log_fp, "[%s]%s\n", asctime(timeinfo), str);
}

gboolean ly_ui_debug_show_info_cb(gpointer message, gpointer data)
{
	lyMsgMessage *m=(lyMsgMessage*)message;
	ly_ui_debug_log_printf("[INFO]%s", m->msg);
	return TRUE;
}

gboolean ly_ui_debug_show_warning_cb(gpointer message, gpointer data)
{

	lyMsgMessage *m=(lyMsgMessage*)message;
	ly_ui_debug_log_printf("[WARNING]%s", m->msg);
	
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

gboolean ly_ui_debug_show_error_cb(gpointer message, gpointer data)
{
	lyMsgMessage *m=(lyMsgMessage*)message;
	ly_ui_debug_log_printf("[ERROR]%s", m->msg);
	
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
gboolean ly_ui_debug_show_fatal_cb(gpointer message, gpointer data)
{
	lyMsgMessage *m=(lyMsgMessage*)message;
	ly_ui_debug_log_printf("[FATAL ERROR]%s",m->msg);
	
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

	exit(0);
	return TRUE;
}


			


