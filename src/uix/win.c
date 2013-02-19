/*
 * HEADERS
 */
#include "win.h"

/*
 * VARABLES
 */
LyWinWindow *ly_win_window=NULL;
int ly_win_timeout=0;
gboolean ly_win_flag_seek=FALSE;
gboolean ly_win_flag_home=FALSE;
gboolean ly_win_flag_home_drag=FALSE;
gint ly_win_home_pos_x;
gint ly_win_home_pos_y;
gint ly_win_window_width=0;
gint ly_win_window_height=0;

/*
 * FUNCTIONS
 */
static gboolean ly_win_on_win_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
static gboolean ly_win_on_win_size_changed_cb(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data);
static gboolean ly_win_on_cursor_move_cb(GtkWidget *widget, GdkEventMotion *event, gpointer data);
static gboolean ly_win_on_cursor_leave_cb(GtkWidget *widget, GdkEventMotion *event, gpointer data);
static gboolean ly_win_on_full_toggled_cb(GtkWidget *widget, gpointer data);
static gboolean ly_win_on_widget_visible_cb(GtkWidget *widget, gpointer data);
static gboolean ly_win_on_prev_clicked_cb(GtkWidget *button, gpointer data);
static gboolean ly_win_on_play_toggled_cb(GtkWidget *button, gpointer data);
static gboolean ly_win_on_next_clicked_cb(GtkWidget *button, gpointer data);
static gboolean ly_win_on_conf_clicked_cb(GtkWidget *button, gpointer data);
static gboolean ly_win_on_mute_toggled_cb(GtkWidget *widget, gpointer data);
static gboolean ly_win_on_volm_changed_cb(GtkRange *widget, gpointer data);
static gboolean ly_win_on_seek_update_cb(gpointer data);
static gboolean ly_win_on_seek_changed_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);

static gboolean ly_win_on_accel_play_cb(GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);
static gboolean ly_win_on_accel_prev_cb(GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);
static gboolean ly_win_on_accel_next_cb(GtkAccelGroup *accel_group, GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);
static gboolean ly_win_on_accel_conf_cb(GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);
static gboolean ly_win_on_accel_volm_cb(GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);
static gboolean ly_win_on_accel_seek_cb(GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);
static gboolean ly_win_on_accel_fullscreen_cb(GtkAccelGroup *accel_group, GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);

static gboolean ly_win_on_tray_popup_cb(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer menu);
static gboolean ly_win_on_play_clicked_cb(GtkWidget *widget, gpointer data);

void ly_win_on_update_meta_cb(LyMbsMessage *message, gpointer data);
void ly_win_on_update_button_cb(LyMbsMessage *message, gpointer data);

void ly_win_fullscreen();
void ly_win_unfullscreen();
static gboolean ly_win_on_home_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
static gboolean ly_win_on_home_clicked_cb(GtkWidget *widget, gpointer data);

void ly_win_init()
{
	ly_win_window=ly_win_new();
	if(!ly_win_window)
	{
		g_error(_("Cannot create main window, abort ..."));
	}
}

void	ly_win_fina	()
{
	ly_win_free(ly_win_window);
}

