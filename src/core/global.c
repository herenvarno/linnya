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
	LY_GLOBAL_PROGDIR=g_strconcat(INSTALL_PATH,NULL);
	LY_GLOBAL_USERDIR=g_strconcat(LY_GLOBAL_HOMEDIR,".linnya/",NULL);
	LY_GLOBAL_TEMPDIR=g_strconcat("/tmp/linnya/",NULL);
	
	mkdir(LY_GLOBAL_USERDIR, 0755);
	mkdir(LY_GLOBAL_TEMPDIR, 0755);
	
	LY_GLOBAL_UIDIR=g_strconcat(LY_GLOBAL_USERDIR,"ui/",NULL);
	LY_GLOBAL_PLUGINDIR=g_strconcat(LY_GLOBAL_USERDIR,"plugin/",NULL);

	mkdir(LY_GLOBAL_UIDIR, 0755);
	mkdir(LY_GLOBAL_PLUGINDIR, 0755);
	
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
	
	LY_GLOBAL_FIRST_FLAG=TRUE;
	
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
	char *str=g_path_get_basename(uri);
	char tmpstr[1024]="";
	g_strlcpy(tmpstr, str, sizeof(tmpstr));
	g_free(str);
	char *p=NULL;
	p=g_strrstr(tmpstr,".");
	if(p==NULL)
		return g_strdup(tmpstr);
	*p='\0';
	return g_strdup(tmpstr);
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
