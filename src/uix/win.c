/*
 * HEADERS
 */
#include "win.h"

/*
 * VARABLES
 */
LyWinWindow		*ly_win_window;
int timeout_id=0;

/*
 * FUNCTIONS
 */
gboolean ly_win_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean ly_win_on_play_changed_cb(GtkWidget *widget, gpointer data);
gboolean ly_win_volume_cb(GtkScaleButton *button,gdouble  value,gpointer   data);
gboolean ly_win_change_volume_cb (GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);
gboolean ly_win_change_seek_cb (GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data);
gboolean ly_win_on_seek_update_cb(gpointer data);
void ly_win_update_meta_cb(LyMbsMessage *message, gpointer data);
void ly_win_update_button_cb(LyMbsMessage *message, gpointer data);
gboolean ly_win_tray_popup_cb(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu);
gboolean ly_win_change_visible_cb(GtkWidget *widget, gpointer data);
static gboolean ly_win_on_prev_clicked_cb(MxButton *button, gpointer data);
static gboolean ly_win_on_play_clicked_cb(MxButton *button, gpointer data);
static gboolean ly_win_on_next_clicked_cb(MxButton *button, gpointer data);
static gboolean ly_win_on_conf_clicked_cb(MxButton *button, gpointer data);
static gboolean ly_win_on_volm_clicked_cb(MxButton *button, gpointer data);
static gboolean ly_win_on_seek_release_cb (ClutterActor *actor, ClutterButtonEvent *event, gpointer data);

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
	GtkWidget *vbox_main;
	GtkWidget *hbox_title;
	ClutterActor *title_stage;
	ClutterActor *btn_menu;
	ClutterActor *hbox_label;
	ClutterActor *btn_adds;
	GtkWidget *notebook_session;
	GtkWidget *hbox_control;
	ClutterActor *control_stage;
	ClutterActor *btn_prev;
	ClutterActor *btn_play;
	ClutterActor *btn_next;
	ClutterActor *bar_seek;
	ClutterActor *btn_conf;
	ClutterActor *btn_volm;
	ClutterLayoutManager *hbox_layout;

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

	screen = gtk_widget_get_screen(win);
	gint custom_winbg=0;
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
	guint winbg[4]={0, 0, 0, 65535};
	ly_reg_get("thm_winbg", "%d:%d:%d:%d", &winbg[0], &winbg[1], &winbg[2], &winbg[3]);

	/*
	 * create main vbox
	 */
	vbox_main=gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(vbox_main), FALSE);
	gtk_container_add(GTK_CONTAINER(win),vbox_main);

	/*
	 * hbox_title
	 */
	ClutterColor stage_title_color = { winbg[0]/256, winbg[1]/256, winbg[2]/256, winbg[3]/256};
	hbox_title=gtk_clutter_embed_new();
	gtk_widget_set_size_request(hbox_title, -1, 30);
	gtk_widget_set_vexpand(hbox_title,FALSE);
	gtk_grid_attach(GTK_GRID(vbox_main), hbox_title, 0, 0, 1, 1);

	title_stage=gtk_clutter_embed_get_stage(GTK_CLUTTER_EMBED(hbox_title));
	clutter_stage_set_use_alpha(CLUTTER_STAGE (title_stage), TRUE);
	clutter_stage_set_color (CLUTTER_STAGE (title_stage), &stage_title_color);
	hbox_layout=clutter_box_layout_new();
	clutter_box_layout_set_vertical(CLUTTER_BOX_LAYOUT(hbox_layout), FALSE);
	clutter_box_layout_set_pack_start(CLUTTER_BOX_LAYOUT(hbox_layout), FALSE);
	clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(hbox_layout), 0);
	clutter_actor_set_layout_manager(title_stage, hbox_layout);
	hbox_label=mx_box_layout_new();
	mx_box_layout_set_orientation(MX_BOX_LAYOUT(hbox_label), MX_ORIENTATION_HORIZONTAL);
	mx_box_layout_set_spacing(MX_BOX_LAYOUT(hbox_label), 2);
	mx_box_layout_set_enable_animations(MX_BOX_LAYOUT(hbox_label), TRUE);
	clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(hbox_layout), hbox_label, TRUE, TRUE, FALSE, 0, CLUTTER_BOX_ALIGNMENT_CENTER);
	btn_menu=mx_button_new();
	clutter_actor_set_size(btn_menu, 80, 30);
	clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(hbox_layout), btn_menu, FALSE, FALSE, FALSE, 0, CLUTTER_BOX_ALIGNMENT_CENTER);
	btn_adds=mx_button_new();
	clutter_actor_set_size(btn_adds, 30, 30);
	clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(hbox_layout), btn_adds, FALSE, FALSE, FALSE, 0, CLUTTER_BOX_ALIGNMENT_CENTER);

	/*
	 * the second part: notebook
	 */
	notebook_session=gtk_notebook_new();
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook_session), TRUE);
	gtk_widget_set_vexpand(notebook_session, TRUE);
	gtk_grid_attach(GTK_GRID(vbox_main), notebook_session, 0, 1, 1, 1);
	//gtk_notebook_set_action_widget(GTK_NOTEBOOK(notebook_session), hbox_title, GTK_PACK_END);

	/*
	 * The third part : control area
	 */
	ClutterColor stage_control_color = { winbg[0]/256, winbg[1]/256, winbg[2]/256, winbg[3]/256};
	hbox_control=gtk_clutter_embed_new();
	gtk_widget_set_size_request(hbox_control, -1, 60);
	gtk_widget_set_vexpand(hbox_control,FALSE);
	gtk_grid_attach(GTK_GRID(vbox_main), hbox_control, 0, 2, 1, 1);

	control_stage=gtk_clutter_embed_get_stage(GTK_CLUTTER_EMBED(hbox_control));
	clutter_stage_set_use_alpha(CLUTTER_STAGE (control_stage), TRUE);
	clutter_stage_set_color (CLUTTER_STAGE (control_stage), &stage_control_color);
	hbox_layout=clutter_box_layout_new();
	clutter_box_layout_set_vertical(CLUTTER_BOX_LAYOUT(hbox_layout), FALSE);
	clutter_box_layout_set_pack_start(CLUTTER_BOX_LAYOUT(hbox_layout), FALSE);
	clutter_actor_set_layout_manager(control_stage, hbox_layout);
	btn_prev=mx_button_new();
	clutter_actor_set_size(btn_prev, 50, 50);
	clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(hbox_layout), btn_prev, FALSE, FALSE, FALSE, 0, CLUTTER_BOX_ALIGNMENT_CENTER);
	btn_play=mx_button_new();
	mx_button_set_is_toggle(MX_BUTTON(btn_play), TRUE);
	clutter_actor_set_size(btn_play, 50, 50);
	clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(hbox_layout), btn_play, FALSE, FALSE, FALSE, 0, CLUTTER_BOX_ALIGNMENT_CENTER);
	btn_next=mx_button_new();
	clutter_actor_set_size(btn_next, 50, 50);
	clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(hbox_layout), btn_next, FALSE, FALSE, FALSE, 0, CLUTTER_BOX_ALIGNMENT_CENTER);
	bar_seek=mx_slider_new();
	clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(hbox_layout), bar_seek, TRUE, TRUE, FALSE, 0, CLUTTER_BOX_ALIGNMENT_CENTER);
	btn_conf=mx_button_new();
	clutter_actor_set_size(btn_conf, 50, 50);
	clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(hbox_layout), btn_conf, FALSE, FALSE, FALSE, 0, CLUTTER_BOX_ALIGNMENT_CENTER);
	btn_volm=mx_button_new();
	clutter_actor_set_size(btn_volm, 50, 50);
	clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(hbox_layout), btn_volm, FALSE, FALSE, FALSE, 0, CLUTTER_BOX_ALIGNMENT_CENTER);


	//tray icon
	g_snprintf(path, sizeof(path), "%sicon/linnya.png", LY_GLB_PROG_UIXDIR);
	GtkStatusIcon *tray_icon  = gtk_status_icon_new_from_file (path);
	//tray menu
	GtkWidget *menu, *menu_item;
	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_label (_("Play/Pause"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_win_on_play_changed_cb),	NULL);
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
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_win_change_visible_cb),	win);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL (menu),menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Quit"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gtk_main_quit),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show_all (menu);
	//tray notify
	gtk_status_icon_set_tooltip_markup(tray_icon,_("<b>Linnya</b> Audio Player"));
	//tray activity
	g_signal_connect(GTK_STATUS_ICON (tray_icon), "activate", G_CALLBACK (ly_win_change_visible_cb), win);
	g_signal_connect(GTK_STATUS_ICON (tray_icon), "popup-menu", G_CALLBACK (ly_win_tray_popup_cb), menu);

	/*
	 * set actor names
	 */
	clutter_actor_set_name(title_stage, "title_stage");
	clutter_actor_set_name(control_stage, "control_stage");
	clutter_actor_set_name(btn_menu, "btn_menu");
	clutter_actor_set_name(btn_adds, "btn_adds");
	clutter_actor_set_name(btn_prev, "btn_prev");
	clutter_actor_set_name(btn_play, "btn_play");
	clutter_actor_set_name(btn_next, "btn_next");
	clutter_actor_set_name(btn_conf, "btn_conf");
	clutter_actor_set_name(btn_volm, "btn_volm");
	clutter_actor_set_name(bar_seek, "bar_seek");
	gtk_widget_show_all(win);

	/*
	 * Load Theme
	 */
	LyThmItem *th=ly_thm_item_new_from_conf();
	if(th && !g_str_equal(th->style, ""))
	{
		mx_style_load_from_file (mx_style_get_default(), th->style, NULL);
		mx_stylable_set_style (MX_STYLABLE (title_stage), mx_style_get_default());
		mx_stylable_set_style (MX_STYLABLE (control_stage), mx_style_get_default());
	}
	ly_thm_item_free(th);

	/*
	 * bind signals
	 */
	if(custom_winbg)
	{
		g_signal_connect(G_OBJECT(win),"draw", G_CALLBACK(ly_win_expose_cb),NULL);
	}
	g_signal_connect(G_OBJECT(win),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(btn_prev), "clicked", G_CALLBACK(ly_win_on_prev_clicked_cb), NULL);
	g_signal_connect(G_OBJECT(btn_play), "clicked", G_CALLBACK(ly_win_on_play_clicked_cb), NULL);
	g_signal_connect(G_OBJECT(btn_next), "clicked", G_CALLBACK(ly_win_on_next_clicked_cb), NULL);
	g_signal_connect(G_OBJECT(btn_conf), "clicked", G_CALLBACK(ly_win_on_conf_clicked_cb), NULL);
	g_signal_connect(G_OBJECT(btn_volm), "clicked", G_CALLBACK(ly_win_on_volm_clicked_cb), NULL);
	timeout_id = g_timeout_add(1000, ly_win_on_seek_update_cb, bar_seek);

	/*
	 * bind custom messages
	 */
	ly_mbs_bind("meta_changed","core",ly_win_update_meta_cb, NULL);
	ly_mbs_bind("play","core:aud",ly_win_update_button_cb, "play");
	ly_mbs_bind("pause","core:aud",ly_win_update_button_cb, "pause");
	ly_mbs_bind("stop","core:aud",ly_win_update_button_cb, "stop");

	/*
	 * bind keys
	 */
	gtk_window_add_accel_group(GTK_WINDOW(win),ly_key_get_accel());

	ly_key_set("volume+", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_change_volume_cb), "+");
	ly_key_set("volume-", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_change_volume_cb), "-");
	ly_key_set("seek+", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_change_seek_cb), "+");
	ly_key_set("seek-", NULL, NULL, NULL, KEY_BIND_TYPE_CALLBACK, G_CALLBACK(ly_win_change_seek_cb), "-");
	ly_key_bind("volume+");
	ly_key_bind("volume-");
	ly_key_bind("seek+");
	ly_key_bind("seek-");

	window->win=win;
	window->vbox_main=vbox_main;
	window->hbox_title=hbox_title;
	window->notebook_session=notebook_session;
	window->btn_menu=btn_menu;
	window->btn_adds=btn_adds;
	window->btn_prev=btn_prev;
	window->btn_play=btn_play;
	window->btn_next=btn_next;
	window->btn_conf=btn_conf;
	window->btn_volm=btn_volm;
	window->bar_seek=bar_seek;

	GstState gst_state;
	gst_state=ly_aud_get_state();
	if(gst_state==GST_STATE_PLAYING)
	{
		mx_button_set_toggled(MX_BUTTON(btn_play), TRUE);
	}
	return window;
}