LyWinWindow*	ly_win_new()
{
	LyWinWindow *window=NULL;
	window=(LyWinWindow*)g_malloc(sizeof(LyWinWindow));
	if(!window)
		return NULL;

	GtkWidget *win;
	GtkWidget *grd;
	GtkWidget *box;
	GtkWidget *nbk_sssn;
	GtkWidget *btn_menu;
	GtkWidget *btn_adds;
	GtkWidget *btn_full;
	GtkWidget *btn_prev;
	GtkWidget *btn_play;
	GtkWidget *btn_next;
	GtkWidget *bar_seek;
	GtkWidget *btn_conf;
	GtkWidget *btn_mute;
	GtkWidget *bar_volm;
	GtkWidget *grd_ctrl;

	char path[1024]="";
	GdkScreen *screen;

	/*
	 * create window
	 */
	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), LY_GLA_APPLICATION);
	g_snprintf(path,sizeof(path),"%sicon/linnya.png",LY_GLB_PROG_UIXDIR);
	gtk_window_set_icon_from_file(GTK_WINDOW(win),path,NULL);
	gtk_window_set_resizable(GTK_WINDOW(win), TRUE);
	gtk_window_set_default_size(GTK_WINDOW(win),800,600);
	gtk_container_set_border_width(GTK_CONTAINER(win),0);

	/*
	 * Load Theme
	 */
	LyThmItem *th=ly_thm_item_new_from_conf();
	if(th && !g_str_equal(th->style, ""))
	{
		screen = gtk_widget_get_screen(win);
		GtkCssProvider *provider = gtk_css_provider_new();
		gtk_css_provider_load_from_path(provider, th->style, NULL);
		gtk_style_context_add_provider_for_screen(screen,GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_THEME);
		gtk_style_context_reset_widgets(screen);
		g_object_unref(provider);
	}

	screen = gtk_widget_get_screen(win);
	gint custom_winbg=1;
	ly_reg_get("thm_custom_winbg", "%d", &custom_winbg);
	gtk_widget_set_app_paintable(win, TRUE);
	if(custom_winbg)
	{

		if(!gtk_widget_is_composited(win))
		{
			ly_log_put_with_flag(G_LOG_LEVEL_MESSAGE, _("Cannot Support transparent!"));
		}
		else
		{
			GdkVisual *visual;
			visual = gdk_screen_get_rgba_visual (screen);
			if (visual == NULL)
				visual = gdk_screen_get_system_visual (screen);
			gtk_widget_set_visual (GTK_WIDGET (win), visual);
		}
	}
	gtk_widget_set_events(win,GDK_ALL_EVENTS_MASK);
	gdouble winbg[4]={0.447, 0.624, 0.812, 0.5};
	ly_reg_get("thm_winbg", "%lf:%lf:%lf:%lf", &winbg[0], &winbg[1], &winbg[2], &winbg[3]);

	/*
	 * create main grid
	 */
	grd=gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(grd), FALSE);
	gtk_container_add(GTK_CONTAINER(win), grd);

	/*
	 * notebook for sessions
	 */
	nbk_sssn=gtk_notebook_new();
	gtk_widget_set_hexpand(nbk_sssn, TRUE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nbk_sssn), TRUE);
	gtk_widget_set_vexpand(nbk_sssn, TRUE);
	gtk_grid_attach(GTK_GRID(grd), nbk_sssn, 0, 0, 6, 1);
	box=gtk_grid_new();
	btn_menu=gtk_button_new();
	gtk_widget_set_size_request(btn_menu, 80, 35);
	gtk_grid_attach(GTK_GRID(box), btn_menu, 0, 0, 1, 1);
	btn_adds=gtk_button_new();
	gtk_widget_set_size_request(btn_adds, 30, 35);
	gtk_grid_attach(GTK_GRID(box), btn_adds, 1, 0, 1, 1);
	btn_full=gtk_toggle_button_new();
	gtk_widget_set_size_request(btn_full, 30, 35);
	gtk_grid_attach(GTK_GRID(box), btn_full, 2, 0, 1, 1);
	gtk_notebook_set_action_widget(GTK_NOTEBOOK(nbk_sssn), box, GTK_PACK_END);
	gtk_widget_show_all(box);

	/*
	 * control area
	 */
	grd_ctrl=gtk_grid_new();
	gtk_widget_set_hexpand(grd_ctrl, TRUE);
	gtk_grid_attach(GTK_GRID(grd), grd_ctrl, 0, 1, 1, 1);
	btn_prev=gtk_button_new();
	gtk_widget_set_size_request(btn_prev, 50, 50);
	gtk_grid_attach(GTK_GRID(grd_ctrl), btn_prev, 0, 0, 1, 1);
	btn_play=gtk_toggle_button_new();
	gtk_widget_set_size_request(btn_play, 50, 50);
	gtk_grid_attach(GTK_GRID(grd_ctrl), btn_play, 1, 0, 1, 1);
	btn_next=gtk_button_new();
	gtk_widget_set_size_request(btn_next, 50, 50);
	gtk_grid_attach(GTK_GRID(grd_ctrl), btn_next, 2, 0, 1, 1);
	bar_seek=gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.0001);
	ly_win_flag_seek=FALSE;
	gtk_widget_set_size_request(bar_seek, 150, -1);
	gtk_scale_set_draw_value(GTK_SCALE(bar_seek),FALSE);
	gtk_widget_set_hexpand(bar_seek, TRUE);
	gtk_widget_set_vexpand(bar_seek, FALSE);
	gtk_grid_attach(GTK_GRID(grd_ctrl), bar_seek, 3, 0, 1, 1);
	btn_conf=gtk_button_new();
	gtk_widget_set_size_request(btn_conf, 50, 50);
	gtk_grid_attach(GTK_GRID(grd_ctrl), btn_conf, 4, 0, 1, 1);
	btn_mute=gtk_toggle_button_new();
	gtk_widget_set_size_request(btn_mute, 50, 50);
	gtk_grid_attach(GTK_GRID(grd_ctrl), btn_mute, 5, 0, 1, 1);
	bar_volm=gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.01);
	gtk_widget_set_size_request(bar_volm, 100, -1);
	gtk_scale_set_draw_value(GTK_SCALE(bar_volm),FALSE);
	gtk_widget_set_hexpand(bar_volm, FALSE);
	gtk_widget_set_vexpand(bar_volm, FALSE);
	gtk_range_set_value(GTK_RANGE(bar_volm), ly_aud_get_volume());
	gtk_grid_attach(GTK_GRID(grd_ctrl), bar_volm, 6, 0, 1, 1);

	//tray icon
	g_snprintf(path, sizeof(path), "%sicon/linnya.png", LY_GLB_PROG_UIXDIR);
	GtkStatusIcon *tray_icon  = gtk_status_icon_new_from_file (path);
	//tray menu
	GtkWidget *menu, *menu_item;
	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_label (_("Play/Pause"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_win_on_play_clicked_cb),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Previous"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_win_on_prev_clicked_cb),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Next"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_win_on_next_clicked_cb),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Configuration"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_win_on_conf_clicked_cb),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL (menu),menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Show/Hide Main Window"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_win_on_widget_visible_cb), win);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL (menu),menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Quit"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gtk_main_quit),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show_all (menu);
	gtk_status_icon_set_tooltip_markup(tray_icon,_("<b>Linnya</b> Audio Player"));
	g_signal_connect(GTK_STATUS_ICON (tray_icon), "activate", G_CALLBACK (ly_win_on_widget_visible_cb), win);
	g_signal_connect(GTK_STATUS_ICON (tray_icon), "popup-menu", G_CALLBACK (ly_win_on_tray_popup_cb), menu);

	/*
	 * set names
	 */
	gtk_widget_set_name(win, "win");
	gtk_widget_set_name(nbk_sssn, "nbk_sssn");
	gtk_widget_set_name(btn_menu, "btn_menu");
	gtk_widget_set_name(btn_adds, "btn_adds");
	gtk_widget_set_name(btn_full, "btn_full");
	gtk_widget_set_name(btn_prev, "btn_prev");
	gtk_widget_set_name(btn_play, "btn_play");
	gtk_widget_set_name(btn_next, "btn_next");
	gtk_widget_set_name(btn_conf, "btn_conf");
	gtk_widget_set_name(btn_mute, "btn_mute");
	gtk_widget_set_name(bar_seek, "bar_seek");
	gtk_widget_set_name(bar_volm, "bar_volm");
	gtk_widget_set_name(grd_ctrl, "grd_ctrl");
	gtk_widget_show_all(win);

	/*
	 * bind signals
	 */
	g_signal_connect(G_OBJECT(win), "destroy",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(win), "draw", G_CALLBACK(ly_win_on_win_expose_cb),NULL);
	g_signal_connect(G_OBJECT(win), "size-allocate", G_CALLBACK(ly_win_on_win_size_changed_cb),NULL);
	g_signal_connect(G_OBJECT(btn_full), "toggled", G_CALLBACK(ly_win_on_full_toggled_cb), NULL);
	g_signal_connect(G_OBJECT(btn_prev), "clicked", G_CALLBACK(ly_win_on_prev_clicked_cb), NULL);
	g_signal_connect(G_OBJECT(btn_play), "toggled", G_CALLBACK(ly_win_on_play_toggled_cb), NULL);
	g_signal_connect(G_OBJECT(btn_next), "clicked", G_CALLBACK(ly_win_on_next_clicked_cb), NULL);
	g_signal_connect(G_OBJECT(btn_conf), "clicked", G_CALLBACK(ly_win_on_conf_clicked_cb), NULL);
	g_signal_connect(G_OBJECT(btn_mute), "clicked", G_CALLBACK(ly_win_on_mute_toggled_cb), NULL);
	g_signal_connect(G_OBJECT(bar_volm), "value_changed", G_CALLBACK(ly_win_on_volm_changed_cb), NULL);
	ly_win_timeout = g_timeout_add(1000, ly_win_on_seek_update_cb, bar_seek);
	g_signal_connect(G_OBJECT(bar_seek),"button_press_event",G_CALLBACK(ly_win_on_seek_changed_cb),NULL);
	g_signal_connect(G_OBJECT(bar_seek),"button_release_event",G_CALLBACK(ly_win_on_seek_changed_cb),NULL);

	/*
	 * bind custom messages
	 */
