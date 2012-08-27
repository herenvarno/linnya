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
	gchar *p=NULL;
	gint i=0;
	gchar *tmp;
	gchar *prefix;
	p=uri;
	while(*p!=':' && *p!='\0')
	{
		p++;
		i++;
	}
	if(*p=='\0' || i==0)
	{
		return NULL;
	}
	tmp=g_strndup(uri, i);
	prefix=g_strconcat(tmp, "://", NULL);
	g_free(tmp);
	return prefix;
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
	p=uri+strlen(uri);
	while(*p!='.' && *p!='/' && p!=uri)
	{
		p--;
	}
	if(*p=='.')
	{
		return g_strdup(p+1);
	}
	return NULL;
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
	if(!uri)
		return NULL;

	gchar *filename=NULL;
	gchar *basename=g_filename_display_basename(uri);
	gint i=strlen(basename);
	gchar *p=basename+strlen(basename);
	while(*p!='.' && p!=basename)
	{
		p--;
		i--;
	}
	if(i==0)
		return basename;
	filename=g_strndup(basename, i);
	g_free(basename);
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
	if(!uri)
		return NULL;

	gchar *basename=ly_gla_uri_get_path(uri);
	gchar *dir=NULL;
	gint i=strlen(basename);
	gchar *p=basename+strlen(basename);
	while(*p!='/' && p!=basename)
	{
		p--;
		i--;
	}
	if(i==0 && *p!='/')
		dir=NULL;
	else
		dir=g_strndup(basename, i+1);
	g_free(basename);
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
