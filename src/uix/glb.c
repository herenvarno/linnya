/*
 * glb.c
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


/*
 * HEADERS
 */
#include "glb.h"

/*
 * FUNCTIONS
 */
void		ly_glb_init		(int *argc, char ***argv)
{
	/*
	 * mkdir
	 */
	LY_GLB_PROG_UIDIR		=	g_strconcat(LY_GLA_PROGDIR,"ui/",NULL);
	LY_GLB_USER_UIDIR		=	g_strconcat(LY_GLA_USERDIR,"ui/",NULL);
	LY_GLB_PROG_PLUGINDIR	=	g_strconcat(LY_GLA_PROGDIR,"plugin/",NULL);
	LY_GLB_USER_PLUGINDIR	=	g_strconcat(LY_GLA_USERDIR,"plugin/",NULL);
	LY_GLB_PROG_THEMEDIR	=	g_strconcat(LY_GLB_PROG_UIDIR,"theme/",NULL);
	LY_GLB_USER_THEMEDIR	=	g_strconcat(LY_GLB_USER_UIDIR,"theme/",NULL);

	mkdir(LY_GLB_USER_UIDIR, 0755);
	mkdir(LY_GLB_USER_PLUGINDIR, 0755);
	mkdir(LY_GLB_USER_THEMEDIR, 0755);
	
	/*
	 * initial gtk
	 */
	gtk_init (argc, argv);
}

void ly_glb_fina()
{
} 