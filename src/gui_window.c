

//HEADERS
#include "gui_window.h"

//GLOBAL VARIABLES
gboolean seek=FALSE;

int i=0;
int cn=0;

gboolean drag = FALSE;	// 只在左键按下时拖动窗体
int nX = 0;
int nY = 0;

cairo_surface_t *images = NULL;

//FUNCTIONS
//********************************************************************************
//*	
//*	FUNCTION:		gui_window_new
//*	DISCRIPTION:	define window GUI
//*
//********************************************************************************
gboolean gui_window_new()
{
	GtkWidget *fixed;
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *hpaned;
	GtkWidget *vbox;
	GtkWidget *scrolled_window;

	GtkTreeViewColumn *column;
	GtkCellRenderer *cell_renderer;
	GtkTreeIter iter,subiter;
	
	GdkScreen *screen;
	GdkColormap *colormap;
	
	GdkPixbuf *pixbuf = NULL;
	GdkBitmap *bitmap = NULL;
	GdkPixmap *pixmap = NULL;
	
	gchar *path=NULL;
	_gl_theme *th=NULL;
	
	GList *p;
	_gl_playlist *plink;
	GList *q;
	_gl_metadata *qlink;
	
	//初始化gtkrc
	th=th_get_th_by_name("gtkrc");
	if(th&&th->path)
		gtk_rc_parse(th->path);
	
//--------------------------定义主窗口---------------------------------------//
	gui_window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(gui_window,"musicme_window");
	gtk_window_set_title(GTK_WINDOW(gui_window),"Linnya");
	path=g_strconcat(gl_dir_icon,"linnya.png",NULL);
	gtk_window_set_icon_from_file(GTK_WINDOW(gui_window),path,NULL);
	g_free(path);
	gtk_window_set_position(GTK_WINDOW(gui_window),GTK_WIN_POS_CENTER);
	gtk_widget_set_app_paintable(gui_window, TRUE);

	//设置窗口主题样式
	th=th_get_th_by_name("window");
	if((th)&&(th->w>0)&&(th->h>0))
		gtk_window_set_default_size(GTK_WINDOW(gui_window),th->w,th->h);
	else
		gtk_window_set_default_size(GTK_WINDOW(gui_window),460,370);
	if(gl_theme)
	{
//		gtk_window_set_resizable(GTK_WINDOW(gui_window), FALSE);
		gtk_window_set_decorated(GTK_WINDOW(gui_window), FALSE);
	}
	if(th&&(th->path))
		images = cairo_image_surface_create_from_png(th->path);
	screen = gtk_widget_get_screen(gui_window);			// 重要
	colormap = gdk_screen_get_rgba_colormap(screen);
	//不支持透明的系统，提示
	if(colormap==NULL)
	{
		de_print(__DE_ERROR,__FUNCTION__,_("Your system does not support transparent! Install Compize or Kwin can solve the problem."));
	}
	gtk_widget_set_colormap(gui_window, colormap);

	if(th&&(th->path))
	{
		pixbuf = gdk_pixbuf_new_from_file(th->path, NULL);       // gdk函数读取png文件
		gdk_pixbuf_render_pixmap_and_mask(pixbuf, &pixmap, &bitmap, 5);    // alpha小于5认为全透明
		gtk_widget_shape_combine_mask(gui_window, bitmap, 0, 0);          // 设置透明蒙板

		g_object_unref(pixbuf);
		g_object_unref(bitmap);
		g_object_unref(pixmap);
	}

gtk_widget_set_events(gui_window,GDK_ALL_EVENTS_MASK);
	g_signal_connect(G_OBJECT(gui_window), "expose-event", G_CALLBACK(gui_window_expose), NULL);
	g_signal_connect(G_OBJECT(gui_window), "destroy", G_CALLBACK(gui_window_quit), NULL);
	if(gl_theme)
	{
		g_signal_connect(G_OBJECT(gui_window), 	"button_press_event", G_CALLBACK( gui_window_drag), gui_window);
		g_signal_connect(G_OBJECT(gui_window), 	"motion_notify_event", G_CALLBACK( gui_window_drag), gui_window);
		g_signal_connect(G_OBJECT(gui_window), 	"button_release_event", G_CALLBACK( gui_window_drag), gui_window);
	}
	
//--------------------------定义主框架---------------------------------------//
	fixed=gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(gui_window),fixed);
	
	if(gl_theme)
	{
	//设置icon主题样式
	th=th_get_th_by_name("icon");
	if(th&&th->path)
		gui_window_image_icon=gtk_image_new_from_file(th->path);
	else
	{
		path=g_strconcat(gl_dir_icon,"linnya16x16.png",NULL);
		gui_window_image_icon=gtk_image_new_from_file(path);
		g_free(path);
	}
	if(th&&th->x&&th->y)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_image_icon,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_image_icon,10,5);
	

	//设置button_min的主题样式
	th=th_get_th_by_name("button_min");
	gui_window_button_min=gtk_button_new();
	gtk_widget_set_name(gui_window_button_min,"musicme_button_min");
	if(th)
		gtk_widget_set_size_request(gui_window_button_min, th->w, th->h);
	else
		gtk_widget_set_size_request(gui_window_button_min, 16, 16);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_min,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_min,420,5);
	g_signal_connect(G_OBJECT(gui_window_button_min), "clicked", G_CALLBACK(gui_window_change_visibal),gui_window);
	//设置button_close主题样式
	th=th_get_th_by_name("button_close");
	gui_window_button_close=gtk_button_new();
	gtk_widget_set_name(gui_window_button_close,"musicme_button_close");
	if(th)
		gtk_widget_set_size_request(gui_window_button_close, th->w, th->h);
	else
		gtk_widget_set_size_request(gui_window_button_close, 16, 16);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_close,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_close,435,5);
	g_signal_connect(G_OBJECT(gui_window_button_close),"clicked", G_CALLBACK(gui_window_quit), NULL);
	}
	
	//设置notebook的主题样式
	th=th_get_th_by_name("notebook");
	gui_window_notebook=gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(gui_window_notebook), GTK_POS_TOP);
	gtk_widget_set_name(gui_window_notebook,"musicme_notebook");
	if(th)
		gtk_widget_set_size_request(gui_window_notebook, th->w, th->h);
	else
		gtk_widget_set_size_request(gui_window_notebook, 440, 270);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_notebook,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_notebook,10,25);
	//设置hscale_seek的主题样式
	th=th_get_th_by_name("hscale_seek");
	gui_window_hscale_seek=gtk_hscale_new_with_range(0,100,0.1);
	gtk_scale_set_draw_value(GTK_SCALE(gui_window_hscale_seek),FALSE);
	gtk_range_set_update_policy(GTK_RANGE(gui_window_hscale_seek),GTK_UPDATE_DISCONTINUOUS);
	if(th)
		gtk_widget_set_size_request(gui_window_hscale_seek, th->w, th->h);
	else
		gtk_widget_set_size_request(gui_window_hscale_seek, 435, -1);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_hscale_seek,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_hscale_seek,10,300);
	g_timeout_add(1000,gui_window_update_hscale,NULL);
	g_signal_connect(G_OBJECT(gui_window_hscale_seek),"button_press_event",G_CALLBACK(gui_window_seek),NULL);
	g_signal_connect(G_OBJECT(gui_window_hscale_seek),"button_release_event",G_CALLBACK(gui_window_seek),NULL);