gboolean ly_win_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	gint width;
	gint height;
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	gint custom_winbg=0;
	ly_reg_get("thm_custom_winbg", "%d", &custom_winbg);

	cairo_surface_t *image=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,1,1);
	cairo_set_source_surface(cr, image, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	cairo_surface_destroy(image);
	image=NULL;

	guint winbg[4]={0, 0, 0, 65535};
	ly_reg_get("thm_winbg", "%d:%d:%d:%d", &winbg[0], &winbg[1], &winbg[2], &winbg[3]);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_set_source_rgba(cr, winbg[0]/65536.0, winbg[1]/65536.0, winbg[2]/65536.0, winbg[3]/65536.0);
	cairo_fill(cr);
	return FALSE;
}

gboolean ly_win_on_play_changed_cb(GtkWidget *widget, gpointer data)
{
	GstState gst_state;
	gst_state=ly_aud_get_state();
	if(gst_state!=GST_STATE_PLAYING)
	{
		ly_aud_play();
	}
	else if(gst_state==GST_STATE_PLAYING)
	{
		ly_aud_pause();
	}
	return FALSE;
}

gboolean ly_win_volume_cb(GtkScaleButton *button,gdouble  value,gpointer   data)
{
	ly_aud_set_volume(value);
	return FALSE;
}

