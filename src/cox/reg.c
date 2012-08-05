/*
 * reg.c
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
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

/*
 * HEADERS
 */
#include "reg.h"

/*
 * VARIABLES
 */
LyRegTable *ly_reg_table;

/*
 * FUNCTIONS
 */
void		ly_reg_init		()
{
	gchar path[1024]="";
	g_snprintf(path, sizeof(path), "%sconf.xml", LY_GLA_USERDIR);
	ly_reg_table=ly_reg_table_new(path);
}

/**
 * ly_reg_fina:
 *
 * Finalize the reg module, it will be called by #ly_cox_fina
 */
void		ly_reg_fina		()
{
	if(ly_reg_table)
	{
		g_object_unref(ly_reg_table);
	}
}


gboolean	ly_reg_get(char *key, const char *format, ...)
{
	char *str=NULL;
	str=ly_reg_table_get(ly_reg_table, key);

	if(str==NULL)
		return FALSE;

	va_list argp;
	va_start(argp, format);
	vsscanf(str, format, argp);
	va_end(argp);

	return TRUE;
}

gboolean	ly_reg_set(char *key, const char *format, ...)
{
	char *str=NULL;
	va_list argp;
	va_start(argp, format);
	str=g_strdup_vprintf(format, argp);
	va_end(argp);

	ly_reg_table_set(ly_reg_table, key, str);
	g_free(str);

	char s[1024]="";
	g_snprintf(s, sizeof(s), "reg_%s_changed", key);
	ly_mbs_put(s, "core:reg", str);
	return TRUE;
}