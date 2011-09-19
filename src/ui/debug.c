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
	char path[1024]="";
	g_snprintf(path, sizeof(path), "%s/ui/log", LY_GLOBAL_USERDIR);
	if(g_file_test(path, G_FILE_TEST_EXISTS))
	{
		remove(path);
	}
	ly_ui_debug_log_printf(" == START == ");
	
	ly_msg_bind("silence", "", ly_ui_debug_show_silence_cb, NULL);
	ly_msg_bind("info", "", ly_ui_debug_show_info_cb, NULL);
	ly_msg_bind("warning", "", ly_ui_debug_show_warning_cb, NULL);
	ly_msg_bind("error", "", ly_ui_debug_show_error_cb, NULL);
	ly_msg_bind("fatal", "", ly_ui_debug_show_fatal_cb, NULL);
	
	return TRUE;
}

gboolean ly_ui_debug_finalize()
{
	ly_ui_debug_log_printf(" == END == ");
	return TRUE;
}

gboolean ly_ui_debug_get_visible(int type)
{
	if(type>DEBUG_FATAL||type<DEBUG_INFO)
	{
		ly_msg_put("error", "ui:debug", "Debug type error!");
		return FALSE;
	}
	int v[DEBUG_COUNT]={1,1,1,1,1};
	if(!ly_conf_get("debug_visible", "%d:%d:%d:%d:%d", &v[DEBUG_SILENCE], &v[DEBUG_INFO], &v[DEBUG_WARNING], &v[DEBUG_ERROR], &v[DEBUG_FATAL]))
	{
		ly_conf_set("debug_visible", "%d:%d:%d:%d:%d", v[DEBUG_SILENCE], v[DEBUG_INFO], v[DEBUG_WARNING], v[DEBUG_ERROR], v[DEBUG_FATAL]);
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
	int v[DEBUG_COUNT]={1,1,1,1,1};
	ly_conf_get("debug_visible", "%d:%d:%d:%d", &v[DEBUG_SILENCE], &v[DEBUG_INFO], &v[DEBUG_WARNING], &v[DEBUG_ERROR], &v[DEBUG_FATAL]);
	if(visible)
	{
		v[type]=1;
	}
	else
	{
		v[type]=0;
	}

	ly_conf_set("debug_visible", "%d:%d:%d:%d", v[DEBUG_SILENCE], v[DEBUG_INFO], v[DEBUG_WARNING], v[DEBUG_ERROR], v[DEBUG_FATAL]);
	return TRUE;
}

void ly_ui_debug_log_printf(const char *format, ...)
{
	/*
	 * Open log file.
	 */
	char path[1024]="";
	g_snprintf(path, sizeof(path), "%s/ui/log", LY_GLOBAL_USERDIR);
	FILE *fp=NULL;
	fp=fopen(path,"a+");
	if(!fp)
	{
		puts("FATAL ERROR: cannot open log file.");
		return;
	}

	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	gchar *str=NULL;
	va_list argp;
	va_start(argp, format);
	str=g_strdup_vprintf(format, argp);
	va_end(argp);
	
	char timestr[1024]="";
	g_strlcpy(timestr, asctime(timeinfo), strlen(asctime(timeinfo)));
	fprintf(fp, "[%s]%s\n", timestr, str);
	fclose(fp);
}

gboolean ly_ui_debug_show_silence_cb(gpointer message, gpointer data)
{
	lyMsgMessage *m=(lyMsgMessage*)message;
	ly_ui_debug_log_printf("[SILENCE]%s", m->msg);
	return TRUE;
}

gboolean ly_ui_debug_show_info_cb(gpointer message, gpointer data)
{

	lyMsgMessage *m=(lyMsgMessage*)message;
	ly_ui_debug_log_printf("[INFO]%s", m->msg);
	if(!ly_ui_debug_get_visible(DEBUG_INFO))
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

gboolean ly_ui_debug_show_warning_cb(gpointer message, gpointer data)
{

	lyMsgMessage *m=(lyMsgMessage*)message;
	ly_ui_debug_log_printf("[WARNING]%s", m->msg);
	if(!ly_ui_debug_get_visible(DEBUG_WARNING))
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

gboolean ly_ui_debug_show_error_cb(gpointer message, gpointer data)
{
	lyMsgMessage *m=(lyMsgMessage*)message;
	ly_ui_debug_log_printf("[ERROR]%s", m->msg);
	if(!ly_ui_debug_get_visible(DEBUG_ERROR))
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
gboolean ly_ui_debug_show_fatal_cb(gpointer message, gpointer data)
{
	lyMsgMessage *m=(lyMsgMessage*)message;
	ly_ui_debug_log_printf("[FATAL ERROR]%s",m->msg);
	if(!ly_ui_debug_get_visible(DEBUG_FATAL))
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

	exit(0);
	return TRUE;
}


			


