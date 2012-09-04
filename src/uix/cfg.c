/*
 * cfg_dialog.c
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * HEADERS
 */
#include "cfg.h"

/*
 * MACRO
 */
#define BACKUP_ENCODING_COUNT 72

/*
 * VARIABLES
 */
GtkWidget	*ly_cfg_dialog	=	NULL;
gchar *encoding[BACKUP_ENCODING_COUNT] = {
"Arabic (IBM-864)",
"Arabic (ISO-8859-6)",
"Arabic (Windows-1256)",
"Armenian (ARMSCII-8)",

"Baltic (ISO-8859-13)",
"Baltic (ISO-8859-4)",
"Baltic (Windows-1257)",

"Celtic (ISO-8859-14)",
"Central European (IBM-852)",
"Central European (ISO-8859-2)",
"Central European (MacCE)",
"Central European (Windows-1250)",
"Chinese Simplified (GB18030)",
"Chinese Simplified (GB2312)",
"Chinese Simplified (GBK)",
"Chinese Simplified (HZ)",
"Chinese Simplified (ISO-2022-CN)",
"Chinese Traditional (Big-5)",
"Chinese Traditional (Big-5-HKSCS)",
"Chinese Traditional (EUC-TW)",
"Croatian (MacCroatian)",
"Cyrillic (IBM-855)",
"Cyrillic (ISO-8859-5)",
"Cyrillic (ISO-IR-111)",
"Cyrillic (KOI8-R)",
"Cyrillic (MacCyrillic)",
"Cyrillic/Russian (CP-866)",
"Cyrillic/Ukrainian (KOI8-U)",
"Cyrillic (Windows-1251)",

"Georgian (GEOSTD8)",
"Greek (ISO-8859-7)",
"Greek (MacGreek)",
"Greek (Windows-1253)",
"Gujarati (MacGujarati)",
"Gurmukhi (MacGurmukhi)",

"Hebrew (IBM-862)",
"Hebrew (ISO-8859-8-I)",
"Hebrew Visual (ISO-8859-8)",
"Hebrew (Windows-1255)",
"Hindi (MacDevanagari)",

"Icelandic (MacIcelandic)",

"Japanese (EUC-JP)",
"Japanese (ISO-2022-JP)",
"Japanese (Shift-JIS)",

"Korean (EUC-KR)",
"Korean (ISO-2022-KR)",
"Korean (JOHAB)",
"Korean (UHC)",

"Nordic (ISO-8859-10)",

"Romanian (ISO-8859-16)",
"Romanian (MacRomanian)",

"South European (ISO-8859-3)",

"Thai (ISO-8859-11)",
"Thai (TIS-620)",
"Thai (Windows-874)",
"Turkish (IBM-857)",
"Turkish (ISO-8859-9)",
"Turkish (MacTurkish)",
"Turkish (Windows-1254)",

"Unicode (UTF-16)",
"Unicode (UTF-16BE)",
"Unicode (UTF-16LE)",
"Unicode (UTF-8)",

"Vietnamese (TCVN)",
"Vietnamese (VISCII)",
"Vietnamese (VPS)",
"Vietnamese (Windows-1258)",

"Western (IBM-850)",
"Western (ISO-8859-1)",
"Western (ISO-8859-15)",
"Western (MacRoman)",
"Western (Windows-1252)"
};

gboolean ly_cfg_lock_eql_equalizer=FALSE;
/*
 * FUNCTIONS
 */
static void	ly_cfg_dialog_init			(LyCfgDialog *obj);
static void	ly_cfg_dialog_class_init	(LyCfgDialogClass *klass);
gboolean	ly_cfg_dialog_on_selection_changed_cb(GtkTreeSelection *selection, gpointer data);
static void	ly_cfg_page_init			(LyCfgPage *obj);
static void	ly_cfg_page_class_init		(LyCfgPageClass *klass);
static void	ly_cfg_item_init			(LyCfgItem *obj);
static void	ly_cfg_item_class_init		(LyCfgItemClass *klass);
gboolean ly_cfg_item_on_expose_cb	(GtkWidget *widget, cairo_t *cr, gpointer data);

