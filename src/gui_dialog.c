

//HEADERS
#include "gui_dialog.h"

//VARIABLES
gboolean gui_dialog_run=FALSE;
gboolean lock_eq=FALSE;

//FUNCTIONS
//********************************************************************************
//*	
//*	FUNCTION:		gui_dialog_new
//*	DISCRIPTION:	define window GUI
//*
//********************************************************************************
gboolean gui_dialog_new()
{

	if(gui_dialog)
		return FALSE;
	
	gchar *path;
	GList *list;
	GList *p;
	_gl_equalizer *eq;
	int i=0;
	GdkPixbuf *pixbuf;
	GdkColor *color;
	
	GtkWidget *vbox;
	GtkWidget *hbox_temp;
	GtkWidget *vbox_temp;
	GtkWidget *button;
	GtkWidget *label;
	GtkWidget *hpaned;
	GtkWidget *radio;
	GtkWidget *table;
	GtkTreeIter iter;
	
	//创建主对话框
	gui_dialog=gtk_dialog_new_with_buttons (
		_("Configuration"),
						GTK_WINDOW(gui_window),
						GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_NO_SEPARATOR,
						NULL);
	path=g_strconcat(gl_dir_icon,"linnya.png",NULL);
	puts(path);
	gtk_window_set_icon_from_file(GTK_WINDOW(gui_dialog),path,NULL);
	g_free(path);
	gtk_window_set_default_size(GTK_WINDOW(gui_dialog),500,440);
	g_signal_connect(G_OBJECT(gui_dialog),	"destroy",	G_CALLBACK(gui_dialog_quit),	NULL);
	//创建按钮
	button=gtk_button_new_from_stock(GTK_STOCK_ABOUT);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(gui_dialog_show_about),NULL);
	gtk_box_pack_end(GTK_BOX(GTK_DIALOG(gui_dialog)->action_area),button,FALSE,FALSE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(gui_dialog_quit),NULL);
	gtk_box_pack_end(GTK_BOX(GTK_DIALOG(gui_dialog)->action_area),button,FALSE,FALSE,0);
	
	//创建第一层左右分区，左边为treeview，右边为notebook
	hpaned=gtk_hpaned_new();
	gtk_container_set_border_width(GTK_CONTAINER(hpaned), 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(gui_dialog)->vbox),hpaned,TRUE,TRUE,0);
	//treeview
	gui_dialog_tree_view=gtk_tree_view_new();
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(gui_dialog_tree_view),FALSE);
	gtk_container_add(GTK_CONTAINER(hpaned), gui_dialog_tree_view);
	gui_dialog_store= gtk_tree_store_new (1,G_TYPE_STRING);
	gtk_tree_store_append (gui_dialog_store, &iter,NULL);
	gtk_tree_store_set(gui_dialog_store, &iter,0,_("Playing Mode"),-1);
	gtk_tree_store_append (gui_dialog_store, &iter,NULL);
	gtk_tree_store_set(gui_dialog_store, &iter,0,_("Library"),-1);
	gtk_tree_store_append (gui_dialog_store, &iter,NULL);
	gtk_tree_store_set(gui_dialog_store, &iter,0,_("Equalizer"),-1);
	gtk_tree_store_append (gui_dialog_store, &iter,NULL);
	gtk_tree_store_set(gui_dialog_store, &iter,0,_("Lyrics"),-1);
	gtk_tree_store_append (gui_dialog_store, &iter,NULL);
	gtk_tree_store_set(gui_dialog_store, &iter,0,_("Theme"),-1);
	GtkCellRenderer *cell_renderer = gtk_cell_renderer_text_new ();
	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes( _("Options"),cell_renderer,"text",0,NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_dialog_tree_view), column);
	gtk_tree_view_set_model(GTK_TREE_VIEW (gui_dialog_tree_view), GTK_TREE_MODEL(gui_dialog_store));
	gui_dialog_selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(gui_dialog_tree_view));
	g_signal_connect(G_OBJECT(gui_dialog_selection),"changed",G_CALLBACK(gui_dialog_change_selection),hpaned);

	//notebook
	gui_dialog_notebook=gtk_notebook_new();
	gtk_notebook_set_show_border(GTK_NOTEBOOK(gui_dialog_notebook),FALSE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(gui_dialog_notebook),FALSE);
	gtk_container_add(GTK_CONTAINER(hpaned),gui_dialog_notebook);
	
	
	//播放配置选项卡
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(gui_dialog_notebook),vbox,NULL);
	path=g_strconcat(gl_dir_icon,"play.png",NULL);
	vbox=gui_dialog_set_title(vbox,_("<b>Playing Mode</b>\nSet the way to play"),path);
	g_free(path);
	
	gui_dialog_set_index(vbox,_("Main Window Mode"));
	gui_dialog_play_check_window_mode=gtk_check_button_new_with_label(_("Show main window"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dialog_play_check_window_mode),GTK_WIDGET_VISIBLE(gui_window));
	g_signal_connect(G_OBJECT(gui_dialog_play_check_window_mode),"clicked",G_CALLBACK(gui_window_change_visibal),gui_window);
	gtk_box_pack_start(GTK_BOX(vbox),gui_dialog_play_check_window_mode,FALSE,FALSE,0);
	
	gui_dialog_set_index(vbox,_("Playing mode"));
	vbox_temp=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),vbox_temp,TRUE,TRUE,0);
	radio=gtk_radio_button_new_with_label(NULL,_("Random"));
	if(gl_conf->random==1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio),FALSE);
	g_signal_connect(G_OBJECT(radio),"toggled",G_CALLBACK(gui_dialog_set_play_mode),NULL);
	gtk_box_pack_start(GTK_BOX(vbox_temp),radio,FALSE,FALSE,0);
	radio=gtk_radio_button_new_with_label(gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio)),_("Repeat list"));
	if((gl_conf->random==0)&&(gl_conf->single==0))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio),TRUE);
	g_signal_connect(G_OBJECT(radio),"toggled",G_CALLBACK(gui_dialog_set_play_mode),NULL);
	gtk_box_pack_start(GTK_BOX(vbox_temp),radio,FALSE,FALSE,0);
	radio=gtk_radio_button_new_with_label(gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio)),_("Repeat song"));
	if(gl_conf->random==0&&gl_conf->single==1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio),TRUE);
	g_signal_connect(G_OBJECT(radio),"toggled",G_CALLBACK(gui_dialog_set_play_mode),NULL);
	gtk_box_pack_start(GTK_BOX(vbox_temp),radio,FALSE,FALSE,0);

	//音乐库配置选项卡
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(gui_dialog_notebook),vbox,NULL);
	path=g_strconcat(gl_dir_icon,"lib.png",NULL);
	vbox=gui_dialog_set_title(vbox,_("<b>Library</b>\nSet your music library"),path);
	g_free(path);

	gui_dialog_set_index(vbox,_("Bind music library"));
	hbox_temp=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox_temp,FALSE,FALSE,0);
	label=gtk_label_new(_("Library:"));
	gtk_box_pack_start(GTK_BOX(hbox_temp),label,FALSE,FALSE,0);
	gui_dialog_lib_entry_bind=gtk_entry_new();
	gtk_widget_set_sensitive(gui_dialog_lib_entry_bind,FALSE);
	gtk_entry_set_text(GTK_ENTRY(gui_dialog_lib_entry_bind),gl_conf->lib);
	gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_lib_entry_bind,TRUE,TRUE,0);
	gui_dialog_lib_button_bind=gtk_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(gui_dialog_lib_button_bind),"clicked",G_CALLBACK(gui_dialog_bind_lib),NULL);
	gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_lib_button_bind,FALSE,FALSE,0);

	gui_dialog_set_index(vbox,_("Start Check"));
	vbox_temp=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),vbox_temp,FALSE,FALSE,0);
	gui_dialog_lib_check_autoload=gtk_check_button_new_with_label(_("AutoLoad Lib When Start"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dialog_lib_check_autoload),(gboolean)(gl_conf->autocheck));
	g_signal_connect(G_OBJECT(gui_dialog_lib_check_autoload),"clicked",G_CALLBACK(gui_dialog_bool_change),&gl_conf->autocheck);
	gtk_box_pack_start(GTK_BOX(vbox_temp),gui_dialog_lib_check_autoload,FALSE,FALSE,0);

	hbox_temp=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox_temp),hbox_temp,FALSE,FALSE,0);
	label=gtk_label_new(_("Divide sublib by: "));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox_temp),label,FALSE,FALSE,0);
	gui_dialog_lib_combo_sublib=gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dialog_lib_combo_sublib), _("none"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dialog_lib_combo_sublib), _("artist"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dialog_lib_combo_sublib), _("album"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dialog_lib_combo_sublib),gl_conf->sublib);
	g_signal_connect(G_OBJECT(gui_dialog_lib_combo_sublib),"changed",G_CALLBACK(gui_dialog_change_sublib),NULL);
	gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_lib_combo_sublib,TRUE,TRUE,0);
	
	gui_dialog_set_index(vbox,_("Default Backup Encode"));
	hbox_temp=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox_temp,FALSE,FALSE,0);
	label=gtk_label_new(_("Encode : "));
	gtk_box_pack_start(GTK_BOX(hbox_temp),label,FALSE,FALSE,0);
	gui_dialog_lib_combo_encode=gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dialog_lib_combo_encode), "GB18030");
	if(g_strcmp0(gl_conf->encode,"GB18030")==0)
		gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dialog_lib_combo_encode),0);
	g_signal_connect(G_OBJECT(gui_dialog_lib_combo_encode),"changed",G_CALLBACK(gui_dialog_char_change),&gl_conf->encode);
	gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_lib_combo_encode,TRUE,TRUE,0);

	
	//均衡器配置选项卡
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(gui_dialog_notebook),vbox,NULL);
	path=g_strconcat(gl_dir_icon,"equalizer.png",NULL);
	vbox=gui_dialog_set_title(vbox,_("<b>Equalizer</b>\nSet equalizer mode of music"),path);
	g_free(path);
	
	gui_dialog_set_index(vbox,_("Automatic equalizer"));
	gui_dialog_eq_check_autoeq=gtk_check_button_new_with_label(_("Choose equlizer automatically"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dialog_eq_check_autoeq),(gboolean)(gl_conf->autoeq));
	g_signal_connect(G_OBJECT(gui_dialog_eq_check_autoeq),"clicked",G_CALLBACK(gui_dialog_change_sensitive_eq),NULL);
	gtk_box_pack_start(GTK_BOX(vbox),gui_dialog_eq_check_autoeq,FALSE,FALSE,0);
	
	gui_dialog_set_index(vbox,_("Preset equalizer"));
	hbox_temp=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox_temp,FALSE,FALSE,0);
	gui_dialog_eq_combo_eq=gtk_combo_box_new_text();
	p=gl_equalizer;
	while(p)
	{
		eq=p->data;
		gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dialog_eq_combo_eq), eq->name);
		if(p==gl_eq)
			gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dialog_eq_combo_eq),g_list_index(gl_equalizer,eq));
		p=p->next;
	}
	g_signal_connect(G_OBJECT(gui_dialog_eq_combo_eq), "changed",G_CALLBACK(gui_dialog_select_eq), NULL);
	gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_eq_combo_eq,TRUE,TRUE,0);
	gui_dialog_eq_button_save=gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
	gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_eq_button_save,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(gui_dialog_eq_button_save),"clicked",G_CALLBACK(gui_dialog_save_eq),NULL);
	gui_dialog_eq_button_delete=gtk_button_new_from_stock(GTK_STOCK_DELETE);
	gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_eq_button_delete,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(gui_dialog_eq_button_delete),"clicked",G_CALLBACK(gui_dialog_delete_eq),NULL);
	
	gui_dialog_set_index(vbox,_("Custom"));
	hbox_temp=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox_temp,TRUE,TRUE,0);
	eq=gl_eq->data;
	for(i=0;i<10;i++)
	{
		gui_dialog_eq_vscale_band[i]=gtk_vscale_new_with_range(-24,12,0.1);
		gtk_scale_set_value_pos(GTK_SCALE(gui_dialog_eq_vscale_band[i]),GTK_POS_BOTTOM);
		gtk_range_set_inverted(GTK_RANGE(gui_dialog_eq_vscale_band[i]),TRUE);
		gtk_range_set_value(GTK_RANGE(gui_dialog_eq_vscale_band[i]),eq->band[i]);
		gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_eq_vscale_band[i],TRUE,TRUE,0);
		g_signal_connect(G_OBJECT(gui_dialog_eq_vscale_band[i]),"value-changed",G_CALLBACK(gui_dialog_set_eq),NULL);
	}
	
	gui_dialog_change_sensitive_eq(gui_dialog_eq_check_autoeq,NULL);
	
	//歌词配置选项卡
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(gui_dialog_notebook),vbox,NULL);
	path=g_strconcat(gl_dir_icon,"lyrics.png",NULL);
	vbox=gui_dialog_set_title(vbox,_("<b>Lyrics</b>\nSet the style of lyrics to show"),path);
	g_free(path);
	gui_dialog_set_index(vbox,_("Desktop Lyrics"));
	gui_dialog_lrc_check_desktop_show=gtk_check_button_new_with_label(_("Show Desktop Lyrics"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dialog_lrc_check_desktop_show),gtk_widget_get_visible(gui_window_window_lrc));
	g_signal_connect(G_OBJECT(gui_dialog_lrc_check_desktop_show),"clicked",G_CALLBACK(gui_window_change_visibal),gui_window_window_lrc);
	gtk_box_pack_start(GTK_BOX(vbox),gui_dialog_lrc_check_desktop_show,FALSE,FALSE,0);
	gui_dialog_lrc_check_desktop_fix=gtk_check_button_new_with_label(_("Fix Desktop Lyrics"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dialog_lrc_check_desktop_fix),gl_conf->desktoplyrics_fix);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_check_desktop_fix),"clicked",G_CALLBACK(gui_dialog_bool_change),&(gl_conf->desktoplyrics_fix));
	gtk_box_pack_start(GTK_BOX(vbox),gui_dialog_lrc_check_desktop_fix,FALSE,FALSE,0);
	
	gui_dialog_set_index(vbox,_("LRC File Location"));
	gui_dialog_lrc_check_lrc_location=gtk_check_button_new_with_label(_("The Same Directory of Music File"));
	g_signal_connect(G_OBJECT(gui_dialog_lrc_check_lrc_location),"clicked",G_CALLBACK(gui_dialog_change_sensitive_lrc_location),NULL);
	gtk_box_pack_start(GTK_BOX(vbox),gui_dialog_lrc_check_lrc_location,FALSE,FALSE,0);
	hbox_temp=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox_temp,FALSE,FALSE,0);
	label=gtk_label_new(_("Custom LRC location:"));
	gtk_box_pack_start(GTK_BOX(hbox_temp),label,FALSE,FALSE,0);
	gui_dialog_lrc_entry_lrc_location=gtk_entry_new();
	gtk_widget_set_sensitive(gui_dialog_lrc_entry_lrc_location,FALSE);
	gtk_entry_set_text(GTK_ENTRY(gui_dialog_lrc_entry_lrc_location),gl_conf->lrc_location);
	gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_lrc_entry_lrc_location,TRUE,TRUE,0);
	gui_dialog_lrc_button_lrc_location=gtk_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_button_lrc_location),"clicked",G_CALLBACK(gui_dialog_bind_lrc_location),NULL);
	gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_lrc_button_lrc_location,FALSE,FALSE,0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dialog_lrc_check_lrc_location),!g_strcmp0(gl_conf->lrc_location,"./"));
	
	gui_dialog_set_index(vbox,_("Lyrics Style"));
	gui_dialog_lrc_check_custom=gtk_check_button_new_with_label(_("Use Custom Setting"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dialog_lrc_check_custom),gl_conf->custom_lrc);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_check_custom),"clicked",G_CALLBACK(gui_dialog_change_sensitive_lrc),&(gl_conf->custom_lrc));
	gtk_box_pack_start(GTK_BOX(vbox),gui_dialog_lrc_check_custom,FALSE,FALSE,0);

	table=gtk_table_new(3,6,FALSE);
	gtk_box_pack_start(GTK_BOX(vbox),table,FALSE,TRUE,0);
	label=gtk_label_new(_("Highlight:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table),label,0,1,0,1,GTK_FILL,GTK_FILL,5,5);
	gui_dialog_lrc_font_highlight=gtk_font_button_new_with_font(gl_conf->font_highlight);
	gtk_font_button_set_show_style(GTK_FONT_BUTTON(gui_dialog_lrc_font_highlight),FALSE);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_font_highlight),"font-set",G_CALLBACK(gui_dialog_font_change),&gl_conf->font_highlight);
	gtk_table_attach_defaults(GTK_TABLE(table),gui_dialog_lrc_font_highlight,1,2,0,1);
	color=gui_dialog_color_new_from_string(gl_conf->color_highlight);
	gui_dialog_lrc_color_highlight=gtk_color_button_new_with_color(color);
	g_free(color);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_color_highlight),"color-set",G_CALLBACK(gui_dialog_color_change),&gl_conf->color_highlight);
	gtk_table_attach_defaults(GTK_TABLE(table),gui_dialog_lrc_color_highlight,2,3,0,1);
	label=gtk_label_new(_("Normal:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table),label,0,1,1,2,GTK_FILL,GTK_FILL,5,5);
	gui_dialog_lrc_font_normal=gtk_font_button_new_with_font(gl_conf->font_normal);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_font_normal),"font-set",G_CALLBACK(gui_dialog_font_change),&gl_conf->font_normal);
	gtk_table_attach_defaults(GTK_TABLE(table),gui_dialog_lrc_font_normal,1,2,1,2);
	color=gui_dialog_color_new_from_string(gl_conf->color_normal);
	gui_dialog_lrc_color_normal=gtk_color_button_new_with_color(color);
	g_free(color);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_color_normal),"color-set",G_CALLBACK(gui_dialog_color_change),&gl_conf->color_normal);
	gtk_table_attach_defaults(GTK_TABLE(table),gui_dialog_lrc_color_normal,2,3,1,2);

	label=gtk_label_new(_("Desktop Font:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table),label,0,1,2,3,GTK_FILL,GTK_FILL,5,5);
	gui_dialog_lrc_font_desktop=gtk_font_button_new_with_font(gl_conf->font_desktop);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_font_desktop),"font-set",G_CALLBACK(gui_dialog_font_change),&gl_conf->font_desktop);
	gtk_table_attach_defaults(GTK_TABLE(table),gui_dialog_lrc_font_desktop,1,3,2,3);
	label=gtk_label_new(_("Desktop fg:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table),label,0,1,3,4,GTK_FILL,GTK_FILL,5,5);
	color=gui_dialog_color_new_from_string(gl_conf->color_desktop_fg1);
	gui_dialog_lrc_color_desktop_fg1=gtk_color_button_new_with_color(color);
	g_free(color);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_color_desktop_fg1),"color-set",G_CALLBACK(gui_dialog_color_change),&gl_conf->color_desktop_fg1);
	gtk_table_attach_defaults(GTK_TABLE(table),gui_dialog_lrc_color_desktop_fg1,1,2,3,4);
	color=gui_dialog_color_new_from_string(gl_conf->color_desktop_fg2);
	gui_dialog_lrc_color_desktop_fg2=gtk_color_button_new_with_color(color);
	g_free(color);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_color_desktop_fg2),"color-set",G_CALLBACK(gui_dialog_color_change),&gl_conf->color_desktop_fg2);
	gtk_table_attach_defaults(GTK_TABLE(table),gui_dialog_lrc_color_desktop_fg2,2,3,3,4);
	label=gtk_label_new(_("Desktop bg:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table),label,0,1,4,5,GTK_FILL,GTK_FILL,5,5);
	color=gui_dialog_color_new_from_string(gl_conf->color_desktop_bg1);
	gui_dialog_lrc_color_desktop_bg1=gtk_color_button_new_with_color(color);
	g_free(color);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_color_desktop_bg1),"color-set",G_CALLBACK(gui_dialog_color_change),&(gl_conf->color_desktop_bg1));
	gtk_table_attach_defaults(GTK_TABLE(table),gui_dialog_lrc_color_desktop_bg1,1,2,4,5);
	color=gui_dialog_color_new_from_string(gl_conf->color_desktop_bg2);
	gui_dialog_lrc_color_desktop_bg2=gtk_color_button_new_with_color(color);
	g_free(color);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_color_desktop_bg2),"color-set",G_CALLBACK(gui_dialog_color_change),&(gl_conf->color_desktop_bg2));
	gtk_table_attach_defaults(GTK_TABLE(table),gui_dialog_lrc_color_desktop_bg2,2,3,4,5);
	label=gtk_label_new(_("Spectrum Color:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_table_attach(GTK_TABLE(table),label,0,1,5,6,GTK_FILL,GTK_FILL,5,5);
	color=gui_dialog_color_new_from_string(gl_conf->color_spectrum);
	gui_dialog_lrc_color_spectrum=gtk_color_button_new_with_color(color);
	g_free(color);
	g_signal_connect(G_OBJECT(gui_dialog_lrc_color_spectrum),"color-set",G_CALLBACK(gui_dialog_color_change),&(gl_conf->color_spectrum));
	gtk_table_attach_defaults(GTK_TABLE(table),gui_dialog_lrc_color_spectrum,1,3,5,6);
	
	gui_dialog_change_sensitive_lrc(gui_dialog_lrc_check_custom,NULL);

	//主题设定选项卡
	vbox=gtk_vbox_new(FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(gui_dialog_notebook),vbox,NULL);
	path=g_strconcat(gl_dir_icon, "theme.png", NULL);
	vbox=gui_dialog_set_title(vbox,_("<b>Theme</b>\nSet theme or skin of Linnya"),path);
	g_free(path);
	
	gui_dialog_set_index(vbox,_("Select theme"));
	hbox_temp=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox_temp,FALSE,FALSE,0);
	label=gtk_label_new(_("Theme:"));
	gtk_box_pack_start(GTK_BOX(hbox_temp),label,FALSE,FALSE,0);
	list=th_get_name_list();
	gui_dialog_th_combo_theme=gtk_combo_box_new_text();
	p=list;
	while(p)
	{
		gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dialog_th_combo_theme), p->data);
		if(g_strcmp0(p->data,gl_conf->theme)==0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dialog_th_combo_theme), g_list_index(list,p->data));
		p=p->next;
	}
	gl_free_list_string(list);
	gtk_box_pack_start(GTK_BOX(hbox_temp),gui_dialog_th_combo_theme,TRUE,TRUE,0);
	g_signal_connect(G_OBJECT(gui_dialog_th_combo_theme), "changed",G_CALLBACK(gui_dialog_select_theme), NULL);
	
	gui_dialog_set_index(vbox,_("Theme Preview"));
	path=g_strconcat(gl_dir_conf2,"themes/",gl_conf->theme,"/",NULL);
	DIR *dir=opendir(path);
	g_free(path);
	if(dir)
	{
		path=g_strconcat(gl_dir_conf2,"themes/",gl_conf->theme,"/screenshot.png",NULL);
	}
	else
	{
		path=g_strconcat(gl_dir_conf1,"themes/",gl_conf->theme,"/screenshot.png",NULL);
	}
	closedir(dir);
	pixbuf=gdk_pixbuf_new_from_file_at_size(path,256,256,NULL);
	g_free(path);
	if(!pixbuf)
	{
		path=g_strconcat(gl_dir_icon,"404.png",NULL);
		pixbuf=gdk_pixbuf_new_from_file_at_size(path,256,256,NULL);
		g_free(path);
	}
	gui_dialog_th_image_preview=gtk_image_new_from_pixbuf(pixbuf);
	gtk_box_pack_start(GTK_BOX(vbox),gui_dialog_th_image_preview,FALSE,FALSE,0);

	gtk_widget_show_all(gui_dialog);
	gui_dialog_run=TRUE;
	return TRUE;
}


