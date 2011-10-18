#ifndef PYTHON_H
#define PYTHON_H

/*
 * HEADERS
 */
#include "core.h"
#include "ui.h"
#include <python3.2mu/Python.h>

/*
 * FUNCTIONS
 */
const gchar *g_module_check_init(GModule *module);
void g_module_unload(GModule *module);

GtkWidget *ly_plugin_python_config();

gboolean ly_plugin_python_init_pyplugin(char *plugin_name);
gboolean ly_plugin_python_finalize_pyplugin();
PyObject* ly_plugin_python_load_module(char *modulename);
void ly_plugin_python_exec(char *cmd);
PyObject* ly_plugin_python_func(PyObject *pModule, char *funcname, char *format, ...);

#endif