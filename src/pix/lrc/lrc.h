/*
 * lrc.h
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

#ifndef LY_3LRC_H
#define LY_3LRC_H

/*
 * HEADERS
 */
#include "cox.h"
#include "uix.h"
#include "lrc_widget.h"
#include "lrc_desktop.h"
#include "lrc_config.h"

/*
 * VARIABLES
 */

/*
 * FUNCTIONS
 */
const gchar*	g_module_check_init		(GModule *module);
void			g_module_unload			(GModule *module);

GtkWidget*		ly_3lrc_create			();
void			ly_3lrc_destroy			();
void			ly_3lrc_refresh			();

#endif	//LY_3LRC_H