GtkWidget*		ly_cfg_new					();
void 			ly_cfg_on_create_cb(LyMbsMessage *message, gpointer data);
gboolean		ly_cfg_on_destroy_cb		(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_show_about_cb		(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_lib_autocheck_changed_cb	(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_lib_changed_cb	(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_encoding_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_aud_autoplay_changed_cb	(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_aud_mode_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_eql_list_cb		(gpointer stmt, gpointer data);
gboolean		ly_cfg_on_eql_auto_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_eql_equalizer_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_eql_band_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_eql_eq_save_cb	(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_eql_eq_delete_cb	(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_key_changed_cb	(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_key_press_cb		(GtkWidget *widget, GdkEvent  *event, gpointer data);
gboolean		ly_cfg_on_thm_theme_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_thm_decorated_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_thm_winbg_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_thm_custom_winbg_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_thm_sssbg_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_thm_custom_sssbg_changed_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_pli_show_about_cb	(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_pli_show_config_cb(GtkWidget *widget, gpointer data);
gboolean		ly_cfg_on_pli_changed_cb	(GtkWidget *widget, gpointer data);

void
ly_cfg_init ()
{
	ly_mbs_bind("config", "ui:win", ly_cfg_on_create_cb, NULL);
}
void
ly_cfg_fina ()
{
}

GtkWidget*
ly_cfg_new (void)
{
	gchar path[1024];
	gchar str[1024];
	int i=0;
	int state=0;

	GtkWidget *dialog;
	GtkWidget *page;
	GtkWidget *item;
	GtkWidget *vbox;
	GtkWidget *hbox;

	GtkWidget *button;
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *check;
	GtkWidget *combo;
	GtkWidget *table;
	GtkWidget *vscale;
	GtkWidget *image;
	GdkPixbuf *pixbuf;

	GHashTableIter iter1;
	gpointer key, value;

	//main dialog
	dialog=ly_cfg_dialog_new(_("Configuration"));
	g_signal_connect(G_OBJECT(dialog), "destroy", G_CALLBACK(ly_cfg_on_destroy_cb), NULL);

	//button
	button=gtk_button_new_from_stock(GTK_STOCK_ABOUT);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_cfg_on_show_about_cb),NULL);
	gtk_box_pack_end(GTK_BOX(gtk_dialog_get_action_area(GTK_DIALOG(dialog))),button,FALSE,FALSE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_cfg_on_destroy_cb),NULL);
	gtk_box_pack_end(GTK_BOX(gtk_dialog_get_action_area(GTK_DIALOG(dialog))),button,FALSE,FALSE,0);

	//page1 Library
	g_snprintf(path,sizeof(path),"%sicon/lib.png",LY_GLB_PROG_UIXDIR);
	page=ly_cfg_page_new(_("Library"), _("All the Music Data"), path);
	ly_cfg_dialog_append(LY_CFG_DIALOG(dialog), page);

	item=ly_cfg_item_new(_("Auto Check Library When Program Start"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	check=gtk_check_button_new_with_label(_("Enable Auto Check"));
	ly_reg_get("lib_autocheck", "%d", &state);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), (gboolean)state);
	ly_cfg_item_append(LY_CFG_ITEM(item), check);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_cfg_on_lib_autocheck_changed_cb),NULL);

	item=ly_cfg_item_new(_("Library location"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	ly_cfg_item_append(LY_CFG_ITEM(item), hbox);
	entry=gtk_entry_new();
	gtk_widget_set_sensitive(entry,FALSE);
	ly_reg_get("lib_path","%s",str);
	gtk_entry_set_text(GTK_ENTRY(entry),str);
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_cfg_on_lib_changed_cb), entry);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);

	item=ly_cfg_item_new(_("Backup Encoding"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	char extra_encoding[1024]="Chinese Simplified (GB18030)";
	ly_reg_get("dbm_extra_encoding", "%1023[^\n]", extra_encoding);
	combo=gtk_combo_box_text_new();
	ly_cfg_item_append(LY_CFG_ITEM(item), combo);
	i=0;
	for(i=0;i<BACKUP_ENCODING_COUNT;i++)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), encoding[i]);
		if(g_str_equal(extra_encoding, encoding[i]))
		{
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo), i);
		}
	}
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(ly_cfg_on_encoding_changed_cb), NULL);

	//page2 Audio
	g_snprintf(path,sizeof(path),"%sicon/audio.png",LY_GLB_PROG_UIXDIR);
	page=ly_cfg_page_new(_("Audio"), _("Set all about playing audio"), path);
	ly_cfg_dialog_append(LY_CFG_DIALOG(dialog), page);

	item=ly_cfg_item_new(_("Auto Play When Program Start"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	check=gtk_check_button_new_with_label(_("Enable Auto Play"));
	ly_reg_get("aud_autoplay", "%d", &state);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), (gboolean)state);
	ly_cfg_item_append(LY_CFG_ITEM(item), check);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_cfg_on_aud_autoplay_changed_cb),NULL);

	item=ly_cfg_item_new(_("Play mode"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	int random=0;
	int repeat=1;
	int single=0;
	ly_reg_get("aud_mode","%d:%d:%d",&random,&repeat,&single);
	vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	ly_cfg_item_append(LY_CFG_ITEM(item), vbox);
	check=gtk_check_button_new_with_label(_("Random"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)random);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_cfg_on_aud_mode_changed_cb), "random");
	gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);
	check=gtk_check_button_new_with_label(_("Repeat"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)repeat);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_cfg_on_aud_mode_changed_cb), "repeat");
	gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);
	check=gtk_check_button_new_with_label(_("Single"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)single);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_cfg_on_aud_mode_changed_cb), "single");
	gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);

	//page3 Equalizer
	g_snprintf(path, sizeof(path),"%sicon/equalizer.png",LY_GLB_PROG_UIXDIR);
	page=ly_cfg_page_new(_("Equalizer"), _("Set equalizer"), path);
	ly_cfg_dialog_append(LY_CFG_DIALOG(dialog), page);

	item=ly_cfg_item_new(_("Auto Equalizer"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	int eql_auto=1;
	ly_reg_get("eql_auto", "%d", &eql_auto);
	check=gtk_check_button_new_with_label(_("Choose Equalizer Setting by Linnya Automatically"));
	ly_cfg_item_append(LY_CFG_ITEM(item), check);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),(gboolean)eql_auto);

	item=ly_cfg_item_new(_("Custom Equalizer Setting"));
	if(eql_auto)
	{
		gtk_widget_set_sensitive(item, FALSE);
	}
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_cfg_on_eql_auto_changed_cb), item);
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	ly_cfg_item_append(LY_CFG_ITEM(item), hbox);
	char equalizer[1024]="default";
	ly_reg_get("equalizer", "%1024[^\n]", equalizer);
	combo=gtk_combo_box_text_new();
	ly_dbm_exec("SELECT name FROM equalizers", ly_cfg_on_eql_list_cb, combo);
	gtk_box_pack_start(GTK_BOX(hbox),combo,TRUE,TRUE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_cfg_on_eql_eq_save_cb),item);
	button=gtk_button_new_from_stock(GTK_STOCK_DELETE);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_cfg_on_eql_eq_delete_cb),combo);
	hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	g_signal_connect(G_OBJECT(combo), "changed",G_CALLBACK(ly_cfg_on_eql_equalizer_changed_cb), hbox);
	ly_cfg_item_append(LY_CFG_ITEM(item), hbox);
	LyEqlEqualizer *eq=ly_eql_new_by_conf();
	for(i=0;i<10;i++)
	{
		vscale=gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,-24,12,0.1);
		gtk_scale_set_value_pos(GTK_SCALE(vscale),GTK_POS_BOTTOM);
		gtk_range_set_inverted(GTK_RANGE(vscale),TRUE);
		gtk_range_set_value(GTK_RANGE(vscale),eq->band[i]);
		gtk_widget_set_size_request(vscale, -1, 200);
		gtk_box_pack_start(GTK_BOX(hbox),vscale,TRUE,TRUE,0);
		g_signal_connect(G_OBJECT(vscale), "value-changed", G_CALLBACK(ly_cfg_on_eql_band_changed_cb), item);
	}
	ly_eql_free(eq);

	//page4 Keyboard
	g_snprintf(path,sizeof(path),"%sicon/key.png",LY_GLB_PROG_UIXDIR);
	page=ly_cfg_page_new(_("Keyboard"),_("Set all shortcut keys of Linnya"),path);
	ly_cfg_dialog_append(LY_CFG_DIALOG(dialog), page);

	item=ly_cfg_item_new(_("Shortcuts"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	table=gtk_table_new(g_list_length(ly_pli_get_list())+1, 2, FALSE);
	ly_cfg_item_append(LY_CFG_ITEM(item), table);
	gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(_("Name and Keys")), 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(_("Preferense")), 1, 2, 0, 1);
	g_hash_table_iter_init (&iter1, ly_key_get_keybinds());
	i=1;
	char str_key[1024]="";
	while (g_hash_table_iter_next (&iter1, &key, &value))
	{
		g_snprintf(str_key, sizeof(str_key),"{%s}%s_%s:%s", (char *)key, ((LyKeyKeybind*)value)->mask0, ((LyKeyKeybind*)value)->mask1, ((LyKeyKeybind*)value)->key);
		label=gtk_label_new(str_key);
		gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, i, i+1);
		button=gtk_button_new_from_stock(GTK_STOCK_PREFERENCES);
		g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_cfg_on_key_changed_cb), label);
		gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, i, i+1);
		i++;
	}

	//page5 Theme
	g_snprintf(path,sizeof(path),"%sicon/theme.png",LY_GLB_PROG_UIXDIR);
	page=ly_cfg_page_new(_("Theme"), _("Set the looks of Linnya."),path);
	ly_cfg_dialog_append(LY_CFG_DIALOG(dialog), page);
	item=ly_cfg_item_new(_("Theme"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	char theme_name[128]="default";
	ly_reg_get("thm_theme", "%s", theme_name);
	combo=gtk_combo_box_text_new();
	ly_cfg_item_append(LY_CFG_ITEM(item), combo);
	GList *list=ly_thm_get_list();
	GList *p=list;
	char *th_name;
	i=0;
	while(p)
	{
		th_name=p->data;

		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), th_name);
		if(g_str_equal(th_name, theme_name))
		{
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo), i);
		}
		g_free(th_name);
		i++;
		p=p->next;
	}
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(ly_cfg_on_thm_theme_changed_cb), NULL);

	item=ly_cfg_item_new(_("Main Window Style"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	check=gtk_check_button_new_with_label(_("Enable Window Manager's Decorated"));
	int decorated=0;
	ly_reg_get("thm_decorated", "%d", &decorated);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), (gboolean)decorated);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_cfg_on_thm_decorated_changed_cb), NULL);
	ly_cfg_item_append(LY_CFG_ITEM(item), check);
	check=gtk_check_button_new_with_label(_("Enable Transparent Effective"));
	int custom_winbg=0;
	ly_reg_get("thm_custom_winbg", "%d", &custom_winbg);
	if(!gtk_widget_is_composited(ly_win_get_window()->win))
	{
		if(custom_winbg)
		{
			custom_winbg=0;
			ly_reg_get("thm_custom_winbg", "%d", custom_winbg);
		}
		gtk_widget_set_sensitive(check, FALSE);
	}
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), (gboolean)custom_winbg);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_cfg_on_thm_custom_winbg_changed_cb), NULL);
	ly_cfg_item_append(LY_CFG_ITEM(item), check);
	GdkColor *color=(GdkColor *)g_malloc(sizeof(GdkColor));
	guint color_alpha=65535;
	ly_reg_get("thm_winbg", "%d:%d:%d:%d", &(color->red), &(color->green), &(color->blue), &color_alpha);
	button=gtk_color_button_new_with_color(color);
	gtk_color_button_set_use_alpha(GTK_COLOR_BUTTON(button), TRUE);
	g_free(color);
	gtk_color_button_set_alpha(GTK_COLOR_BUTTON(button), color_alpha);
	ly_cfg_item_append(LY_CFG_ITEM(item), button);
	g_signal_connect(G_OBJECT(button), "color-set", G_CALLBACK(ly_cfg_on_thm_winbg_changed_cb), hbox);

	item=ly_cfg_item_new(_("Session Style"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	check=gtk_check_button_new_with_label(_("Use Custom Session Background Image"));
	int custom_sssbg=0;
	ly_reg_get("thm_custom_sssbg", "%d", &custom_sssbg);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), (gboolean)custom_sssbg);
	g_signal_connect(G_OBJECT(check), "toggled", G_CALLBACK(ly_cfg_on_thm_custom_sssbg_changed_cb), NULL);
	ly_cfg_item_append(LY_CFG_ITEM(item), check);
	ly_reg_get("thm_sssbg", "%[^\n]", str);
	hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	ly_cfg_item_append(LY_CFG_ITEM(item), hbox);
	entry=gtk_entry_new();
	gtk_widget_set_sensitive(entry,FALSE);
	gtk_entry_set_text(GTK_ENTRY(entry),str);
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ly_cfg_on_thm_sssbg_changed_cb), entry);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);

	//page6 Plug-ins
	g_snprintf(path,sizeof(path),"%sicon/plugin.png",LY_GLB_PROG_UIXDIR);
	page=ly_cfg_page_new(_("Plugins"),_("All the plug-ins of Linnya."),path);
	ly_cfg_dialog_append(LY_CFG_DIALOG(dialog), page);

	item=ly_cfg_item_new(_("Plugins"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	table=gtk_table_new(g_list_length(ly_pli_get_list()), 5, FALSE);
	ly_cfg_item_append(LY_CFG_ITEM(item), table);

	i=0;
	LyPliPlugin *pl;
	gboolean locked;
	gchar *name;
	gchar *logo;
	gchar *config_symbol=NULL;
	p=ly_pli_get_list();
	while (p)
	{
		pl=LY_PLI_PLUGIN(p->data);
		check=gtk_check_button_new();
		g_object_get(G_OBJECT(pl), "name", &name, "locked", &locked, "logo", &logo, "config_symbol", &config_symbol, NULL);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), !locked);
		g_signal_connect(G_OBJECT(check),"toggled",G_CALLBACK(ly_cfg_on_pli_changed_cb), pl);
		gtk_table_attach_defaults(GTK_TABLE(table),check,0,1,i,i+1);

		pixbuf=gdk_pixbuf_new_from_file_at_size(logo, 16, 16, NULL);
		g_free(logo);
		image=gtk_image_new_from_pixbuf(pixbuf);
		g_object_unref(pixbuf);
		gtk_table_attach_defaults(GTK_TABLE(table),image,1,2,i,i+1);

		label=gtk_label_new(name);
		g_free(name);
		gtk_table_attach_defaults(GTK_TABLE(table),label,2,3,i,i+1);

		if(config_symbol)
		{
			g_free(config_symbol);
			button=gtk_button_new_from_stock(GTK_STOCK_PREFERENCES);
			g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_cfg_on_pli_show_config_cb), pl);
			gtk_table_attach_defaults(GTK_TABLE(table),button,3,4,i,i+1);
		}

		button=gtk_button_new_from_stock(GTK_STOCK_ABOUT);
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(ly_cfg_on_pli_show_about_cb), pl);
		gtk_table_attach_defaults(GTK_TABLE(table),button,4,5,i,i+1);
		i++;
		p=p->next;
	}

	return dialog;
}


