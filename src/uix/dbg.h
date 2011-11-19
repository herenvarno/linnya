/*
 * dbg.h
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

/**
 */

#ifndef LY_DBG_H
#define LY_DBG_H

/*
 * HEADERS
 */
#include "glb.h"


/*
 * TYPES
 */
enum{
	DBG_INFO,
	DBG_WARNING,
	DBG_ERROR,
	DBG_FATAL,
	DBG_COUNT
};

/*
 * VARABLES
 */


/*
 * FUNCTIONS
 */
void		ly_dbg_init			(void);
void		ly_dbg_fina			(void);

gboolean	ly_dbg_get_visible	(int type);
gboolean	ly_dbg_set_visible	(int type, gboolean visible);

#endif
