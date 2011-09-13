#include "ui.h"

gboolean ly_ui_init()
{
	ly_global_debug("info","Initial ui module ...\n");
	ly_ui_global_init();
	ly_ui_debug_init();
	ly_ui_key_init();
	ly_ui_theme_init();
	ly_ui_win_init();
	ly_ui_pl_init();
	ly_ui_session_init();
	ly_ui_config_init();
	
	
	ly_ui_session_tab_add_refresh();
	gtk_main();
	return TRUE;
}
gboolean ly_ui_finalize()
{
	ly_global_debug("info","Finalize ui module ...\n");
	
	ly_ui_config_finalize();
	ly_ui_session_finalize();
	ly_ui_pl_finalize();
	ly_ui_win_finalize();
	ly_ui_theme_finalize();
	ly_ui_key_finalize();
	ly_ui_global_finalize();
	return TRUE;
}