void ly_cfg_on_create_cb(LyMbsMessage *message, gpointer data)
{
	if(ly_cfg_dialog!=NULL)
		return;
	ly_cfg_dialog=ly_cfg_new();
	gtk_widget_show_all(ly_cfg_dialog);
}

gboolean ly_cfg_on_destroy_cb(GtkWidget *widget, gpointer data)
{
	if(ly_cfg_dialog)
		gtk_widget_destroy(ly_cfg_dialog);
	ly_cfg_dialog=NULL;
	return FALSE;
}


gboolean ly_cfg_on_show_about_cb(GtkWidget *widget, gpointer data)
{
	gchar *path=NULL;
	GdkPixbuf *logo=NULL;

	path=g_strconcat(LY_GLB_PROG_UIXDIR,"icon/linnya.png",NULL);
	logo=gdk_pixbuf_new_from_file_at_size(path, 48, 48,NULL);
	g_free(path);

	char *artists[]={"Hillar Liiv<liivhillar@gmail.com>","Edward<edward@linnya.org>",NULL};
	char *authors[]={"Edward<edward@linnya.org>","Carl Yu<shdxcy@gmail.com>",NULL};
	char translators[]={"Edward<edward@linnya.org>"};

	gtk_show_about_dialog(GTK_WINDOW(ly_cfg_dialog),
						  "program-name",	LY_GLA_APPLICATION,
						  "artists",		&artists,
						  "authors",		&authors,
						  "comments",		LY_GLA_COMMENTS,
						  "copyright",		LY_GLA_COPYRIGHT,
						  "license",		LY_GLA_LICENSE_FULL,
						  "logo",			logo,
						  "translator-credits",&translators,
						  "version",		LY_GLA_VERSION_STR,
						  "website",		LY_GLA_WEBSITE,
						  "website-label",	_("Official Website"),
						  "wrap-license",	TRUE,
						  NULL);
	g_object_unref(logo);
	return FALSE;
}