//--------------------------定义控制区域---------------------------------------//
	
	//设置button_play主题样式
	th=th_get_th_by_name("button_play");
	if(gl_theme)
		gui_window_button_play=gtk_toggle_button_new();
	else
		gui_window_button_play=gtk_toggle_button_new_with_label(_("Play"));
	gtk_widget_set_name( gui_window_button_play,"musicme_button_play");
	if(th)
		gtk_widget_set_size_request(gui_window_button_play, th->w, th->h);
	else
		gtk_widget_set_size_request(gui_window_button_play, 50, 25);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_play,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_play,65,325);
	g_signal_connect(G_OBJECT(gui_window_button_play),	"clicked", 	G_CALLBACK(gui_window_play),	NULL);
		
	//设置button_stop主题样式
	th=th_get_th_by_name("button_stop");
	if(gl_theme)
		gui_window_button_stop=gtk_button_new();
	else
		gui_window_button_stop=gtk_button_new_with_label(_("Stop"));
	gtk_widget_set_name(gui_window_button_stop,"musicme_button_stop");
	if(th)
		gtk_widget_set_size_request(gui_window_button_stop, th->w, th->h);
	else
		gtk_widget_set_size_request(gui_window_button_stop, 50, 25);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_stop,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_stop,120,325);
	g_signal_connect(G_OBJECT(gui_window_button_stop),	"clicked",	G_CALLBACK(gui_window_stop),	NULL);
	
	//设置button_prev主题样式
	th=th_get_th_by_name("button_prev");
	if(gl_theme)
		gui_window_button_prev=gtk_button_new();
	else
		gui_window_button_prev=gtk_button_new_with_label(_("Prev"));
	gtk_widget_set_name( gui_window_button_prev,"musicme_button_prev");
	if(th)
		gtk_widget_set_size_request(gui_window_button_prev, th->w, th->h);
	else
		gtk_widget_set_size_request(gui_window_button_prev, 50, 25);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_prev,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_prev,10,325);
	g_signal_connect(G_OBJECT(gui_window_button_prev),	"clicked",	G_CALLBACK(gui_window_prev),	NULL);

	//设置button_next主题样式
	th=th_get_th_by_name("button_next");
	if(gl_theme)
		gui_window_button_next=gtk_button_new();
	else
		gui_window_button_next=gtk_button_new_with_label(_("Next"));
	gtk_widget_set_name( gui_window_button_next,"musicme_button_next");
	if(th)
		gtk_widget_set_size_request(gui_window_button_next, th->w, th->h);
	else
		gtk_widget_set_size_request(gui_window_button_next, 50, 25);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_next,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_next,175,325);
	g_signal_connect(G_OBJECT(gui_window_button_next),	"clicked",	G_CALLBACK(gui_window_next),	NULL);
	
	//设置button_conf主题样式
	th=th_get_th_by_name("button_conf");
	if(gl_theme)
		gui_window_button_conf=gtk_button_new();
	else
		gui_window_button_conf=gtk_button_new_with_label(_("CF"));
	gtk_widget_set_name( gui_window_button_conf,"musicme_button_conf");
	if(th)
		gtk_widget_set_size_request(gui_window_button_conf, th->w, th->h);
	else
		gtk_widget_set_size_request(gui_window_button_conf, 25, 25);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_conf,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_conf,230,325);
	g_signal_connect(G_OBJECT(gui_window_button_conf),	"clicked",	G_CALLBACK(gui_dialog_new),	NULL);
	
	//设置label_volume主题样式
	th=th_get_th_by_name("label_volume");
	if(th&&th->path)
		label=gtk_label_new(th->path);
	else
		label=gtk_label_new(_("Vol."));
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),label,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),label,370,330);

	//设置button_volume主题样式
	th=th_get_th_by_name("button_volume");
	gui_window_button_volume=gtk_volume_button_new();
	gtk_widget_set_name( gui_window_button_volume,"musicme_button_volume");
	gtk_scale_button_set_value(GTK_SCALE_BUTTON(gui_window_button_volume),(gl_conf->volume)/100.0);
	if(th)
		gtk_widget_set_size_request(gui_window_button_volume, th->w, th->h);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_volume,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_button_volume,410,322);
	g_signal_connect(G_OBJECT(gui_window_button_volume),"value-changed",G_CALLBACK(gui_window_set_volume),NULL);
