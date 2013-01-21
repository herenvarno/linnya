/*
 * win.h
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



#ifndef LY_WIN_H
#define LY_WIN_H

/*
 * HEADERS
 */
#include "glb.h"
#include "dbg.h"
#include "key.h"
#include "thm.h"

/*
 * TYPES
 */
typedef struct _LyWinWindow
{
	GtkWidget *win;
	GtkWidget *nbk_sssn;
	GtkWidget *btn_menu;
	GtkWidget *btn_adds;
	GtkWidget *btn_full;
	GtkWidget *btn_play;
	GtkWidget *btn_prev;
	GtkWidget *btn_next;
	GtkWidget *btn_conf;
	GtkWidget *btn_mute;
	GtkWidget *bar_seek;
	GtkWidget *bar_volm;
	GtkWidget *grd_ctrl;
	GtkWidget *btn_home;
}LyWinWindow;


/*
 * FUNCTIONS [PUBLIC]
 */
void			ly_win_init			();
void			ly_win_fina			();

LyWinWindow*	ly_win_new			();
void			ly_win_free			();

LyWinWindow*	ly_win_get_window	();

#endif	//LY_WIN_H
