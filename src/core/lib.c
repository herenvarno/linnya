/*
 * HEADERS
 */
#include "lib.h"

/*
 * FUNCTIONS
 */
 
gboolean ly_lib_check_is_exist_list_cb(gpointer stmt, gpointer data);
 
 
gboolean ly_lib_add(lyDbMetadata *md)
{
	ly_global_replace_str(md->title, sizeof(md->title), "'", "''");
	ly_global_replace_str(md->artist, sizeof(md->artist), "'", "''");
	ly_global_replace_str(md->album, sizeof(md->album), "'", "''");
	ly_global_replace_str(md->codec, sizeof(md->codec), "'", "''");
	ly_global_replace_str(md->start, sizeof(md->start), "'", "''");
	ly_global_replace_str(md->duration, sizeof(md->duration), "'", "''");
	ly_global_replace_str(md->uri, sizeof(md->uri), "'", "''");
	
	char sql[10240]="";
	snprintf(sql, sizeof(sql), "INSERT INTO metadatas (title, artist, album, codec, start, duration, uri, playing, num, flag, tmpflag) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', 0, ifnull((SELECT MAX(num) FROM metadatas),0)+1, 0, 1)",
		md->title,
		md->artist,
		md->album,
		md->codec,
		md->start,
		md->duration,
		md->uri);
	
	ly_db_exec(sql, NULL, NULL);
	return TRUE;
}

gboolean ly_lib_check_is_exist_list(GList **l)
{
	if(*l==NULL)
		return FALSE;
	
	GList *list=*l;
	GList *p=*l;
	GList *q=*l;
	while(p)
	{
		q=p->next;
		if(!ly_db_check_isaudio((char *)(p->data)))
		{
			g_free(p->data);
			p->data=NULL;
			*l=g_list_delete_link(list,p);
			list=*l;
		}
		p=q;
	}
	
	char sql[10240];
	g_snprintf(sql, sizeof(sql), "SELECT uri FROM metadatas WHERE flag=0");
	ly_db_exec(sql, ly_lib_check_is_exist_list_cb, l);
	return TRUE;
}
gboolean ly_lib_check_is_exist_list_cb(gpointer stmt, gpointer data)
{
	
	const char *uri=NULL;
	GList *list=*(GList**)data;
	GList *p=list;
	uri=sqlite3_column_text(stmt, 0);
	while(p)
	{
		if(g_str_equal(uri,p->data))
		{
			
			g_free(p->data);
			p->data=NULL;
			*(GList**)data=g_list_delete_link(list,p);
			list=*(GList**)data;
			break;
		}
		p=p->next;
	}

	return FALSE;
}

