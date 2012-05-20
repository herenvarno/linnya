/*
 * thm.h
 * This file is part of linnya
 *
 * Copyright (C) 2012 - Edward Yang
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


#ifndef UI_THEME_H
#define UI_THEME_H

/*
 * HEADERS
 */
#include "glb.h"

/*
 * TYPES
 */
typedef struct _LyThmItem
{
	gchar name[128];
	gchar style[512];
	gchar sssbg[512];
}LyThmItem;

/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */
void		ly_thm_init();
void		ly_thm_fina();

GList*		ly_thm_get_list();
LyThmItem*	ly_thm_item_new();
LyThmItem*	ly_thm_item_new_with_name(gchar *name);
LyThmItem*	ly_thm_item_new_from_conf();

void		ly_thm_item_free(LyThmItem* item);
#endif