gboolean ly_win_change_volume_cb (GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data)
{
	gdouble inc=0;
	if(g_str_equal((gchar*)data,"+"))
		inc=0.1;
	else if(g_str_equal((gchar*)data,"-"))
		inc=-0.1;
	gdouble volume=ly_aud_get_volume();
	volume+=inc;
	if(volume<=0)
		volume=0;
	if(volume>=1)
		volume=1;
	ly_aud_set_volume(volume);
//	gtk_scale_button_set_value(GTK_SCALE_BUTTON(ly_win_window->btn_volm), ly_aud_get_volume());
	return FALSE;
}

gboolean ly_win_change_seek_cb (GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data)
{
	gdouble inc=0;
	if(g_str_equal((gchar*)data,"+"))
		inc=0.05;
	else if(g_str_equal((gchar*)data,"-"))
		inc=-0.05;
	gdouble position=ly_aud_get_position();
	position+=inc;
	if(position<=0)
		position=0;
	if(position>=1)
		position=1;
	ly_aud_set_position(position);
	mx_slider_set_value(MX_SLIDER(ly_win_window->bar_seek), position);
	return FALSE;
}
gboolean ly_win_on_seek_update_cb(gpointer data)
{
	gdouble pos_aud=ly_aud_get_position();
	gdouble pos_bar=mx_slider_get_value(MX_SLIDER(data));

	if(((pos_aud-pos_bar)>=0.05 || (pos_aud-pos_bar)<=-0.05) && pos_bar<=0.95)
	{
		ly_aud_set_position(pos_bar);
	}
	else
	{
		mx_slider_set_value(MX_SLIDER(data), pos_aud);
	}
	return TRUE;
}

