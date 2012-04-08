/*
 * log.c
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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
#include "log.h"

/*
 * VARIABLES
 */
char ly_log_file[1024] = "";

/*
 * FUNCTIONS
 */
gboolean	ly_log_bind_cb	(gpointer message, gpointer data);

/**
 * ly_log_init:
 *
 * Initialize the log module, it will be called by #ly_cox_init.
 */
void	ly_log_init		()
{
	/*
	 * create log file
	 */
	g_snprintf(ly_log_file, sizeof(ly_log_file), "%slog", LY_GLA_USERDIR);

	if(g_file_test(ly_log_file, G_FILE_TEST_EXISTS))
	{
		remove(ly_log_file);
	}

	ly_log_put(" == START == ");

	ly_msg_bind("info", NULL, ly_log_bind_cb, NULL);
	ly_msg_bind("warning", NULL, ly_log_bind_cb, NULL);
	ly_msg_bind("error", NULL, ly_log_bind_cb, NULL);
	ly_msg_bind("fatal", NULL, ly_log_bind_cb, NULL);
}

/**
 * ly_log_fina:
 *
 * Finalize the log module, it will be called by #ly_cox_fina.
 */
void	ly_log_fina		()
{
	ly_log_put(_("[info] Finalize core module: LOG ..."));
	ly_msg_unbind("info", NULL, ly_log_bind_cb);
	ly_msg_unbind("warning", NULL, ly_log_bind_cb);
	ly_msg_unbind("error", NULL, ly_log_bind_cb);
	ly_msg_unbind("fatal", NULL, ly_log_bind_cb);
	ly_log_put(" == END == ");
}

/**
 * ly_log_put:
 * @format :	a standard printf() format string, but notice string precision pitfalls.
 * @... :		the arguments to insert in the output.
 *
 * Put a log record to log file.
 */
void	ly_log_put		(const char *format, ...)
{
	/*
	 * Open log file.
	 */
	FILE *fp=NULL;
	fp=fopen(ly_log_file, "a+");
	if(!fp)
		return;

	/*
	 * Write to file
	 */
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
	fprintf(fp, "[%s] %s\n", timestr, str);
	fclose(fp);
}

/**
 * ly_log_clear:
 *
 * Set the contents of log file to be blank.
 */
void	ly_log_clear	()
{
	/*
	 * Open log file.
	 */
	FILE *fp=NULL;
	fp=fopen(ly_log_file, "w+");
	if(!fp)
		return;

	/*
	 * Write to file
	 */
	fprintf(fp, "\n");
	fclose(fp);
}

/**
 * ly_log_bind_cb:
 * @message:	message that bind.
 * @data:		data passed in the callback function when this function is called.
 *
 * The callback function to bind all debug information.
 *
 * Returns:		TRUE for stop exec continues functions that bind to the same message,
 * FALSE for other condition.
 */
gboolean	ly_log_bind_cb	(gpointer message, gpointer data)
{
	LyMsgMsg *m=(LyMsgMsg *)message;
	ly_log_put("[%s] %s\n", m->type, m->msg);
	return FALSE;
}
