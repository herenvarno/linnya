/*
 * pqm.h
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
 * SECTION:		pqm
 * @short_description: playing queue
 * @stability:	stable
 * @include:	cox.h
 *
 * This module is a module to
 */

#ifndef LY_PQM_H
#define LY_PQM_H

/*
 * HEADERS
 */
#include "gla.h"
#include "msg.h"
#include "log.h"
#include "reg.h"
#include "dbm.h"
#include "mdh.h"
#include "lib.h"
#include "plm.h"

/*
 * TYPES
 */


/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */
void			ly_pqm_init				();
void			ly_pqm_fina				();

void			ly_pqm_add_md			(int id);
void			ly_pqm_add_md_by_where_from_lib	(char *where);
void			ly_pqm_add_md_by_where_from_plm	(int pid, char *where);
void			ly_pqm_del_md			(int id);
void			ly_pqm_del_md_by_where	(char *where);
void			ly_pqm_clear_md			();
LyMdhMetadata*	ly_pqm_get_md			(int id);
LyMdhMetadata*	ly_pqm_get_md_by_sql	(char *sql);

LyMdhMetadata*	ly_pqm_get_current_md	();
void			ly_pqm_set_current_md	(int id);
void			ly_pqm_set_rand			();
void			ly_pqm_set_next			();
void			ly_pqm_set_prev			();


#endif //LY_PQM_H
