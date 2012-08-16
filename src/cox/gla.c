/*
 * gla.c
 * This file is part of linnya
 *
 * Copyright (C) 2010-2012 - Edward
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * linnya is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * HEADERS
 */
#include "gla.h"


/*
 * FUNCTIONS
 */
// INIT & FINAL
void ly_gla_init(int *argc, char ***argv)
{
	// DIRECTORIY
	LY_GLA_HOMEDIR=NULL;
	LY_GLA_PROGDIR=NULL;
	LY_GLA_USERDIR=NULL;

	const gchar *str;
	str = g_getenv("HOME");
	if(str == NULL)
		str = g_get_home_dir();
	if(str == NULL)
	{
		exit(0);
		return;
	}
	LY_GLA_HOMEDIR=g_strconcat(str,"/",NULL);
	LY_GLA_PROGDIR=g_strconcat(INSTALL_PATH, NULL);
	LY_GLA_USERDIR=g_strconcat(LY_GLA_HOMEDIR,".linnya/",NULL);
	LY_GLA_TEMPDIR=g_strconcat("/tmp/linnya/",NULL);

	g_mkdir_with_parents(LY_GLA_USERDIR, 0755);
	g_mkdir_with_parents(LY_GLA_TEMPDIR, 0755);

	// GETTEXT
	#ifndef WIN32_BUILD
	bindtextdomain(LY_GLA_PACKAGE,LY_GLA_LOCALEDIR);
	textdomain(LY_GLA_PACKAGE);
	#endif

	// INIT
	ly_gla_argc=argc;
	ly_gla_argv=argv;
	gst_init (ly_gla_argc, ly_gla_argv);

	// SET FLAG
	LY_GLA_FIRST_FLAG=TRUE;
}

void ly_gla_fina()
{

}

// API

/**
 * ly_gla_uri_get_prefix:
 * @uri: the uri
 *
 * Get the protocol prefix of a uri string.
 *
 * Returns:		the prefix
 */
char*		ly_gla_uri_get_prefix		(char *uri)
{
	char str[1024]="";
	sscanf(uri, "%1023[^:\n]://%*s", str);
	if(g_str_equal(str, ""))
		return 	NULL;
	char *s=g_strconcat(str, "://", NULL);
	return s;
}
/**
 * ly_gla_uri_get_suffix:
 * @uri: the uri
 *
 * Get the type suffix of a uri string.
 *
 * Returns: the suffix
 */
char*		ly_gla_uri_get_suffix		(char *uri)
{
	char *p=NULL;
	char *rt=NULL;
	p=g_strrstr(uri,".");
	if(p==NULL)
		return NULL;
	rt=g_strdup(p+1);
	return rt;
}
/**
 * ly_gla_uri_get_filename:
 * @uri: the uri
 *
 * Get the filename of a uri string.
 *
 * Returns: the filename
 */
char*		ly_gla_uri_get_filename		(char *uri)
{
	gchar *filename=NULL;
	gint i=0;
	gchar *p=uri+strlen(uri);
	while(*p!='/')
	{
		p--;
		i++;
	}
	p++;
	gchar *q=uri+strlen(uri);
	while(*q!='.' && *q!='/')
	{
		q--;
		i--;
	}
	i--;
	filename=g_strndup(p, i);
	return filename;
}
/**
 * ly_gla_uri_get_dir:
 * @uri: the uri
 *
 * Get the directory of a uri string.
 *
 * Returns: the dir
 */
char*		ly_gla_uri_get_dir			(char *uri)
{

	GRegex *regex=NULL;
	GMatchInfo *info=NULL;
	gchar *dir=NULL;

	regex=g_regex_new("://(.+/)[^/]+$", G_REGEX_MULTILINE, 0, NULL);
	g_regex_match(regex, uri, 0, &info);
	if(g_match_info_matches(info))
	{
		dir = g_match_info_fetch(info, 1);
	}
	g_regex_unref(regex);
	g_match_info_unref(info);
	return dir;
}
/**
 * ly_gla_uri_get_path:
 * @uri: the uri
 *
 * Get the full path of a uri string.
 *
 * Returns: the path
 */
char*		ly_gla_uri_get_path		(char *uri)
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

/**
 * ly_gla_traverse_dir:
 * @path: the dir path
 * @pattern: the filter pattern string for GRegex.
 * @depth: the traverse depth, to prevent endless cirtulate.
 * @showhide: set true to ignor files which is hidden.
 *
 * Get the list of file within the given directory.
 *
 * Returns: the newly allocate list, free it as well as its data after using.
 */
GList* ly_gla_traverse_dir(const gchar *path, const gchar *pattern, gint depth, gboolean showhide)
{
	if(depth<=0)
		return NULL;

	if(path[strlen(path)-1]!='/')
		path=g_strconcat(path,"/",NULL);
	else
		path=g_strconcat(path,NULL);

	if(!g_file_test(path, G_FILE_TEST_EXISTS))
		return NULL;

	GDir *dir=g_dir_open(path,0,NULL);
	const gchar *filename=NULL;
	gchar *location=NULL;
	filename=g_dir_read_name(dir);
	GList *list=NULL;
	GList *tmplist=NULL;
	GRegex *regex=NULL;
	if(pattern)
	{
		regex=g_regex_new(pattern, G_REGEX_OPTIMIZE, 0, NULL);
	}
	while(filename)
	{
		if(filename[0]=='.'&&(!showhide))
		{
			filename=g_dir_read_name(dir);
			continue;
		}
		location=g_strconcat(path , filename, NULL);
		if(g_file_test(location, G_FILE_TEST_IS_DIR))
		{
			tmplist=ly_gla_traverse_dir(location, pattern, depth-1, showhide);
			if(tmplist)
				list=g_list_concat(list,tmplist);
			tmplist=NULL;
		}
		if(!regex || g_regex_match(regex, location, 0, NULL))
			list=g_list_append(list,g_strconcat("file://",location,NULL));
		g_free(location);
		filename=g_dir_read_name(dir);
	}
	if(regex)
	{
		g_regex_unref(regex);
	}
	return list;
}

/**
 * ly_gla_get_subdir:
 * @path: the dir path
 * @showhide: set true to ignor files which is hidden.
 *
 * Get the list of subdirecoty within the given directory.
 *
 * Returns: the newly allocate list, free it as well as its data after using.
 */
GList* ly_gla_get_subdirs(const char *path, gboolean showhide)
{

	if(path[strlen(path)-1]!='/')
		path=g_strconcat(path,"/",NULL);
	else
		path=g_strconcat(path,NULL);

	GDir *dir=g_dir_open(path,0,NULL);

	const gchar *filename=NULL;
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
