/*
 * HEADERS
 */
#include "dnc.h"

/*
 * VARIABLES
 */
static GMutex *mutex_cov=NULL;
static GMutex *mutex_lrc=NULL;

/*
 * FUNCTIONS
 */

void ly_3dnc_download_cov		();
void ly_3dnc_run_thread_cov		(gpointer data);

const gchar* g_module_check_init(GModule *module)
{
	curl_global_init(0);
	mutex_cov = g_mutex_new();
	mutex_lrc = g_mutex_new();
	
	
	ly_msg_bind("download_cover_request", "", ly_3dnc_download_cov, NULL);
	return NULL;
}
void g_module_unload(GModule *module)
{
	ly_msg_unbind("download_cover_request", "", ly_3dnc_download_cov);
}

void ly_3dnc_download_cov()
{
	g_thread_create(ly_3dnc_run_thread_cov, NULL, TRUE, NULL);
}

void ly_3dnc_run_thread_cov(gpointer data)
{
	LyMdhMetadata *md=NULL;
	int i;
	for(i=0; i<3; i++)
	{
		if(g_mutex_trylock(mutex_cov) == FALSE)
		{
			md=ly_pqm_get_current_md();
			if(!md)
				return;
			ly_3dnc_cov_lastfm_get(md->artist, md->album);
			g_mutex_unlock(mutex_cov);
		}
		else
		{
			g_usleep(5);
		}
	}
	ly_msg_put("meta_update", "plugin:dnc", "cover");
	puts("DONE");
}
