/*
 * sss.obj.h
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

#ifndef LY_SSS_OBJ_H
#define LY_SSS_OBJ_H

/*
 * HEADERS
 */
#include "glb.h"
#include "key.h"
#include "win.h"
#include "pli.h"

/*
 * MACROS
 */
#define LY_SSS_SESSION_TYPE \
(ly_sss_session_get_type())
#define LY_SSS_SESSION(obj) \
(G_TYPE_CHECK_INSTANCE_CAST((obj), LY_SSS_SESSION_TYPE, LySssSession))

/*
 * TYPES
 */
typedef struct _LySssSession LySssSession;
typedef struct _LySssSessionClass LySssSessionClass;

struct _LySssSession{
	GtkWidget parent;
};
struct _LySssSessionClass{
	GtkWidgetClass parent_class;
};

/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */
GType		ly_sss_session_get_type	();
GtkWidget*	ly_sss_session_new		(gchar *name);
gboolean	ly_sss_session_refresh	(LySssSession *obj);
gboolean	ly_sss_session_config	(LySssSession *obj);

#endif