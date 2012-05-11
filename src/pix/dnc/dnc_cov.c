#include "dnc_cov.h"

/*
 * VARIABLES
 */
static GMutex *ly_3dnc_cov_mutex=NULL;
static gchar ly_3dnc_cov_artist[128]="";
static gchar ly_3dnc_cov_album[128]="";

/*
 * FUNCTIONS
 */

gboolean	ly_3dnc_cov_check(gpointer message, gpointer data);
gpointer	ly_3dnc_cov_search(gpointer data);
gboolean	ly_3dnc_cov_notify(gpointer data);
gpointer	ly_3dnc_cov_analysis(gpointer data);
gpointer	ly_3dnc_cov_down(gpointer data);
size_t		ly_3dnc_cov_down_cb(char *buffer, size_t size, size_t nitems, void *outstream);


void		ly_3dnc_cov_init()
{
	char server[128]="last.fm";
	if(!ly_reg_get("3dnc_cov_server", "%[^\n]", server))
	{
		ly_reg_set("3dnc_cov_server", "%s", server);
	}

	ly_3dnc_cov_mutex = g_mutex_new();
	ly_msg_bind("cov_missing", "", ly_3dnc_cov_check, NULL);
}
void		ly_3dnc_cov_fina()
{
	ly_msg_unbind("cov_missing", "", ly_3dnc_cov_check);
}

