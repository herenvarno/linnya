

#ifndef	METADATA_H
#define	METADATA_H

//HEADERS
#include "global.h"
#include "debug.h"
#include "common.h"
#include "configure.h"

//FUNCTIONS
_gl_metadata*	md_get_metadata(gchar *uri);
gchar**			md_get_mp3_tag(gchar *filename);
#endif
