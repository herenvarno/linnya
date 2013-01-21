#ifndef LY_3INF_DRAW_H
#define LY_3INF_DRAW_H

/*
 * HEADERS
 */
#include "../../cox/cox.h"
#include "../../uix/uix.h"

/*
 * FUNCTIONS
 */
GdkPixbuf* ly_3inf_draw_pic_blur(GdkPixbuf *pixbuf, gint top, gint left, gint height, gint width);

#endif