#include "gl.h"

gboolean mm_gl_init()
{
	mm_gl_mainloop=NULL;
	mm_gl_mainloop = g_main_loop_new(NULL, FALSE);
	if(mm_gl_mainloop==NULL)
	{
		exit(0);
		return FALSE;
	}
	return TRUE;
}