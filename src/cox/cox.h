/*
 * cox.h
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
 * SECTION:		cox
 * @short_description:	the top module of core.
 * @stability:	stable
 * @include:	cox.h
 *
 * This module is the top level of the core part. It provides a public interface
 * to upper module to initialize and finalize all the core module, such as GLA or
 * DBM. This module can only be used for initializing and finalizing the core.
 */

#ifndef LY_COX_H
#define LY_COX_H

/*
 * HEADERS
 */
#include "gla.h"
#include "mbs.h"
#include "log.h"
#include "reg.h"
#include "dbm.h"
#include "mdh.h"
#include "lib.h"
#include "plm.h"
#include "pqm.h"
#include "ppl.h"
#include "eql.h"
#include "lrc.h"
#include "aud.h"

/*
 * FUNCTIONS
 */
void	ly_cox_init	(int *argc, char ***argv);
void	ly_cox_fina	();

#endif
