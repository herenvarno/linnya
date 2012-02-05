/*
 * plm.h
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

#ifndef LY_PLM_H
#define LY_PLM_H

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

/*
 * TYPES
 */


/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */
void		ly_plm_init				();
void		ly_plm_fina				();

int			ly_plm_add_pl			(char *name);
void		ly_plm_del_pl			(int pid);
void		ly_plm_del_pl_by_where	(char *where);
void		ly_plm_clear_pl			();
void		ly_plm_rename_pl		(int pid, char *name);

gboolean	ly_plm_import_pl		(char *path);
gboolean	ly_plm_export_pl		(int pid, char *path);

void		ly_plm_add_md			(int pid, int mid);
void		ly_plm_add_md_in_order	(int pid, int mid, int num);
void		ly_plm_del_md			(int pid, int mid);
void		ly_plm_del_md_by_where	(int pid, char *where);
void		ly_plm_clear_md			(int pid);

int			ly_plm_get_id			(char *name);


#endif //LY_PLM_H
