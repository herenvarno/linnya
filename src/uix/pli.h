/*
 * pl.h
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



#ifndef LY_PLI_H
#define LY_PLI_H

/*
 * HEADERS
 */
#include "glb.h"
#include "key.h"
#include "pli.obj.h"

/*
 * TYPES
 */

/*
 * FUNCTIONS
 */
void ly_pli_init ();
void ly_pli_fina ();
gboolean ly_pli_add (gchar *name);
LyPliPlugin* ly_pli_get (gchar *name);
void ly_pli_del (gchar *name);
gboolean ly_pli_lock (gchar *name);
gboolean ly_pli_unlock (gchar *name);
GList *ly_pli_get_list();

#endif	//LY_PLI_H