//---------------------------------------------------------------------------------------------------//
//
//定义页面一
//
//---------------------------------------------------------------------------------------------------//
	
	fixed=gtk_fixed_new();
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label),_("Playing"));
	gtk_notebook_append_page(GTK_NOTEBOOK(gui_window_notebook), fixed, label);

	//设置img_cd主题样式
	th=th_get_th_by_name("image_cd");
	if(th&&th->path)
	{
		if(th->w&&th->h)
			gl_pixbuf_cd=gdk_pixbuf_new_from_file_at_size(th->path,th->w,th->h,NULL);
		else
			gl_pixbuf_cd=gdk_pixbuf_new_from_file_at_size(th->path,150,150,NULL);
		gui_window_image_cd=gtk_image_new_from_pixbuf(pixbuf);
	}
	else
	{
		path=g_strconcat(gl_dir_icon,"cd.png",NULL);
		if(th&&th->w&&th->h)
			gl_pixbuf_cd=gdk_pixbuf_new_from_file_at_size(path,th->w,th->h,NULL);
		else
			gl_pixbuf_cd=gdk_pixbuf_new_from_file_at_size(path,150,150,NULL);
		g_free(path);
		gui_window_image_cd=gtk_image_new_from_pixbuf(gl_pixbuf_cd);
	}
	if(th&&th->x&&th->y)
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_image_cd,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),gui_window_image_cd,10,40);

	
	//设置table主题样式
	th=th_get_th_by_name("table");
	table=gtk_table_new(2, 4,FALSE);
	if(th&&th->x>=0&&th->y>=0)
		gtk_fixed_put(GTK_FIXED(fixed),table,th->x,th->y);
	else
		gtk_fixed_put(GTK_FIXED(fixed),table,170,60);
	
	label=gtk_label_new(_("Title:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Artist:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Album:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	label=gtk_label_new(_("Duration:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	gui_window_label_title=gtk_label_new(_("Unknown Title"));
	gtk_misc_set_alignment(GTK_MISC(gui_window_label_title),0,0);
	gtk_label_set_ellipsize(GTK_LABEL(gui_window_label_title),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(gui_window_label_title),20);
	gtk_table_attach(GTK_TABLE(table), gui_window_label_title, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
	gui_window_label_artist=gtk_label_new(_("Unknown Artist"));
	gtk_misc_set_alignment(GTK_MISC(gui_window_label_artist),0,0);
	gtk_label_set_ellipsize(GTK_LABEL(gui_window_label_artist),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(gui_window_label_artist),20);
	gtk_table_attach(GTK_TABLE(table), gui_window_label_artist, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
	gui_window_label_album=gtk_label_new(_("Unknown Album"));
	gtk_misc_set_alignment(GTK_MISC(gui_window_label_album),0,0);
	gtk_label_set_ellipsize(GTK_LABEL(gui_window_label_album),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(gui_window_label_album),20);
	gtk_table_attach(GTK_TABLE(table), gui_window_label_album, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
	gui_window_label_duration=gtk_label_new(_("Unknown Duration"));
	gtk_misc_set_alignment(GTK_MISC(gui_window_label_duration),0,0);
	gtk_label_set_ellipsize(GTK_LABEL(gui_window_label_duration),PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(gui_window_label_duration),20);
	gtk_table_attach(GTK_TABLE(table), gui_window_label_duration, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
	

//---------------------------------------------------------------------------------------------------//
//
//定义页面二
//
//---------------------------------------------------------------------------------------------------//
	hpaned=gtk_hpaned_new();
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label),_("Manager"));
	gtk_notebook_append_page(GTK_NOTEBOOK(gui_window_notebook), hpaned, label);
	
	gl_selecting=gl_lib;
	
	gui_window_pixbuf_playingicon=gdk_pixbuf_new_from_file_at_size(gl_file_playingicon,25,25,NULL);
	vbox=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(hpaned), vbox);
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window,TRUE,TRUE,0);
	gui_window_tree_left=gtk_tree_view_new();
	gtk_container_add(GTK_CONTAINER(scrolled_window),gui_window_tree_left);
	
	//store_left 由两个域组成，GDK_TYPE_PIXBUF的图片、G_TYPE_STRING的名称
	gui_window_store_left = gtk_tree_store_new (2,GDK_TYPE_PIXBUF,G_TYPE_STRING);
	
	gtk_tree_store_append (gui_window_store_left, &iter,NULL);
	gtk_tree_store_set(gui_window_store_left, &iter,0,NULL,1,_("Library"),-1);
	gtk_tree_store_append (gui_window_store_left,&subiter, &iter);
	gtk_tree_store_set(gui_window_store_left, &subiter,0,NULL,1,_("All Tracks"),-1);
	
	gtk_tree_store_append (gui_window_store_left, &iter,NULL);
	switch(gl_conf->sublib)
	{
		case 0:
			gtk_tree_store_set(gui_window_store_left, &iter,0,NULL,1,_("No Sublib"),-1);
			break;
		case 1:
			gtk_tree_store_set(gui_window_store_left, &iter,0,NULL,1,_("Artists"),-1);
			break;
		case 2:
			gtk_tree_store_set(gui_window_store_left, &iter,0,NULL,1,_("Albums"),-1);
			break;
		default:
			break;
	}
	p=gl_sublib;
	while(p)
	{
		plink=p->data;
		gtk_tree_store_append (gui_window_store_left,&subiter, &iter);
		gtk_tree_store_set(gui_window_store_left, &subiter,0,NULL,1,plink->name,-1);
		p=p->next;
	}
	
	gtk_tree_store_append(gui_window_store_left, &iter,NULL);
	gtk_tree_store_set(gui_window_store_left, &iter,0,NULL,1,_("Playlist"),-1);
	p=gl_playlist;
	while(p)
	{
		plink=p->data;
		gtk_tree_store_append (gui_window_store_left,&subiter, &iter);
		gtk_tree_store_set(gui_window_store_left, &subiter,0,NULL,1,plink->name,-1);
		p=p->next;
	}
	
	column = gtk_tree_view_column_new();
	cell_renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_start(column, cell_renderer,FALSE);
	gtk_tree_view_column_add_attribute(column,cell_renderer,"pixbuf",0);
	cell_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, cell_renderer,FALSE);
	gtk_tree_view_column_add_attribute(column,cell_renderer,"text",1);
	gtk_tree_view_column_set_title(column,_("Manager"));
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_window_tree_left), column);
	gtk_tree_view_set_model(GTK_TREE_VIEW (gui_window_tree_left), GTK_TREE_MODEL(gui_window_store_left));
	gui_window_selection_left=gtk_tree_view_get_selection(GTK_TREE_VIEW(gui_window_tree_left));
	
	vbox=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(hpaned), vbox);
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window,TRUE,TRUE,0);
	gui_window_tree_right=gtk_tree_view_new();
	gtk_container_add(GTK_CONTAINER(scrolled_window),gui_window_tree_right);
	
	//store_right 由四个域组成，GDK_TYPE_PIXBUF的图片、G_TYPE_STRING的名称、G_TYPE_STRING的艺术家、G_TYPE_STRING的专辑名
	gui_window_store_right = gtk_tree_store_new (4,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);

	q=((_gl_playlist *)(gl_lib->data))->list;
	while(q)
	{
		qlink=q->data;
		gtk_tree_store_append (gui_window_store_right, &iter,NULL);
		gtk_tree_store_set(gui_window_store_right, &iter,
				0,NULL,
				1,qlink->title,
				2,qlink->artist,
				3,qlink->album,
				-1);
		q=q->next;
	}
	
	cell_renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes(_("*"),cell_renderer,"pixbuf",0,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_window_tree_right), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Title"),cell_renderer,"text",1,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),50);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_window_tree_right), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Artist"),cell_renderer,"text",2,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),50);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_window_tree_right), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Album"),cell_renderer,"text",3,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_column_set_min_width  (GTK_TREE_VIEW_COLUMN(column),50);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_window_tree_right), column);
	gtk_tree_view_set_model(GTK_TREE_VIEW (gui_window_tree_right), GTK_TREE_MODEL(gui_window_store_right));
	gui_window_selection_right=gtk_tree_view_get_selection(GTK_TREE_VIEW(gui_window_tree_right));
	gtk_tree_selection_set_mode(gui_window_selection_right,GTK_SELECTION_MULTIPLE);

