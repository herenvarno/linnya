/*
 * log.obj.c
 * This file is part of linnya
 *
 * Copyright (C) 2010-2012 - Edward
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
#include "log.obj.h"


/*
 * MACROS
 */
#define LY_LOG_LOGGER_GET_PRIVATE(obj) \
	G_TYPE_INSTANCE_GET_PRIVATE((obj), LY_LOG_LOGGER_TYPE, LyLogLoggerPrivate)


/*
 * TYPES
 */
typedef struct _LyLogLoggerPrivate LyLogLoggerPrivate;
struct _LyLogLoggerPrivate {
    GFile *file;
};


/*
 * FUNCTIONS
 */
static void ly_log_logger_init(LyLogLogger *obj);
static void ly_log_logger_finalize(GObject *obj);
static void ly_log_logger_class_init(LyLogLoggerClass *klass);


GType ly_log_logger_get_type()
{
	static GType ly_log_logger_type = 0;
	if(!ly_log_logger_type)
	{
		static const GTypeInfo ly_log_logger_info = {
			sizeof(LyLogLoggerClass),
			NULL,
			NULL,
			(GClassInitFunc)ly_log_logger_class_init,
			NULL,
			NULL,
			sizeof(LyLogLogger),
			0,
			(GInstanceInitFunc)ly_log_logger_init,
			NULL
		};
		ly_log_logger_type = g_type_register_static(G_TYPE_OBJECT, "LyLogLogger", &ly_log_logger_info, 0);
	}
	return ly_log_logger_type;
}

static void ly_log_logger_init(LyLogLogger *obj)
{

}

static void ly_log_logger_finalize(GObject *obj)
{
	LyLogLoggerPrivate *priv=LY_LOG_LOGGER_GET_PRIVATE(LY_LOG_LOGGER(obj));
	g_object_unref(priv->file);
}

static void ly_log_logger_class_init(LyLogLoggerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = ly_log_logger_finalize;
	g_type_class_add_private(klass, sizeof(LyLogLoggerPrivate));
}

LyLogLogger*	ly_log_logger_new			(gchar *path)
{
	g_return_val_if_fail(path!=NULL, NULL);
	
	LyLogLogger *logger;
	logger = g_object_new(LY_LOG_LOGGER_TYPE, NULL);
	LyLogLoggerPrivate *priv=LY_LOG_LOGGER_GET_PRIVATE(LY_LOG_LOGGER(logger));
	priv->file=g_file_new_for_path(path);
	return logger;
}

/**
* ly_log_logger_add:
* @logger: the logger
* @str: the string data to be write to log file.
*
* Add a log item to log file.
*/
void	ly_log_logger_add		(LyLogLogger *logger, gchar *str)
{
	LyLogLoggerPrivate *priv=LY_LOG_LOGGER_GET_PRIVATE(LY_LOG_LOGGER(logger));
	GFileOutputStream *ostream;
	ostream=g_file_append_to(G_FILE(priv->file), G_FILE_CREATE_NONE, \
		NULL, NULL);
	g_output_stream_write(G_OUTPUT_STREAM(ostream), (const void *)str, \
		strlen(str), NULL, NULL);
	g_output_stream_close(G_OUTPUT_STREAM(ostream), NULL, NULL);
	g_object_unref(ostream);
}

/**
* ly_log_logger_clear:
* @logger: the logger
*
* Clear all the logs written before. This function always attempts to
* create a temp file whose filename is ended up with "~" and store all
* the old logs into it.
*/
void	ly_log_logger_clear		(LyLogLogger *logger)
{
	LyLogLoggerPrivate *priv=LY_LOG_LOGGER_GET_PRIVATE(LY_LOG_LOGGER(logger));
	GFileOutputStream *ostream;
	gchar str[64]="LOG BEGIN >>\n";	
	ostream=g_file_replace(G_FILE(priv->file), NULL, TRUE, \
		G_FILE_CREATE_NONE, NULL, NULL);
	g_output_stream_write(G_OUTPUT_STREAM(ostream), str, strlen(str), \
		NULL, NULL);
	g_output_stream_close(G_OUTPUT_STREAM(ostream), NULL, NULL);
	g_object_unref(ostream);
}
