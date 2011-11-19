#ifndef LY_3INF_H
#define LY_3INF_H

/*
 * HEADERS
 */
#include "cox.h"
#include "uix.h"
#include "inf_cover.h"

/*
 * MACRO
 */


/*
 * VARIABLES
 */


/*
 * FUNCTIONS
 */
const gchar*	g_module_check_init		(GModule *module);
void			g_module_unload			(GModule *module);

GtkWidget*		ly_3inf_create			();
void			ly_3inf_destroy			();
GtkWidget*		ly_3inf_config			();

#endif
