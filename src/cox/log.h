/*
 * log.h
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

/**
 * SECTION:		log
 * @short_description:	record logs
 * @stability:	stable
 * @include:	cox.h
 *
 * This is a module to record logs into a specific logfile.
 * 
 * Logs are recorded through an inner object with a type of #LyLogLogger, called
 * logger which is built in the init function, and destroyed in fina function.
 * Once the initialization finished, you can use #ly_log_put_with_flag to record
 * a log with any log level given in #GLogLevelFlag.
 *
 * All level of logs produced by inner signal of glib are captured automatically
 * by a pre-defined handler which push the significant log record into logfile. 
 * so once you call a debug function of glib like #g_critical, the log will be 
 * pushed into logfile without doing #ly_log_put_with_flag. which is a much easier 
 * and safer way to record a log than using #ly_log_put_with_flag.
 */


#ifndef LY_LOG_H
#define LY_LOG_H

#include "gla.h"
#include "log.obj.h"

G_BEGIN_DECLS

/*
 * FUNCTION
 */
// INIT & FINAL
void ly_log_init ();
void ly_log_fina ();

// API
void ly_log_put_with_flag (GLogLevelFlags flag, const char *format, ...);

G_END_DECLS

#endif	//LY_LOG_H
