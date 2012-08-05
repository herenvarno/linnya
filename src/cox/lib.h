/*
 * lib.h
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
 * SECTION: lib
 * @short_description: the music library module.
 * @stability: stable
 * @include: cox.h
 *
 * This module is a module dealing with all operation ralated to music 
 * raw data in library. This data is stored in the table named "metadatas"
 * which is defined in the database managed by module dbm. 
 */


#ifndef LY_LIB_H
#define LY_LIB_H

/*
 * HEADERS
 */
#include "gla.h"
#include "mbs.h"
#include "log.h"
#include "dbm.h"
#include "mdh.h"


/*
 * FUNCTIONS
 */
void			ly_lib_init				();
void			ly_lib_fina				();

int				ly_lib_add_md			(LyMdhMetadata *md);
int				ly_lib_update_md		(LyMdhMetadata *md);
void			ly_lib_del_md			(int id);
void			ly_lib_del_md_by_where	(char *sql);
void			ly_lib_del_md_from_disk	(int id);

LyMdhMetadata*	ly_lib_get_md			(int id);
LyMdhMetadata*	ly_lib_get_md_by_sql	(char *sql);
int				ly_lib_get_id			(LyMdhMetadata *md);

void			ly_lib_check_library	();
gboolean		ly_lib_check_is_audio	(char *uri);
gboolean		ly_lib_check_is_video	(char *uri);
GList*			ly_lib_check_is_legal_list	(GList **l, gpointer func);


#endif // LY_LIB_H
