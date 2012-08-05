#ifndef LY_REG_OBJ_H
#define LY_REG_OBJ_H

/*
 * HEADERS
 */
#include "gla.h"

G_BEGIN_DECLS

/*
 * MACROS
 */
#define LY_REG_TABLE_TYPE \
	(ly_reg_table_get_type())
#define LY_REG_TABLE(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), LY_REG_TABLE_TYPE, LyRegTable))
#define LY_REG_TABLE_CLASS(klass) \
	(G_TYPE_CHECK_INSTANCE_CAST((klass), LY_REG_TABLE_CLASS_TYPE, LyRegTableClass))
#define LY_REG_TABLE_IS_TABLE(obj) \
	(G_TYPE_CHECK_VALUE_TYPE ((obj), LY_REG_TABLE_TYPE))
#define LY_REG_TABLE_IS_TABLE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), LY_REG_TABLE_CLASS_TYPE))
#define LY_REG_TABLE_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), LY_REG_TABLE_TYPE, LyRegTableClass))

/*
 * TYPES
 */
typedef struct _LyRegTable LyRegTable;
typedef struct _LyRegTableClass LyRegTableClass;

struct _LyRegTable {
	/*<private>*/
	GObject parent;
};
struct _LyRegTableClass {
	/*<private>*/
	GObjectClass parent_class;
};

/*
 * FUNCTIONS
 */
GType ndl_reg_table_get_type ();
LyRegTable* ly_reg_table_new (gchar *path);
gchar *ly_reg_table_get(LyRegTable *table, char *key);
void ly_reg_table_set (LyRegTable *table, gchar *key, gchar *val);
void ly_reg_table_clear (LyRegTable *table);
void ly_reg_table_save (LyRegTable *table);
void ly_reg_table_load (LyRegTable *table);

G_END_DECLS

#endif	// LY_REG_OBJ_H