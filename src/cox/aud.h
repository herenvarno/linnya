/*
 * aud.h
 * This file is part of linnya
 *
 * Copyright (C) 2010-2012 - Edward Yang
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
 * SECTION:		aud
 * @short_description:	audio control
 * @stability:	stable
 * @include:	cox.h
 *
 * This module is a module to manage all operation that related to audio pipeline.
 * Such as setting the state of pipeline or setting the volume, etc.
 *
 * Typcally, the pipeline is established in PPL module before the initialization
 * of AUD. Once initialized, you can use #ly_aud_play, #ly_aud_pause, #ly_aud_prev,
 * #ly_aud_next to control the pipeline state.
 */

#ifndef LY_AUD_H
#define LY_AUD_H

/*
 * HEADERS
 */
#include "gla.h"
#include "mbs.h"
#include "dbm.h"
#include "mdh.h"
#include "lib.h"
#include "plm.h"
#include "pqm.h"
#include "ppl.h"

G_BEGIN_DECLS

/*
 *  MACROS
 */


/*
 *  VARIABLES
 */


/*
 *  FUNCTIONS
 */
void ly_aud_init ();
void ly_aud_fina ();

gboolean ly_aud_prev ();
gboolean ly_aud_next ();
gboolean ly_aud_play ();
gboolean ly_aud_pause ();
gboolean ly_aud_stop ();
gboolean ly_aud_set_mute (gboolean mute);
gboolean ly_aud_get_mute ();
gboolean ly_aud_set_volume (double volume);
gdouble ly_aud_get_volume ();
gboolean ly_aud_set_position (double position);
gboolean ly_aud_set_position_abs (gint64 position);
gdouble ly_aud_get_position ();
gint64 ly_aud_get_position_abs ();
GstState ly_aud_get_state ();

G_END_DECLS

#endif