//	ly_mbs_bind("meta_changed","core",ly_win_on_update_meta_cb, NULL);
	ly_mbs_bind("play","core:aud",ly_win_on_update_button_cb, g_strconcat("play", NULL));
	ly_mbs_bind("pause","core:aud",ly_win_on_update_button_cb, g_strconcat("pause", NULL));
	ly_mbs_bind("stop","core:aud",ly_win_on_update_button_cb, g_strconcat("stop", NULL));

	/*
	 * bind keys
	 */
	gtk_window_add_accel_group(GTK_WINDOW(win),ly_key_get_accel());
	ly_key_set("play", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_on_accel_play_cb), NULL);
	ly_key_set("prev", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_on_accel_prev_cb), NULL);
	ly_key_set("next", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_on_accel_next_cb), NULL);
	ly_key_set("conf", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_on_accel_conf_cb), NULL);
	ly_key_set("seek+", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_on_accel_seek_cb), "+");
	ly_key_set("seek-", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_on_accel_seek_cb), "-");
	ly_key_set("volume+", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_on_accel_volm_cb), "+");
	ly_key_set("volume-", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_on_accel_volm_cb), "-");
	ly_key_set("fullscreen", NULL, NULL, "Escape", KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_on_accel_fullscreen_cb), NULL);
	ly_key_bind("play");
	ly_key_bind("prev");
	ly_key_bind("next");
	ly_key_bind("conf");
	ly_key_bind("seek+");
	ly_key_bind("seek-");
	ly_key_bind("volume+");
	ly_key_bind("volume-");
	ly_key_bind("fullscreen");

	window->win=win;
	window->nbk_sssn=nbk_sssn;
	window->btn_menu=btn_menu;
	window->btn_adds=btn_adds;
	window->btn_full=btn_full;
	window->btn_prev=btn_prev;
	window->btn_play=btn_play;
	window->btn_next=btn_next;
	window->btn_conf=btn_conf;
	window->btn_mute=btn_mute;
	window->bar_seek=bar_seek;
	window->bar_volm=bar_volm;
	window->grd_ctrl=grd_ctrl;
	window->win_home=NULL;
	window->btn_home=NULL;

	int autoplay=0;
	if(!ly_reg_get("aud_autoplay", "%d", &autoplay))
		ly_reg_set("aud_autoplay", "%d", autoplay);
	if(autoplay)
	{
		gtk_button_clicked(GTK_BUTTON(btn_play));
	}

	return window;
}

