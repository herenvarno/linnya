/*
 * HEADERS
 */
#include "pli.obj.h"

/*
 * MACROS
 */
#define LY_PLI_PLUGIN_GET_PRIVATE(obj) \
	G_TYPE_INSTANCE_GET_PRIVATE((obj), LY_PLI_PLUGIN_TYPE, LyPliPluginPrivate)

/*
 * TYPES
 */
typedef enum _LyPliPluginProperty LyPliPluginProperty;
enum _LyPliPluginProperty{
	PROPERTY_0,
	PROPERTY_MODULE,
	PROPERTY_WIDGET,
	PROPERTY_DAEMON,
	PROPERTY_NAME,
	PROPERTY_ALIAS,
	PROPERTY_VERSION,
	PROPERTY_AUTHOR,
	PROPERTY_LICENSE,
	PROPERTY_LOGO,
	PROPERTY_COMMENT,
	PROPERTY_WEBSITE,
	PROPERTY_DEPENDS,
	PROPERTY_CREATE_SYMBOL,
	PROPERTY_DESTROY_SYMBOL,
	PROPERTY_REFRESH_SYMBOL,
	PROPERTY_CONFIG_SYMBOL,
	PROPERTY_LOCKED
};

typedef struct _LyPliPluginPrivate LyPliPluginPrivate;
struct _LyPliPluginPrivate{
	GModule *module;
	GtkWidget *widget;
	gboolean daemon;
	gchar *name;
	gchar *alias;
	gchar *version;
	gchar *author;
	gchar *license;
	gchar *logo;
	gchar *comment;
	gchar *website;
	gchar *depends;
	gchar *create_symbol;
	gchar *destroy_symbol;
	gchar *refresh_symbol;
	gchar *config_symbol;
	gboolean locked;
};

/*
 * VARIABLES
 */
gchar *ly_pli_plugin_element_name;

/*
 * FUNCTIONS
 */
static void ly_pli_plugin_init(LyPliPlugin *obj);
static void ly_pli_plugin_fina(GObject *obj);
static void ly_pli_plugin_set_property(GObject *obj, guint property_id, \
		const GValue *value, GParamSpec *pspec);
static void ly_pli_plugin_get_property(GObject *obj, guint property_id, \
		GValue *value, GParamSpec *pspec);
static void ly_pli_plugin_class_init(LyPliPluginClass *klass);

void ly_pli_plugin_new_start_cb(GMarkupParseContext *context, const gchar *element_name, \
const gchar **attribute_names, const gchar **attribute_values, \
gpointer data, GError **error);
void ly_pli_plugin_new_text_cb(GMarkupParseContext * context, const gchar *text, \
gsize text_len, gpointer data, GError **error);
void ly_pli_plugin_new_end_cb(GMarkupParseContext *context, const gchar *element_name, \
gpointer data, GError **error);

GType
ly_pli_plugin_get_type()
{
	static GType ly_pli_plugin_type = 0;
	if(!ly_pli_plugin_type)
	{
		static const GTypeInfo ly_pli_plugin_info={
			sizeof(LyPliPluginClass),
			NULL,
			NULL,
			(GClassInitFunc)ly_pli_plugin_class_init,
			NULL,
			NULL,
			sizeof(LyPliPlugin),
			0,
			(GInstanceInitFunc)ly_pli_plugin_init,
			NULL
		};
		ly_pli_plugin_type=g_type_register_static(G_TYPE_OBJECT, "LyPliPlugin",
				&ly_pli_plugin_info, 0);
	}
	return ly_pli_plugin_type;
}

static void
ly_pli_plugin_init(LyPliPlugin *obj)
{

}

static void
ly_pli_plugin_fina(GObject *obj)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	if(!priv)
		return;
	g_free(priv->name);
	g_free(priv->alias);
	g_free(priv->version);
	g_free(priv->author);
	g_free(priv->license);
	g_free(priv->logo);
	g_free(priv->comment);
	g_free(priv->website);
	g_free(priv->create_symbol);
	g_free(priv->destroy_symbol);
	g_free(priv->refresh_symbol);
	g_free(priv->config_symbol);
	g_free(priv->depends);
	if(priv->module)
		g_module_close(priv->module);
}

static void
ly_pli_plugin_set_property(GObject *obj, guint property_id, \
	const GValue *value, GParamSpec *pspec)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	switch (property_id)
	{
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
			break;
	}
}

