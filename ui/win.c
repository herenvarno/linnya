/*
 * HEADERS
 */
#include "win.h"

/*
 * VARABLES
 */
cairo_surface_t *image=NULL;
int nX, nY;
gboolean drag=FALSE;

/*
 * FUNCTIONS
 */
gboolean ly_ui_win_init()
{
	ly_global_debug("info",_("Initial ui module: WIN ...\n"));
	
	/*
	 * Load Theme
	 */
	gchar path[1024]="";
	g_snprintf(path, sizeof(path), "%sui/theme/dark/linnya-style.css", LY_GLOBAL_PROGDIR);
	GdkScreen *screen = gdk_screen_get_default();
	GtkCssProvider *provider = gtk_css_provider_get_default();
	gtk_css_provider_load_from_path(provider,path,NULL);
	gtk_style_context_add_provider_for_screen(screen,GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_style_context_reset_widgets(screen);

	
	GtkWidget *win;
	GtkWidget *vbox_main;
	GtkWidget *hbox_title;
	GtkWidget *notebook_session;
	GtkWidget *hbox_control;
 	GtkWidget *button_close;
 	GtkWidget *button_max;
 	GtkWidget *button_min;
 	GtkWidget *button_mini;
	GtkWidget *fixed_control_left;
	GtkWidget *fixed_control_right;
	GtkWidget *button_play;
	GtkWidget *button_prev;
	GtkWidget *button_next;
	GtkWidget *button_config;
	GtkWidget *button_volume;
	GtkWidget *hscale_seek;

	lyUiThemeTheme *th=NULL;

	/*
	 * create window
	 */
	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win),"Linnya");
	g_snprintf(path,sizeof(path),"%s%s",LY_GLOBAL_PROGDIR,"ui/icon/linnya.svg");
	gtk_window_set_icon_from_file(GTK_WINDOW(win),path,NULL);
	gtk_widget_set_app_paintable(win, TRUE);
	gtk_window_set_resizable(GTK_WINDOW(win), TRUE);
	th=ly_ui_theme_get("win");
	if(!th)
	{
		th=(lyUiThemeTheme *)g_malloc(sizeof(lyUiThemeTheme));
		
		th->pos[0]=th->pos[1]=100;
		th->size[0]=800;
		th->size[1]=600;
		th->decorated=0;
		th->border=10;
		g_strlcpy(th->background, "_", sizeof(th->background));
		g_strlcpy(th->font, "_", sizeof(th->font));
		th->fg_color0[0]=th->fg_color0[1]=th->fg_color0[2]=0;
		th->fg_color1[0]=th->fg_color1[1]=th->fg_color1[2]=0;
		th->bg_color0[0]=th->bg_color0[1]=th->bg_color0[2]=1;
		th->bg_color1[0]=th->bg_color1[1]=th->bg_color1[2]=0;
		th->fg_color0[3]=th->fg_color1[3]=th->bg_color0[3]=th->bg_color1[3]=1;
		ly_ui_theme_set("win",th);
	}
	gtk_window_set_decorated(GTK_WINDOW(win), (gboolean)(th->decorated));
	gtk_window_set_default_size(GTK_WINDOW(win),th->size[0],th->size[1]);
	gtk_container_set_border_width(GTK_CONTAINER(win),th->border);
	gtk_window_move(GTK_WINDOW(win),th->pos[0],th->pos[1]);
	
	if(!gtk_widget_is_composited(win))
	{
		ly_global_debug("Warnning", "Cannot Support transparent!");
	}
	GdkVisual *visual;
	screen = gtk_widget_get_screen(win);
	visual = gdk_screen_get_rgba_visual (screen);
	if (visual == NULL)
		visual = gdk_screen_get_system_visual (screen);
	gtk_widget_set_visual (GTK_WIDGET (win), visual);

	gtk_widget_set_events(win,GDK_ALL_EVENTS_MASK);

	/*
	 * create main vbox
	 */
	vbox_main=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(win),vbox_main);

	/*
	 * hbox_title
	 */
	 		
 	hbox_title=gtk_hbox_new(FALSE,0);
 	gtk_box_pack_start(GTK_BOX(vbox_main),hbox_title,FALSE,FALSE,0);

 	button_close=gtk_button_new();
 	gtk_widget_set_size_request(button_close, 16, 16);
 	gtk_box_pack_end(GTK_BOX(hbox_title),button_close,FALSE,FALSE,0);
 	button_max=gtk_button_new();
 	gtk_widget_set_size_request(button_max, 16,16);
 	gtk_box_pack_end(GTK_BOX(hbox_title),button_max,FALSE,FALSE,0); 
 	button_min=gtk_button_new();
 	gtk_widget_set_size_request(button_min, 16,16);
 	gtk_box_pack_end(GTK_BOX(hbox_title),button_min,FALSE,FALSE,0); 
 	button_mini=gtk_button_new();
 	gtk_widget_set_size_request(button_mini, 16,16);
 	gtk_box_pack_end(GTK_BOX(hbox_title),button_mini,FALSE,FALSE,0);

	/*
	 * the second part: notebook
	 */
	notebook_session=gtk_notebook_new();
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook_session), TRUE);
	gtk_box_pack_start(GTK_BOX(vbox_main),notebook_session,TRUE,TRUE,0);
	

	/*
	 * the third part: control area
	 */
	hbox_control=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox_main),hbox_control,FALSE,FALSE,0);

	th=ly_ui_theme_get("fixed_control_left");
	if(!th)
	{
		th=(lyUiThemeTheme *)g_malloc(sizeof(lyUiThemeTheme));
		
		th->pos[0]=th->pos[1]=0;
		th->size[0]=120;
		th->size[1]=70;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->background, "_", sizeof(th->background));
		g_strlcpy(th->font, "_", sizeof(th->font));
		th->fg_color0[0]=th->fg_color0[1]=th->fg_color0[2]=0;
		th->fg_color1[0]=th->fg_color1[1]=th->fg_color1[2]=0;
		th->bg_color0[0]=th->bg_color0[1]=th->bg_color0[2]=0;
		th->bg_color1[0]=th->bg_color1[1]=th->bg_color1[2]=0;
		th->fg_color0[3]=th->fg_color1[3]=th->bg_color0[3]=th->bg_color1[3]=1;
		ly_ui_theme_set("fixed_control_left",th);
	}
	fixed_control_left=gtk_fixed_new();
	gtk_widget_set_size_request(fixed_control_left,th->size[0],th->size[1]);
	gtk_box_pack_start(GTK_BOX(hbox_control),fixed_control_left,FALSE,FALSE,0);

	th=ly_ui_theme_get("button_prev");
	if(!th)
	{
		th=(lyUiThemeTheme *)g_malloc(sizeof(lyUiThemeTheme));
		
		th->pos[0]=10;
		th->pos[1]=20;
		th->size[0]=50;
		th->size[1]=50;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->background, "_", sizeof(th->background));
		g_strlcpy(th->font, "_", sizeof(th->font));
		th->fg_color0[0]=th->fg_color0[1]=th->fg_color0[2]=0;
		th->fg_color1[0]=th->fg_color1[1]=th->fg_color1[2]=0;
		th->bg_color0[0]=th->bg_color0[1]=th->bg_color0[2]=0;
		th->bg_color1[0]=th->bg_color1[1]=th->bg_color1[2]=0;
		th->fg_color0[3]=th->fg_color1[3]=th->bg_color0[3]=th->bg_color1[3]=1;
		ly_ui_theme_set("button_prev",th);
	}
	button_prev=gtk_button_new();
	gtk_widget_set_size_request(button_prev,th->size[0],th->size[1]);
	gtk_fixed_put(GTK_FIXED(fixed_control_left),button_prev,th->pos[0],th->pos[1]);
	
	th=ly_ui_theme_get("button_next");
	if(!th)
	{
		th=(lyUiThemeTheme *)g_malloc(sizeof(lyUiThemeTheme));
		
		th->pos[0]=110;
		th->pos[1]=20;
		th->size[0]=50;
		th->size[1]=50;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->background, "_", sizeof(th->background));
		g_strlcpy(th->font, "_", sizeof(th->font));
		th->fg_color0[0]=th->fg_color0[1]=th->fg_color0[2]=0;
		th->fg_color1[0]=th->fg_color1[1]=th->fg_color1[2]=0;
		th->bg_color0[0]=th->bg_color0[1]=th->bg_color0[2]=0;
		th->bg_color1[0]=th->bg_color1[1]=th->bg_color1[2]=0;
		th->fg_color0[3]=th->fg_color1[3]=th->bg_color0[3]=th->bg_color1[3]=1;
		ly_ui_theme_set("button_next",th);
	}
	button_next=gtk_button_new();
	gtk_widget_set_size_request(button_next,th->size[0],th->size[1]);
	gtk_fixed_put(GTK_FIXED(fixed_control_left),button_next,th->pos[0],th->pos[1]);

	th=ly_ui_theme_get("button_play");
	if(!th)
	{
		th=(lyUiThemeTheme *)g_malloc(sizeof(lyUiThemeTheme));
		
		th->pos[0]=50;
		th->pos[1]=10;
		th->size[0]=70;
		th->size[1]=70;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->background, "_", sizeof(th->background));
		g_strlcpy(th->font, "_", sizeof(th->font));
		th->fg_color0[0]=th->fg_color0[1]=th->fg_color0[2]=0;
		th->fg_color1[0]=th->fg_color1[1]=th->fg_color1[2]=0;
		th->bg_color0[0]=th->bg_color0[1]=th->bg_color0[2]=0;
		th->bg_color1[0]=th->bg_color1[1]=th->bg_color1[2]=0;
		th->fg_color0[3]=th->fg_color1[3]=th->bg_color0[3]=th->bg_color1[3]=1;
		ly_ui_theme_set("button_play",th);
	}
	button_play=gtk_toggle_button_new();
	gtk_widget_set_size_request(button_play,70,70);
	gtk_fixed_put(GTK_FIXED(fixed_control_left),button_play,th->pos[0],th->pos[1]);

	hscale_seek=gtk_hscale_new_with_range(0,1,0.0001);
	gtk_widget_set_size_request(hscale_seek, 150, -1);
	gtk_scale_set_draw_value(GTK_SCALE(hscale_seek),FALSE);
	ly_ui_win_flag_seek=FALSE;
	gtk_box_pack_start(GTK_BOX(hbox_control),hscale_seek,TRUE,TRUE,10);

	th=ly_ui_theme_get("fixed_control_right");
	if(!th)
	{
		th=(lyUiThemeTheme *)g_malloc(sizeof(lyUiThemeTheme));
		
		th->pos[0]=th->pos[1]=0;
		th->size[0]=90;
		th->size[1]=70;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->background, "_", sizeof(th->background));
		g_strlcpy(th->font, "_", sizeof(th->font));
		th->fg_color0[0]=th->fg_color0[1]=th->fg_color0[2]=0;
		th->fg_color1[0]=th->fg_color1[1]=th->fg_color1[2]=0;
		th->bg_color0[0]=th->bg_color0[1]=th->bg_color0[2]=0;
		th->bg_color1[0]=th->bg_color1[1]=th->bg_color1[2]=0;
		th->fg_color0[3]=th->fg_color1[3]=th->bg_color0[3]=th->bg_color1[3]=1;
		ly_ui_theme_set("fixed_control_right",th);
	}
	fixed_control_right=gtk_fixed_new();
	gtk_widget_set_size_request(fixed_control_right,th->size[0],th->size[1]);
	gtk_box_pack_start(GTK_BOX(hbox_control),fixed_control_right,FALSE,FALSE,0);
	
	th=ly_ui_theme_get("button_config");
	if(!th)
	{
		th=(lyUiThemeTheme *)g_malloc(sizeof(lyUiThemeTheme));
		
		th->pos[0]=10;
		th->pos[1]=30;
		th->size[0]=37;
		th->size[1]=27;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->background, "_", sizeof(th->background));
		g_strlcpy(th->font, "_", sizeof(th->font));
		th->fg_color0[0]=th->fg_color0[1]=th->fg_color0[2]=0;
		th->fg_color1[0]=th->fg_color1[1]=th->fg_color1[2]=0;
		th->bg_color0[0]=th->bg_color0[1]=th->bg_color0[2]=0;
		th->bg_color1[0]=th->bg_color1[1]=th->bg_color1[2]=0;
		th->fg_color0[3]=th->fg_color1[3]=th->bg_color0[3]=th->bg_color1[3]=1;
		ly_ui_theme_set("button_config",th);
	}
	button_config=gtk_button_new();
	gtk_widget_set_size_request(button_config,th->size[0],th->size[1]);
	gtk_fixed_put(GTK_FIXED(fixed_control_right),button_config,th->pos[0],th->pos[1]);
	
	th=ly_ui_theme_get("button_volume");
	if(!th)
	{
		th=(lyUiThemeTheme *)g_malloc(sizeof(lyUiThemeTheme));
		
		th->pos[0]=45;
		th->pos[1]=28;
		th->size[0]=37;
		th->size[1]=27;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->background, "", sizeof(th->background));
		g_strlcpy(th->font, "", sizeof(th->font));
		th->fg_color0[0]=th->fg_color0[1]=th->fg_color0[2]=0;
		th->fg_color1[0]=th->fg_color1[1]=th->fg_color1[2]=0;
		th->bg_color0[0]=th->bg_color0[1]=th->bg_color0[2]=0;
		th->bg_color1[0]=th->bg_color1[1]=th->bg_color1[2]=0;
		th->fg_color0[3]=th->fg_color1[3]=th->bg_color0[3]=th->bg_color1[3]=1;
		ly_ui_theme_set("button_volume",th);
	}
	button_volume=gtk_volume_button_new();
	gtk_widget_set_size_request(button_volume,th->size[0],th->size[1]);
	gtk_fixed_put(GTK_FIXED(fixed_control_right),button_volume,th->pos[0],th->pos[1]);
	
	gtk_scale_button_set_value(GTK_SCALE_BUTTON(button_volume), ly_audio_get_volume());


	/*
	 * set widget names
	 */
	gtk_widget_set_name(win,"ly_win");
	gtk_widget_set_name(button_close,"ly_btn_close");
	gtk_widget_set_name(button_min,"ly_btn_min");
	gtk_widget_set_name(button_max,"ly_btn_max");
	gtk_widget_set_name(button_mini,"ly_btn_mini");
	gtk_widget_set_name(button_play,"ly_btn_play");
	gtk_widget_set_name(button_prev,"ly_btn_prev");
	gtk_widget_set_name(button_next,"ly_btn_next");
	gtk_widget_set_name(button_config,"ly_btn_config");
	gtk_widget_set_name(button_volume,"ly_btn_volume");
	gtk_widget_set_name(notebook_session,"ly_notebook_session");
	gtk_widget_set_name(hscale_seek,"ly_hscale_seek");
	gtk_widget_set_name(hbox_title,"ly_hbox_title");
	gtk_widget_set_name(hbox_control,"ly_hbox_control");
	gtk_widget_show_all(win);

	/*
	 * bind signal
	 */
	g_signal_connect(G_OBJECT(win),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(win),"draw", G_CALLBACK(ly_ui_win_expose_cb),NULL);
	g_signal_connect(G_OBJECT(win), "button_press_event", G_CALLBACK(ly_ui_win_drag_cb), win);
	g_signal_connect(G_OBJECT(win), "motion_notify_event", G_CALLBACK(ly_ui_win_drag_cb), win);
	g_signal_connect(G_OBJECT(win), "button_release_event", G_CALLBACK(ly_ui_win_drag_cb), win);
 	g_signal_connect(G_OBJECT(button_close),"clicked",G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(button_play),"clicked",G_CALLBACK(ly_ui_win_play_cb),NULL);
	g_signal_connect(G_OBJECT(button_prev),"clicked",G_CALLBACK(ly_ui_win_prev_cb),NULL);
	g_signal_connect(G_OBJECT(button_next),"clicked",G_CALLBACK(ly_ui_win_next_cb),NULL);
	g_signal_connect(G_OBJECT(button_volume),"value-changed",G_CALLBACK(ly_ui_win_volume_cb),NULL);
	g_signal_connect(G_OBJECT(button_config),"clicked",G_CALLBACK(ly_ui_win_config_cb),NULL);
	
	g_timeout_add(1000,ly_ui_win_update_hscale_cb,NULL);
	g_signal_connect(G_OBJECT(hscale_seek),"button_press_event",G_CALLBACK(ly_ui_win_seek_cb),NULL);
	g_signal_connect(G_OBJECT(hscale_seek),"button_release_event",G_CALLBACK(ly_ui_win_seek_cb),NULL);

	/*
	 * bind custom signal
	 */
	ly_msg_bind("meta_changed","core:audio",ly_ui_win_update_meta_cb,NULL);
	ly_msg_bind("play","core:audio",ly_ui_win_update_button_cb,"play");
	ly_msg_bind("pause","core:audio",ly_ui_win_update_button_cb,"pause");
	ly_msg_bind("stop","core:audio",ly_ui_win_update_button_cb,"stop");
	
	/*
	 * bind keys
	 */
	gtk_window_add_accel_group(GTK_WINDOW(win),ly_ui_key_accel);
	
	ly_ui_key_set_default_if_not_exist("close");
	ly_ui_key_set_default_if_not_exist("max");
	ly_ui_key_set_default_if_not_exist("min");
	ly_ui_key_set_default_if_not_exist("mini");
	ly_ui_key_set_default_if_not_exist("play");
	ly_ui_key_set_default_if_not_exist("prev");
	ly_ui_key_set_default_if_not_exist("next");
	ly_ui_key_set_default_if_not_exist("config");
	ly_ui_key_set_default_if_not_exist("volume+");
	ly_ui_key_set_default_if_not_exist("volume-");
	ly_ui_key_set_default_if_not_exist("seek+");
	ly_ui_key_set_default_if_not_exist("seek-");
	
	ly_ui_key_set_args("close", KEY_BIND_SIGNAL, button_close,"clicked");
	ly_ui_key_set_args("max", KEY_BIND_SIGNAL, button_max,"clicked");
	ly_ui_key_set_args("min", KEY_BIND_SIGNAL, button_min,"clicked");
	ly_ui_key_set_args("mini", KEY_BIND_SIGNAL, button_mini,"clicked");
	ly_ui_key_set_args("play", KEY_BIND_SIGNAL, button_play,"clicked");
	ly_ui_key_set_args("prev", KEY_BIND_SIGNAL, button_prev,"clicked");
	ly_ui_key_set_args("next", KEY_BIND_SIGNAL, button_next,"clicked");
	ly_ui_key_set_args("config", KEY_BIND_SIGNAL, button_config,"clicked");
	ly_ui_key_set_args("volume+", KEY_BIND_CALLBACK, G_CALLBACK(ly_ui_win_change_volume_cb),"+");
	ly_ui_key_set_args("volume-", KEY_BIND_CALLBACK, G_CALLBACK(ly_ui_win_change_volume_cb),"-");
	ly_ui_key_set_args("seek+", KEY_BIND_CALLBACK, G_CALLBACK(ly_ui_win_change_seek_cb),"+");
	ly_ui_key_set_args("seek-", KEY_BIND_CALLBACK, G_CALLBACK(ly_ui_win_change_seek_cb),"-");
	
	ly_ui_key_bind("close");
	ly_ui_key_bind("max");
	ly_ui_key_bind("min");
	ly_ui_key_bind("mini");
	ly_ui_key_bind("play");
	ly_ui_key_bind("prev");
	ly_ui_key_bind("next");
	ly_ui_key_bind("config");
	ly_ui_key_bind("volume+");
	ly_ui_key_bind("volume-");
	ly_ui_key_bind("seek+");
	ly_ui_key_bind("seek-");
	
	/*
	 * set outer API
	 */
	ly_ui_win_window=(lyUiWinWindow*)g_malloc(sizeof(lyUiWinWindow));
	ly_ui_win_window->win=win;
	ly_ui_win_window->vbox_main=vbox_main;
	ly_ui_win_window->hbox_title=hbox_title;
