/*
 * opc_right.h
 * This file is part of linnya:plugin
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya:plugin is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya:plugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya:plugin. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LY_3OPC_H
#define LY_3OPC_H

/*
 * HEADERS
 */
#include "cox.h"
#include "uix.h"

/*
 * FUNCTIONS
 */
void		ly_3opc_right_init		();
void		ly_3opc_right_fina		();
GtkWidget*	ly_3opc_right_create	();
void		ly_3opc_right_destroy	();

void		ly_3opc_right_refresh	();


#endif	//LY_3OPC_H