// API
LyWinWindow*
ly_win_get_window()
{
	return ly_win_window;
}

void
ly_win_free(LyWinWindow	*window)
{
	if(!window)
		return;
	g_free(window);
}

// Callbacks for win
static gboolean
ly_win_on_win_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	gint width;
	gint height;
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	gint custom_winbg=1;
	ly_reg_get("thm_custom_winbg", "%d", &custom_winbg);

	cairo_surface_t *image=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,1,1);
	cairo_set_source_surface(cr, image, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	cairo_surface_destroy(image);
	image=NULL;

	GdkRGBA rgba;
	rgba.red=0.447;
	rgba.green=0.624;
	rgba.blue=0.812;
	rgba.alpha=0.5;
	ly_reg_get("thm_winbg", "%lf:%lf:%lf:%lf", &(rgba.red), &(rgba.green), &(rgba.blue), &(rgba.alpha));
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_set_source_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
	cairo_fill(cr);
	return FALSE;
}
static gboolean
ly_win_on_win_size_changed_cb(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data)
{
	// NOTE: This block of code is only useful for KDE environment.
	if(allocation->width!=ly_win_window_width || allocation->height!=ly_win_window_height)
	{
		ly_win_window_width=allocation->width;
		ly_win_window_height=allocation->height;
		Display *dpy = gdk_x11_display_get_xdisplay(gdk_display_get_default());
		Atom atom = XInternAtom(dpy, "_KDE_NET_WM_BLUR_BEHIND_REGION", False);
		unsigned long data0[4];
		data0[0]=allocation->x;
		data0[1]=allocation->y;
		data0[2]=allocation->width;
		data0[3]=allocation->height;
		XChangeProperty(dpy, gdk_x11_window_get_xid(gtk_widget_get_window(ly_win_window->win)), atom, XA_CARDINAL, 32, PropModeReplace, (const unsigned char *)data0, sizeof(data0));
	}
	return FALSE;
}


