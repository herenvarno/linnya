/*
 * app.h
 * This file is part of linnya
 *
 * Copyright (C) 2010-2012 - Edward Yang
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
 * SECTION:		app
 * @short_description:	The top level module of the whole application.
 * @stability:	stable
 * @include:	app.h
 *
 * This module is the top level of the whole application. It defines the main
 * function, which is the code entry.
 */

#ifndef LY_APP_H
#define LY_APP_H

/*
 * HEADERS
 */
#include "cox/cox.h"
#include "uix/uix.h"

G_BEGIN_DECLS

/*
 * FUNCTIONS
 */
int main (int argc, char *argv[]);

G_END_DECLS

#endif
