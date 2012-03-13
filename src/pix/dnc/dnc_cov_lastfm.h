#ifndef LY_3DNC_COV_LASTFM_H
#define LY_3DNC_COV_LASTFM_H

#include "cox.h"
#include "uix.h"
#include <curl/curl.h>

gchar*		ly_3dnc_cov_lastfm_search(gchar *artist, gchar *album);
gboolean	ly_3dnc_cov_lastfm_save(gchar *url, gchar *path);
gboolean	ly_3dnc_cov_lastfm_get(gchar *artist, gchar *album);

#endif
