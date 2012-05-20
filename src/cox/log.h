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
 * SECTION:		dbm
 * @short_description:	database storage
 * @stability:	stable
 * @include:	cox.h
 *
 * This module is a module to store data into a database file through sqlite3.
 */


#ifndef LY_LOG_H
#define LY_LOG_H

#include "gla.h"
#include "log.obj.h"

G_BEGIN_DECLS

/*
 * FUNCTION
 */
void ly_log_init ();
void ly_log_fina ();

void ly_log_put (const char *format, ...);
void ly_log_put_with_flag (GLogLevelFlags flag, const char *format, ...);

G_END_DECLS

#endif	//LY_LOG_H
