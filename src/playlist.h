

//HEADERS
#include "global.h"
#include "debug.h"
#include "common.h"
#include "configure.h"
#include "metadata.h"
#include "library.h"

//FUNCTIONS
gboolean	pl_read_playlist();
gboolean	pl_write_playlist();
gboolean	pl_read_playinglist();
gboolean	pl_write_playinglist();

gboolean	pl_add_playlist(gchar *name);
gboolean	pl_add_to_playlist(GList *p, _gl_metadata *md);
gboolean	pl_add_to_playinglist(_gl_playinglist *pgl);
gboolean	pl_import_playlist(gchar *path);
gboolean	pl_export_playlist(GList *p, gchar *path);

gboolean	pl_remove_playlist(GList *p);
gboolean	pl_rename_playlist(GList *p, gchar *name);
gboolean	pl_remove_md_from_playlist(GList *p, GList *q);
gboolean	pl_remove_md_from_playinglist(GList *q);

GList*		pl_find_pl_by_playing();