// 	ly_ui_win_window->icon=icon;
	ly_ui_win_window->notebook_session=notebook_session;
	ly_ui_win_window->hbox_control=hbox_control;
	ly_ui_win_window->fixed_control_left=fixed_control_left;
	ly_ui_win_window->fixed_control_right=fixed_control_right;
	ly_ui_win_window->button_play=button_play;
	ly_ui_win_window->button_prev=button_prev;
	ly_ui_win_window->button_next=button_next;
	ly_ui_win_window->button_config=button_config;
	ly_ui_win_window->button_volume=button_volume;
 	ly_ui_win_window->button_close=button_close;
 	ly_ui_win_window->button_max=button_max;
 	ly_ui_win_window->button_min=button_min;
 	ly_ui_win_window->button_mini=button_mini;
	ly_ui_win_window->hscale_seek=hscale_seek;
	
	return TRUE;
}

gboolean ly_ui_win_finalize()
{
	ly_global_debug("info",_("Finalize ui module: WIN ...\n"));
	return TRUE;
}

gboolean ly_ui_win_quit_cb(GtkWidget *widget, gpointer data)
{
	g_main_loop_quit((GMainLoop*)data);
	return TRUE;
}

gboolean ly_ui_win_expose_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	gchar path[1024];
	g_snprintf(path,sizeof(path),"%sui/icon/core.png",LY_GLOBAL_PROGDIR);