//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_set_title
//*	DISCRIPTION:	set title of a notebook page
//*
//*****************************************************************************************************
GtkWidget* gui_dialog_set_title(GtkWidget *widget, gchar *title, gchar *icon)
{
	GtkWidget	*hbox;
	GtkWidget	*scrolled_window;
	GtkWidget	*label;
	GtkWidget	*image;
	GtkWidget	*vbox;
	
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(widget),hbox,FALSE,FALSE,0);
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label),title);
	gtk_misc_set_alignment(GTK_MISC(label),0.1,0.5);
	gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,0);
	image=gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_size(icon,64,64,NULL));
	gtk_box_pack_end(GTK_BOX(hbox),image,FALSE,FALSE,0);
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(widget),scrolled_window,TRUE,TRUE,0);
	vbox=gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 6);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),vbox);
	return vbox;
}
//*****************************************************************************************************
//*
//*	FUNCTION:	title_draw
//*	DISCRIPTION:	draw title with cairo, serve for func. conf_window_set_index
//*
//*****************************************************************************************************
gboolean gui_dialog_callback_set_index( GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
	GtkStyle *style=gtk_widget_get_style(gui_dialog);
	cairo_t *cr=gdk_cairo_create (widget->window);
	gint width=0,height=0,x,y,r;
	gdk_drawable_get_size(widget->window, &width, &height);
	gdk_cairo_set_source_color (cr,&(style->bg[3]));
	cairo_set_line_width (cr, 2);
	
	x =0;
	y =8;
	width = width-2*x;
	height = height-2*y+4;
	r = height / 4.0;
	
	cairo_move_to (cr, x + r, y);
	cairo_line_to (cr, x + width - r, y);
	
	cairo_move_to (cr, x + width, y + r);
	cairo_line_to (cr, x + width, y + height - r);
	
	cairo_move_to (cr, x + width - r, y + height);
	cairo_line_to (cr, x + r, y + height);
	
	cairo_move_to (cr, x, y + height - r);
	cairo_line_to (cr, x, y + r);
	
	cairo_arc (cr, x + r, y + r, r, M_PI, 3 * M_PI / 2.0);
	cairo_arc (cr, x + width - r, y + r, r, 3 * M_PI / 2, 2 * M_PI);
	cairo_arc (cr, x + width - r, y + height - r, r, 0, M_PI / 2);
	cairo_arc (cr, x + r, y + height - r, r, M_PI / 2, M_PI);
	
	cairo_stroke_preserve (cr);
	cairo_fill (cr);
	
	gdk_cairo_set_source_color (cr,&(style->fg[3]));
	cairo_move_to(cr,10,10);
	gui_window_draw_text(cr,(gchar *)data,"Sans Regular 9");
	cairo_destroy ( cr) ;
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_set_index
//*	DISCRIPTION:	set index of a page
//*
//*****************************************************************************************************
gboolean gui_dialog_set_index(GtkWidget *widget, gchar *str)
{
	GtkWidget *drawing_area=gtk_drawing_area_new();
	gtk_widget_set_size_request(drawing_area,-1,30);
	g_signal_connect(G_OBJECT(drawing_area),"expose-event",G_CALLBACK(gui_dialog_callback_set_index),str);
	gtk_box_pack_start(GTK_BOX(widget),drawing_area,FALSE,TRUE,0);
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_quit
//*	DISCRIPTION:	quit configuration dialog
//*
//*****************************************************************************************************
gboolean gui_dialog_quit(GtkWidget *widget,gpointer data)
{
	gtk_widget_destroy(GTK_WIDGET(gui_dialog));
	gui_dialog_run=FALSE;
	gui_dialog=NULL;
	return FALSE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_diglog_show_about
//*	DISCRIPTION:	create and show about_dialog
//*
//*****************************************************************************************************
gboolean gui_dialog_show_about(GtkWidget *widget,gpointer data)
{
	gchar *path=NULL;
	GdkPixbuf *logo=NULL;
	
	path=g_strconcat(gl_dir_icon,"linnya.png",NULL);
	logo=gdk_pixbuf_new_from_file_at_size(path,60,60,NULL);
	g_free(path);
	
	gchar *artists[]	= {	"Edward<yy890521@gmail.com>",
							NULL
						};
	gchar *authors[]	= {	"Edward<yy890521@gmail.com>",
							"wpcworkstation<wpcworkstation@gmail.com>",
							NULL
						};
	gchar translators[]	= {	"Edward<yy890521@gmail.com>\n于强<shdxcy@gmail.com>"
						};
	
	gtk_show_about_dialog(GTK_WINDOW(gui_dialog),
					"artists",&artists,
					"authors",&authors,
					"comments",_("A simple music player for Linux, which can bind music library, change theme, auto download lyrics and etc."),
					"copyright",_("(c) Edward<yy890521@gmail.com>"),
//					"documenters",&authors,
					"license",_("\
Linnya\n\n\
Copyright (C) 2010 - Edward\n\n\
Linnya is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2 of the License, or\n\
(at your option) any later version.\n\n\
Linnya is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\n\
You should have received a copy of the GNU General Public License\n\
along with Linnya; if not, write to the Free Software\n\
Foundation, Inc., 51 Franklin St, Fifth Floor, \n\
Boston, MA  02110-1301  USA"),
					"logo",logo,
//					"logo-icon-name","linnya",
					"translator-credits",&translators,
					"version",_("1.0.2"),
					"website","http://code.google.com/p/linnya",
					"website-label",_("Official Website"),
					"wrap-license",TRUE,
					NULL);
	g_object_unref(logo);
	return FALSE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_change_selection
//*	DISCRIPTION:	change selection
//*
//*****************************************************************************************************
gboolean gui_dialog_change_selection(GtkWidget *widget,gpointer data)
{

	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;
	gchar *str;
	gint num;
	gtk_tree_selection_get_selected(gui_dialog_selection ,&model,&iter);
	path=gtk_tree_model_get_path(model,&iter);
	str=gtk_tree_path_to_string(path);
	sscanf(str,"%d",&num);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui_dialog_notebook),num);
	return FALSE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_set_play_mode
//*	DISCRIPTION:	set playing mode
//*
//*****************************************************************************************************
gboolean gui_dialog_set_play_mode(GtkWidget *widget,gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	
	const gchar *label=gtk_button_get_label(GTK_BUTTON(widget));
	if(g_strcmp0(label,_("Random"))==0)
	{
		gl_conf->random=1;
	}
	else if(g_strcmp0(label,_("Repeat list"))==0)
	{
		gl_conf->random=0;
		gl_conf->single=0;
	}
	else if(g_strcmp0(label, _("Repeat song"))==0)
	{
		gl_conf->random=0;
		gl_conf->single=1;
	}
	return FALSE;	
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_bind_lib
//*	DISCRIPTION:	bind music library
//*
//*****************************************************************************************************
gboolean gui_dialog_bind_lib(GtkWidget *widget,gpointer data)
{	
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	
	gint result=0;
	GtkWidget *dialog=gtk_file_chooser_dialog_new(	_("Select library"),
													GTK_WINDOW(gui_dialog),
													GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
													GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
													GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
													NULL);
	result=gtk_dialog_run (GTK_DIALOG (dialog));
	switch (result)
	{
		case GTK_RESPONSE_ACCEPT:
			break;
		case GTK_RESPONSE_CANCEL:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
		default:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
	}
	gchar *path=gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
	gchar *str=g_strconcat(path,"/",NULL);
	g_free(path);

	gtk_entry_set_text(GTK_ENTRY(gui_dialog_lib_entry_bind),str);
	if(gl_conf->lib)
		g_free(gl_conf->lib);
	gl_conf->lib=str;
	
	gtk_widget_destroy(dialog);
	gl_selecting=gl_lib;
	lib_check_lib();
	lib_refresh_sublib();
	gui_treeview_left_refresh();
	gui_treeview_right_refresh();
	return FALSE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_change_sublib
//*	DISCRIPTION:	change sublib to show
//*
//*****************************************************************************************************
gboolean gui_dialog_change_sublib(GtkWidget *widget,gpointer data)
{	
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	gl_conf->sublib=gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
	gl_selecting=gl_lib;
	lib_refresh_sublib();
	gui_treeview_left_refresh();
	gui_treeview_right_refresh();
	return FALSE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_bool_change
//*	DISCRIPTION:	general func for change bool varibales
//*
//*****************************************************************************************************
gboolean gui_dialog_bool_change(GtkWidget *widget, gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	gint *d=data;
	if(*d)
		*d=0;
	else
		*d=1;
	return FALSE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_int_change
//*	DISCRIPTION:	general func for change bool varibales
//*
//*****************************************************************************************************
gboolean gui_dialog_int_change(GtkWidget *widget, gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	gint *d=data;
	*d=gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
	return FALSE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_char_change
//*	DISCRIPTION:	general func for change string varibales
//*
//*****************************************************************************************************
gboolean gui_dialog_char_change(GtkWidget *widget, gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	gchar **d=data;
	if(*d)
		g_free(*d);
	*d=g_strconcat(gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)),NULL);
	return TRUE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_font_change
//*	DISCRIPTION:	general func for change fonts
//*
//*****************************************************************************************************
gboolean gui_dialog_font_change(GtkWidget *widget, gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	gchar **d=data;
	if(*d)
		g_free(*d);
	*d=g_strconcat(gtk_font_button_get_font_name(GTK_FONT_BUTTON(widget)),NULL);
	return TRUE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_color_change
//*	DISCRIPTION:	general func for change colors
//*
//*****************************************************************************************************
gboolean gui_dialog_color_change(GtkWidget *widget, gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	
	gchar **d=data;
	if(*d)
		g_free(*d);
	GdkColor *color=(GdkColor *)g_malloc(sizeof(GdkColor));
	gtk_color_button_get_color(GTK_COLOR_BUTTON(widget),color);
	*d=(gchar*)g_malloc(128);
	sprintf(*d,"%d:%d:%d",color->red,color->green,color->blue);
	return TRUE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_change_sensitive_eq
//*	DISCRIPTION:	change the sensitive of widget relative to equlizer
//*
//*****************************************************************************************************
gboolean gui_dialog_change_sensitive_eq(GtkWidget *widget, gpointer data)
{
	
	gl_conf->autoeq=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	
	gtk_widget_set_sensitive(gui_dialog_eq_combo_eq,!(gl_conf->autoeq));
	gtk_widget_set_sensitive(gui_dialog_eq_button_save,!(gl_conf->autoeq));
	gtk_widget_set_sensitive(gui_dialog_eq_button_delete,!(gl_conf->autoeq));
	int i=0;
	for(i=0;i<10;i++)
	{
		gtk_widget_set_sensitive(gui_dialog_eq_vscale_band[i],!(gl_conf->autoeq));
	}
	
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_select_eq
//*	DISCRIPTION:	select equlizer
//*
//*****************************************************************************************************
gboolean gui_dialog_select_eq(GtkWidget *widget, gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	
	int i=0;
	_gl_equalizer *eq;
	gl_eq=eq_find_eq_by_name(gl_equalizer,gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
	eq_set_equalizer();
	eq=gl_eq->data;
	lock_eq=TRUE;
	for(i=0;i<10;i++)
	{
		gtk_range_set_value(GTK_RANGE(gui_dialog_eq_vscale_band[i]),eq->band[i]);
	}
	lock_eq=FALSE;
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_set_eq
//*	DISCRIPTION:	select equlizer
//*
//*****************************************************************************************************
gboolean gui_dialog_set_eq(GtkWidget *widget, gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	if(lock_eq)
	{
		return FALSE;
	}
	
	int i=0;
	
	GList *p;
	_gl_equalizer *plink;
	GList *q;
	_gl_equalizer *qlink;
	
	p=gl_eq;
	plink=p->data;
	if(g_strcmp0(plink->name,"default")!=0)
	{
		q=eq_find_eq_by_name(gl_equalizer,"default");
		qlink=q->data;
		for(i=0;i<10;i++)
			qlink->band[i]=plink->band[i];
		gl_eq=q;
	}
	plink=p->data;
	
	for(i=0;i<10;i++)
	{
		if(widget==gui_dialog_eq_vscale_band[i])
			plink->band[i]=gtk_range_get_value(GTK_RANGE(gui_dialog_eq_vscale_band[i]));
	}
	
	gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dialog_eq_combo_eq),g_list_index(gl_equalizer,plink));
	
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_save_eq
//*	DISCRIPTION:	select equlizer
//*
//*****************************************************************************************************
gboolean gui_dialog_save_eq(GtkWidget *widget, gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	
	int i=0;
	int result;
	gchar *name;
	
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	
	dialog=gtk_dialog_new_with_buttons(	_("Name"),
										GTK_WINDOW(gui_dialog),
										GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
										GTK_STOCK_OK,
										GTK_RESPONSE_ACCEPT,
										GTK_STOCK_CANCEL,
										GTK_RESPONSE_REJECT,
										NULL);
	gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox,TRUE,TRUE,0);
	label=gtk_label_new(_("Name"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entry=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,0);
	gtk_widget_show_all(dialog);
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	switch(result)
	{
		case GTK_RESPONSE_ACCEPT:
			break;
		default:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
	}
	
	name=g_strconcat(gtk_entry_get_text(GTK_ENTRY(entry)),NULL);
	if(gtk_entry_get_text(GTK_ENTRY(entry))==NULL||g_strcmp0(name,"")==0||g_strcmp0(name,"default")==0)
	{
		g_free(name);
		gtk_widget_destroy(dialog);
		de_dialog_print(gui_dialog,__DE_ERROR, __FUNCTION__, _("Illegal equalizer name"));
		return FALSE;
	}
	gtk_widget_destroy(dialog);
	
	GList *p;
	_gl_equalizer *plink;
	GList *q;
	_gl_equalizer *qlink;
	_gl_equalizer *eq=NULL;
	
	p=eq_find_eq_by_name(gl_equalizer,name);
	if(p)
	{
		dialog=gui_treeview_warnning_dialog_new(gui_dialog, _("<b>Warnning</b>!\n an equalizer with the same name is exist, change it?"));
		result=gtk_dialog_run(GTK_DIALOG(dialog));
		switch(result)
		{
			case GTK_RESPONSE_ACCEPT:
				break;
			default:
				g_free(name);
				gtk_widget_destroy(dialog);
				return FALSE;
				break;
		}
		gtk_widget_destroy(dialog);
		if(p==gl_eq)
			return FALSE;
		plink=p->data;
		q=gl_eq;
		qlink=q->data;
		for(i=0;i<10;i++)
			plink->band[i]=qlink->band[i];
		eq=plink;
	}
	else
	{
		p=gl_eq;
		plink=p->data;
	
		eq=(_gl_equalizer*)g_malloc(sizeof(_gl_equalizer));
		eq->name=g_strconcat(name,NULL);
		for(i=0;i<10;i++)
			eq->band[i]=plink->band[i];
		gl_equalizer=g_list_append(gl_equalizer,eq);
		gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dialog_eq_combo_eq), eq->name);
	}
	
	gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dialog_eq_combo_eq),g_list_index(gl_equalizer,eq));
	
	return FALSE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_delete_eq
//*	DISCRIPTION:	select equlizer
//*
//*****************************************************************************************************
gboolean gui_dialog_delete_eq(GtkWidget *widget, gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	
	int result;
	GList *p;
	_gl_equalizer *eq;
	
	GtkWidget *dialog;
	
	if(g_strcmp0(gtk_combo_box_get_active_text(GTK_COMBO_BOX(gui_dialog_eq_combo_eq)),"default")==0)
	{
		de_dialog_print(gui_dialog,__DE_ERROR,__FUNCTION__,_("Illegal delete"));
		return FALSE;
	}
	
	dialog=gui_treeview_warnning_dialog_new(gui_dialog, _("<b>Warnning</b>!\n Do you really want to delete it?"));
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	switch(result)
	{
		case GTK_RESPONSE_ACCEPT:
			break;
		default:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
	}
	gtk_widget_destroy(dialog);
	
	gl_free_equalizer(gl_eq->data);
	gl_equalizer=g_list_delete_link(gl_equalizer,gl_eq);
	gl_eq=eq_find_eq_by_name(gl_equalizer,"default");
	
	GtkListStore *model=NULL;
	gtk_combo_box_set_model(GTK_COMBO_BOX(gui_dialog_eq_combo_eq),GTK_TREE_MODEL(model));
	g_object_unref(model);
	model=gtk_list_store_new(1,G_TYPE_STRING);
	gtk_combo_box_set_model(GTK_COMBO_BOX(gui_dialog_eq_combo_eq),GTK_TREE_MODEL(model));
	p=gl_equalizer;
	while(p)
	{
		eq=p->data;
		gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dialog_eq_combo_eq), eq->name);
		if(p==gl_eq)
			gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dialog_eq_combo_eq),g_list_index(gl_equalizer,eq));
		p=p->next;
	}
	
	return FALSE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_select_theme
//*	DISCRIPTION:	select theme
//*
//*****************************************************************************************************
gboolean gui_dialog_select_theme(GtkWidget *widget, gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	
	gchar *path;
	GdkPixbuf *pixbuf;
	
	if(gl_conf->theme)
		g_free(gl_conf->theme);
	gl_conf->theme=g_strconcat(gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)),NULL);
	path=g_strconcat(gl_dir_conf2,"themes/",gl_conf->theme,"/",NULL);
	DIR *dir=opendir(path);
	g_free(path);
	if(dir)
	{
		path=g_strconcat(gl_dir_conf2,"themes/",gl_conf->theme,"/screenshot.png",NULL);
	}
	else
	{
		path=g_strconcat(gl_dir_conf1,"themes/",gl_conf->theme,"/screenshot.png",NULL);
	}
	closedir(dir);
	pixbuf=gdk_pixbuf_new_from_file_at_size(path,256,256,NULL);
	g_free(path);
	if(!pixbuf)
	{
		path=g_strconcat(gl_dir_icon,"404.png",NULL);
		pixbuf=gdk_pixbuf_new_from_file_at_size(path,256,256,NULL);
		g_free(path);
	}
	gtk_image_set_from_pixbuf(GTK_IMAGE(gui_dialog_th_image_preview),pixbuf);
	de_dialog_print(gui_dialog,__DE_INFORMATION,__FUNCTION__,_("To make the new theme active, Linnya must be restarted."));
	
	return FALSE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_change_sensitive_lrc_location
//*	DISCRIPTION:	change the sensitive of widget relative to equlizer
//*
//*****************************************************************************************************
gboolean gui_dialog_change_sensitive_lrc_location(GtkWidget *widget, gpointer data)
{
	gboolean sensitive;
	sensitive=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	if(sensitive)
	{
		if(gl_conf->lrc_location)
			g_free(gl_conf->lrc_location);
		gl_conf->lrc_location=g_strconcat("./",NULL);
		gtk_entry_set_text(GTK_ENTRY(gui_dialog_lrc_entry_lrc_location),gl_conf->lrc_location);
	}
	gtk_widget_set_sensitive(gui_dialog_lrc_button_lrc_location,!sensitive);
	
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_bind_lrc_location
//*	DISCRIPTION:	bind music library
//*
//*****************************************************************************************************
gboolean gui_dialog_bind_lrc_location(GtkWidget *widget,gpointer data)
{
	//过滤误设置
	if(!gui_dialog_run)
	{
		return FALSE;
	}
	
	gint result=0;
	GtkWidget *dialog=gtk_file_chooser_dialog_new(	_("Select LRC Location"),
													GTK_WINDOW(gui_dialog),
													GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
													GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
													GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
													NULL);
	result=gtk_dialog_run (GTK_DIALOG (dialog));
	switch (result)
	{
		case GTK_RESPONSE_ACCEPT:
			break;
		case GTK_RESPONSE_CANCEL:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
		default:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
	}
	gchar *path=gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
	gchar *str=g_strconcat(path,"/",NULL);
	g_free(path);
	
	gtk_entry_set_text(GTK_ENTRY(gui_dialog_lrc_entry_lrc_location),str);
	if(gl_conf->lrc_location)
		g_free(gl_conf->lrc_location);
	gl_conf->lrc_location=str;
	
	gtk_widget_destroy(dialog);
	return FALSE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_change_sensitive_lrc
//*	DISCRIPTION:	change the sensitive of widget relative to equlizer
//*
//*****************************************************************************************************
gboolean gui_dialog_change_sensitive_lrc(GtkWidget *widget, gpointer data)
{
	
	gl_conf->custom_lrc=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	
	gtk_widget_set_sensitive(gui_dialog_lrc_font_highlight,gl_conf->custom_lrc);
	gtk_widget_set_sensitive(gui_dialog_lrc_font_normal,gl_conf->custom_lrc);
	gtk_widget_set_sensitive(gui_dialog_lrc_font_desktop,gl_conf->custom_lrc);
	gtk_widget_set_sensitive(gui_dialog_lrc_color_highlight,gl_conf->custom_lrc);
	gtk_widget_set_sensitive(gui_dialog_lrc_color_normal,gl_conf->custom_lrc);
	gtk_widget_set_sensitive(gui_dialog_lrc_color_spectrum,gl_conf->custom_lrc);
	gtk_widget_set_sensitive(gui_dialog_lrc_color_desktop_bg1,gl_conf->custom_lrc);
	gtk_widget_set_sensitive(gui_dialog_lrc_color_desktop_bg2,gl_conf->custom_lrc);
	gtk_widget_set_sensitive(gui_dialog_lrc_color_desktop_fg1,gl_conf->custom_lrc);
	gtk_widget_set_sensitive(gui_dialog_lrc_color_desktop_fg2,gl_conf->custom_lrc);
	
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		gui_dialog_color_new_from_string
//*	DISCRIPTION:	create a new GdkColor from string like "0:0:0"
//*
//*****************************************************************************************************
GdkColor *gui_dialog_color_new_from_string(gchar *string)
{
	GdkColor *color=(GdkColor *)g_malloc(sizeof(GdkColor));
	gint r,g,b;
	
	gchar **str=g_strsplit_set (string, ":", -1);
	sscanf(str[0],"%d",&r);
	sscanf(str[1],"%d",&g);
	sscanf(str[2],"%d",&b);
	
	color->red=r;
	color->green=g;
	color->blue=b;
	return color;
}
