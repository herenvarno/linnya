#include "dnc_config.h"

#define LY_3DNC_CONFIG_COV_SERVER_NUM 2
#define LY_3DNC_CONFIG_LRC_SERVER_NUM 2

char *ly_3dnc_config_cov_servers[LY_3DNC_CONFIG_COV_SERVER_NUM]={
"last.fm",
"top100"
};

char *ly_3dnc_config_lrc_servers[LY_3DNC_CONFIG_COV_SERVER_NUM]={
"qianqian",
"lrc123"
};

gboolean ly_3dnc_config_set_cov_cb(GtkWidget *widget, gpointer data);
gboolean ly_3dnc_config_set_lrc_cb(GtkWidget *widget, gpointer data);


GtkWidget* ly_3dnc_config()
{
	GtkWidget *page;
	GtkWidget *item;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *combo;
	char server[128]="";
	int i=0;
	
	char name[1024]="";
	LyPliPlugin *pl=ly_pli_get("dnc");
	g_snprintf(name, sizeof(name), "PLUGIN:%s", pl->name);
	page=ly_cfg_page_new(name, pl->alias, pl->logo);
	
	item=ly_cfg_item_new(_("Cover Download Server"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	combo=gtk_combo_box_text_new();
	ly_reg_get("3dnc_cov_server", "%[^\n]", server);
	for(i=0;i<LY_3DNC_CONFIG_COV_SERVER_NUM;i++)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), ly_3dnc_config_cov_servers[i]);
		if(g_str_equal(ly_3dnc_config_cov_servers[i], server))
		{
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo),i);
		}
	}
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(ly_3dnc_config_set_cov_cb), NULL);
	ly_cfg_item_append(LY_CFG_ITEM(item), combo);
	
	item=ly_cfg_item_new(_("Lyrics Download Server"));
	ly_cfg_page_append(LY_CFG_PAGE(page), item);
	combo=gtk_combo_box_text_new();
	ly_reg_get("3dnc_lrc_server", "%[^\n]", server);
	for(i=0;i<LY_3DNC_CONFIG_COV_SERVER_NUM;i++)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), ly_3dnc_config_lrc_servers[i]);
		if(g_str_equal(ly_3dnc_config_lrc_servers[i], server))
		{
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo),i);
		}
	}
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(ly_3dnc_config_set_lrc_cb), NULL);
	ly_cfg_item_append(LY_CFG_ITEM(item), combo);
	
	return page;
}

gboolean ly_3dnc_config_set_cov_cb(GtkWidget *widget, gpointer data)
{
	gchar *str=NULL;
	str=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	ly_reg_set("3dnc_cov_server", "%s", str);
	g_free(str);
	return FALSE;
}

gboolean ly_3dnc_config_set_lrc_cb(GtkWidget *widget, gpointer data)
{
	gchar *str=NULL;
	str=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	ly_reg_set("3dnc_lrc_server", "%s", str);
	g_free(str);
	return FALSE;
}