/*	TODO:这里有段错误
	GdkPixbuf *pixbuf;
	unsigned char *pixel;
	int width,height,stride;
	
	gtk_window_get_size(GTK_WINDOW(widget),&width,&height);
	printf("%d:%d\n", width,height);
	pixbuf = gdk_pixbuf_new_from_file_at_size(path, width, height, NULL);
	pixel = gdk_pixbuf_get_pixels(pixbuf);
	stride = gdk_pixbuf_get_rowstride(pixbuf);
	image = cairo_image_surface_create_for_data(pixel,CAIRO_FORMAT_ARGB32,width,height,stride);
*/
	image=cairo_image_surface_create_from_png(path);
	cairo_set_source_surface(cr, image, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	cairo_surface_destroy(image);
	image=NULL;
	return FALSE;
}

gboolean ly_ui_win_drag_cb(GtkWidget * widget,GdkEventButton * event, gpointer data)
{
	if (event->button == 1)
	{	
		switch(event->type)
		{
			case GDK_BUTTON_PRESS:
				nX = event->x;
				nY = event->y;
				drag = TRUE;
				
				//TODO:鼠标变成手形
				break;
        		case GDK_BUTTON_RELEASE:
				drag=FALSE;
				//TODO:鼠标变成默认箭头
				break;
			default:
				break;
		}
	}
	if(drag)
	{
		int x, y;
		GtkWidget *window = widget;
		gtk_window_get_position(GTK_WINDOW(window), &x, &y);
		gtk_window_move(GTK_WINDOW(window), x + event->x - nX,y + event->y - nY);
	}
        return FALSE;
}

