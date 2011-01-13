

#ifndef COMMON_H
#define	COMMON_H

//HEADER FILES
#include "global.h"
#include "debug.h"

//VARIABLES
typedef enum _com_filetype
{
	com_type_others,
	com_type_music,
	com_type_cue,
	com_type_m3u,
	com_type_lrc
}_com_filetype;

typedef enum _com_cursorshape
{
	com_cursor_default,
	com_cursor_drag,
	com_cursor_move
}_com_cursorshape;


//FUNCTIONS
gboolean		com_init (int argc, char *argv[]);
_com_filetype	com_get_filetype (gchar *filename);
GList*			com_get_file_list (gchar *path, gint depth, gboolean showhide, gboolean recursive);
GList*			com_get_uri_list (gchar *path, gint depth, gboolean showhide, gboolean recursive);
GList*			com_get_dir_list (gchar *path, gboolean showhide);
gboolean		com_set_cursor (GtkWidget *widget, _com_cursorshape cs);
gchar*			com_get_filename(gchar *path);
gchar*			com_get_file(gchar *path);
#endif
