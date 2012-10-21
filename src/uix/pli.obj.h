#ifndef LY_PLI_OBJ_H
#define LY_PLI_OBJ_H

/*
 * HEADERS
 */
#include "glb.h"

G_BEGIN_DECLS

/*
 * MACROS
 */
#define LY_PLI_PLUGIN_TYPE \
	(ly_pli_plugin_get_type())
#define LY_PLI_PLUGIN(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), LY_PLI_PLUGIN_TYPE, LyPliPlugin))
#define LY_PLI_PLUGIN_CLASS(klass) \
	(G_TYPE_CHECK_INSTANCE_CAST((klass), LY_PLI_PLUGIN_CLASS_TYPE, LyPliPlugin))
#define LY_PLI_PLUGIN_IS_PLUGIN(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj), LY_PLI_PLUGIN_TYPE))
#define LY_PLI_PLUGIN_IS_PLUGIN_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass), LY_PLI_PLUGIN_CLASS_TYPE))
#define LY_PLI_PLUGIN_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS(obj), LY_PLI_PLUGIN_TYPE, LyPliPluginClass)

/*
 * TYPES
 */
typedef struct _LyPliPlugin LyPliPlugin;
typedef struct _LyPliPluginClass LyPliPluginClass;
struct _LyPliPlugin{
	/*<private>*/
	GObject parent;
};
struct _LyPliPluginClass{
	/*<private>*/
	GObjectClass parent_class;
};

/*
 * VARIABLES
 */

/*
 * FUNCTIONS
 */
GType ly_pli_plugin_get_type();
LyPliPlugin* ly_pli_plugin_new(gchar *name);

gboolean ly_pli_plugin_load_module(LyPliPlugin *obj);
gboolean ly_pli_plugin_unload_module(LyPliPlugin *obj);
GtkWidget* ly_pli_plugin_create(LyPliPlugin *obj);
gboolean ly_pli_plugin_destroy(LyPliPlugin *obj);
gboolean ly_pli_plugin_refresh(LyPliPlugin *obj);
GtkWidget* ly_pli_plugin_config(LyPliPlugin *obj);
gboolean ly_pli_plugin_lock(LyPliPlugin *obj);
gboolean ly_pli_plugin_unlock(LyPliPlugin *obj);

G_END_DECLS

#endif //LY_PLI_OBJ_H
