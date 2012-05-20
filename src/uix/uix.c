/*
 * uix.c
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

#include "uix.h"

/**
 * ly_uix_init:
 * @argc:			the pointer of args count passed in from main .
 * @argv:			the pointer of args container address passed in from main .
 *
 * Initialize all the ui module, it calls all init functions of other modules in
 * ui from base to top level. Usually, it is used in main function, just after
 * the application starts.
 */
void ly_uix_init(int *argc, char ***argv)
{
	g_debug("Init UIX module");
	g_debug("Init UIX module: GLB");
	ly_glb_init(argc, argv);
	g_debug("Init UIX module: KEY");
	ly_key_init();
	g_debug("Init UIX module: THM");
	ly_thm_init();
	g_debug("Init UIX module: WIN");
	ly_win_init();
	g_debug("Init UIX module: PLI");
	ly_pli_init();
	g_debug("Init UIX module: SSS");
	ly_sss_init();
	g_debug("Init UIX module: CFG");
	ly_cfg_init();
	g_debug("Init UIX module: DBG");
	ly_dbg_init();
	g_debug("Main Loop begin");
	gtk_main();
}

/**
 * ly_uix_fina:
 *
 * Finalize all the ui module, it calls all fina functions of other modules in
 * ui from top to base level. Usually, it is used in main function, before the
 * application ends.
 */
void ly_uix_fina()
{
	g_debug("Fina UIX module");
	g_debug("Fina UIX module: DBG");
	ly_dbg_fina();
	g_debug("Fina UIX module: CFG");
	ly_cfg_fina();
	g_debug("Fina UIX module: SSS");
	ly_sss_fina();
	g_debug("Fina UIX module: PLI");
	ly_pli_fina();
	g_debug("Fina UIX module: WIN");
	ly_win_fina();
	g_debug("Fina UIX module: THM");
	ly_thm_fina();
	g_debug("Fina UIX module: KEY");
	ly_key_fina();
	g_debug("Fina UIX module: GLB");
	ly_glb_fina();
}
