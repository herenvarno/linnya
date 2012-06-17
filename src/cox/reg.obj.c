/*
 * HEADERS
 */
#include "reg.obj.h"


/*
 * MACROS
 */
#define LY_REG_TABLE_GET_PRIVATE(obj) \
	G_TYPE_INSTANCE_GET_PRIVATE((obj), LY_REG_TABLE_TYPE, LyRegTablePrivate)


/*
 * TYPES
 */
typedef struct _LyRegTablePrivate LyRegTablePrivate;

struct _LyRegTablePrivate {
    GHashTable *tab;
	gchar *path;
};


/*
 * FUNCTIONS
 */
static void ly_reg_table_init(LyRegTable *obj);
static void ly_reg_table_finalize(GObject *obj);
static void ly_reg_table_class_init(LyRegTableClass *klass);
void ly_reg_table_load_start_cb (GMarkupParseContext *context, \
	const gchar *element_name, const gchar **attribute_names, \
	const gchar **attribute_values, gpointer data, GError **error);

GType ly_reg_table_get_type()
{
	static GType ly_reg_table_type = 0;
	if(!ly_reg_table_type)
	{
		static const GTypeInfo ly_reg_table_info = {
			sizeof(LyRegTableClass),
			NULL,
			NULL,
			(GClassInitFunc)ly_reg_table_class_init,
			NULL,
			NULL,
			sizeof(LyRegTable),
			0,
			(GInstanceInitFunc)ly_reg_table_init,
			NULL
		};
		ly_reg_table_type = g_type_register_static(G_TYPE_OBJECT, "LyRegTable", &ly_reg_table_info, 0);
	}
	return ly_reg_table_type;
}

static void ly_reg_table_init(LyRegTable *obj)
{

}

static void ly_reg_table_finalize(GObject *obj)
{
	LyRegTablePrivate *priv=LY_REG_TABLE_GET_PRIVATE(LY_REG_TABLE(obj));
	ly_reg_table_save(LY_REG_TABLE(obj));
	g_hash_table_destroy(priv->tab);
}

static void ly_reg_table_class_init(LyRegTableClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = ly_reg_table_finalize;
	g_type_class_add_private(klass, sizeof(LyRegTablePrivate));
}

