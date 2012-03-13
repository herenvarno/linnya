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

void ly_3dnc_download_cov();

const gchar* g_module_check_init(GModule *module)
{
	ly_msg_bind("download_cover_request", "", ly_3dnc_download_cov, NULL);
	return NULL;
}
void g_module_unload(GModule *module)
{
	ly_msg_unbind("download_cover_request", "", ly_3dnc_download_cov);
}

void ly_3dnc_download_cov()
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return;
	ly_3dnc_cov_lastfm_get(md->artist, md->album);
	ly_msg_put("meta_update", "plugin:dnc", "cover");
}
