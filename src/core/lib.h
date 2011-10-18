#ifndef LY_LIB_H
#define LY_LIB_H

/*
 * HEADERS
 */
#include "db.h"

/*
 * FUNCTIONS
 */
gboolean ly_lib_add(lyDbMetadata *md);

gboolean ly_lib_check_is_exist(lyDbMetadata *md);
gboolean ly_lib_check_is_exist_list(GList **l);

#endif