gboolean ly_cfg_on_pli_show_about_cb(GtkWidget *widget, gpointer data)
{
	g_return_val_if_fail((data!=NULL), FALSE);
	LyPliPlugin *pl=LY_PLI_PLUGIN(data);
	gchar *name;
	gchar *alias;
	gchar *version;
	gchar *author;
	gchar *comment;
	gchar *license;
	gchar *logo;
	gchar *website;

	g_object_get(G_OBJECT(pl),
		"name", &name,
		"alias", &alias,
		"version", &version,
		"author", &author,
		"comment", &comment,
		"license", &license,
		"logo", &logo,
		"website", &website,
		NULL);

	GdkPixbuf *img_logo=NULL;
	GtkWidget *about=gtk_about_dialog_new();
	if(author)
	{
		char *authors[]={author,NULL};
		gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about), (const char **)authors);
		char copyright[1024]="";
		g_snprintf(copyright, sizeof(copyright), _("(c) %s"), author);
		gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), copyright);
		g_free(author);
	}
	if(comment)
	{
		gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about), comment);
		g_free(comment);
	}
	if(license)
	{
		gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about), license);
		g_free(license);
	}
	if(logo)
	{
		img_logo=gdk_pixbuf_new_from_file_at_size(logo, 48, 48,NULL);
		gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), img_logo);
		g_free(logo);
	}
	if(alias)
	{
		gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about), alias);
		g_free(alias);
	}
	if(version)
	{
		gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), version);
		g_free(version);
	}
	if(website)
	{
		gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about), website);
		gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about), _("Official Website"));
		g_free(website);
	}
	gtk_dialog_run(GTK_DIALOG(about));
	if(img_logo)
	{
		g_object_unref(img_logo);
	}
	gtk_widget_destroy(about);
	return FALSE;
}