// 	//设置dnd
// 	static GtkTargetEntry entry[2];   
// 	entry[0].target = "MusicMe/ListItem";
// 	entry[0].flags = GTK_TARGET_SAME_WIDGET;
// 	entry[0].info = 0;
// 	entry[1].target = "MusicMe/MusicItem";
// 	entry[1].flags = GTK_TARGET_SAME_APP;
// 	entry[1].info = 1;
// 
// 	gtk_tree_view_enable_model_drag_source(GTK_TREE_VIEW(gui_window_tree_right),
// 						GDK_BUTTON1_MASK, entry, 1, GDK_ACTION_MOVE);
// 	gtk_tree_view_enable_model_drag_dest(GTK_TREE_VIEW(gui_window_tree_right),
// 						entry, 2, GDK_ACTION_COPY|GDK_ACTION_MOVE|GDK_ACTION_LINK);

//---------------------------------------------------------------------------------------------------//
//
//定义页面三
//
//---------------------------------------------------------------------------------------------------//

	gui_window_eventbox_lrc=gtk_drawing_area_new();
	gtk_widget_set_events(gui_window_eventbox_lrc,GDK_ALL_EVENTS_MASK);
	gtk_widget_set_name(gui_window_eventbox_lrc,"drawingarealyrics");
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("Lyrics"));
	gtk_notebook_append_page(GTK_NOTEBOOK(gui_window_notebook), gui_window_eventbox_lrc, label);

	//设置样式
	th=th_get_th_by_name("desktop");
	
	//创建桌面歌词窗口
	gui_window_window_lrc=gtk_window_new(GTK_WINDOW_POPUP);
	if(th&&th->w&&th->h)
		gtk_window_set_default_size(GTK_WINDOW(gui_window_window_lrc),th->w,th->h);
	else
		gtk_window_set_default_size(GTK_WINDOW(gui_window_window_lrc),1000,50);
	gtk_window_move(GTK_WINDOW(gui_window_window_lrc),gl_conf->desktoplyrics_x,gl_conf->desktoplyrics_y);

	gtk_widget_set_app_paintable(gui_window_window_lrc, TRUE);
	gtk_window_set_decorated(GTK_WINDOW(gui_window_window_lrc), FALSE);
	screen = gtk_widget_get_screen(gui_window_window_lrc);			// 重要
	colormap = gdk_screen_get_rgba_colormap(screen);
	//不支持透明的系统，提示
	if(colormap==NULL)
	{
		de_print(__DE_ERROR,__FUNCTION__,_("Your system do not support transparent! Install Compize or Kwin can solve the problem."));
	}
	gtk_widget_set_colormap(gui_window_window_lrc, colormap);

	gtk_widget_set_events(gui_window_window_lrc,GDK_ALL_EVENTS_MASK);
	if(gl_conf->desktoplyrics_fix)
		gtk_widget_set_sensitive(gui_window_window_lrc,FALSE);

