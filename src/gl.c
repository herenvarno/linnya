#include "gl.h"

gboolean ly_gl_init()
{
	ly_gl_mainloop=NULL;
	ly_gl_mainloop = g_main_loop_new(NULL, FALSE);
	if(ly_gl_mainloop==NULL)
	{
		exit(0);
		return FALSE;
	}
	return TRUE;
}
