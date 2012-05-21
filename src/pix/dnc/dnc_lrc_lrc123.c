#include "dnc_lrc_lrc123.h"


/*
 * FUNCTIONS
 */
size_t		ly_3dnc_lrc_lrc123_search_cb	(char *buffer, size_t size, size_t nitems, void *outstream);

GtkTreeStore* ly_3dnc_lrc_lrc123_search(gchar *title, gchar *artist)
{
    gchar *xml_data=g_strconcat("", NULL);
    gchar *search_url;
    gchar *esc_title = NULL, *esc_artist = NULL, *url = NULL;
	gchar *search_base_url = "http://www.lrc123.com/?keyword=";
	
    GRegex *down_regex, *title_regex, *artist_regex;
    GMatchInfo *down_match_info, *title_match_info, *artist_match_info;
    if(title==NULL && artist==NULL) return NULL;
    if(title!=NULL)
        esc_title = g_uri_escape_string(title, NULL, FALSE);
    if(artist!=NULL)
        esc_artist = g_uri_escape_string(artist, NULL, FALSE);
    if(esc_title!=NULL && esc_artist!=NULL)
    {
        search_url = g_strconcat(search_base_url, esc_title, "%20",
            esc_artist, "&field=all", NULL);
    }
    else if(esc_title!=NULL)
        search_url = g_strconcat(search_base_url, esc_title,
            "&field=song", NULL);
    else
        search_url = g_strconcat(search_base_url, esc_artist,
            "&field=singer", NULL);
    if(esc_title!=NULL) g_free(esc_title);
    if(esc_artist!=NULL) g_free(esc_artist);
    
	CURL* pCurl = curl_easy_init();
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)(&xml_data));
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, ly_3dnc_lrc_lrc123_search_cb);
	curl_easy_setopt(pCurl, CURLOPT_URL, search_url);
	curl_easy_setopt(pCurl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux i686; rv:10.0.2) Gecko/20100101 Firefox/10.0.2");
	curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 30L);
	curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);

    down_regex = g_regex_new("(<a href=\"http://www.lrc123.com/download/"
        "lrc/)[^\"]+", G_REGEX_MULTILINE, 0, NULL);
    title_regex = g_regex_new("(field=song&keyword=)[^\"]+",
        G_REGEX_MULTILINE, 0, NULL);
    artist_regex = g_regex_new("(field=singer&keyword=)[^\"]+",
        G_REGEX_MULTILINE, 0, NULL);
    g_regex_match(down_regex, xml_data, 0, &down_match_info);
    g_regex_match(title_regex, xml_data, 0, &title_match_info);
    g_regex_match(artist_regex, xml_data, 0, &artist_match_info);
    int i=0;
   GtkTreeStore *store=gtk_tree_store_new (3,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);
   GtkTreeIter iter;

    for(i=0;i<10 && g_match_info_matches(down_match_info);i++)
    {
        esc_title = NULL;
        esc_artist = NULL;
        url = g_match_info_fetch(down_match_info, 0);
        g_match_info_next(down_match_info, NULL);
        if(g_match_info_matches(title_match_info))
        {
            esc_title = g_match_info_fetch(title_match_info, 0);
            g_match_info_next(title_match_info, NULL);
        }
        else
            esc_title = NULL;
        if(g_match_info_matches(artist_match_info))
        {
            esc_artist = g_match_info_fetch(artist_match_info, 0);
            g_match_info_next(artist_match_info, NULL);
        }
        else
            esc_artist = NULL;
        if(url==NULL)
        {
            if(esc_title!=NULL) g_free(esc_title);
            if(esc_artist!=NULL) g_free(esc_artist);
            continue;
        }
		
		gchar *t=NULL;
		gchar *a=NULL;
        if(esc_title!=NULL)
        {
            t = g_uri_unescape_string(esc_title+19, NULL);
            t = g_strdelimit(t, "+", ' ');
            g_free(esc_title);
        }
        if(esc_artist!=NULL)
        {
            a = g_uri_unescape_string(esc_artist+21, NULL);
            a = g_strdelimit(a, "+", ' ');
            g_free(esc_artist);
        }

        gtk_tree_store_append (store , &iter, NULL);
		gtk_tree_store_set(store, &iter, 0, t, 1, a, 2, url+9, -1);
        g_free(url);
	}

	if(i>0)
		return store;
	
	g_object_unref(store);
	return NULL;
}

gchar*	ly_3dnc_lrc_lrc123_analysis(gchar *url)
{
	return g_strconcat(url,NULL);
}


size_t ly_3dnc_lrc_lrc123_search_cb(char *buffer, size_t size, size_t nitems, void *outstream)
{
	char **out=outstream;
	char *tmp=*out;
	*out=g_strconcat(tmp, buffer, NULL);
	g_free(tmp);
	return nitems;
}