static void
ly_pli_plugin_get_property (GObject *obj, guint property_id, \
	GValue *value, GParamSpec *pspec)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	switch (property_id)
	{
		case PROPERTY_MODULE:
			g_value_set_pointer(value, priv->module);
			break;
		case PROPERTY_WIDGET:
			g_value_set_pointer(value, priv->widget);
			break;
		case PROPERTY_DAEMON:
			g_value_set_boolean(value, priv->daemon);
			break;
		case PROPERTY_NAME:
			g_value_set_string(value, priv->name);
			break;
		case PROPERTY_ALIAS:
			g_value_set_string(value, priv->alias);
			break;
		case PROPERTY_VERSION:
			g_value_set_string(value, priv->version);
			break;
		case PROPERTY_AUTHOR:
			g_value_set_string(value, priv->author);
			break;
		case PROPERTY_LICENSE:
			g_value_set_string(value, priv->license);
			break;
		case PROPERTY_LOGO:
			g_value_set_string(value, priv->logo);
			break;
		case PROPERTY_COMMENT:
			g_value_set_string(value, priv->comment);
			break;
		case PROPERTY_WEBSITE:
			g_value_set_string(value, priv->website);
			break;
		case PROPERTY_DEPENDS:
			g_value_set_string(value, priv->depends);
			break;
		case PROPERTY_CREATE_SYMBOL:
			g_value_set_string(value, priv->create_symbol);
			break;
		case PROPERTY_DESTROY_SYMBOL:
			g_value_set_string(value, priv->destroy_symbol);
			break;
		case PROPERTY_REFRESH_SYMBOL:
			g_value_set_string(value, priv->refresh_symbol);
			break;
		case PROPERTY_CONFIG_SYMBOL:
			g_value_set_string(value, priv->config_symbol);
			break;
		case PROPERTY_LOCKED:
			g_value_set_boolean(value, priv->locked);
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
			break;
	}
}


