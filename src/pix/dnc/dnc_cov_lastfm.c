#include "dnc_cov_lastfm.h"


/*
 * FUNCTIONS
 */
size_t		ly_3dnc_cov_lastfm_search_cb	(char *buffer, size_t size, size_t nitems, void *outstream);

GtkTreeStore* ly_3dnc_cov_lastfm_search(gchar *artist, gchar *album)
{
	/*
	 * replace <space> to <+>
	 */
	gchar ar_str[1024]="";
	gchar al_str[1024]="";
	gchar *p_str=NULL;
	static GRegex *regex = NULL;
    regex = g_regex_new(" +", G_REGEX_OPTIMIZE, 0, NULL);
    p_str=g_regex_replace(regex, artist, strlen(artist), 0, "+", 0, NULL);
    if(p_str)
    {
    	g_strlcpy(ar_str, p_str, sizeof(ar_str));
    	g_free(p_str);
    	p_str=NULL;
    }
	p_str=g_regex_replace(regex, album, strlen(album), 0, "+", 0, NULL);
    if(p_str)
    {
    	g_strlcpy(al_str, p_str, sizeof(al_str));
    	g_free(p_str);
    	p_str=NULL;
    }
	g_regex_unref (regex);

	/*
	 * CURL get
	 */
	char tmp[1024]="http://www.last.fm/search?q=";
	char url[1024]="";
	g_snprintf(url, sizeof(url), "%s%s&type=album", tmp, al_str);
	CURL* pCurl = curl_easy_init();
	char *html=g_strconcat("", NULL);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)(&html));
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, ly_3dnc_cov_lastfm_search_cb);
	curl_easy_setopt(pCurl, CURLOPT_URL, url);
	curl_easy_setopt(pCurl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux i686; rv:10.0.2) Gecko/20100101 Firefox/10.0.2");
	curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 30L);
	curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);
	
	/*
	 * analysis the page and get artist/album/url in list
	 */
	gchar str[1024]="";
	g_strlcpy(tmp, "", sizeof(tmp));
	GMatchInfo *match_info;
	GRegex *regex1=NULL;
	
	gboolean flag_find=FALSE;
	gchar *p_ur=NULL;
	gchar ur[1024]="";
	gchar ar[128]="";
	gchar al[128]="";
	gint i=0;
	
	GtkTreeStore *store=gtk_tree_store_new (3,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);
	GtkTreeIter iter;
	
    regex = g_regex_new("<span class=\"albumCover coverMedium\"><img  width=\"64\" src=\"[^\"]+\"[^>]+><span class=\"jewelcase\"></span></span> [^<]+</a></strong>\n[ ]*<br />[^>]+>[^<]+</a>", G_REGEX_OPTIMIZE, 0, NULL);
	g_regex_match(regex, html, 0, &match_info);
	regex1 = g_regex_new("64s", G_REGEX_OPTIMIZE, 0, NULL);
	for(i=0; i<10 && g_match_info_matches (match_info); i++)
	{
		p_str=g_match_info_fetch (match_info, 0);
		sscanf(p_str, "<span class=\"albumCover coverMedium\"><img  width=\"64\" src=\"%[^\"]\"%*[^>]><span class=\"jewelcase\"></span></span> %[^<]</a></strong>\n<%*[^>]>%*[^>]>%[^<]</a>", ur, al, ar);
		g_free(p_str);
		
		/*
		 * get the real pic url from p_ur
		 */
		p_ur=g_regex_replace(regex1, ur, strlen(ur), 0, "174s", 0, NULL);
		gtk_tree_store_append (store , &iter, NULL);
		gtk_tree_store_set(store, &iter, 0, ar, 1, al, 2, p_ur, -1);
		g_match_info_next (match_info, NULL);
		g_free(p_ur);
	}
	g_regex_unref (regex1);
	g_free(html);
	g_match_info_free (match_info);
	g_regex_unref(regex);
	
	if(i>0)
		return store;
	
	g_object_unref(store);
	return NULL;
}

gchar*	ly_3dnc_cov_lastfm_analysis(gchar *url)
{
	return g_strconcat(url,NULL);
}


size_t ly_3dnc_cov_lastfm_search_cb(char *buffer, size_t size, size_t nitems, void *outstream)
{
	char **out=outstream;
	char *tmp=*out;
	*out=g_strconcat(tmp, buffer, NULL);
	g_free(tmp);
	return nitems;
}
