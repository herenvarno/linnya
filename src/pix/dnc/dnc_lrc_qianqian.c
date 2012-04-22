#include "dnc_lrc_qianqian.h"


/*
 * FUNCTIONS
 */
size_t		ly_3dnc_lrc_qianqian_search_cb	(char *buffer, size_t size, size_t nitems, void *outstream);
gchar *ly_3dnc_lrc_qianqian_to_hex_string(gchar *string);

GtkTreeStore* ly_3dnc_lrc_qianqian_search(gchar *title, gchar *artist)
{
	gchar *ti=ly_3dnc_lrc_qianqian_to_hex_string(title);
	gchar *ar=ly_3dnc_lrc_qianqian_to_hex_string(artist);
	gchar url[1024] = "";
	gchar tmp[1024] = "";
	g_snprintf(url, sizeof(url), "http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?sh?Artist=%s&Title=%s&Flags=0", ar, ti);
	g_free(ti);
	g_free(ar);
	
	
    puts(url);
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
	/*
	 * Filter
	 */
	char *s=NULL;
	char *p_s=NULL;
	s=g_utf8_strdown(string, -1);
	
	static GRegex *regex = NULL;
    regex = g_regex_new("[()\[\]{}-/:@`~—‘“”…、。（） 《》『』【】・！，：；？～￥]+", G_REGEX_OPTIMIZE, 0, NULL);
    p_s=g_regex_replace(regex, s, strlen(s), 0, "", 0, NULL);
    g_free(s);
	s=p_s;
	
	/*
	 * to Unicode
	 */
	gunichar2 *u_s=NULL;
	u_s=g_utf8_to_utf16(s, -1, NULL, NULL, NULL);
	
	return (gchar *)u_s;
}