//---------------------------------------------------------------------------------------------------//
//
//设置托盘显示功能，hide模式
//
//---------------------------------------------------------------------------------------------------//
	//设置图标
	path=g_strconcat(gl_dir_icon,"linnya.png",NULL);
	GtkStatusIcon *tray_icon  = gtk_status_icon_new_from_file (path);
	g_free(path);
	//设置右键菜单
	GtkWidget *menu, *menu_item;
	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_label (_("Play/Pause"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gui_window_play),	"TRAY");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Stop"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gui_window_stop),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Previous"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gui_window_prev),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Next"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gui_window_next),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Configuration"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gui_dialog_new),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL (menu),menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Show/Hide Main Window"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gui_window_change_visibal),	gui_window);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Show/Hide Lyrics Window"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gui_window_change_visibal),	gui_window_window_lrc);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	menu_item=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL (menu),menu_item);
	menu_item = gtk_menu_item_new_with_label (_("Quit"));
	g_signal_connect(G_OBJECT(menu_item),	"activate", 	G_CALLBACK(gui_window_quit),	NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show_all (menu);
	//设置提示
	gtk_status_icon_set_tooltip (tray_icon,
								 _("Linnya Audio Player"));
	//绑定鼠标事件和处理函数
	g_signal_connect(GTK_STATUS_ICON (tray_icon), "activate", GTK_SIGNAL_FUNC (gui_window_change_visibal), gui_window);
	g_signal_connect(GTK_STATUS_ICON (tray_icon), "popup-menu", GTK_SIGNAL_FUNC (gui_window_tray_icon_popup), menu);

	gtk_widget_show_all(gui_window_window_lrc);
	gtk_widget_show_all(gui_window);

	if(gl_conf->mainwindow==FALSE)
		gtk_widget_hide(gui_window);
	if(gl_conf->desktoplyrics==FALSE)
		gtk_widget_hide(gui_window_window_lrc);
	
	gui_window_update_display();
	return TRUE;
}



