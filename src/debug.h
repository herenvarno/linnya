


#ifndef DEBUG_H
#define	DEBUG_H

//HEADER FILES
#include "global.h"

//MACROS

#define __DE_ERROR			0
#define __DE_WARNNING		1
#define __DE_INFORMATION	2


//FUNCTIONS
void	de_print		(int type, const char *func, const char *msg);
void	de_dialog_print	(GtkWidget *parent, int type, const char *func, const char *msg);

#endif