gboolean ly_cfg_on_pli_show_config_cb(GtkWidget *widget, gpointer data)
{
	g_return_val_if_fail(data!=NULL, FALSE);
	GtkWidget *page=ly_pli_plugin_config(LY_PLI_PLUGIN(data));
	g_return_val_if_fail(page!=NULL, FALSE);

	GtkWidget *dialog=ly_cfg_dialog_new(_("Configuration of Plugins"));
	gtk_window_set_default_size(GTK_WINDOW(dialog), 450, 400);
	gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);

	ly_cfg_dialog_append(LY_CFG_DIALOG(dialog), page);
	gtk_widget_show_all(dialog);
	ly_cfg_dialog_set_nav_visible(LY_CFG_DIALOG(dialog), FALSE);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return FALSE;
}

gboolean	ly_cfg_on_pli_changed_cb	(GtkWidget *widget, gpointer data)
{
	g_return_val_if_fail(!data, FALSE);
	LyPliPlugin *pl=(LyPliPlugin*)data;
	gboolean state=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	GtkWidget *widget1;
	gchar *name;
	g_object_get(G_OBJECT(pl),"name", &name, "widget", &widget1, NULL);
	if(state==FALSE && widget1)
	{
		ly_sss_destroy(widget1);
	}
	if(state)
	{
		ly_pli_unlock(name);
	}
	else
	{
		ly_pli_lock(name);
	}
	ly_sss_tab_add_refresh();
	ly_dbg_message(_("The configuration button of this plugin will NOT be updated untill the configuration dialog restart!"));
	g_free(name);
	return FALSE;
}