gboolean ly_win_on_widget_visible_cb(GtkWidget *widget, gpointer data)
{
	if(gtk_widget_get_visible(GTK_WIDGET(data))==TRUE)
	{
		gtk_widget_hide(GTK_WIDGET(data));
	}
	else
	{
		gtk_widget_show(GTK_WIDGET(data));
	}
	return FALSE;
}

// Callbacks for buttons
static gboolean
ly_win_on_full_toggled_cb(GtkWidget *widget, gpointer data)
{
	gboolean btn_state;
	btn_state=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	if(btn_state==TRUE)
	{
		ly_win_fullscreen();
	}
	else
	{
		ly_win_unfullscreen();
	}
	return FALSE;
}
static gboolean
ly_win_on_prev_clicked_cb(GtkWidget *widget, gpointer data)
{
	ly_aud_prev();
	return FALSE;
}
static gboolean
ly_win_on_play_toggled_cb(GtkWidget *widget, gpointer data)
{
	gboolean btn_state;
	GstState gst_state;
	btn_state=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	gst_state=ly_aud_get_state();
	if(gst_state!=GST_STATE_PLAYING && btn_state==TRUE)
	{
		ly_aud_play();
	}
	else if(gst_state==GST_STATE_PLAYING && btn_state==FALSE)
	{
		ly_aud_pause();
	}
	return FALSE;
}
static gboolean
ly_win_on_next_clicked_cb(GtkWidget *widget, gpointer data)
{
	ly_aud_next();
	return FALSE;
}

static gboolean
ly_win_on_conf_clicked_cb(GtkWidget *widget, gpointer data)
{
	ly_mbs_put("config", "ui:win", NULL);
	return FALSE;
}
static gboolean
ly_win_on_mute_toggled_cb(GtkWidget *widget, gpointer data)
{
	gboolean btn_state;
	btn_state=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	ly_aud_set_mute(btn_state);
	return FALSE;
}

static gboolean
ly_win_on_volm_changed_cb(GtkRange *widget, gpointer data)
{
	ly_aud_set_volume(gtk_range_get_value(GTK_RANGE(widget)));
	return FALSE;
}

// Callbacks for seek bar
static gboolean
ly_win_on_seek_update_cb(gpointer data)
{
	if(ly_win_flag_seek==FALSE)
	{
		gdouble position=ly_aud_get_position();
		gtk_range_set_value(GTK_RANGE(ly_win_window->bar_seek), position);
	}
	return TRUE;
}

static gboolean
ly_win_on_seek_changed_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	gdouble val;
	if (event->button == 1)
	{
		switch(event->type)
		{
			case GDK_BUTTON_PRESS:
				ly_win_flag_seek=TRUE;
				break;
			case GDK_BUTTON_RELEASE:
				val=gtk_range_get_value(GTK_RANGE(ly_win_window->bar_seek));
				ly_aud_set_position(val);
				ly_win_flag_seek=FALSE;
				break;
			default:
				break;
		}
	}
	return FALSE;
}

