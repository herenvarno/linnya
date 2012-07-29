/*
 * shr.h
 * This file is part of linnya
 *
 * Copyright (C) 2012 - Edward
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

#ifndef LY_3SHR_H
#define LY_3SHR_H

/*
 * HEADERS
 */
#include "cox.h"
#include "uix.h"

/*
 * MACROS
 */

/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */
const gchar*	g_module_check_init		(GModule *module);
void			g_module_unload			(GModule *module);

GtkWidget*		ly_3shr_create			();
void			ly_3shr_destroy			();
void			ly_3shr_refresh			();


#endif // LY_3SHR_H