gboolean ly_cfg_on_lib_autocheck_changed_cb(GtkWidget *widget, gpointer data)
{
	ly_reg_set("lib_autocheck", "%d", (int)gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
	return FALSE;
}

gboolean ly_cfg_on_lib_changed_cb(GtkWidget *widget, gpointer data)
{
	gint result=0;
	GtkWidget *dialog=gtk_file_chooser_dialog_new(_("Select library directory"),
												  GTK_WINDOW(ly_cfg_dialog),
												  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
												  GTK_STOCK_OPEN,
												  GTK_RESPONSE_ACCEPT,
												  GTK_STOCK_CANCEL,
												  GTK_RESPONSE_CANCEL,
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

	gtk_entry_set_text(GTK_ENTRY(data),str);

	ly_reg_set("lib_path", "%s", str);
	g_free(str);
	gtk_widget_destroy(dialog);
	ly_lib_check_library();
	ly_mbs_put("lib_changed", "ui:config", NULL);
	return FALSE;
}
gboolean ly_cfg_on_encoding_changed_cb(GtkWidget *widget, gpointer data)
{
	char *encoding=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	ly_reg_set("dbm_extra_encoding", "%s", encoding);
	return FALSE;
}

gboolean ly_cfg_on_aud_autoplay_changed_cb(GtkWidget *widget, gpointer data)
{
	ly_reg_set("aud_autoplay", "%d", (int)gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
	return FALSE;
}

gboolean ly_cfg_on_aud_mode_changed_cb(GtkWidget *widget, gpointer data)
{
	char *type=(char *)data;
	int random=0;
	int repeat=1;
	int single=0;

	ly_reg_get("aud_mode", "%d:%d:%d", &random, &repeat, &single);

	if(g_str_equal(type, "random"))
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		{
			random=1;
		}
		else
		{
			random=0;
		}
	}
	else if(g_str_equal(type,"repeat"))
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		{
			repeat=1;
		}
		else
		{
			repeat=0;
		}
	}
	else if(g_str_equal(type, "single"))
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		{
			single=1;
		}
		else
		{
			single=0;
		}
	}
	ly_reg_set("aud_mode", "%d:%d:%d", random, repeat, single);
	return FALSE;
}

gboolean ly_cfg_on_key_changed_cb(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *label;
	dialog=gtk_dialog_new_with_buttons(_("Key"),
									   GTK_WINDOW(ly_cfg_dialog),
									   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
									   GTK_STOCK_OK,
									   GTK_RESPONSE_ACCEPT,
									   GTK_STOCK_CANCEL,
									   GTK_RESPONSE_CANCEL,
									   NULL);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 100, 80);

	label=gtk_label_new(_("Press key to change shortcut ..."));
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label, FALSE, FALSE, 0);

	label=gtk_label_new(gtk_label_get_text(GTK_LABEL(data)));
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(dialog), "key_press_event", G_CALLBACK(ly_cfg_on_key_press_cb), label);

	gtk_widget_show_all(dialog);

	int result=0;
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	switch(result)
	{
		case GTK_RESPONSE_CANCEL:
			gtk_widget_destroy(dialog);
			return FALSE;
			break;
		case GTK_RESPONSE_ACCEPT:
			break;
		default:
			return FALSE;
			break;
	}

	char keyname[128]="";
	char mask0[128]="";
	char mask1[128]="";
	char key[128]="";
	const char *str=gtk_label_get_text(GTK_LABEL(label));
	sscanf(str,"{%128[^\n}]%128[^\n_]%128[^\n:]%128[^\n]",keyname, mask0, mask1, key);
	if(ly_key_get_conflict(keyname, mask0+1, mask1+1, key+1))
 	{
 		ly_dbg_message(_("Shortcuts conflict!"));
 		gtk_widget_destroy(dialog);
 		return FALSE;
 	}

	gtk_label_set_text(GTK_LABEL(data),str);
 	ly_key_unbind(keyname);
	ly_key_set(keyname, mask0+1, mask1+1, key+1, -1, NULL, NULL);
	ly_key_bind(keyname);
	gtk_widget_destroy(dialog);
	return FALSE;
}

