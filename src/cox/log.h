
#ifndef LY_LOG_H
#define LY_LOG_H

#include "gla.h"
#include "log.obj.h"

G_BEGIN_DECLS

/*
 * FUNCTION
 */
void ly_log_init ();
void ly_log_fina ();

void ly_log_put (const char *format, ...);
void ly_log_put_with_flag (GLogLevelFlags flag, const char *format, ...);

G_END_DECLS

#endif	//LY_LOG_H