// Callbacks for accel
static gboolean
ly_win_on_accel_play_cb(GtkAccelGroup *accel_group, GObject *acceleratable, \
guint keyval, GdkModifierType modifier, gpointer data)
{
	gboolean btn_state;
	GstState gst_state;
	btn_state=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ly_win_window->btn_play));
	gst_state=ly_aud_get_state();
	if(gst_state!=GST_STATE_PLAYING && btn_state==FALSE)
	{
		ly_aud_play();
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ly_win_window->btn_play), TRUE);
	}
	else if(gst_state==GST_STATE_PLAYING && btn_state==TRUE)
	{
		ly_aud_pause();
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ly_win_window->btn_play), FALSE);
	}
	return FALSE;
}

static gboolean
ly_win_on_accel_prev_cb(GtkAccelGroup *accel_group, GObject *acceleratable, \
guint keyval, GdkModifierType modifier, gpointer data)
{
	ly_aud_prev();
	return FALSE;
}
static gboolean
ly_win_on_accel_next_cb(GtkAccelGroup *accel_group, GObject *acceleratable, \
guint keyval, GdkModifierType modifier, gpointer data)
{
	ly_aud_next();
	return FALSE;
}

static gboolean
ly_win_on_accel_conf_cb(GtkAccelGroup *accel_group, GObject *acceleratable, \
guint keyval, GdkModifierType modifier, gpointer data)
{
	ly_mbs_put("config", "ui:win", NULL);
	return FALSE;
}
static gboolean
ly_win_on_accel_volm_cb(GtkAccelGroup *accel_group, GObject *acceleratable, \
	guint keyval, GdkModifierType modifier, gpointer data)
{
	gdouble inc=0;
	if(g_str_equal((gchar*)data, "+"))
		inc=0.1;
	else if(g_str_equal((gchar*)data, "-"))
		inc=-0.1;
	gdouble volume=ly_aud_get_volume();
	volume+=inc;
	volume=volume<=1?volume:1;
	volume=volume>=0?volume:0;
	ly_aud_set_volume(volume);
	gtk_scale_button_set_value(GTK_SCALE_BUTTON(ly_win_window->bar_volm), \
		ly_aud_get_volume());
	return FALSE;
}
static gboolean
ly_win_on_accel_seek_cb(GtkAccelGroup *accel_group, GObject *acceleratable, \
	guint keyval, GdkModifierType modifier, gpointer data)
{
	gdouble inc=0;
	if(g_str_equal((gchar*)data,"+"))
		inc=0.05;
	else if(g_str_equal((gchar*)data,"-"))
		inc=-0.05;
	gdouble position=ly_aud_get_position();
	position+=inc;
	position=position<=1?position:1;
	position=position>=0?position:0;
	ly_aud_set_position(position);
	gtk_range_set_value(GTK_RANGE(ly_win_window->bar_seek),position);
	return FALSE;
}
static gboolean
ly_win_on_accel_fullscreen_cb(GtkAccelGroup *accel_group, GObject *acceleratable, \
	guint keyval, GdkModifierType modifier, gpointer data)
{
	GdkWindow *window=gtk_widget_get_window(ly_win_window->win);
	GdkWindowState state=gdk_window_get_state(window);
	if((state&GDK_WINDOW_STATE_FULLSCREEN))
	{
		ly_win_unfullscreen();
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ly_win_window->btn_full), FALSE);
	}
	else
	{
		ly_win_fullscreen();
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ly_win_window->btn_full), TRUE);
	}
	return FALSE;
}

// Callbacks for custom signals
void ly_win_on_update_meta_cb(LyMbsMessage *message, gpointer data)
{
	gchar *signal=g_strconcat(data,NULL);
	if(g_str_equal(signal,"play") && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ly_win_window->btn_play)))
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ly_win_window->btn_play), TRUE);
	}
	else if((g_str_equal(signal,"pause")||g_str_equal(signal,"stop")) && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ly_win_window->btn_play)))
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ly_win_window->btn_play), FALSE);
	}
	g_free(signal);
}
void ly_win_on_update_button_cb(LyMbsMessage *message, gpointer data)
{
}

