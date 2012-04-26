/*
 * HEADERS
 */
#include "dnc.h"

/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */

void ly_3dnc_download_cov		();
void ly_3dnc_run_thread_cov		(gpointer data);

const gchar* g_module_check_init(GModule *module)
{
	curl_global_init(0);
	ly_3dnc_cov_init();
	ly_3dnc_lrc_init();
	return NULL;
}
void g_module_unload(GModule *module)
{
	ly_3dnc_lrc_fina();
	ly_3dnc_cov_fina();
}
