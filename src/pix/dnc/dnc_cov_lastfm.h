#ifndef LY_3DNC_COV_LASTFM_H
#define LY_3DNC_COV_LASTFM_H

/*
 * HEADERS
 */
#include "cox.h"
#include "uix.h"
#include <curl/curl.h>

/*
 * FUNCTIONS
 */
GtkTreeStore*	ly_3dnc_cov_lastfm_search(gchar *artist, gchar *album);
gchar*			ly_3dnc_cov_lastfm_analysis(gchar *url);

#endif
