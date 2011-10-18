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
int timeout_id=0;

/*
 * FUNCTIONS
 */
gboolean ly_ui_win_init()
{
	ly_global_debug("info",_("Initial ui module: WIN ...\n"));
	
	/*
	 * Load Theme
	 */
	GdkScreen *screen;
	lyUiThemeItem *th=NULL;
	th=ly_ui_theme_get("gtk3");
	if(th && !g_str_equal(th->bg, ""))
	{
		screen = gdk_screen_get_default();
		GtkCssProvider *provider = gtk_css_provider_new();
		gtk_css_provider_load_from_path(provider, th->bg, NULL);
		gtk_style_context_add_provider_for_screen(screen,GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);
		gtk_style_context_reset_widgets(screen);
		g_object_unref(provider);
	}

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
	GtkWidget *img;
	
	char path[1024]="";

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
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		
		th->pos[0]=100;
		th->pos[1]=100;
		th->size[0]=800;
		th->size[1]=600;
		th->decorated=1;
		th->border=10;
		g_strlcpy(th->bg, "", sizeof(th->bg));
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

	th=ly_ui_theme_get("button_close");
	if(!th)
	{
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		
		th->pos[0]=0;
		th->pos[1]=0;
		th->size[0]=16;
		th->size[1]=16;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("button_close",th);
	}
 	button_close=gtk_button_new();
 	gtk_widget_set_size_request(button_close, th->size[0], th->size[1]);
 	gtk_box_pack_end(GTK_BOX(hbox_title),button_close,FALSE,FALSE,0);
 	th=ly_ui_theme_get("button_max");
	if(!th)
	{
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		
		th->pos[0]=0;
		th->pos[1]=0;
		th->size[0]=16;
		th->size[1]=16;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("button_max",th);
	}
 	button_max=gtk_button_new();
 	gtk_widget_set_size_request(button_max, th->size[0], th->size[1]);
 	gtk_box_pack_end(GTK_BOX(hbox_title),button_max,FALSE,FALSE,0); 
 	th=ly_ui_theme_get("button_min");
	if(!th)
	{
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		
		th->pos[0]=0;
		th->pos[1]=0;
		th->size[0]=16;
		th->size[1]=16;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("button_min",th);
	}
 	button_min=gtk_button_new();
 	gtk_widget_set_size_request(button_min, th->size[0], th->size[1]);
 	gtk_box_pack_end(GTK_BOX(hbox_title),button_min,FALSE,FALSE,0);
 	th=ly_ui_theme_get("button_mini");
	if(!th)
	{
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		
		th->pos[0]=0;
		th->pos[1]=0;
		th->size[0]=16;
		th->size[1]=16;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("button_mini",th);
	}
 	button_mini=gtk_button_new();
 	gtk_widget_set_size_request(button_mini, th->size[0], th->size[1]);
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
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		th->pos[0]=0;
		th->pos[1]=0;
		th->size[0]=120;
		th->size[1]=70;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("fixed_control_left",th);
	}
	fixed_control_left=gtk_fixed_new();
	gtk_widget_set_size_request(fixed_control_left,th->size[0],th->size[1]);
	gtk_box_pack_start(GTK_BOX(hbox_control),fixed_control_left,FALSE,FALSE,0);

	button_prev=gtk_button_new();
	th=ly_ui_theme_get("button_prev");
	if(!th)
	{
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		th->pos[0]=20;
		th->pos[1]=15;
		th->size[0]=40;
		th->size[1]=40;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("button_prev",th);
		
		img=gtk_image_new_from_stock(GTK_STOCK_MEDIA_PREVIOUS,GTK_ICON_SIZE_SMALL_TOOLBAR);
		gtk_container_add(GTK_CONTAINER(button_prev), img);
	}
	
	gtk_widget_set_size_request(button_prev,th->size[0],th->size[1]);
	gtk_fixed_put(GTK_FIXED(fixed_control_left),button_prev,th->pos[0],th->pos[1]);
	
	button_next=gtk_button_new();
	th=ly_ui_theme_get("button_next");
	if(!th)
	{
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		th->pos[0]=100;
		th->pos[1]=15;
		th->size[0]=40;
		th->size[1]=40;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("button_next",th);
		
		img=gtk_image_new_from_stock(GTK_STOCK_MEDIA_NEXT,GTK_ICON_SIZE_SMALL_TOOLBAR);
		gtk_container_add(GTK_CONTAINER(button_next), img);
	}
	gtk_widget_set_size_request(button_next,th->size[0],th->size[1]);
	gtk_fixed_put(GTK_FIXED(fixed_control_left),button_next,th->pos[0],th->pos[1]);

	button_play=gtk_toggle_button_new();
	th=ly_ui_theme_get("button_play");
	if(!th)
	{
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		th->pos[0]=60;
		th->pos[1]=15;
		th->size[0]=40;
		th->size[1]=40;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("button_play",th);
		img=gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY,GTK_ICON_SIZE_SMALL_TOOLBAR);
		gtk_container_add(GTK_CONTAINER(button_play), img);
	}
	gtk_widget_set_size_request(button_play,th->size[0],th->size[1]);
	gtk_fixed_put(GTK_FIXED(fixed_control_left),button_play,th->pos[0],th->pos[1]);

	GtkWidget *vbox;
	vbox=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbox_control),vbox,TRUE,TRUE,0);
	hscale_seek=gtk_hscale_new_with_range(0,1,0.0001);
	gtk_widget_set_size_request(hscale_seek, 150, -1);
	gtk_scale_set_draw_value(GTK_SCALE(hscale_seek),FALSE);
	ly_ui_win_flag_seek=FALSE;
	gtk_box_pack_start(GTK_BOX(vbox),hscale_seek,TRUE,FALSE,10);
		
	th=ly_ui_theme_get("fixed_control_right");
	if(!th)
	{
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		th->pos[0]=th->pos[1]=0;
		th->size[0]=90;
		th->size[1]=70;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("fixed_control_right",th);
	}
	fixed_control_right=gtk_fixed_new();
	gtk_widget_set_size_request(fixed_control_right,th->size[0],th->size[1]);
	gtk_box_pack_start(GTK_BOX(hbox_control),fixed_control_right,FALSE,FALSE,0);

	button_config=gtk_button_new();
	th=ly_ui_theme_get("button_config");
	if(!th)
	{
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		th->pos[0]=10;
		th->pos[1]=15;
		th->size[0]=40;
		th->size[1]=40;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("button_config",th);
		
		img=gtk_image_new_from_stock(GTK_STOCK_PREFERENCES,GTK_ICON_SIZE_SMALL_TOOLBAR);
		gtk_container_add(GTK_CONTAINER(button_config), img);
	}
	gtk_widget_set_size_request(button_config,th->size[0],th->size[1]);
	gtk_fixed_put(GTK_FIXED(fixed_control_right),button_config,th->pos[0],th->pos[1]);
	
	th=ly_ui_theme_get("button_volume");
	if(!th)
	{
		th=(lyUiThemeItem *)g_malloc(sizeof(lyUiThemeItem));
		th->pos[0]=50;
		th->pos[1]=15;
		th->size[0]=40;
		th->size[1]=40;
		th->decorated=0;
		th->border=0;
		g_strlcpy(th->bg, "", sizeof(th->bg));
		ly_ui_theme_set("button_volume",th);
	}
	button_volume=gtk_volume_button_new();
	gtk_widget_set_size_request(button_volume,th->size[0],th->size[1]);
	gtk_fixed_put(GTK_FIXED(fixed_control_right),button_volume,th->pos[0],th->pos[1]);
	
	gtk_scale_button_set_value(GTK_SCALE_BUTTON(button_volume), ly_audio_get_volume());


	//设置图标
	g_snprintf(path, sizeof(path), "%sui/icon/linnya.svg", LY_GLOBAL_PROGDIR);
	GtkStatusIcon *tray_icon  = gtk_status_icon_new_from_file (path);

	//设置右键菜单
	GtkWidget *menu, *menu_item;
	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_label (_("Play/Pause"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_ui_win_play_cb),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Previous"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_ui_win_prev_cb),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Next"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_ui_win_next_cb),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Configuration"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_ui_win_config_cb),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL (menu),menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Show/Hide Main Window"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(ly_ui_win_change_visible_cb),	win);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL (menu),menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Quit"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gtk_main_quit),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show_all (menu);
	//设置提示
	gtk_status_icon_set_tooltip_markup(tray_icon,_("<b>Linnya</b> Audio Player"));

	
	//绑定鼠标事件和处理函数
	g_signal_connect(GTK_STATUS_ICON (tray_icon), "activate", G_CALLBACK (ly_ui_win_change_visible_cb), win);
	g_signal_connect(GTK_STATUS_ICON (tray_icon), "popup-menu", G_CALLBACK (ly_ui_win_tray_popup_cb), menu);


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
 	g_signal_connect(G_OBJECT(button_min),"clicked",G_CALLBACK(ly_ui_win_change_visible_cb), win);
	g_signal_connect(G_OBJECT(button_play),"clicked",G_CALLBACK(ly_ui_win_play_cb),NULL);
	g_signal_connect(G_OBJECT(button_prev),"clicked",G_CALLBACK(ly_ui_win_prev_cb),NULL);
	g_signal_connect(G_OBJECT(button_next),"clicked",G_CALLBACK(ly_ui_win_next_cb),NULL);
	g_signal_connect(G_OBJECT(button_volume),"value-changed",G_CALLBACK(ly_ui_win_volume_cb),NULL);
	g_signal_connect(G_OBJECT(button_config),"clicked",G_CALLBACK(ly_ui_win_config_cb),NULL);
	
	timeout_id = g_timeout_add(1000,ly_ui_win_update_hscale_cb,NULL);
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
	lyUiThemeItem *th=ly_ui_theme_get("win");
	if(!th || g_str_equal(th->bg,""))
	{
		return FALSE;
	}

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

	image=cairo_image_surface_create_from_png(th->bg);
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
		gtk_widget_set_state(ly_ui_win_window->button_play,GTK_STATE_ACTIVE);
	}
	else if((g_str_equal(signal,"pause")||g_str_equal(signal,"stop")) && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ly_ui_win_window->button_play)))
	{
		gtk_widget_set_state(ly_ui_win_window->button_play,GTK_STATE_NORMAL);
	}
	return FALSE;
}

gboolean ly_ui_win_config_cb(GtkWidget *widget, gpointer data)
{
	ly_msg_put("create_config","ui:win",NULL);
	return FALSE;
}


gboolean ly_ui_win_tray_popup_cb(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu)
{
	gtk_menu_popup(GTK_MENU(popUpMenu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
	return FALSE;
}

gboolean ly_ui_win_change_visible_cb(GtkWidget *widget, gpointer data)
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