//********************************************************************************
//*	
//*	FUNCTION:		gui_callback_on_window_expose_event
//*	DISCRIPTION:	draw backgroud image of window
//*
//********************************************************************************
gboolean gui_window_expose(GtkWidget * widget,GdkEventExpose * event, gpointer data)
{
	cairo_t *cr;
	cr = gdk_cairo_create(widget->window);
	cairo_set_source_surface(cr, images, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);			// 重要
	cairo_paint(cr);		// 绘图
	cairo_destroy(cr);
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_quit
//*	DISCRIPTION:	process end, destroy all
//*
//********************************************************************************
gboolean gui_window_quit(GtkWidget *widget, gpointer data)
{
	g_mutex_free(gl_mutex_download);
	eq_write_equalizer();
	pl_write_playinglist();
	pl_write_playlist();
	lib_write_sublib();
	lib_write_lib();
	cf_write();
	gtk_main_quit();
	co_quit();
	exit(0);
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_window_drag
//*	DISCRIPTION:	deal with window's drag event
//*
//********************************************************************************
gboolean gui_window_drag(GtkWidget * widget,GdkEventButton * event, gpointer data)
{
	if (event->button == 1)       // 判断是否左键按下
	{	
		//事件循环，设置拖动标志
		switch(event->type)
		{
			case GDK_BUTTON_PRESS:		//按下
				nX = event->x;  	// 取得鼠标相对于窗口的位置
				nY = event->y;
//				if(nY<30)
				{
					drag = TRUE;
				
					//鼠标变成手形
					com_set_cursor(widget,com_cursor_move);
				}
				break;
        		case GDK_BUTTON_RELEASE:	//释放
				drag=FALSE;
				
				//鼠标变成默认箭头
				com_set_cursor(widget,com_cursor_default);
				break;
			default:
				break;
		}
	}
	//实际处理窗口移动
	if(drag)
	{
		int x, y;
		GtkWidget *window = (GtkWidget *) data;
		gtk_window_get_position((GtkWindow *) window, &x, &y);         // 取窗体绝对坐标
		gtk_window_move((GtkWindow *) window, x + event->x - nX,y + event->y - nY); // 移动窗体
	}

        return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_change_visibal
//*	DISCRIPTION:	hide/show main window
//*
//********************************************************************************
gboolean gui_window_change_visibal(GtkWidget *widget, gpointer data)
{
	if(GTK_WIDGET_VISIBLE(GTK_WIDGET(data))==TRUE)
	{
		gtk_widget_hide(GTK_WIDGET(data));
	}
	else
		gtk_widget_show(GTK_WIDGET(data));
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_tray_icon_popup
//*	DISCRIPTION:	show the menu of icon
//*
//********************************************************************************
gboolean gui_window_tray_icon_popup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu)
{
	gtk_menu_popup(GTK_MENU(popUpMenu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:		gui_window_play
//*	DISCRIPTION:	play/pause
//*
//********************************************************************************
gboolean gui_window_play (GtkWidget *widget, gpointer data)
{
	//按动按钮发出的信号
	if(data==NULL)
	{
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gui_window_button_play)))
		{
			/* 如果运行到这里，开关按钮是按下的 */
			co_play();
		}
		else
		{
			;/* 如果运行到这里，开关按钮是弹起的 */
			co_pause();
		}
	}
	//tray菜单发出的信号
	else
	{
		if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gui_window_button_play)))
		{
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gui_window_button_play),FALSE);
		}
		else
		{
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gui_window_button_play),TRUE);
		}
	}
	return FALSE;
}


//********************************************************************************
//*	
//*	FUNCTION:	gui_window_stop
//*	DISCRIPTION:	stop
//*
//********************************************************************************
gboolean gui_window_stop (GtkWidget *widget, gpointer data)
{
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(gui_window_button_play), FALSE);
	co_stop();
	gui_window_update_display();
	return FALSE;
}