gboolean ly_ui_win_play_cb(GtkWidget *widget, gpointer data)
{
	GstState state;
	state=ly_audio_get_state();
	if(state==GST_STATE_PLAYING)
		ly_audio_pause();
	else
		ly_audio_play();
	return FALSE;
}

gboolean ly_ui_win_next_cb(GtkWidget *widget, gpointer data)
{
	ly_audio_next();
	return FALSE;
}

gboolean ly_ui_win_prev_cb(GtkWidget *widget, gpointer data)
{
	ly_audio_prev();
	return FALSE;
}

gboolean ly_ui_win_volume_cb(GtkScaleButton *button,gdouble  value,gpointer   data)
{
	ly_audio_set_volume(value);
	return FALSE;
}

gboolean ly_ui_win_change_volume_cb (GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data)
{
	gdouble inc=0;
	if(g_str_equal((gchar*)data,"+"))
		inc=0.1;
	else if(g_str_equal((gchar*)data,"-"))
		inc=-0.1;
	gdouble volume=ly_audio_get_volume();
	volume+=inc;
	if(volume<=0)
		volume=0;
	if(volume>=1)
		volume=1;
	ly_audio_set_volume(volume);
	gtk_scale_button_set_value(GTK_SCALE_BUTTON(ly_ui_win_window->button_volume), ly_audio_get_volume());
	return FALSE;
}
gboolean ly_ui_win_seek_cb(GtkWidget * widget, GdkEventButton * event, gpointer data)
{
	gdouble val;
	if (event->button == 1)	/* left button */
	{
		switch(event->type)
		{
			case GDK_BUTTON_PRESS:		/* press */
				ly_ui_win_flag_seek=TRUE;
				break;
			case GDK_BUTTON_RELEASE:	/* release */
				val=gtk_range_get_value(GTK_RANGE(ly_ui_win_window->hscale_seek));
				ly_audio_set_position(val);
				ly_ui_win_flag_seek=FALSE;
				break;
			default:
				break;
		}
	}
	return FALSE;
}

