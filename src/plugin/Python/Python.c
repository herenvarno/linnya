#include "Python.h"

const gchar* g_module_check_init(GModule *module)
{
	Py_Initialize();
	if (!Py_IsInitialized())  
	{
		printf("Error init python!\n");
		return NULL;
	}
	return NULL;
	
}
void g_module_unload(GModule *module)
{
	Py_Finalize();
}

GtkWidget *ly_plugin_python_config()
{
	return NULL;
}


gboolean ly_plugin_python_init_pyplugin(char *plugin_name)
{
	/*
	 * APPEND MODULE PATH
	 */
	PyRun_SimpleString("import sys");
	char cmd[1024]="";
	snprintf(cmd, sizeof(cmd), "sys.path.append('%splugin/%s')", LY_GLOBAL_PROGDIR, plugin_name);
	PyRun_SimpleString(cmd);
	return TRUE;
}
gboolean ly_plugin_python_finalize_pyplugin()
{
	return TRUE;
}

PyObject* ly_plugin_python_load_module(char *modulename)
{
	/*
	 * IMPORT MODULE
	 */
	PyObject *pModule=NULL;
	if(modulename==NULL|| g_str_equal(modulename, ""))
	{
		ly_msg_put("error", "plugin:python", _("Cannot load module!"));
		return NULL;
	}
	
	pModule = PyImport_ImportModule(modulename);
	return pModule;
}
void ly_plugin_python_unload_module(char *module)
{
	;
}
void ly_plugin_python_exec(char *cmd)
{
	PyRun_SimpleString(cmd);
}
PyObject* ly_plugin_python_func(PyObject *pModule, char *funcname, char *format, ...)
{
	PyObject *pFunc=NULL;
	PyObject *pArg=NULL;
	PyObject *pRetVal=NULL;
	
	pFunc = PyObject_GetAttrString(pModule, funcname);
	va_list argp;
	va_start(argp, format);
	pArg=Py_VaBuildValue(format,argp);
	va_end(argp);
	
	pRetVal = PyObject_CallObject(pFunc,pArg);
	return pRetVal;
}