gboolean ly_cfg_on_key_press_cb(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	if(event->type!=GDK_KEY_PRESS)
	{
		return FALSE;
	}

	gchar mask0[64]="";
	gchar mask1[64]="";
	gchar key[64]="";

	if((event->key).state&GDK_CONTROL_MASK)
	{
		g_strlcpy(mask0,"ctrl",sizeof(mask0));
	}
	if((event->key).state&GDK_MOD1_MASK)
	{
		if(g_str_equal(mask0,""))
		{
			g_strlcpy(mask0,"alt",sizeof(mask0));
		}
		else if(g_str_equal(mask1,""))
		{
			g_strlcpy(mask1,"alt",sizeof(mask1));
		}
	}
	if((event->key).state&GDK_SUPER_MASK)
	{
		if(g_str_equal(mask0,""))
		{
			g_strlcpy(mask0,"super",sizeof(mask0));
		}
		else if(g_str_equal(mask1,""))
		{
			g_strlcpy(mask1,"super",sizeof(mask1));
		}
	}
	if((event->key).state&GDK_SHIFT_MASK)
	{
		if(g_str_equal(mask0,""))
		{
			g_strlcpy(mask0,"shift",sizeof(mask0));
		}
		else if(g_str_equal(mask1,""))
		{
			g_strlcpy(mask1,"shift",sizeof(mask1));
		}
	}
	if((event->key).state&GDK_META_MASK)
	{
		if(g_str_equal(mask0,""))
		{
			g_strlcpy(mask0,"meta",sizeof(mask0));
		}
		else if(g_str_equal(mask1,""))
		{
			g_strlcpy(mask1,"meta",sizeof(mask1));
		}
	}
	if((event->key).state&GDK_HYPER_MASK)
	{
		if(g_str_equal(mask0,""))
		{
			g_strlcpy(mask0,"hyper",sizeof(mask0));
		}
		else
		{
			g_strlcpy(mask1,"hyper",sizeof(mask1));
		}
	}
	g_strlcpy(key,gdk_keyval_name((event->key).keyval), sizeof(key));
	char keyname[128]="";
	sscanf(gtk_label_get_text(GTK_LABEL(data)), "{%128[^\n}]", keyname);
	gchar str[512]="";
	g_snprintf(str, sizeof(str), "{%s}%s_%s:%s", keyname, mask0, mask1, key);
	gtk_label_set_text(GTK_LABEL(data), str);
	return FALSE;
}

gboolean ly_cfg_on_thm_theme_changed_cb(GtkWidget *widget, gpointer data)
{
	char *theme_name=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	ly_reg_set("thm_theme", "%s", theme_name);
	ly_dbg_message(_("Setting will not be actived until program restart!"));
	return FALSE;
}

