/*
 * cox.c
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

#include "cox.h"

/**
 * ly_cox_init:
 * @argc:			the pointer of args count passed in from main .
 * @argv:			the pointer of args container address passed in from main .
 *
 * Initialize all the core module, it calls all init functions of other modules in
 * core from base to top level. Usually, it is used in main function, just after
 * the application starts.
 */
void ly_cox_init(int *argc, char ***argv)
{
	ly_gla_init(argc, argv);
	ly_log_init();
	ly_msg_init();
	ly_reg_init();
	ly_dbm_init();
	ly_mdh_init();
	ly_lib_init();
	ly_plm_init();
	ly_pqm_init();
	ly_ppl_init();
	ly_eql_init();
	ly_lrc_init();
	ly_aud_init();
}

/**
 * ly_cox_fina:
 *
 * Finalize all the core module, it calls all fina functions of other modules in
 * core from top to base level. Usually, it is used in main function, before the
 * application ends.
 */
void ly_cox_fina()
{
	ly_aud_fina();
	ly_lrc_fina();
	ly_eql_fina();
	ly_ppl_fina();
	ly_pqm_fina();
	ly_plm_fina();
	ly_lib_fina();
	ly_mdh_fina();
	ly_dbm_fina();
	ly_reg_fina();
	ly_msg_fina();
	ly_log_fina();
	ly_gla_fina();
}