// Funcs for popup menu
static gboolean
ly_win_on_tray_popup_cb(GtkStatusIcon *status_icon, guint button, \
	guint32 activate_time, gpointer menu)
{
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, gtk_status_icon_position_menu, \
		status_icon, button, activate_time);
	return FALSE;
}

static gboolean
ly_win_on_play_clicked_cb(GtkWidget *widget, gpointer data)
{
 	gtk_button_clicked(GTK_BUTTON(ly_win_window->btn_play));
	return FALSE;
}

/*******************************************************************************
 * THE FULLSCREEN MODE
 ******************************************************************************/
void
ly_win_fullscreen()
{
	gtk_window_set_decorated(GTK_WINDOW(ly_win_window->win), FALSE);
	gtk_widget_hide(ly_win_window->grd_ctrl);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ly_win_window->nbk_sssn), FALSE);
	gtk_widget_show(ly_win_window->btn_home);
	gtk_window_fullscreen(GTK_WINDOW(ly_win_window->win));

	if(ly_win_window->win_home!=NULL)
		return;
	GtkWidget *win_home;
	GtkWidget *btn_home;
	win_home=gtk_window_new(GTK_WINDOW_POPUP);
	gtk_window_set_default_size(GTK_WINDOW(win_home), 40, 40);
	gtk_container_set_border_width(GTK_CONTAINER(win_home), 0);
	int x = gdk_window_get_width(gdk_get_default_root_window())-40;
	int y = gdk_window_get_height(gdk_get_default_root_window())/2-20;
	gtk_window_move(GTK_WINDOW(win_home), x, y);
	GdkScreen *screen = gtk_widget_get_screen(win_home);
	gtk_widget_set_app_paintable(win_home, TRUE);
	if(gtk_widget_is_composited(win_home))
	{
		GdkVisual *visual;
		visual = gdk_screen_get_rgba_visual (screen);
		if (visual == NULL)
			visual = gdk_screen_get_system_visual (screen);
		gtk_widget_set_visual (GTK_WIDGET (win_home), visual);
	}
	g_signal_connect(G_OBJECT(win_home), "draw", G_CALLBACK(ly_win_on_home_expose_cb),NULL);

	btn_home=gtk_button_new();
	gtk_widget_set_name(btn_home, "btn_home");
	g_signal_connect(G_OBJECT(btn_home), "clicked", G_CALLBACK(ly_win_on_home_clicked_cb), NULL);
	gtk_container_add(GTK_CONTAINER(win_home), btn_home);
	ly_win_window->win_home=win_home;
	ly_win_window->btn_home=btn_home;
	gtk_widget_show_all(win_home);
}
void
ly_win_unfullscreen()
{
	if(ly_win_window->win_home)
	{
		gtk_widget_destroy(ly_win_window->win_home);
		ly_win_window->win_home=NULL;
		ly_win_window->btn_home=NULL;
	}
	gtk_window_set_decorated(GTK_WINDOW(ly_win_window->win), TRUE);
	gtk_widget_show(ly_win_window->grd_ctrl);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ly_win_window->nbk_sssn), TRUE);
	gtk_window_unfullscreen(GTK_WINDOW(ly_win_window->win));
}

static gboolean
ly_win_on_home_clicked_cb(GtkWidget *widget, gpointer data)
{
	if(gtk_widget_get_visible(GTK_WIDGET(ly_win_window->grd_ctrl))==TRUE)
	{
		gtk_widget_hide(GTK_WIDGET(ly_win_window->grd_ctrl));
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ly_win_window->nbk_sssn), FALSE);
	}
	else
	{
		gtk_widget_show(GTK_WIDGET(ly_win_window->grd_ctrl));
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ly_win_window->nbk_sssn), TRUE);
	}
	return FALSE;
}
static gboolean
ly_win_on_home_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	gint width;
	gint height;
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	cairo_surface_t *image=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,1,1);
	cairo_set_source_surface(cr, image, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	cairo_surface_destroy(image);
	image=NULL;
	return FALSE;
}