/*
 * reg.h
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
 * SECTION:		reg
 * @short_description:	configuration register
 * @stability:	stable
 * @include:	cox.h
 *
 * This module is a module to manage all configuration records. It has a hash
 * table to store all the records. And, the module REG also provides a serious
 * of API functions to access a particular reg records, Such as set or get.
 */

#ifndef LY_REG_H
#define LY_REG_H

/*
 * HEADERS
 */
#include "gla.h"
#include "mbs.h"
#include "log.h"
#include "reg.obj.h"

/*
 * FUNCTIONS
 */
void		ly_reg_init		();
void		ly_reg_fina		();

gboolean	ly_reg_get		(char *name, const char *format, ...);	/*similar to scanf()*/
gboolean	ly_reg_set		(char *name, const char *format, ...);	/*similar to printf()*/

#endif
