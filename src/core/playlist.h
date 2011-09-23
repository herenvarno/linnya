#ifndef LY_PLAYLIST_H
#define LY_PLAYLIST_H

/*
 * HEADERS
 */
#include "db.h"

/*
 * FUNCTIONS
 */
gboolean	ly_playlist_init				(void);
gboolean	ly_playlist_finalize			(void);

gboolean	ly_playlist_import_from_file	(char *path, char *name);
gboolean	ly_playlist_import_from_cue		(char *path, char *name);
gboolean	ly_playlist_import_from_m3u		(char *path, char *name);

gboolean	ly_playlist_test_is_exist		(char *name);
gboolean	ly_playlist_test_is_empty		(int pid);

gboolean	ly_playlist_add					(const char *name);
gboolean	ly_playlist_delete				(int pid);

gboolean	ly_playlist_get_metadatas		(int pid, char *where, int base, int length, gpointer func, gpointer data);
gboolean	ly_playlist_get_playlists		(char *where, int base, int length,  gpointer func, gpointer data);

#endif
