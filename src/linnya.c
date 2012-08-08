/*
 * app.c
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

#include "linnya.h"

/**
 * main:
 * @argc:			the args count.
 * @argv:			the args container array.
 *
 * Code entry.
 *
 * Returns:			0 for success, others fail.
 */
int main(int argc, char *argv[])
{
	/*
	 * Initial
	 */
	ly_cox_init(&argc, &argv);
	ly_uix_init(&argc, &argv);

	/*
	 * Finalzie
	 */
	ly_uix_fina();
	ly_cox_fina();

	return 0;
}
