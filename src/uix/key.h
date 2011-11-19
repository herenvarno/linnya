/*
 * key.h
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



#ifndef LY_KEY_H
#define LY_KEY_H

/*
 * HEADERS
 */
#include "glb.h"
#include "dbg.h"

/*
 * TYPES
 */
enum{
	KEY_BIND_TYPE_UNDEFINED,
	KEY_BIND_TYPE_SIGNAL,
	KEY_BIND_TYPE_CALLBACK
};
enum{
	KEY_BIND_MASK0,
	KEY_BIND_MASK1,
	KEY_BIND_KEY
};

typedef struct _LyKeyKeybind
{
	char name[1024];
	char mask0[64];
	char mask1[64];
	char key[64];
	int type;
	/*
	 * widget for KEY_BIND_TYPE_SIGNAL
	 * func for KEY_BIND_TYPE_CALLBACK
	 */
	gpointer arg0;
	/*
	 * signal name for KEY_BIND_TYPE_SIGNAL
	 * user define data for KEY_BIND_TYPE_CALLBACK
	 */
	gpointer arg1;
}LyKeyKeybind;

/*
 * FUNCTIONS
 */
void			ly_key_init			();
void			ly_key_fina			();

LyKeyKeybind*	ly_key_new			(char *name);
void			ly_key_free			(gpointer *kb);

LyKeyKeybind*	ly_key_get			(char *name);
gboolean		ly_key_set			(char *name, char *mask0, char *mask1, char *key, int type, gpointer arg0, gpointer arg1);
void			ly_key_del			(char *name);
char*			ly_key_get_conflict	(char *except_name, char *mask0, char *mask1, char *key);

gboolean		ly_key_bind			(char *name);
gboolean		ly_key_unbind		(char *name);

GtkAccelGroup*	ly_key_get_accel	();
GHashTable*		ly_key_get_keybinds	();

#endif