//********************************************************************************
//*	
//*	FUNCTION:		gui_window_next
//*	DISCRIPTION:	play next
//*
//********************************************************************************
gboolean gui_window_next (GtkWidget *widget, gpointer data)
{
	co_set_next();
	if(co_get_state()==GST_STATE_PLAYING)
		co_play();
	else
		co_stop();
	gui_window_update_display();
	return FALSE;
}


//********************************************************************************
//*	
//*	FUNCTION:		gui_window_prev
//*	DISCRIPTION:	play previous
//*
//********************************************************************************
gboolean gui_window_prev (GtkWidget *widget, gpointer data)
{
	co_set_prev();
	if(co_get_state()==GST_STATE_PLAYING)
		co_play();
	else
		co_stop();
	gui_window_update_display();
	
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:	gui_window_seek
//*	DISCRIPTION:	update the time of music
//*
//********************************************************************************
gboolean gui_window_seek(GtkWidget * widget,GdkEventButton * event, gpointer data)
{
	gdouble val;
	if (event->button == 1)       // 判断是否左键按下
	{	
		//事件循环，设置拖动标志
		switch(event->type)
		{
			case GDK_BUTTON_PRESS:		//按下
				seek=TRUE;
				break;
			case GDK_BUTTON_RELEASE:	//释放
				val=gtk_range_get_value(GTK_RANGE(gui_window_hscale_seek));
				co_set_position(val/100.0);
				seek=FALSE;
				break;
			default:
				break;
		}
	}
	return FALSE;
}

//********************************************************************************
//*	
//*	FUNCTION:	gui_window_set_volume
//*	DISCRIPTION:	set_volume
//*
//********************************************************************************
gboolean gui_window_set_volume(GtkScaleButton *button,gdouble  value,gpointer   data)
{
	co_set_volume(value);
	return FALSE;
}


//********************************************************************************
//*	
//*	FUNCTION:		gui_window_timeout
//*	DISCRIPTION:	update scale automatically every 1 second
//*
//********************************************************************************
gboolean gui_window_update_hscale(gpointer data)
{
	if(seek==FALSE)
	{
		gdouble position=co_get_position();
		gtk_range_set_value(GTK_RANGE(gui_window_hscale_seek),position*100.0);
	}
	
	return TRUE;
}


//********************************************************************************
//*	
//*	FUNCTION:	gui_update_display
//*	DISCRIPTION:	update metadata to show
//*
//********************************************************************************
gboolean gui_window_update_display()
{
	
	if(!gl_playing)
	{
		de_print(__DE_ERROR,__FUNCTION__,_("gl_playing is null, no music to be played."));
		return FALSE;
	}

	//mp3文件更新meta
	_gl_metadata *md=((_gl_playinglist*)(gl_playing->data))->md;
	if(g_strcmp0(md->start,"00:00:000")==0)		//非cue歌曲，更新TAG
	{
		_gl_metadata *temp_md=md_get_metadata(md->uri);
		if(g_strcmp0(md->title,temp_md->title))
		{
			if(md->title!=NULL)
			 	g_free(md->title);
			 md->title = g_strconcat(temp_md->title,NULL);
		}
		if(g_strcmp0(md->artist,temp_md->artist))
		{
			if(md->artist!=NULL)
				g_free(md->artist);
			md->artist = g_strconcat(temp_md->artist,NULL);
		}
		if(g_strcmp0(md->album,temp_md->album))
		{
			if(md->album!=NULL)
				g_free(md->album);
			md->album = g_strconcat(temp_md->album,NULL);
		}
		gl_free_metadata(temp_md);
		temp_md=NULL;
	}
	//更新标签
	_gl_metadata *plink=((_gl_playinglist*)(gl_playing->data))->md;
	gtk_label_set_text(GTK_LABEL(gui_window_label_title),plink->title);
	gtk_label_set_text(GTK_LABEL(gui_window_label_artist),plink->artist);
	gtk_label_set_text(GTK_LABEL(gui_window_label_album),plink->album);
	gtk_label_set_text(GTK_LABEL(gui_window_label_duration),plink->duration);
	printf("NOW PLAYING : %s - %s\n",plink->title,plink->artist);
	
	//更新专辑封面
	GdkPixbuf *pixbuf=NULL;

	pixbuf=gtk_image_get_pixbuf(GTK_IMAGE(gui_window_image_cd));
	if(pixbuf!=gl_pixbuf_cd)
	{
		gdk_pixbuf_unref(pixbuf);
		gtk_image_set_from_pixbuf(GTK_IMAGE(gui_window_image_cd),gl_pixbuf_cd);

	}
	pixbuf=NULL;

	//更新均衡器
	if(gl_conf->autoeq)
	{
		gl_eq=eq_find_eq_by_name(gl_equalizer,"default");
		eq_set_equalizer();
	}
	
	GList *p;
	GList *q;
	_gl_metadata *qlink;
	gint i=0;
	GtkTreeIter iter;
	gchar *str;
	
	//更新treeview
	if((g_list_first(gl_played)==gl_lib)&&gl_played)
	{
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(gui_window_store_left),&iter,"0:0");
		gtk_tree_store_set(gui_window_store_left,&iter,0,NULL,-1);
	}
	if((g_list_first(gl_played)==gl_sublib)&&gl_played)
	{
		i=g_list_index(gl_sublib,gl_played->data);
		if(i<0)
			return FALSE;
		str=(gchar *)g_malloc(64*sizeof(gchar));
		g_snprintf(str,64,"1:%d",i);
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(gui_window_store_left),&iter,str);
		gtk_tree_store_set(gui_window_store_left,&iter,0,NULL,-1);
		g_free(str);
	}
	if((g_list_first(gl_played)==gl_playlist)&&gl_played)
	{
		i=g_list_index(gl_playlist,gl_played->data);
		if(i<0)
			return FALSE;
		str=(gchar *)g_malloc(64*sizeof(gchar));
		g_snprintf(str,64,"2:%d",i);
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(gui_window_store_left),&iter,str);
		gtk_tree_store_set(gui_window_store_left,&iter,0,NULL,-1);
		g_free(str);
	}
	
	p=pl_find_pl_by_playing();
	if((g_list_first(p)==gl_lib)&&p)
	{
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(gui_window_store_left),&iter,"0:0");
		gtk_tree_store_set(gui_window_store_left,&iter,0,gl_pixbuf_playing,-1);
	}
	if((g_list_first(p)==gl_sublib)&&p)
	{
		i=g_list_index(gl_sublib,p->data);
		if(i<0)
			return FALSE;
		str=(gchar *)g_malloc(64*sizeof(gchar));
		g_snprintf(str,64,"1:%d",i);
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(gui_window_store_left),&iter,str);
		gtk_tree_store_set(gui_window_store_left,&iter,0,gl_pixbuf_playing,-1);
		g_free(str);
	}
	if((g_list_first(p)==gl_playlist)&&p)
	{
		i=g_list_index(gl_playlist,p->data);
		if(i<0)
			return FALSE;
		str=(gchar *)g_malloc(64*sizeof(gchar));
		g_snprintf(str,64,"2:%d",i);
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(gui_window_store_left),&iter,str);
		gtk_tree_store_set(gui_window_store_left,&iter,0,gl_pixbuf_playing,-1);
		g_free(str);
	}
	p=pl_find_pl_by_playing();
	if(gl_selecting==p)
	{
		q=((_gl_playlist *)(gl_selecting->data))->list;
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(gui_window_store_right),&iter,"0");
		while(q)
		{
			qlink=q->data;
			if(qlink==((_gl_playinglist*)(gl_playing->data))->md)
				gtk_tree_store_set(gui_window_store_right,&iter,0,gl_pixbuf_playing,-1);
			else
				gtk_tree_store_set(gui_window_store_right,&iter,0,NULL,-1);
			gtk_tree_model_iter_next(GTK_TREE_MODEL(gui_window_store_right),&iter);
			q=q->next;
		}
	}   
	
	return FALSE;
}