gboolean ly_cfg_on_thm_decorated_changed_cb(GtkWidget *widget, gpointer data)
{
	ly_reg_set("thm_decorated", "%d", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
	return FALSE;
}

gboolean ly_cfg_on_thm_custom_winbg_changed_cb(GtkWidget *widget, gpointer data)
{
	ly_reg_set("thm_custom_winbg", "%d", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
	return FALSE;
}

gboolean ly_cfg_on_thm_winbg_changed_cb(GtkWidget *widget, gpointer data)
{
	GdkColor *color=(GdkColor *)g_malloc(sizeof(GdkColor));
	guint color_alpha=65535;
	gtk_color_button_get_color(GTK_COLOR_BUTTON(widget), color);
	color_alpha=gtk_color_button_get_alpha(GTK_COLOR_BUTTON(widget));
	ly_reg_set("thm_winbg", "%d:%d:%d:%d", (color->red), (color->green), (color->blue), color_alpha);
	g_free(color);
	return FALSE;
}
gboolean ly_cfg_on_thm_custom_sssbg_changed_cb(GtkWidget *widget, gpointer data)
{
	ly_reg_set("thm_custom_sssbg", "%d", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
	return FALSE;
}
gboolean ly_cfg_on_thm_sssbg_changed_cb(GtkWidget *widget, gpointer data)
{
	gint result=0;
	GtkWidget *dialog=gtk_file_chooser_dialog_new(_("Select Session Background directory"),
												  GTK_WINDOW(ly_cfg_dialog),
												  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
												  GTK_STOCK_OPEN,
												  GTK_RESPONSE_ACCEPT,
												  GTK_STOCK_CANCEL,
												  GTK_RESPONSE_CANCEL,
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

	gtk_entry_set_text(GTK_ENTRY(data),str);

	ly_reg_set("thm_sssbg", "%s", str);
	g_free(str);
	gtk_widget_destroy(dialog);
	return FALSE;
}


/*
 * EQL
 */
gboolean ly_cfg_on_eql_list_cb(gpointer stmt, gpointer data)
{
	if(data==NULL||stmt==NULL)
		return TRUE;

	const char *name= (const char *)sqlite3_column_text(stmt, 0);
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data), name);

	char equalizer[1024]="default";
	ly_reg_get("equalizer", "%[^\n]", equalizer);
	if(g_str_equal(name, equalizer))
	{
		GtkTreeModel *model;
		int index=0;
		model=gtk_combo_box_get_model(GTK_COMBO_BOX(data));
		index=gtk_tree_model_iter_n_children(GTK_TREE_MODEL(model), NULL);
		gtk_combo_box_set_active(GTK_COMBO_BOX(data), index-1);
	}
	return FALSE;
}

gboolean ly_cfg_on_eql_auto_changed_cb(GtkWidget *widget, gpointer data)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	{
		ly_reg_set("eql_auto", "%d", 1);
		gtk_widget_set_sensitive(GTK_WIDGET(data), FALSE);
	}
	else
	{
		ly_reg_set("eql_auto", "%d", 0);
		gtk_widget_set_sensitive(GTK_WIDGET(data), TRUE);
	}
	return FALSE;
}

gboolean ly_cfg_on_eql_equalizer_changed_cb(GtkWidget *widget, gpointer data)
{
	ly_cfg_lock_eql_equalizer=TRUE;
	char *name= gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	ly_reg_set("equalizer", "%s", name);
	g_free(name);

	LyEqlEqualizer *eq=ly_eql_new_by_conf();
	GList *list=NULL;
	GList *p=NULL;
	list=gtk_container_get_children(GTK_CONTAINER(data));
	p=list;

	int i=0;
	for(i=0;i<10;i++)
	{
		gtk_range_set_value(GTK_RANGE(p->data), eq->band[i]);
		p=p->next;
	}
	ly_eql_free(eq);
	ly_cfg_lock_eql_equalizer=FALSE;
	return FALSE;
}

gboolean ly_cfg_on_eql_band_changed_cb(GtkWidget *widget, gpointer data)
{
	if(ly_cfg_lock_eql_equalizer)
		return FALSE;

	LyEqlEqualizer *eq=ly_eql_new_by_conf();
	GList *list=NULL;
	GList *sublist=NULL;
	GList *p=NULL;
	GList *q=NULL;
	list=gtk_container_get_children(GTK_CONTAINER(ly_cfg_item_get_content_area(data)));
	int i=0;
	p=list->next;
	sublist=gtk_container_get_children(GTK_CONTAINER(p->data));
	q=sublist;
	for(i=0;i<10;i++)
	{
		eq->band[i]=gtk_range_get_value(GTK_RANGE(q->data));
		q=q->next;
	}

	ly_eql_set_eq(eq);
	ly_eql_free(eq);
	return FALSE;
}

gboolean ly_cfg_on_eql_eq_save_cb(GtkWidget *widget, gpointer data)
{
	LyEqlEqualizer *eq=ly_eql_new_by_conf();

	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	int result;
	char *name=NULL;

	dialog=gtk_dialog_new_with_buttons(_("Save Equalizer"),
					 GTK_WINDOW(ly_win_get_window()->win),
					 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				     GTK_STOCK_OK,
				     GTK_RESPONSE_ACCEPT,
				     GTK_STOCK_CANCEL,
				     GTK_RESPONSE_REJECT,
				     NULL);
	gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
	hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),hbox,TRUE,TRUE,0);
	label=gtk_label_new(_("Eq Name :"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entry=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), eq->name);
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
	name=(char *)gtk_entry_get_text(GTK_ENTRY(entry));
	if(!name || g_str_equal(name,""))
		gtk_widget_destroy(dialog);

	g_strlcpy(eq->name, name, sizeof(eq->name));
	gtk_widget_destroy(dialog);

	GList *list=NULL;
	GList *sublist=NULL;
	GList *p=NULL;
	GList *q=NULL;
	list=gtk_container_get_children(GTK_CONTAINER(ly_cfg_item_get_content_area(data)));
	p=list;

	int i=0;
	p=p->next;
	sublist=gtk_container_get_children(GTK_CONTAINER(p->data));
	q=sublist;
	for(i=0;i<10;i++)
	{
		eq->band[i]=gtk_range_get_value(GTK_RANGE(q->data));
		q=q->next;
	}
	ly_eql_put(eq);
	ly_reg_set("equalizer", "%s", eq->name);


	p=p->prev;
	sublist=gtk_container_get_children(GTK_CONTAINER(p->data));
	q=sublist;
	GtkListStore *model;
	model=gtk_list_store_new(1, G_TYPE_STRING);
	gtk_combo_box_set_model(GTK_COMBO_BOX(q->data), GTK_TREE_MODEL(model));
	ly_dbm_exec("SELECT name FROM equalizers", ly_cfg_on_eql_list_cb, q->data);
	ly_eql_free(eq);
	return FALSE;
}

gboolean ly_cfg_on_eql_eq_delete_cb(GtkWidget *widget, gpointer data)
{
	char *name=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(data));
	char eq_name[1024]="";
	g_strlcpy(eq_name, name, sizeof(eq_name));
	g_free(name);

	if(g_str_equal(eq_name, "default"))
	{
		ly_dbg_message(_("Cannot delete the default equalizer setting!"));
		return FALSE;
	}
	ly_dbm_replace_str(eq_name, sizeof(eq_name));
	char sql[1024]="";
	g_snprintf(sql, sizeof(sql), "DELETE FROM equalizers WHERE name='%s'", eq_name);
	ly_dbm_exec(sql, NULL, NULL);

	ly_reg_set("equalizer", "%s", "default");
	GtkListStore *model;
	model=gtk_list_store_new(1, G_TYPE_STRING);
	gtk_combo_box_set_model(GTK_COMBO_BOX(data), GTK_TREE_MODEL(model));
	ly_dbm_exec("SELECT name FROM equalizers", ly_cfg_on_eql_list_cb, data);
	return FALSE;
}

