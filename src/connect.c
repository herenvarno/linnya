

//HEADERS
#include "connect.h"

//FUNCTIONS
gboolean timeout()
{
	if(gtk_notebook_get_current_page(GTK_NOTEBOOK(gui_window_notebook))==2)
		gtk_widget_queue_draw (gui_window_eventbox_lrc);
	gtk_widget_queue_draw (gui_window_window_lrc);
	gui_lrc_update_lrc();
	return TRUE;
}

gboolean con_connect_signal()
{
	g_signal_connect(G_OBJECT(gui_window_tree_left), "button_press_event", G_CALLBACK(gui_treeview_left_mask), NULL);
	g_signal_connect(G_OBJECT(gui_window_tree_right), "button_press_event", G_CALLBACK(gui_treeview_right_mask), NULL);
	g_signal_connect(G_OBJECT(gui_window_selection_left), "changed", G_CALLBACK(gui_treeview_left_change), NULL);
	g_signal_connect(G_OBJECT(gui_window_tree_right), "row_activated", G_CALLBACK(gui_treeview_right_active), NULL);
	g_signal_connect(G_OBJECT(gui_window_tree_left), "button_release_event", G_CALLBACK(gui_treeview_left_popup), NULL);
	g_signal_connect(G_OBJECT(gui_window_tree_right), "button_release_event", G_CALLBACK(gui_treeview_right_popup), NULL);
	g_signal_connect(G_OBJECT(gui_window_eventbox_lrc), "expose-event" ,G_CALLBACK (gui_lrc_eventbox_expose) , NULL) ;
	g_signal_connect(G_OBJECT(gui_window_eventbox_lrc),"button_press_event", G_CALLBACK(gui_lrc_seek),NULL);
	g_signal_connect(G_OBJECT(gui_window_eventbox_lrc),"motion_notify_event", G_CALLBACK(gui_lrc_seek),NULL);
	g_signal_connect(G_OBJECT(gui_window_eventbox_lrc),"button_release_event", G_CALLBACK(gui_lrc_seek),NULL);
	g_signal_connect(G_OBJECT(gui_window_window_lrc),"expose-event", G_CALLBACK(gui_lrc_expose_desktop),NULL);
	g_signal_connect(G_OBJECT(gui_window_window_lrc),"button_press_event", G_CALLBACK(gui_lrc_move_desktop),NULL);
	g_signal_connect(G_OBJECT(gui_window_window_lrc),"motion_notify_event", G_CALLBACK(gui_lrc_move_desktop),NULL);
	g_signal_connect(G_OBJECT(gui_window_window_lrc),"button_release_event", G_CALLBACK(gui_lrc_move_desktop),NULL);
	g_signal_connect (G_OBJECT(gui_window_window_lrc),"enter_notify_event",	G_CALLBACK(gui_lrc_move_desktop) , NULL ) ;
	g_signal_connect (G_OBJECT(gui_window_window_lrc),"leave_notify_event",	G_CALLBACK(gui_lrc_move_desktop) , NULL ) ;
	gtk_timeout_add(100,(GtkFunction)timeout,NULL);
	return TRUE;
}
