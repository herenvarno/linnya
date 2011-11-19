/*
 * lrc.h
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
 * SECTION:		lrc
 * @Short_description:	lyrics related
 * @Stability:	stable
 *
 * This module is a module to ...
 */

#ifndef LY_LRC_H
#define LY_LRC_H

/*
 * HEADERS
 */
#include "gla.h"
#include "msg.h"
#include "log.h"
#include "pqm.h"
#include "ppl.h"

/*
 * TYPES
 */
typedef struct _LyLrcLyric
{
	gint64 time;
	gchar text[128];
}LyLrcLyric;

/*
 * FUNCTIONS
 */
void		ly_lrc_init		();
void		ly_lrc_fina		();

gboolean	ly_lrc_load		(char *path);
char*		ly_lrc_build_path	(LyMdhMetadata *md);

LyLrcLyric*	ly_lrc_new();
LyLrcLyric*	ly_lrc_new_full(gint64 time, gchar *text);
void		ly_lrc_free(LyLrcLyric *lyric);







#endif	//LY_LRC_H
