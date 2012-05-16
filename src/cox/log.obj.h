/*
 * log.obj.h
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


#ifndef LY_LOG_OBJ_H
#define LY_LOG_OBJ_H

/*
 * HEADERS
 */
#include "gla.h"

G_BEGIN_DECLS

/*
 * MACROS
 */
#define LY_LOG_LOGGER_TYPE \
	(ly_log_logger_get_type())
#define LY_LOG_LOGGER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), LY_LOG_LOGGER_TYPE, LyLogLogger))
#define LY_LOG_LOGGER_CLASS(klass) \
	(G_TYPE_CHECK_INSTANCE_CAST((klass), LY_LOG_LOGGER_CLASS_TYPE, LyLogLoggerClass))
#define LY_LOG_LOGGER_IS_LOGGER(obj) \
	(G_TYPE_CHECK_VALUE_TYPE ((obj), LY_LOG_LOGGER_TYPE))
#define LY_LOG_LOGGER_IS_LOGGER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), LY_LOG_LOGGER_CLASS_TYPE))
#define LY_LOG_LOGGER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), LY_LOG_LOGGER_TYPE, LyLogLoggerClass))

/*
 * TYPES
 */
typedef struct _LyLogLogger LyLogLogger;
typedef struct _LyLogLoggerClass LyLogLoggerClass;

/**
 * LyLogLogger
 *
 * The #LyLogLogger object is used for writing various kind of log to a
 * specific log file while program is running. The contents of the
 * #LyLogLogger structure are private and should only be accessed via
 * provided APIs.
 */
struct _LyLogLogger {
	/*<private>*/
	GObject parent;
};

/**
 * LyLogLogger
 *
 * the class structure of #LyLogLogger.
 */
struct _LyLogLoggerClass {
	/*<private>*/
	GObjectClass parent_class;
};

/*
 * FUNCTIONS
 */
GType ly_log_logger_get_type ();
LyLogLogger* ly_log_logger_new (gchar *path);
void ly_log_logger_add (LyLogLogger *logger, gchar *str);
void ly_log_logger_clear (LyLogLogger *logger);

G_END_DECLS

#endif	//LY_LOG_OBJ_H