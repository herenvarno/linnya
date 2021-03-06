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
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module");
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: GLA");
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: LOG");
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: MSG");
	ly_mbs_init();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: REG");
	ly_reg_init();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: DBM");
	ly_dbm_init();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: MDH");
	ly_mdh_init();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: LIB");
	ly_lib_init();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: PLM");
	ly_plm_init();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: PQM");
	ly_pqm_init();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: PPL");
	ly_ppl_init();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: EQL");
	ly_eql_init();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: LRC");
	ly_lrc_init();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Init COX module: AUD");
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
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module");
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: AUD");
	ly_aud_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: LRC");
	ly_lrc_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: EQL");
	ly_eql_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: PPL");
	ly_ppl_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: PQM");
	ly_pqm_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: PLM");
	ly_plm_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: LIB");
	ly_lib_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: MDH");
	ly_mdh_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: DBM");
	ly_dbm_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: REG");
	ly_reg_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: MSG");
	ly_mbs_fina();
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: LOG");
	ly_log_put_with_flag(G_LOG_LEVEL_DEBUG,  "Fina COX module: GLA");
	ly_log_fina();
	ly_gla_fina();
}
