#include "global.h"

gboolean ly_ui_global_init()
{
	/*
	 * mkdir
	 */
	gchar *path=(gchar*)g_malloc(128*sizeof(gchar));
	g_snprintf(path,sizeof(path),"%sui/default/",LY_GLOBAL_USERDIR);
	mkdir(path, 0755);
	g_snprintf(path,sizeof(path),"%sui/default/themes",LY_GLOBAL_USERDIR);
	mkdir(path, 0755);
	g_free(path);
	
	/*
	 * initial gtk
	 */
	gtk_init (ly_global_argc, ly_global_argv);

	return TRUE;
}

gboolean ly_ui_global_finalize()
{
	return TRUE;
} 