LyRegTable*	ly_reg_table_new			(gchar *path)
{
	g_return_val_if_fail(path!=NULL, NULL);

	LyRegTable *table;
	table = g_object_new(LY_REG_TABLE_TYPE, NULL);
	LyRegTablePrivate *priv=LY_REG_TABLE_GET_PRIVATE(LY_REG_TABLE(table));
	priv->tab=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	priv->path=g_strdup(path);

	if(g_file_test(path, G_FILE_TEST_EXISTS))
	{
		ly_reg_table_load(LY_REG_TABLE(table));
	}

	return table;
}
gchar *ly_reg_table_get(LyRegTable *table, char *key)
{
	g_return_val_if_fail((table!=NULL && key!=NULL), NULL);
	LyRegTablePrivate *priv=LY_REG_TABLE_GET_PRIVATE(LY_REG_TABLE(table));
	g_return_val_if_fail((priv->tab!=NULL), NULL);

	char *val=NULL;
	val=g_hash_table_lookup(priv->tab, key);

	return val;
}
void ly_reg_table_set (LyRegTable *table, gchar *key, gchar *val)
{
	g_return_if_fail((table!=NULL && key!=NULL && val!=NULL));
	LyRegTablePrivate *priv=LY_REG_TABLE_GET_PRIVATE(LY_REG_TABLE(table));
	g_return_if_fail((priv->tab!=NULL));

	g_hash_table_replace(priv->tab, g_strdup(key), g_strdup(val));
}
void ly_reg_table_clear (LyRegTable *table)
{
	g_return_if_fail((table!=NULL));
	LyRegTablePrivate *priv=LY_REG_TABLE_GET_PRIVATE(LY_REG_TABLE(table));
	g_return_if_fail((priv->tab!=NULL));

	g_hash_table_destroy(priv->tab);
	priv->tab=g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
}
void ly_reg_table_save (LyRegTable *table)
{
	g_return_if_fail((table!=NULL));
	LyRegTablePrivate *priv=LY_REG_TABLE_GET_PRIVATE(LY_REG_TABLE(table));
	g_return_if_fail((priv->tab!=NULL && priv->path!=NULL));

	GFileOutputStream *ostream;
	GFile *file;

	file=g_file_new_for_path(priv->path);
	if(!file)
	{
		g_warning(_("Cannot save reg file!"));
		return;
	}

	gchar *str=NULL;
	gchar *tmp=NULL;
	gchar *buf=NULL;

	buf=g_markup_printf_escaped ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<configurations>\n");
	str=g_strconcat(buf, NULL);
	g_free(buf);

	GHashTableIter iter;
	gpointer key, val;
	g_hash_table_iter_init (&iter, priv->tab);
	while (g_hash_table_iter_next (&iter, &key, &val))
	{
		buf=g_markup_printf_escaped ("\t<conf key=\"%s\" val=\"%s\" />\n", (gchar*)key, (gchar*)val);
		tmp=str;
		str=g_strconcat(tmp, buf, NULL);
		g_free(tmp);
		g_free(buf);
	}
	buf=g_markup_printf_escaped ("</configurations>");
	tmp=str;
	str=g_strconcat(tmp, buf, NULL);
	g_free(buf);
	g_free(tmp);

	ostream=g_file_replace(G_FILE(file), NULL, TRUE, \
		G_FILE_CREATE_NONE, NULL, NULL);
	g_output_stream_write(G_OUTPUT_STREAM(ostream), str, strlen(str), \
		NULL, NULL);
	g_output_stream_close(G_OUTPUT_STREAM(ostream), NULL, NULL);
	g_free(str);
	g_object_unref(ostream);
	g_object_unref(file);
}
void ly_reg_table_load (LyRegTable *table)
{
	g_return_if_fail((table!=NULL));
	LyRegTablePrivate *priv=LY_REG_TABLE_GET_PRIVATE(LY_REG_TABLE(table));
	g_return_if_fail((priv->tab!=NULL && priv->path!=NULL));

	if(!g_file_test(priv->path, G_FILE_TEST_EXISTS))
		return;

	GMarkupParser parser=
	{
		.start_element = ly_reg_table_load_start_cb,
		.end_element = NULL,
		.text = NULL,
		.passthrough = NULL,
		.error = NULL
	};

	gchar *buf;
	gsize length;
	GMarkupParseContext *context;

	g_file_get_contents(priv->path, &buf, &length, NULL);
	context = g_markup_parse_context_new(&parser, 0, (gpointer)table, NULL);

	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		ly_reg_table_clear(LY_REG_TABLE(table));
		g_warning(_("Error occur while loading reg!"));
	}
	g_markup_parse_context_free(context);
}

void ly_reg_table_load_start_cb	(	GMarkupParseContext *context,
								const gchar *element_name,
								const gchar **attribute_names,
								const gchar **attribute_values,
								gpointer data,
								GError **error)
{
	if(g_str_equal(element_name,"conf"))
	{
		gchar *key=NULL;
		gchar *val=NULL;

		const gchar **name_cursor = attribute_names;
		const gchar **value_cursor = attribute_values;

		while (*name_cursor)
		{
			if (g_str_equal(*name_cursor, "key"))
				key = g_strdup (*value_cursor);
			else if (g_str_equal(*name_cursor, "val"))
				val = g_strdup (*value_cursor);
			name_cursor++;
			value_cursor++;
		}

		if(!key || !val)
		{
			g_free(key);
			g_free(val);
			return;
		}
		ly_reg_table_set(LY_REG_TABLE(data), key, val);
	}
}