gboolean ly_ui_win_change_seek_cb (GtkAccelGroup *accel_group,GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer data)
{
	gdouble inc=0;
	if(g_str_equal((gchar*)data,"+"))
		inc=0.05;
	else if(g_str_equal((gchar*)data,"-"))
		inc=-0.05;
	gdouble position=ly_audio_get_position();
	position+=inc;
	if(position<=0)
		position=0;
	if(position>=1)
		position=1;
	ly_audio_set_position(position);
	gtk_range_set_value(GTK_RANGE(ly_ui_win_window->hscale_seek),position);
	return FALSE;
}
gboolean ly_ui_win_update_hscale_cb(gpointer data)
{
	if(ly_ui_win_flag_seek==FALSE)
	{
		gdouble position=ly_audio_get_position();
		gtk_range_set_value(GTK_RANGE(ly_ui_win_window->hscale_seek),position);
	}
	return TRUE;
}

gboolean ly_ui_win_update_meta_cb(gpointer message, gpointer data)
{
	return FALSE;
}

gboolean ly_ui_win_update_button_cb(gpointer message, gpointer data)
{
	gchar *signal=data;
	if(g_str_equal(signal,"play") && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ly_ui_win_window->button_play)))
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ly_ui_win_window->button_play),TRUE);
	}
	else if((g_str_equal(signal,"pause")||g_str_equal(signal,"stop")) && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ly_ui_win_window->button_play)))
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ly_ui_win_window->button_play),FALSE);
	}
	return FALSE;
}

gboolean ly_ui_win_config_cb(GtkWidget *widget, gpointer data)
{
	ly_msg_put("create_config","ui:win",NULL);
	return FALSE;
}
