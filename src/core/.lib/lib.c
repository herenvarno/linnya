#include "lib.h"

/*
 * FUNCTIONS
 */
gboolean ly_lib_update (void)
{
	ly_db_check_library();
}

goolean ly_lib_get_metadatas(int base, int length, gpointer func, gpointer data, const char *fmt, ...)
{
	char *str=NULL;
	va_list argp;
	va_start(argp, format);
	str=g_strdup_vprintf(format, argp);
	va_end(argp);
	
	char *sql=strconcat();
	if(str!==NULL && !g_str_equal(str, ""))
	{
		g_str_concat("SELECT * FROM metadatas WHERE", str, NULL);
		g_free(str);
	}
	else
	{
		g_str_concat("SELECT * FROM metadatas",NULL);
	}
	ly_db_exec(sql, funck, data);
	g_free(sql);
	return TRUE;
}

goolean ly_lib_get_artists(int base, int length, gpointer func, gpointer data, const char *fmt, ...)
{
	char *str=NULL;
	va_list argp;
	va_start(argp, format);
	str=g_strdup_vprintf(format, argp);
	va_end(argp);
	
	char *sql=strconcat();
	if(str!==NULL && !g_str_equal(str, ""))
	{
		g_str_concat("SELECT * FROM metadatas WHERE", str, NULL);
		g_free(str);
	}
	else
	{
		g_str_concat("SELECT * FROM metadatas",NULL);
	}
	ly_db_exec(sql, funck, data);
	g_free(sql);
	return TRUE;
}
