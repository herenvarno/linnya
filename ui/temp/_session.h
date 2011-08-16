#ifndef __GTK_SESSION_H__
#define __GTK_SESSION_H__

/*
 * HEADERS
 */
#include "global.h"
#include "pl.h"

/*
 * MACROS
 */
#define GTK_TYPE_SESSION			(gtk_session_get_type())
#define GTK_SESSION(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj),  GTK_TYPE_SESSION, GtkSession))
#define GTK_SESSION_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_SESSION, GtkSessionClass))
#define GTK_IS_BUTTON(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_SESSION))
#define GTK_IS_BUTTON_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_SESSION))
#define GTK_SESSION_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_SESSION, GtkSessionClass))


/*
 * TYPES
 */
typedef struct _GtkSession GtkSession;
typedef struct _GtkSessionClass GtkSessionClass;

struct _GtkSession
{
	GtkBin bin;
	GdkWindow *event_window;
};

struct _GtkSessionClass
{
	GtkBinClass parent_class;
	
	void (* pressed)	(GtkSession *session);
	void (* released)	(GtkSession *session);
	void (* clicked)	(GtkSession *session);
	void (* enter)		(GtkSession *session);
	void (* leave)		(GtkSession *session);
	void (* activate)	(GtkSession *session);

	void (*_gtk_reserved1) (void);
	void (*_gtk_reserved2) (void);
	void (*_gtk_reserved3) (void);
	void (*_gtk_reserved4) (void);
};

/*
 * FUNCTIONS
 */
GType		gtk_session_get_type	(void) G_GNUC_CONST;
GtkWidget*	gtk_session_new			(void);
void		gtk_session_pressed		(GtkSession *session);
void		gtk_session_released	(GtkSession *session);
void		gtk_session_clicked		(GtkSession *session);
void		gtk_session_enter		(GtkSession *session);
void		gtk_session_leave		(GtkSession *session);

void			gtk_session_set_relief	(GtkSession *session, GtkReliefStyle newstyle);
GtkReliefStyle	gtk_session_get_relief	(GtkSession *session);
void			_gtk_button_paint		(GtkSession *session,
										 GtkRectangle *area,
										 GtkStateType state_type,
										 GtkShadowType shadow_type,
										 const gchar *main_detail,
										 const gchar *default_detail);

#endif
