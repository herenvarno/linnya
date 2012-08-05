#include "log.h"

/*
 * VARIABLES
 */
LyLogLogger *ly_log_logger;


/*
 * FUNCTIONS
 */
void ly_log_on_set_default_handler_cb(const gchar *log_domain, \
	GLogLevelFlags log_level, const gchar *message, gpointer user_data);

void	ly_log_init		()
{
	gchar path[1024]="";
	g_snprintf(path, sizeof(path), "%slog", LY_GLA_USERDIR);
	ly_log_logger=ly_log_logger_new(path);
	if(!ly_log_logger)
	{
		g_error(_("Cannot build logger. Abort ..."));
	}
	ly_log_logger_clear(ly_log_logger);

	//HANDLER
	g_log_set_default_handler(ly_log_on_set_default_handler_cb, NULL);
}

void	ly_log_fina		()
{
	g_object_unref(ly_log_logger);
}

/**
 * ly_log_put_with_flag:
 *
 * @flag: the log level flag, defined in #GLogLevelFlag.
 * @format: the format, which is similar to the format style of printf.
 *
 * Put a log record to logfile with specific log level flag.
 *
 */
void	ly_log_put_with_flag	(GLogLevelFlags flag, const char *format, ...)
{
	if(flag<G_LOG_LEVEL_ERROR || flag>G_LOG_LEVEL_DEBUG)
		return;

	gchar timestr[128]="";
	gchar timetmp[128]="";
	gchar flagstr[64]="";
	gchar *logstr=NULL;
	gchar *str=NULL;

	//TIMESTR
	time_t rawtime;
	struct tm *timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	g_strlcpy(timetmp, asctime(timeinfo), strlen(asctime(timeinfo)));
	g_snprintf(timestr, sizeof(timestr), "[%s]", timetmp);

	//FLAGSTR
	switch(flag)
	{
		case G_LOG_LEVEL_ERROR:
			g_strlcpy(flagstr, "** ERROR ** :", sizeof(flagstr));
			break;
		case G_LOG_LEVEL_CRITICAL:
			g_strlcpy(flagstr, "** CRITICAL ** :", sizeof(flagstr));
			break;
		case G_LOG_LEVEL_WARNING:
			g_strlcpy(flagstr, "** WARNING ** :", sizeof(flagstr));
			break;
		case G_LOG_LEVEL_MESSAGE:
			g_strlcpy(flagstr, "** MESSAGE ** :", sizeof(flagstr));
			break;
		case G_LOG_LEVEL_INFO:
			g_strlcpy(flagstr, "** INFO ** :", sizeof(flagstr));
			break;
		case G_LOG_LEVEL_DEBUG:
			g_strlcpy(flagstr, "** DEBUG ** :", sizeof(flagstr));
			break;
		default:
			g_strlcpy(flagstr, "** DEBUG ** :", sizeof(flagstr));
			break;
	}

	//LOGSTR
	va_list argp;
	va_start(argp, format);
	logstr=g_strdup_vprintf(format, argp);
	va_end(argp);

	//WRITE THE WHOLE STR TO FILE
	str=g_strconcat(timestr, flagstr, logstr, "\n", NULL);
	ly_log_logger_add(LY_LOG_LOGGER(ly_log_logger), str);
	g_free(logstr);
	g_free(str);
}

void ly_log_on_set_default_handler_cb(const gchar *log_domain, \
	GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
	ly_log_put_with_flag(log_level, message);
}
