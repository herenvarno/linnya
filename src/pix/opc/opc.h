#ifndef LY_3OPC_H
#define LY_3OPC_H

/*
 * HEADERS
 */
#include "opc_left.h"
#include "opc_right.h"

/*
 * VARABLES
 */
GtkWidget *widget;
GtkWidget *hpaned;
GtkWidget *scrolled_window;

/*
 * FUNCTIONS
 */
const gchar*	g_module_check_init	(GModule *module);
void			g_module_unload		(GModule *module);

GtkWidget*		ly_3opc_create		();
void			ly_3opc_destroy		();
GtkWidget*		ly_3opc_config		();

#endif	//LY_3OPC_H
