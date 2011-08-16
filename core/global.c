#include "global.h"

gboolean ly_global_init(int *argc, char ***argv)
{
	ly_global_debug("info",_("Initial core module: GLOBAL ...\n"));
	
	/*
	 * creating global path
	 */
	LY_GLOBAL_HOMEDIR=NULL;
	LY_GLOBAL_PROGDIR=NULL;
	LY_GLOBAL_USERDIR=NULL;
	
	const gchar *str;
	str = g_getenv("HOME");
	if(str == NULL)
		str = g_get_home_dir();
	if(str == NULL)
	{
		exit(0);
		return FALSE;
	}
	LY_GLOBAL_HOMEDIR=g_strconcat(str,"/",NULL);
//	LY_GLOBAL_PROGDIR=g_strconcat(INSTALL_PATH,"/".NULL);
	LY_GLOBAL_PROGDIR=g_strconcat("./conf/",NULL);
	LY_GLOBAL_USERDIR=g_strconcat(LY_GLOBAL_HOMEDIR,".linnya/",NULL);
	LY_GLOBAL_TEMPDIR=g_strconcat("/tmp/linnya/",NULL);
	
	mkdir(LY_GLOBAL_USERDIR, 0755);
	mkdir(LY_GLOBAL_TEMPDIR, 0755);

	gchar *path=(gchar*)g_malloc(128*sizeof(gchar));
	g_snprintf(path,127,"%sui",LY_GLOBAL_USERDIR);
	mkdir(path, 0755);
	g_snprintf(path,127,"%splugin",LY_GLOBAL_USERDIR);
	mkdir(path, 0755);
	
	/*
	 * localizition
	 */
	bindtextdomain(LY_GLOBAL_PACKAGE,LY_GLOBAL_LOCALEDIR);
	textdomain(LY_GLOBAL_PACKAGE);
	
	/*
	 * initial gstreamer
	 */
	ly_global_argc=argc;
	ly_global_argv=argv;
	gst_init (ly_global_argc, ly_global_argv);
	
	return TRUE;
}

gboolean ly_global_finalize()
{
	ly_global_debug("info",_("Finalize core module: GLOBAL ...\n"));
	return TRUE;
}

GList* ly_global_traverse_dir(gchar *path, gint depth, gboolean showhide)
{
	if(depth<=0)
		return NULL;
	
	if(path[strlen(path)-1]!='/')
		path=g_strconcat(path,"/",NULL);
	else
		path=g_strconcat(path,NULL);
	
	GDir *dir=g_dir_open(path,0,NULL);
	
	const gchar *filename=NULL;	/* do not free it */
	gchar *location=NULL;
	filename=g_dir_read_name(dir);
	GList *list=NULL;
	GList *tmplist=NULL;
	while(filename)
	{
		if(filename[0]=='.'&&(!showhide))
			continue;
		location=g_strconcat(path , filename, NULL);
		if(g_file_test(location, G_FILE_TEST_IS_DIR))
		{
			tmplist=ly_global_traverse_dir(location, depth-1, showhide);
			if(tmplist)
				list=g_list_concat(list,tmplist);
			tmplist=NULL;
		}
		list=g_list_append(list,g_strconcat("file://",location,NULL));
		g_free(location);
		filename=g_dir_read_name(dir);
	}
	return list;
}

void ly_global_replace_str(gchar *string, gint length, gchar *pSrc, gchar *pDst)
{
	gchar *pi, *po, *p;
	gint nSrcLen, nDstLen, nLen;
	
	gchar *pOutput=(gchar *)g_malloc(512*sizeof(gchar));
	
	pi = string;
	po = pOutput;
	nSrcLen = strlen(pSrc);
	nDstLen = strlen(pDst);
	
	p = strstr(pi, pSrc);
	if(p)
	{
		while(p)
		{
			nLen = (gint)(p - pi);
			memcpy(po, pi, nLen);
			memcpy(po + nLen, pDst, nDstLen);
			pi = p + nSrcLen;
			po = po + nLen + nDstLen;
			p = strstr(pi, pSrc);
		}
		strcpy(po, pi);
	}
	else
	{
		strcpy(po, pi);
	}
	g_strlcpy(string,(const gchar *)pOutput,length);
	g_free(pOutput);
}


char *ly_global_get_suffix(char *uri)
{
	char *p=NULL;
	char *rt=NULL;
	p=g_strrstr(uri,".");
	if(p==NULL)
		return NULL;
	rt=g_strdup(p);
	return rt;
}
char *ly_global_get_path(char *uri)
{
	char *p=NULL;
	p=g_strrstr(uri,"://");
	if(p==NULL)
	{
		return g_strdup(uri);
	}
	p=p+3;
	return g_strdup(p);
}
char *ly_global_get_dir(char *uri)
{
	char *p=NULL;
	char *str=NULL;
	char *rt=NULL;
	p=g_strrstr(uri,"/");	
	if(p==NULL)
		return NULL;
	str=g_strndup(uri,p-uri+1);

	rt=ly_global_get_path(str);
	g_free(str);
	return rt;
}
char *ly_global_get_filename(char *uri)
{
	char *rt=NULL;
	char *path=ly_global_get_path(uri);
	char *prefix=ly_global_get_dir(uri);
	char *suffix=ly_global_get_suffix(uri);
	
	if(path==NULL || prefix==NULL)
		return NULL;
	
	char *p=NULL;
	p=path+strlen(prefix);
	
	rt=g_strndup(p,strlen(path)-strlen(prefix)-strlen(suffix));
	g_free(path);
	g_free(prefix);
	g_free(suffix);
	return rt;
}

void ly_global_debug(gchar *type, const gchar *format, ...)
{
	if(!type)
		type="info";
	printf("[Linnya %s] ",type);
	
	va_list argp;
	va_start(argp, format);
	g_vprintf(format, argp);
	va_end(argp);
}

GList* ly_global_get_subdirs(gchar *path, gboolean showhide)
{
	
	if(path[strlen(path)-1]!='/')
		path=g_strconcat(path,"/",NULL);
	else
		path=g_strconcat(path,NULL);
	
	GDir *dir=g_dir_open(path,0,NULL);
	
	const gchar *filename=NULL;	/* do not free it */
	filename=g_dir_read_name(dir);
	gchar *location=NULL;
	GList *list=NULL;
	while(filename)
	{
		if(filename[0]=='.'&&(!showhide))
		{
			filename=g_dir_read_name(dir);
			continue;
		}
		location=g_strconcat(path,filename,"/",NULL);
		if(!g_file_test(location, G_FILE_TEST_IS_DIR))
		{
			filename=g_dir_read_name(dir);
			continue;
		}
		g_free(location);
		list=g_list_append(list,g_strconcat(filename,NULL));
		filename=g_dir_read_name(dir);
	}
	return list;
}