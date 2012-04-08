/*
 * eql.h
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

#ifndef LY_EQL_H
#define ly_EQL_H

/*
 * HEADERS
 */
#include "gla.h"
#include "msg.h"
#include "log.h"
#include "dbm.h"
#include "pqm.h"
#include "ppl.h"

/*
 * TYPES
 */
typedef struct _LyEqlEqualizer{
	char	name[1024];
	double	band[10];
}LyEqlEqualizer;

/*
 * FUNCTIONS
 */
void			ly_eql_init				();
void			ly_eql_fina				();

LyEqlEqualizer*	ly_eql_new				();
LyEqlEqualizer*	ly_eql_new_from_database(char *name);
LyEqlEqualizer* ly_eql_new_by_conf		();
void			ly_eql_conf_set_by_genre();
void			ly_eql_free				(LyEqlEqualizer *eq);

gboolean		ly_eql_set_eq			(LyEqlEqualizer *eq);
gboolean		ly_eql_put				(LyEqlEqualizer *eq);


#endif
