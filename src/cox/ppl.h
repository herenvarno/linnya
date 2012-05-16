/*
 * ppl.h
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
 */

#ifndef LY_PPL_H
#define LY_PPL_H

/*
 * HEADERS
 */
#include "gla.h"
#include "mbs.h"
#include "log.h"
#include "reg.h"
#include "dbm.h"
#include "mdh.h"
#include "pqm.h"

/*
 * TYPES
 */


/*
 * FUNCTIONS
 */
void				ly_ppl_init						();
void				ly_ppl_fina						();

GstElement*			ly_ppl_get_playbin				();
GstElement*			ly_ppl_audio_get_element		(char *name);
gboolean			ly_ppl_audio_bin_insert_before	(GstElement *ele, char *before_name);
gboolean			ly_ppl_audio_bin_insert_after	(GstElement *ele, char *after_name);
gboolean			ly_ppl_audio_bin_prepend		(GstElement *ele);
gboolean			ly_ppl_audio_bin_append			(GstElement *ele);
gboolean			ly_ppl_audio_bin_delete			(char *name);
GstElement*			ly_ppl_video_get_element		(char *name);


#endif //LY_PPL_H