static void
ly_pli_plugin_class_init(LyPliPluginClass *klass)
{
	GObjectClass *object_class=G_OBJECT_CLASS(klass);
	object_class->finalize=ly_pli_plugin_fina;
	object_class->set_property=ly_pli_plugin_set_property;
	object_class->get_property=ly_pli_plugin_get_property;
	g_type_class_add_private(klass, sizeof(LyPliPluginPrivate));

	// REGIST PROPERTIES
	GParamSpec *pspec;
	pspec=g_param_spec_pointer(
		"module",
		"Plugin module",
		"The module of a plugin",
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_MODULE, pspec);
	pspec=g_param_spec_pointer(
		"widget",
		"Plugin widget in session",
		"The widget of a non-daemon plugin",
		G_PARAM_READABLE|G_PARAM_WRITABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_WIDGET, pspec);
	pspec=g_param_spec_boolean(
		"daemon",
		"Daemon mode",
		"The property to decide the plugin is visible or not",
		FALSE,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_DAEMON, pspec);
	pspec=g_param_spec_string(
		"name",
		"Plugin name",
		"The name of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_NAME, pspec);
	pspec=g_param_spec_string(
		"alias",
		"Plugin alias",
		"The alias name of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_ALIAS, pspec);
	pspec=g_param_spec_string(
		"version",
		"Plugin version",
		"The version of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_VERSION, pspec);
	pspec=g_param_spec_string(
		"author",
		"Plugin Author",
		"The author of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_AUTHOR, pspec);
	pspec=g_param_spec_string(
		"license",
		"Plugin Lincense",
		"The license of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_LICENSE, pspec);
	pspec=g_param_spec_string(
		"logo",
		"Plugin Logo",
		"The logo of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_LOGO, pspec);
	pspec=g_param_spec_string(
		"comment",
		"Plugin Comment",
		"The comment of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_COMMENT, pspec);
	pspec=g_param_spec_string(
		"website",
		"Plugin Website",
		"The website of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_WEBSITE, pspec);
	pspec=g_param_spec_string(
		"depends",
		"Plugin Depends",
		"The depends of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_DEPENDS, pspec);
	pspec=g_param_spec_string(
		"create_symbol",
		"Plugin Create Symbol",
		"The create function name of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_CREATE_SYMBOL, pspec);
	pspec=g_param_spec_string(
		"destroy_symbol",
		"Plugin Destroy Symbol",
		"The destroy function name of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_DESTROY_SYMBOL, pspec);
	pspec=g_param_spec_string(
		"refresh_symbol",
		"Plugin Refresh Symbol",
		"The refresh function name of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_REFRESH_SYMBOL, pspec);
	pspec=g_param_spec_string(
		"config_symbol",
		"Plugin Config Symbol",
		"The config function name of plugin",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_CONFIG_SYMBOL, pspec);
	pspec=g_param_spec_boolean(
		"locked",
		"Is Locked?",
		"The property to decide the plugin is locked or not",
		FALSE,
		G_PARAM_READABLE|G_PARAM_WRITABLE);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROPERTY_LOCKED, pspec);
}

LyPliPlugin* ly_pli_plugin_new(gchar *name)
{
	// CHECK
	if(!name)
		return NULL;

	const gchar *dir=LY_GLB_USER_PIXDIR;
	gchar path[1024];
	g_snprintf(path, sizeof(path),"%s%s/plugin.xml", dir, name);
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		dir=LY_GLB_PROG_PIXDIR;
		g_snprintf(path, sizeof(path),"%s%s/plugin.xml", dir, name);
		if(!g_file_test(path, G_FILE_TEST_EXISTS))
		{
			return NULL;
		}
	}

	// SET DEFAULT VALUES OF VARIABLES
	LyPliPlugin *obj;
	obj = g_object_new(LY_PLI_PLUGIN_TYPE, NULL);
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	priv->name=g_strdup(name);

	// START PHASE THE XML FILE
	GMarkupParser parser=
	{
		.start_element = ly_pli_plugin_new_start_cb,
		.end_element = ly_pli_plugin_new_end_cb,
		.text = ly_pli_plugin_new_text_cb,
		.passthrough = NULL,
		.error = NULL
	};
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	g_file_get_contents(path, &buf, &length, NULL);
	context = g_markup_parse_context_new(&parser, 0, priv, NULL);

	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		g_warning(_("Read configuration file error."));
		g_object_unref(obj);
		g_markup_parse_context_free(context);
		return NULL;
	}
	g_markup_parse_context_free(context);

	// MORE ADJUSTMENT
	if(priv->logo && !g_str_equal(priv->logo,""))
	{
		g_snprintf(path, sizeof(path),"%s%s/%s", dir, name, priv->logo);
		priv->logo=g_strdup(path);
	}
	if(!priv->alias || g_str_equal(priv->alias,""))
	{
		priv->alias=g_strdup(priv->name);
	}
	gchar path_lock[1024]="";
	g_snprintf(path_lock, sizeof(path_lock),"%s%s.lock", LY_GLB_USER_PIXDIR, priv->name);
	if(g_file_test(path_lock, G_FILE_TEST_EXISTS))
	{
		priv->locked=TRUE;
	}
	return obj;
}

gboolean ly_pli_plugin_load_module(LyPliPlugin *obj)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	if(!priv)
		return FALSE;

	if(priv->module)
		ly_pli_plugin_unload_module(obj);

	gchar path[1024]="";
	if(!(priv->locked))
	{
		g_snprintf(path, sizeof(path),"%s%s/lib%s.so", LY_GLB_USER_PIXDIR, priv->name, priv->name);
		if(g_file_test(path, G_FILE_TEST_EXISTS))
		{
			priv->module=g_module_open(path, G_MODULE_BIND_LAZY);
			return TRUE;
		}
		g_snprintf(path, sizeof(path),"%s%s/lib%s.so", LY_GLB_PROG_PIXDIR, priv->name, priv->name);
		if(g_file_test(path, G_FILE_TEST_EXISTS))
		{
			priv->module=g_module_open(path, G_MODULE_BIND_LAZY);
			return TRUE;
		}
	}
	return FALSE;
}
gboolean ly_pli_plugin_unload_module(LyPliPlugin *obj)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	if(!priv)
		return FALSE;

	if(!(priv->module))
		return TRUE;

	g_module_close(priv->module);
	priv->module=NULL;
	return TRUE;
}
GtkWidget* ly_pli_plugin_create(LyPliPlugin *obj)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	if(!priv)
		return NULL;

	if(!priv->daemon && !priv->module)
	{
		ly_pli_plugin_load_module(obj);
	}

	GtkWidget *(*f)(void);
	g_module_symbol(priv->module, priv->create_symbol, (gpointer)&f);
	if(f==NULL)
		return NULL;

	GtkWidget *widget=f();
	priv->widget=widget;
	return widget;
}
gboolean ly_pli_plugin_destroy(LyPliPlugin *obj)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	if(!priv)
		return FALSE;

	if(!priv->daemon && !priv->module)
	{
		ly_pli_plugin_load_module(obj);
	}

	void (*f)(void);
	g_module_symbol(priv->module, priv->destroy_symbol, (gpointer)&f);
	if(f!=NULL)
		f();
	priv->widget=NULL;
	return TRUE;
}
gboolean ly_pli_plugin_refresh(LyPliPlugin *obj)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	if(!priv)
		return FALSE;

	if(!priv->daemon && !priv->module)
	{
		ly_pli_plugin_load_module(obj);
	}

	void (*f)(void);
	g_module_symbol(priv->module, priv->refresh_symbol, (gpointer)&f);
	if(f!=NULL)
		f();
	return TRUE;
}
GtkWidget* ly_pli_plugin_config(LyPliPlugin *obj)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	if(!priv)
		return NULL;

	if(!priv->module)
	{
		ly_pli_plugin_load_module(obj);
	}

	GtkWidget *(*f)(void);
	g_module_symbol(priv->module, priv->config_symbol, (gpointer)&f);
	if(f==NULL)
		return NULL;
	return f();
}

gboolean ly_pli_plugin_lock(LyPliPlugin *obj)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	if(!priv)
		return FALSE;

	ly_pli_plugin_unload_module(obj);

	priv->locked=TRUE;
	gchar path[1024]="";
	g_snprintf(path, sizeof(path),"%s%s.lock", LY_GLB_USER_PIXDIR, priv->name);
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		g_file_set_contents(path, "== THIS IS A LOCK FILE FOR PLUGINS, DO NOT DELETE IT ==", -1, NULL);
	}
	return TRUE;
}

gboolean ly_pli_plugin_unlock(LyPliPlugin *obj)
{
	LyPliPluginPrivate *priv=LY_PLI_PLUGIN_GET_PRIVATE(LY_PLI_PLUGIN(obj));
	if(!priv)
		return FALSE;

	priv->locked=FALSE;
	gchar path[1024]="";
	g_snprintf(path, sizeof(path),"%s%s.lock", LY_GLB_USER_PIXDIR, priv->name);
	if(g_file_test(path, G_FILE_TEST_EXISTS))
	{
		remove(path);
	}
	return TRUE;
}

void ly_pli_plugin_new_start_cb(GMarkupParseContext *context, const gchar *element_name, \
		const gchar **attribute_names, const gchar **attribute_values, \
		gpointer data, GError **error)
{
	g_free(ly_pli_plugin_element_name);
	ly_pli_plugin_element_name=g_strdup (element_name);
}
void ly_pli_plugin_new_text_cb(GMarkupParseContext * context, const gchar *text, \
		gsize text_len, gpointer data, GError **error)
{
	LyPliPluginPrivate *priv=data;
	if(!ly_pli_plugin_element_name||!text)
		return;
	else if(g_str_equal(ly_pli_plugin_element_name,"alias"))
		priv->alias=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"version"))
		priv->version=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"author"))
		priv->author=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"license"))
		priv->license=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"logo"))
		priv->logo=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"comment"))
		priv->comment=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"website"))
		priv->website=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"depends"))
		priv->depends=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"daemon"))
	{
		gint daemon=0;
		sscanf(text,"%d",&daemon);
		priv->daemon=(gboolean)(daemon);
	}
	else if(g_str_equal(ly_pli_plugin_element_name,"create_symbol"))
		priv->create_symbol=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"refresh_symbol"))
		priv->refresh_symbol=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"destroy_symbol"))
		priv->destroy_symbol=g_strdup(text);
	else if(g_str_equal(ly_pli_plugin_element_name,"config_symbol"))
		priv->config_symbol=g_strdup(text);
}
void ly_pli_plugin_new_end_cb(GMarkupParseContext *context, const gchar *element_name, \
		gpointer data, GError **error)
{
	g_free(ly_pli_plugin_element_name);
	ly_pli_plugin_element_name=NULL;
}
