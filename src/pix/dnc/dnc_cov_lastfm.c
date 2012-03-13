#include "dnc_cov_lastfm.h"


/*
 * FUNCTIONS
 */
size_t ly_3dnc_cov_lastfm_search_cb(char *buffer, size_t size, size_t nitems, void *outstream);
size_t ly_3dnc_cov_lastfm_save_cb(char *buffer, size_t size, size_t nitems, void *outstream);

gchar* ly_3dnc_cov_lastfm_search(gchar *artist, gchar *album)
{

	char tmp[1024]="http://www.last.fm/search?q=";
	char url[1024]="";
	static GRegex *regex = NULL;
    regex = g_regex_new(" +", G_REGEX_OPTIMIZE, 0, NULL);
	char *p_al=g_regex_replace(regex, album, strlen(album), 0, "+", 0, NULL);
	g_snprintf(url, sizeof(url), "%s%s&type=album", tmp, p_al);
	g_free(p_al);
	g_regex_unref (regex);
	puts(url);
	CURL* pCurl = curl_easy_init();
	char *html=g_strconcat("", NULL);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)(&html));
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, ly_3dnc_cov_lastfm_search_cb);
	curl_easy_setopt(pCurl, CURLOPT_URL, url);
	curl_easy_setopt(pCurl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux i686; rv:10.0.2) Gecko/20100101 Firefox/10.0.2");
	curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);
	
	gchar str[1024]="";
	g_strlcpy(tmp, "", sizeof(tmp));
	GMatchInfo *match_info;
	gchar *word;
	GList *list=NULL;
	gboolean flag_find=FALSE;
	gchar ar[1024]="";
	gchar al[1024]="";
    regex = g_regex_new("<strong><a href=\"/music/[^\"]+\" class=\"summary\">", G_REGEX_OPTIMIZE, 0, NULL);
	g_regex_match(regex, html, 0, &match_info);
	int i=0;
	while (g_match_info_matches (match_info))
	{
		if(i>10)
			break;
		word= g_match_info_fetch (match_info, 0);
		sscanf(word, "<strong><a href=\"/music/%[^/]/%[^\"]\" class=\"summary\">", ar, al);
		
		if(g_str_equal(ar, artist) && g_str_equal(al, album))
		{
			flag_find=TRUE;
			break;
		}
		
		g_strlcpy(tmp, str, sizeof(tmp));
		g_snprintf(str, sizeof(str), "%s%d: %s - %s\n",tmp, i, ar, al);
		g_snprintf(url, sizeof(url), "http://www.last.fm/music/%s/%s", ar, al);
		list=g_list_append(list, g_strconcat(url, NULL));
		g_match_info_next (match_info, NULL);
		g_free(word);
		i++;
	}
	g_free(html);
	g_match_info_free (match_info);
	g_regex_unref(regex);
	
	if(!flag_find&&list)
	{
		GtkWidget *dialog;
		GtkWidget *vbox;
		GtkWidget *label;
		GtkWidget *entry;
		gint rt=0;
		dialog=gtk_dialog_new_with_buttons(_("Album Find ..."),
					 GTK_WINDOW(ly_win_get_window()->win),
					 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				     GTK_STOCK_OK,
				     GTK_RESPONSE_ACCEPT,
				     GTK_STOCK_CANCEL,
				     GTK_RESPONSE_REJECT,
				     NULL);
		gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
		vbox=gtk_vbox_new(FALSE,0);
		gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),vbox,TRUE,TRUE,0);
		label=gtk_label_new(str);
		gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);
		entry=gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(vbox),entry,TRUE,TRUE,0);
		gtk_widget_show_all(dialog);
		rt=gtk_dialog_run(GTK_DIALOG(dialog));
		switch(rt)
		{
			case GTK_RESPONSE_ACCEPT:
				break;
			default:
				gtk_widget_destroy(dialog);
				return NULL;
				break;
		}
		const gchar *str_id=NULL;
		gint id=0;
		str_id=gtk_entry_get_text(GTK_ENTRY(entry));
		if(str_id&&!g_str_equal(str_id,""))
		{
			sscanf(str_id, "%d", &id);
			printf("id=%d\n", id);
			g_strlcpy(url, (gchar *)g_list_nth_data(list, id), sizeof(url));
			puts(url);
		}
		gtk_widget_destroy(dialog);
	}
	
	GList *p=list;
	while(p)
	{
		if(p->data)
		{
			g_free(p->data);
		}
		p=p->next;
	}
	g_list_free(list);
	
	puts(url);
	
	pCurl = curl_easy_init();
	html=g_strconcat("", NULL);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)(&html));
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, ly_3dnc_cov_lastfm_search_cb);
	curl_easy_setopt(pCurl, CURLOPT_URL, url);
	curl_easy_setopt(pCurl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux i686; rv:10.0.2) Gecko/20100101 Firefox/10.0.2");
	curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);
	
	g_strlcpy(tmp, "", sizeof(tmp));
    regex = g_regex_new("<img  width=\"174\" src=\"([^\"]+)\" class=\"art\"  id=\"albumCover\" itemprop=\"image\" class=\"albumCover coverMega\" />", G_REGEX_OPTIMIZE, 0, NULL);
	g_regex_match(regex, html, 0, &match_info);
	if (g_match_info_matches (match_info))
	{
		word= g_match_info_fetch (match_info, 0);
		sscanf(word, "<img  width=\"174\" src=\"%[^\"]+\".*", tmp);
		g_match_info_next (match_info, NULL);
		g_free(word);
	}
	g_match_info_free (match_info);
	g_regex_unref(regex);
	if(!g_str_equal(tmp, ""))
		return g_strconcat(tmp, NULL);
	return NULL;
}

gboolean ly_3dnc_cov_lastfm_save(gchar *url, gchar *path)
{
	FILE *fp=fopen(path, "w+");
	if(!fp)
		return FALSE;
	CURL* pCurl = curl_easy_init();
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)fp);
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, ly_3dnc_cov_lastfm_save_cb);
	curl_easy_setopt(pCurl, CURLOPT_URL, url);
	curl_easy_setopt(pCurl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux i686; rv:10.0.2) Gecko/20100101 Firefox/10.0.2");
	curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);
	fclose(fp);
	
	return TRUE;
}

gboolean	ly_3dnc_cov_lastfm_get(gchar *artist, gchar *album)
{
	gchar *url=NULL;
	url=ly_3dnc_cov_lastfm_search(artist, album);
	if(!url||(g_str_equal(url, "http://cdn.last.fm/flatness/catalogue/noimage/2/default_album_mega.png")))
	{
		return FALSE;
	}

	gchar *tmp=ly_gla_uri_get_suffix(url);
	gchar path[1024]="";
	g_snprintf(path, sizeof(path), "%sinf/cover/%s-%s.%s",LY_GLB_USER_PLUGINDIR, artist, album, tmp);
	g_free(tmp);
	if(!ly_3dnc_cov_lastfm_save(url, path))
		return FALSE;
	return TRUE;
}

size_t ly_3dnc_cov_lastfm_search_cb(char *buffer, size_t size, size_t nitems, void *outstream)
{
	char **out=outstream;
	char *tmp=*out;
	*out=g_strconcat(tmp, buffer, NULL);
	g_free(tmp);
	return nitems;
}

size_t ly_3dnc_cov_lastfm_save_cb(char *buffer, size_t size, size_t nitems, void *outstream)
{
	int written = fwrite(buffer, size, nitems, (FILE*)outstream);
	return written;
}
