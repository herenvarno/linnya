/*
 * dbm.h
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

/**
 * SECTION:		dbm
 * @short_description:	database storage
 * @stability:	stable
 * @include:	cox.h
 *
 * This module is a module to store data into a database file through sqlite3.
 */

#ifndef LY_DBM_H
#define LY_DBM_H

/*
 * HEADERS
 */
#include "gla.h"
#include "log.h"
#include "msg.h"
#include "reg.h"

/*
 * FUNCTIONS
 */
void		ly_dbm_init		();
void		ly_dbm_fina		();

int			ly_dbm_exec		(char *sql, gpointer func, gpointer data);
int			ly_dbm_get_count(char *sql);

void		ly_dbm_replace_str	(char *string, int length);
int			ly_dbm_get_last_insert_rowid();

#endif