//*****************************************************************************************************
//*
//*	FUNCTION:		gui_window_draw_text
//*	DISCRIPTION:	draw text with pango
//*
//*****************************************************************************************************
gboolean gui_window_draw_text (cairo_t *cr, gchar *text, gchar *font)
{
	PangoLayout *layout;
	PangoFontDescription *desc;

	layout = pango_cairo_create_layout (cr);
	
	pango_layout_set_text (layout, text, -1);
	desc = pango_font_description_from_string (font);
	pango_layout_set_font_description (layout, desc);
	pango_font_description_free (desc);
	
	pango_cairo_update_layout (cr, layout);
	
	pango_cairo_show_layout (cr, layout);
	
	g_object_unref (layout);
	
	return TRUE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		gui_window_draw_text_midx
//*	DISCRIPTION:	draw text with pango, the position of text is the middle of width
//*
//*****************************************************************************************************
gboolean gui_window_draw_text_midx (cairo_t *cr, gchar *text, gchar *font, gint width_x, gint height_y)
{
	PangoLayout *layout;
	PangoFontDescription *desc;
	
	int width, height;
	
	layout = pango_cairo_create_layout (cr);
	
	pango_layout_set_text (layout, text, -1);
	desc = pango_font_description_from_string (font);
	pango_layout_set_font_description (layout, desc);
	pango_font_description_free (desc);
	
	pango_cairo_update_layout (cr, layout);
	
	pango_layout_get_size (layout, &width, &height);
	if(width_x-(double)width/PANGO_SCALE>0)
		cairo_move_to (cr, (width_x - (double)width / PANGO_SCALE) / 2, height_y);
	else
		cairo_move_to (cr, 0, height_y);
	pango_cairo_show_layout (cr, layout);
	
	g_object_unref (layout);
	
	return TRUE;
}
