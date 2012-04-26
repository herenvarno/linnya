#ifndef LY_3DNC_LRC_QIANQIAN_H
#define LY_3DNC_LRC_QIANQIAN_H

/*
 * HEADERS
 */
#include "cox.h"
#include "uix.h"
#include <curl/curl.h>

/*
 * FUNCTIONS
 */
GtkTreeStore*	ly_3dnc_lrc_qianqian_search(gchar *title, gchar *artist);
gchar*			ly_3dnc_lrc_qianqian_analysis(gchar *url);

#endif
