#include "core.h"

void ly_core_init(gint *argc, gchar ***argv)
{
	ly_global_debug("info",_("Initial core module ...\n"));
	ly_global_init(argc, argv);
	ly_msg_init();
	ly_conf_init();
	ly_db_init();
	ly_audio_init();
}

void ly_core_finalize()
{	ly_global_debug("info",_("Finalize core module ...\n"));
	ly_audio_finalize();
	ly_db_finalize();
	ly_conf_finalize();
	ly_msg_finalize();
	ly_global_finalize();
}
