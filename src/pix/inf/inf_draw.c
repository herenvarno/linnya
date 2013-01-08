/*
 * HEADERS
 */
#include "inf_draw.h"

/*
 * HEADERS
 */

static void
set_pixel (GdkPixbuf *pixbuf, int x, int y, guchar red, guchar green, guchar blue, guchar alpha)
{
	int width, height, rowstride, n_channels;
	guchar *pixels, *p;

	n_channels = gdk_pixbuf_get_n_channels (pixbuf);

	g_assert (gdk_pixbuf_get_colorspace (pixbuf) == GDK_COLORSPACE_RGB);
	g_assert (gdk_pixbuf_get_bits_per_sample (pixbuf) == 8);
	g_assert (gdk_pixbuf_get_has_alpha (pixbuf));
	g_assert (n_channels == 4);

	width = gdk_pixbuf_get_width (pixbuf);
	height = gdk_pixbuf_get_height (pixbuf);

	g_assert (x >= 0 && x < width);
	g_assert (y >= 0 && y < height);

	rowstride = gdk_pixbuf_get_rowstride (pixbuf);
	pixels = gdk_pixbuf_get_pixels (pixbuf);

	p = pixels + y * rowstride + x * n_channels;
	p[0] = red;
	p[1] = green;
	p[2] = blue;
	p[3] = alpha;
}
static guchar*
get_pixel (GdkPixbuf *pixbuf, int x, int y)
{
	int width, height, rowstride, n_channels;
	guchar *pixels, *p;

	n_channels = gdk_pixbuf_get_n_channels (pixbuf);

	g_assert (gdk_pixbuf_get_colorspace (pixbuf) == GDK_COLORSPACE_RGB);
	g_assert (gdk_pixbuf_get_bits_per_sample (pixbuf) == 8);
	g_assert (gdk_pixbuf_get_has_alpha (pixbuf));
	g_assert (n_channels == 4);

	width = gdk_pixbuf_get_width (pixbuf);
	height = gdk_pixbuf_get_height (pixbuf);

	g_assert (x >= 0 && x < width);
	g_assert (y >= 0 && y < height);

	rowstride = gdk_pixbuf_get_rowstride (pixbuf);
	pixels = gdk_pixbuf_get_pixels (pixbuf);

	p = pixels + y * rowstride + x * n_channels;
	return p;
}

GdkPixbuf* ly_3inf_draw_pic_blur(GdkPixbuf *pixbuf, gint x, gint y, gint h, gint w)
{
	width = gdk_pixbuf_get_width (pixbuf);
	height = gdk_pixbuf_get_height (pixbuf);

	int i,j;
	guchar *c00, *c01, *c02, *c10, *c11, *c12, *c20, *c21, *c22;
	guchar red, green, blue, alpha;

	for(j=y;j<j+h && j<height;j++)
		for(i=x;i<x+w && i<width; i++)
		{
			if(j==0 || i==0 || i==width-1 || j==height-1)
				continue;

			c00=get_pixel(pixbuf, i-1, j-1);
			c01=get_pixel(pixbuf, i-1, j);
			c02=get_pixel(pixbuf, i-1, j+1);
			c10=get_pixel(pixbuf, i, j-1);
			c12=get_pixel(pixbuf, i, j+1);
			c20=get_pixel(pixbuf, i+1, j-1);
			c21=get_pixel(pixbuf, i+1, j);
			c22=get_pixel(pixbuf, i+1, j+1);

			red=(c00[0]+c01[0]+c02[0]+c10[0]+c12[0]+c20[0]+c21[0]+c22[0])/8.0;
			green=(c00[1]+c01[1]+c02[1]+c10[1]+c12[1]+c20[1]+c21[1]+c22[1])/8.0;
			blue=(c00[2]+c01[2]+c02[2]+c10[2]+c12[2]+c20[2]+c21[2]+c22[2])/8.0;
			alpha=(c00[3]+c01[3]+c02[3]+c10[3]+c12[3]+c20[3]+c21[3]+c22[3])/8.0;
			set_pixel(pixbuf, i, j, red, green, blue, alpha);
		}
		return pixbuf;
}
