#include "dnc_lrc_qianqian.h"


/*
 * FUNCTIONS
 */
size_t		ly_3dnc_lrc_qianqian_search_cb	(char *buffer, size_t size, size_t nitems, void *outstream);
gchar*		ly_3dnc_lrc_qianqian_to_hex_string(gchar *string);
static gint32 ly_3dnc_lrc_qianqian_get_url_code(gint id, const char *data);

GtkTreeStore* ly_3dnc_lrc_qianqian_search(gchar *title, gchar *artist)
{
	gchar *ti_str=ly_3dnc_lrc_qianqian_to_hex_string(title);
	gchar *ar_str=ly_3dnc_lrc_qianqian_to_hex_string(artist);
	gchar url[1024] = "";
	gchar tmp[1024] = "";
	g_snprintf(url, sizeof(url), "http://ttlrcct.qianqian.com/dll/lyricsvr.dll?sh?Artist=%s&Title=%s&Flags=0", ar_str, ti_str);
	g_free(ti_str);
	g_free(ar_str);
	
	/*
	 * CURL get
	 */
	CURL* pCurl = curl_easy_init();
	char *html=g_strconcat("", NULL);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)(&html));
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, ly_3dnc_lrc_qianqian_search_cb);
	curl_easy_setopt(pCurl, CURLOPT_URL, url);
	curl_easy_setopt(pCurl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux i686; rv:10.0.2) Gecko/20100101 Firefox/10.0.2");
	curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 30L);
	curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);
	
	/*
	 * analysis the page and get artist/album/url in list
	 */
	gchar *p_str=NULL;
	static GRegex *regex = NULL;
	g_strlcpy(tmp, "", sizeof(tmp));
	GMatchInfo *match_info;
	
	gint id=0;
	gchar ti[128]="";
	gchar ar[128]="";
	gint i=0;
	
	GtkTreeStore *store=gtk_tree_store_new (3,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);
	GtkTreeIter iter;
	
    regex = g_regex_new("<lrc id=\"[0-9]*\" artist=\"[^\"\n]+\" title=\"[^\"\n]+\"></lrc>", G_REGEX_OPTIMIZE, 0, NULL);
	g_regex_match(regex, html, 0, &match_info);
	for(i=0; i<10 && g_match_info_matches (match_info); i++)
	{
		p_str=g_match_info_fetch (match_info, 0);
		sscanf(p_str, "<lrc id=\"%d\" artist=\"%[^\"\n]\" title=\"%[^\"\n]\"></lrc>", &id, ar, ti);
		g_free(p_str);
		
		/*
		 * get the real pic url from ur
		 */
		g_snprintf(tmp, sizeof(tmp), "%s%s", ar, ti);
		g_snprintf(url, sizeof(url), "http://ttlrcct.qianqian.com/dll/"
    "lyricsvr.dll?dl?Id=%d&Code=%d", id, ly_3dnc_lrc_qianqian_get_url_code(id, tmp));
		gtk_tree_store_append (store , &iter, NULL);
		gtk_tree_store_set(store, &iter, 0, ti, 1, ar, 2, url, -1);
		g_match_info_next (match_info, NULL);
	}
	g_free(html);
	g_match_info_free (match_info);
	g_regex_unref(regex);

	if(i>0)
		return store;
	
	g_object_unref(store);
	return NULL;
}

gchar*	ly_3dnc_lrc_qianqian_analysis(gchar *url)
{
	return g_strconcat(url,NULL);
}


size_t ly_3dnc_lrc_qianqian_search_cb(char *buffer, size_t size, size_t nitems, void *outstream)
{
	char **out=outstream;
	char *tmp=*out;
	*out=g_strconcat(tmp, buffer, NULL);
	g_free(tmp);
	return nitems;
}

gchar *ly_3dnc_lrc_qianqian_to_hex_string(gchar *string)
{
    gchar *utf16_str;
    gchar *result;
    gsize utf16_len;
    gsize i;
    gchar *tmp;
    guint x, y;
    guint tmp_len;
    if(string==NULL)
    	return NULL;
    tmp = g_utf8_strdown(string, -1);
    if(tmp==NULL)
    	return NULL;
    tmp_len = strlen(tmp);
    for(x=0, y=0;x<tmp_len;x++)
    {
        if(!isblank(tmp[x]))
        {
            tmp[y] = tmp[x];
            y++;
        }
        if(tmp[x]=='\0')
        	break;
    }
    tmp[y] = '\0';
    utf16_str = g_convert(tmp, -1, "UTF16LE", "UTF-8", NULL, &utf16_len, NULL);
    g_free(tmp);
    
    if(utf16_str==NULL)
    	return NULL;
    if(utf16_len<=0)
    {
        g_free(utf16_str);
        return NULL;
    }
    result = g_new0(gchar, 2*utf16_len+1);
    for(i=0;i<utf16_len;i++)
    {
        if((guchar)utf16_str[i]!=L' ')
        g_snprintf(result+2*i, 3, "%02X", (guchar)utf16_str[i]);
    }
    g_free(utf16_str);
    return result;
}

static gint32 ly_3dnc_lrc_qianqian_get_url_code(gint id, const char *data)
{
    gint32 encoded_id;
    gint32 encoded_data = 0;
    gint32 encoded_data2 = 0;
    gint32 ret;
    gsize len;
    gint i;
    guint32 id1 = id & 0xff;
    guint32 id2 = (id >> 8) & 0xff;
    guint32 id3 = (id >> 16) & 0xff;
    guint32 id4 = (id >> 24) & 0xff;
    if(id3 == 0) id3 = ~id2 & 0xff;
    if(id4 == 0) id4 = ~id1 & 0xff;
    encoded_id = (id1 << 24) | (id3 << 16) | (id2 << 8) | id4;
    len = strlen(data);
    for(i=len-1;i>=0;i--)
        encoded_data = encoded_data + data[i] + (encoded_data<<(i%2+4));
    for(i=0;i<len;i++)
        encoded_data2 = encoded_data2 + data[i] + (encoded_data2<<(i%2+3));
    ret = ((encoded_data ^ encoded_id) + (encoded_data2 | id)) *
        (encoded_data2 | encoded_id);
    ret = ret * (encoded_data ^ id);
    return ret;
}
