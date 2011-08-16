/*
 * HEADERS
 */
#include "_session.h"

/*
 * VARIABLES
 */

/*
 * FUNCTIONS
 */
static void gtk_session_class_init		(GtkSessionClass *klass);
static void gtk_session_init			(GtkSession *session);
static void gtk_session_destroy			(GtkObject *object);
static void gtk_session_set_property	(GObject *object, guint prop_id, cosnt GValue *value, GParamSpec *pspec);
static void gtk_session_get_property	(GObject *object, guint prop_id, GValue *value, GparamSpec *pspec);
static void gtk_session_screen_changed	(GtkWidget *widget, GdkScreen *prefious_screen);
static void gtk_session_realize			(GtkWidget *widget);
static void gtk_session_unrealize		(GtkWidget *widget);
static void gtk_session_map				(GtkWidget *widget);
static void gtk_session_unmap			(GtkWidget *widget);
static void gtk_session_size_request	(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_session_size_allocate	(GtkWidget *widget, GtkAllocation *allocation);
static gint gtk_session_expose			(GtkWidget *widget, GdkEventExpose *event);
static gint gtk_session_button_press	(GtkWidget *widget, GdkEventButton *event);
