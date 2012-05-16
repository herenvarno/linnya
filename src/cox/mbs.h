/*
 * mbs.h
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
 * SECTION:		msg
 * @short_description:	communication between modules
 * @stability:	stable
 * @include:	cox.h
 *
 * This module is a module to make communications between modules more easier.
 * It provides a standerad method to create, recive messages, and atomatic exec
 * functions that have binded to such particular message.
 */

#ifndef LY_MBS_H
#define LY_MBS_H

/*
 * HEADERS
 */
#include "gla.h"
#include "mbs.obj.h"

/*
 * FUNCTIONS
 */
void		ly_mbs_init		();
void		ly_mbs_fina		();	
gboolean	ly_mbs_put			(gchar *title, gchar *from, gchar *body);
gboolean	ly_mbs_bind		(gchar *title, gchar *from, \
	LyMbsFilterBindFunc func, gpointer data);
gboolean	ly_mbs_unbind		(gchar *title, gchar *from, \
	LyMbsFilterBindFunc func);

#endif
