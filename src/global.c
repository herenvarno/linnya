#include "global.h"

gboolean ly_global_init()
{
	/*
	 * building main loop
	 */
	ly_global_mainloop=NULL;
	ly_global_mainloop = g_main_loop_new(NULL, FALSE);
	if(ly_global_mainloop==NULL)
	{
		exit(0);
		return FALSE;
	}
	
	/*
	 * creating global path
	 */
	LY_GLOBAL_HOMEDIR=NULL;
	LY_GLOBAL_PROGDIR=NULL;
	LY_GLOBAL_USERDIR=NULL;
	
	LY_GLOBAL_HOMEDIR = g_getenv("HOME");
	if(LY_GLOBAL_HOMEDIR == NULL)
		LY_GLOBAL_HOMEDIR = g_get_home_dir();
	if(LY_GLOBAL_HOMEDIR == NULL)
	{
		exit(0);
		return FALSE;
	}
	LY_GLOBAL_PROGDIR=g_strconcat(INSTALL_PATH,NULL);
	LY_GLOBAL_USERDIR=g_strconcat(LY_GLOBAL_HOMEDIR,"/.linnya",NULL);
	LY_GLOBAL_TEMPDIR=g_strconcat("/tmp/linnya",NULL);
	
	mkdir(LY_GLOBAL_USERDIR, 0755);
	mkdir(LY_GLOBAL_TEMPDIR, 0755);
	
	/*
	 * localizition
	 */
	bindtextdomain(LY_GLOBAL_PACKAGE,LY_GLOBAL_LOCALEDIR);
	textdomain(LY_GLOBAL_PACKAGE);
	
	/*
	 * initial gstreamer
	 */
	gst_init (&argc, &argv);
	
	return TRUE;
}

gboolean ly_global_run()
{
	g_main_loop_run(ly_global_mainloop);
	return TRUE;
}

gboolean ly_global_finalize()
{
	g_main_loop_unref(ly_global_mainloop);
	g_free(LY_GLOBAL_HOMEDIR);
	g_free(LY_GLOBAL_PROGDIR);
	g_free(LY_GLOBAL_USERDIR);
	g_free(LY_GLOBAL_TEMPDIR);
	return TRUE;
}