gboolean	ly_3dnc_cov_check(gpointer message, gpointer data)
{
	if(g_mutex_trylock(ly_3dnc_cov_mutex) == FALSE)
	{
		ly_msg_put("info", "plugin:dnc", _("A download task already exists, try again later!"));
		return FALSE;
	}
	
	LyMdhMetadata *md=NULL;
	md=ly_pqm_get_current_md();
	if(!md)
	{
		g_mutex_unlock(ly_3dnc_cov_mutex);
		ly_msg_put("info", "plugin:dnc", _("No Playing Track!"));
		return FALSE;
	}
	g_strlcpy(ly_3dnc_cov_artist, md->artist, sizeof(ly_3dnc_cov_artist));
	g_strlcpy(ly_3dnc_cov_album, md->album, sizeof(ly_3dnc_cov_album));
	g_thread_create(ly_3dnc_cov_search, NULL, TRUE, NULL);
	
	return FALSE;
}
gpointer		ly_3dnc_cov_search(gpointer data)
{
	GtkTreeStore *store=NULL;
	
	char server[128]="last.fm";
	ly_reg_get("3dnc_cov_server", "%[^\n]", server);
	if(g_str_equal(server, "last.fm"))
	{
		store=ly_3dnc_cov_lastfm_search(ly_3dnc_cov_artist, ly_3dnc_cov_album);
	}
	else if(g_str_equal(server, "top100"))
	{
		store=ly_3dnc_cov_top100_search(ly_3dnc_cov_artist, ly_3dnc_cov_album);
	}
	else
	{
		g_mutex_unlock(ly_3dnc_cov_mutex);
		ly_msg_put("info", "plugin:dnc", _("Illegal server name!"));
		return NULL;
	}
	
	if(!store)
	{
		g_mutex_unlock(ly_3dnc_cov_mutex);
		ly_msg_put("info", "plugin:dnc", _("Find nothing by searching the web!"));
		return NULL;
	}
	g_idle_add(ly_3dnc_cov_notify, store);
	return NULL;
}
gboolean	ly_3dnc_cov_notify(gpointer data)
{
	GtkTreeStore *store=(GtkTreeStore *)data;
	GtkWidget *dialog;
	GtkWidget *grid;
	GtkWidget *treeview;
	GtkTreeViewColumn *column;
	GtkCellRenderer *cell_renderer;
	GtkTreeSelection *selection;
	gint rt=0;
	dialog=gtk_dialog_new_with_buttons(_("Found"),
					 GTK_WINDOW(ly_win_get_window()->win),
					 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				     GTK_STOCK_OK,
				     GTK_RESPONSE_ACCEPT,
				     GTK_STOCK_CANCEL,
				     GTK_RESPONSE_REJECT,
				     NULL);
	gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
	treeview=gtk_tree_view_new();
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), treeview, TRUE, TRUE, 0);

	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Artist"),cell_renderer,"text",0,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes(_("Album"),cell_renderer,"text",1,NULL);
	gtk_tree_view_column_set_resizable  (GTK_TREE_VIEW_COLUMN(column),TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	gtk_tree_view_set_model(GTK_TREE_VIEW (treeview), GTK_TREE_MODEL(store));
	selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	
	gtk_widget_show_all(dialog);
	rt=gtk_dialog_run(GTK_DIALOG(dialog));
	switch(rt)
	{
		case GTK_RESPONSE_ACCEPT:
			break;
		default:
			g_object_unref(store);
			gtk_widget_destroy(dialog);
			g_mutex_unlock(ly_3dnc_cov_mutex);
			return FALSE;
			break;
	}
	GtkTreeIter iter;
	gchar *url;
	gtk_tree_selection_get_selected(selection, NULL, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 2, &url, -1);
	g_thread_create(ly_3dnc_cov_analysis, url, TRUE, NULL);
	g_object_unref(store);
	gtk_widget_destroy(dialog);
	return FALSE;
}
gpointer	ly_3dnc_cov_analysis(gpointer data)
{
	gchar *url=NULL;
	
	char server[128]="last.fm";
	ly_reg_get("3dnc_cov_server", "%[^\n]", server);
	if(g_str_equal(server, "last.fm"))
	{
		url=ly_3dnc_cov_lastfm_analysis((gchar *)(data));
	}
	else if(g_str_equal(server, "top100"))
	{
		url=ly_3dnc_cov_top100_analysis((gchar *)(data));
	}
	else
	{
		g_mutex_unlock(ly_3dnc_cov_mutex);
		ly_msg_put("info", "plugin:dnc", _("Illegal server name!"));
		return NULL;
	}

	g_free(data);
	if(!url)
	{
		g_mutex_unlock(ly_3dnc_cov_mutex);
		ly_msg_put("info", "plugin:dnc", _("Cannot get the real resource adress!"));
		return NULL;
	}
	g_thread_create(ly_3dnc_cov_down, url, TRUE, NULL);
	
}
gpointer	ly_3dnc_cov_down(gpointer data)
{
	if(g_str_equal((gchar *)data, ""))
	{
		g_mutex_unlock(ly_3dnc_cov_mutex);
		ly_msg_put("info", "plugin:dnc", _("Illegal url adress!"));
		return NULL;
	}
	
	gchar path[1024]="";
	gchar *tmp;
	tmp=ly_gla_uri_get_suffix((gchar *)data);
	g_snprintf(path, sizeof(path), "%sinf/cover/%s-%s.%s",LY_GLB_USER_PIXDIR, ly_3dnc_cov_artist, ly_3dnc_cov_album, tmp);
	g_free(tmp);
	
	FILE *fp=fopen(path, "w+");
	if(!fp)
	{
		g_mutex_unlock(ly_3dnc_cov_mutex);
		ly_msg_put("info", "plugin:dnc", _("Cannot open file stream!"));
		return NULL;
	}
	CURL* pCurl = curl_easy_init();
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)fp);
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, ly_3dnc_cov_down_cb);
	curl_easy_setopt(pCurl, CURLOPT_URL, (gchar *)data);
	curl_easy_setopt(pCurl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux i686; rv:10.0.2) Gecko/20100101 Firefox/10.0.2");
	curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 30L);
	curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);
	fclose(fp);
	
	g_free(data);
	ly_msg_put("meta_update", "plugin:dnc", "cover");
	g_mutex_unlock(ly_3dnc_cov_mutex);
}
size_t ly_3dnc_cov_down_cb(char *buffer, size_t size, size_t nitems, void *outstream)
{
	int written = fwrite(buffer, size, nitems, (FILE*)outstream);
	return written;
}