void ly_win_update_meta_cb(LyMbsMessage *message, gpointer data)
{
	mx_slider_set_value(MX_SLIDER(ly_win_window->bar_seek), 0);
}

void ly_win_update_button_cb(LyMbsMessage *message, gpointer data)
{
	gchar *signal=data;
	if(g_str_equal(signal,"play") && !mx_button_get_toggled(MX_BUTTON(ly_win_window->btn_play)))
	{
		mx_button_set_toggled(MX_BUTTON(ly_win_window->btn_play), TRUE);
	}
	else if((g_str_equal(signal,"pause")||g_str_equal(signal,"stop")) && mx_button_get_toggled(MX_BUTTON(ly_win_window->btn_play)))
	{
		mx_button_set_toggled(MX_BUTTON(ly_win_window->btn_play), FALSE);
	}
}


gboolean ly_win_tray_popup_cb(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu)
{
	gtk_menu_popup(GTK_MENU(popUpMenu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
	return FALSE;
}

gboolean ly_win_change_visible_cb(GtkWidget *widget, gpointer data)
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

LyWinWindow*	ly_win_get_window()
{
	return ly_win_window;
}

void		ly_win_free	(LyWinWindow *window)
{
	if(!window)
		return;
	g_free(window);
}
static gboolean
ly_win_on_prev_clicked_cb(MxButton	*button,
						  gpointer	 data)
{
	ly_aud_prev();
	return FALSE;
}
static gboolean
ly_win_on_play_clicked_cb(MxButton	*button,
						  gpointer	 data)
{
	gboolean btn_state;
	GstState gst_state;
	btn_state=mx_button_get_toggled(MX_BUTTON(button));
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
ly_win_on_next_clicked_cb(MxButton	*button,
						  gpointer	 data)
{
	ly_aud_next();
	return FALSE;
}

static gboolean
ly_win_on_conf_clicked_cb(MxButton	*button,
						  gpointer	 data)
{
	ly_mbs_put("config", "ui:win", NULL);
	return FALSE;
}

static gboolean
ly_win_on_volm_clicked_cb(MxButton	*button,
						  gpointer	 data)
{
	return FALSE;
}