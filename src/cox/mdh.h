/*
 * mdh.h
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
 * SECTION:		mdh
 * @short_description:	metadata handler
 * @stability:	stable
 * @include:	cox.h
 *
 * This module is a module to manage all kinds of operation that related to
 * metadata. Usually you can create a metadata type structure by using #ly_mdh_new
 * or #ly_mdh_new_with_uri .
 */

#ifndef LY_MDH_H
#define LY_MDH_H

/*
 * HEADERS
 */
#include "gla.h"
#include "msg.h"
#include "log.h"
#include "reg.h"
#include "dbm.h"

/*
 * TYPES
 */
typedef struct _LyMdhMetadata{
	gint		id;
	gchar		title[128];
	gchar		artist[128];
	gchar		album[128];
	gchar		comment[1024];
	gchar		codec[128];
	gchar		genre[128];
	guint		bitrate;
	guint		track;
	gchar		start[64];
	gchar		duration[64];
	gchar		uri[1024];
	gint		date;
	gint		playing;
	gint		num;
	gint		flag;
	gint		tmpflag;
	GstBuffer	*cover;
	gchar		lrc[5120];
}LyMdhMetadata;

enum{
	MDH_FLAG_INVALID		=	-1,
	MDH_FLAG_LOCAL_AUDIO	=	000,
	MDH_FLAG_CUE_AUDIO		=	001,
	MDH_FLAG_REMOTE_AUDIO	=	002,
	MDH_FLAG_LOCAL_VIDEO	=	010,
	MDH_FLAG_CUE_VIDEO		=	011,
	MDH_FLAG_REMOTE_VIDEO	=	012,
	MDH_FLAG_RADIO			=	100,
	MDH_FLAG_TV				=	110
};

/*
 * FUNCTIONS
 */
void			ly_mdh_init				();
void			ly_mdh_fina				();

LyMdhMetadata*	ly_mdh_new				();
LyMdhMetadata*	ly_mdh_new_with_uri		(char *uri);
LyMdhMetadata*	ly_mdh_new_with_uri_full(char *uri);
void			ly_mdh_free				(LyMdhMetadata* md);

char*			ly_mdh_time_int2str		(gint64 t_int);
gint64			ly_mdh_time_str2int		(char *t_str);

gboolean		ly_mdh_push(LyMdhMetadata *md);


#endif
