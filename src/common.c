
//HEADER FILES
#include "common.h"

//GLOBAL VARIABLES
gchar *musicfile[]={
			".mp3",".MP3",
			".wma",".WMA",
			".flac",".FLAC",
			".cda",".CDA",
			".mid",".MID",
			".midi",".MIDI",
			".ogg",".OGG",
			".wav",".WAV",
			".acc",".ACC",
			".ape",".APE"};

//FUNCTIONS
//********************************************************************************
//*	
//*	FUNCTION:		com_initial
//*	DISCRIPTION:	initial
//*
//********************************************************************************
gboolean	com_init (int argc, char *argv[])
{
	const gchar *home;
	gchar *path;
	
	//获取家目录
	home= g_getenv("HOME");
	if(home==NULL)
		home = g_get_home_dir();
	gl_dir_home		= g_strconcat(home, "/", NULL);
	gl_dir_prog		= g_strconcat(INSTALL_PATH,NULL);
	gl_dir_conf1	= g_strconcat(gl_dir_prog,"conf/",NULL);
	gl_dir_conf2	= g_strconcat(gl_dir_home,".linnya/",NULL);
	gl_dir_icon		= g_strconcat(gl_dir_prog,"icons/",NULL);
	
	gl_file_conf1	= g_strconcat(gl_dir_conf1,"linnya.conf",NULL);
	gl_file_conf2	= g_strconcat(gl_dir_conf2,"linnya.conf",NULL);

	gl_file_lib		= g_strconcat(gl_dir_conf2,"lib.xml",NULL);
	gl_file_sublib	= g_strconcat(gl_dir_conf2,"sublib.xml",NULL);
	gl_file_playlist	= g_strconcat(gl_dir_conf2,"playlist.xml",NULL);
	gl_file_playinglist	= g_strconcat(gl_dir_conf2,"playinglist.xml",NULL);
	gl_file_equalizer1	= g_strconcat(gl_dir_conf1,"equalizer.xml",NULL);
	gl_file_equalizer2	= g_strconcat(gl_dir_conf2,"equalizer.xml",NULL);
	gl_file_playingicon	= g_strconcat(gl_dir_icon,"playingicon.png",NULL);
	
	
	//建立用户配置目录
	mkdir(gl_dir_conf2, 0755);
	
	//国际化
	bindtextdomain(__GL_PACKAGE,__GL_LOCALEDIR);
	textdomain(__GL_PACKAGE);
	
	//初始化gstreamer和gtk
	if(!g_thread_supported())
		g_thread_init(NULL);
	gdk_threads_init();
	gtk_init (&argc, &argv);
	gst_init (&argc, &argv);
	
	path=g_strconcat(gl_dir_icon,"playing.png",NULL);
	gl_pixbuf_playing=gdk_pixbuf_new_from_file_at_size(path,20,20,NULL);
	g_free(path);

	gl_mutex_download=g_mutex_new();
	
	return TRUE;
}
//********************************************************************************
//*	
//*	FUNCTION:		com_get_filetype
//*	DISCRIPTION:	get file type
//*
//********************************************************************************
_com_filetype	com_get_filetype (gchar *filename)
{
	gint	i=0;
	gchar	*p=NULL;
	
	//get position to the last '.'
	p=filename;
	p=p+strlen(filename);
	for(i=0;i<(strlen(filename));i++)
	{
		if(*p=='.')
			break;
		else
			p--;
	}
	
	//others
	if(i>=(strlen(filename)+1))
	{	
		return com_type_others;
	}
	
	//playlist
	if((g_strcmp0(p,".cue")==0)||(g_strcmp0(p,".CUE")==0))
	{
		return com_type_cue;
	}
	if((g_strcmp0(p,".m3u")==0)||(g_strcmp0(p,".M3U")==0))
	{
		return com_type_m3u;
	}
	//lyric
	if((g_strcmp0(p,".lrc")==0)||(g_strcmp0(p,".LRC")==0))
	{
		return com_type_lrc;
	}
	//music
	for(i=0;i<20;i++)
	{
		if(g_strcmp0(p,musicfile[i])==0)
		{
			return com_type_music;
		}
	}
	//others
	return com_type_others;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		com_get_file_list
//*	DISCRIPTION:	get all files' name in paticular directory.
//*
//*****************************************************************************************************
GList*	com_get_file_list	(gchar *path, gint depth, gboolean showhide, gboolean recursive)
{
	if(depth<=0)
		return NULL;
	
	gchar *tempstr;
	
	//处理path，必须以'/'结尾
	if(path[strlen(path)-1]!='/')
		path=g_strconcat(path,"/",NULL);
	else
		path=g_strconcat(path,NULL);	
	
	GList *list=NULL;
	GList *templist=NULL;
	gchar *temp_dir=NULL;
	
	DIR *dir;
	dir=opendir(path);
	if(!dir)
	{
		de_print(__DE_ERROR,__FUNCTION__,"open dir error");
		return NULL;
	}
	struct dirent *ent=NULL;
	
	while((ent=readdir(dir))!=NULL)
	{
		if(ent->d_name[0]=='.'&&(!showhide))	//如果设置为不显示隐藏文件(夹)则排除隐藏文件(夹)
			continue;
		if(ent->d_type==8)	//file
		{
			tempstr=g_strdup(ent->d_name);
			list=g_list_append(list,tempstr);
		}
		else			//dir
		{
			if(g_strcmp0(ent->d_name,".")==0||g_strcmp0(ent->d_name,"..")==0)
				continue;
			if(recursive)
			{
				temp_dir=g_strconcat(path,ent->d_name,"/",NULL);
				templist=com_get_file_list(temp_dir,depth-1,showhide,recursive);
				g_free(temp_dir);
				if(templist)
					list=g_list_concat(list,templist);
			}
		}
	}
	closedir(dir);
	g_free(path);
	return list;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		com_get_uri_list
//*	DISCRIPTION:	get all files' name in paticular directory.
//*
//*****************************************************************************************************
GList*	com_get_uri_list	(gchar *path, gint depth, gboolean showhide, gboolean recursive)
{
	if(depth<=0)
		return NULL;

	gchar *tempstr;
	
	//处理path，必须以'/'结尾
	if(path[strlen(path)-1]!='/')
		path=g_strconcat(path,"/",NULL);
	else
		path=g_strconcat(path,NULL);
	
	GList *list=NULL;
	GList *templist=NULL;
	gchar *temp_dir=NULL;
	
	DIR *dir;
	dir=opendir(path);
	if(!dir)
	{
		de_print(__DE_ERROR,__FUNCTION__,"open dir error");
		return NULL;
	}
	struct dirent *ent=NULL;
	
	while((ent=readdir(dir))!=NULL)
	{
		if(ent->d_name[0]=='.'&&(!showhide))	//如果设置为不显示隐藏文件(夹)则排除隐藏文件(夹)
			continue;
		if(ent->d_type==8)	//file
		{
			tempstr=g_strconcat("file://",path,ent->d_name,NULL);
			list=g_list_append(list,tempstr);
		}
		else			//dir
		{
			if(g_strcmp0(ent->d_name,".")==0||g_strcmp0(ent->d_name,"..")==0)
				continue;
			if(recursive)
			{
				temp_dir=g_strconcat(path,ent->d_name,"/",NULL);
				templist=com_get_uri_list(temp_dir,depth-1,showhide,recursive);
				g_free(temp_dir);
				if(templist)
					list=g_list_concat(list,templist);
			}
		}
	}
	closedir(dir);
	g_free(path);
	return list;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		com_get_dir_list
//*	DISCRIPTION:	get all dir's name in paticular directory.
//*
//*****************************************************************************************************
GList* com_get_dir_list	(gchar *path, gboolean showhide)
{
	gchar *tempstr;
	//处理path，必须以'/'结尾
	if(path[strlen(path)-1]!='/')
		path=g_strconcat(path,"/",NULL);
	else
		path=g_strconcat(path,NULL);

	GList *list=NULL;
	
	DIR *dir=NULL;
	dir=opendir(path);
	if(!dir)
	{
		de_print(__DE_ERROR,__FUNCTION__,"open dir error");
		return NULL;
	}
	
	struct dirent *ent=NULL;
	
	
	while((ent=readdir(dir))!=NULL)
	{
		if(ent->d_type==8)	//file
		{
			continue;
		}
		else			//dir
		{
			if(g_strcmp0(ent->d_name,".")==0||g_strcmp0(ent->d_name,"..")==0)
				continue;
			if(ent->d_name[0]=='.'&&(!showhide))	//如果设置为不显示隐藏文件夹则排除隐藏文件夹
				continue;
			tempstr=g_strdup(ent->d_name);
			list=g_list_append(list,tempstr);
		}
	}
	closedir(dir);
	g_free(path);
	return list;
}

//*****************************************************************************************************
//*
//*	FUNCTION:	global_set_cursor
//*	DISCRIPTION:	set the shape of cursor
//*
//*****************************************************************************************************
gboolean com_set_cursor(GtkWidget *widget, _com_cursorshape cs)
{
	GdkCursor *cursor;
	switch(cs)
	{
		case com_cursor_default:
			cursor = gdk_cursor_new(GDK_LEFT_PTR);
			break;
		case com_cursor_drag:
			cursor= gdk_cursor_new(GDK_HAND1);
			break;
		case com_cursor_move:
			cursor= gdk_cursor_new(GDK_FLEUR);
			break;
		default:
			return FALSE;
			break;
	}
	gdk_window_set_cursor(widget->window,cursor);
	gdk_cursor_destroy(cursor); 
	return TRUE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		com_get_filename
//*	DISCRIPTION:	get filename
//*
//*****************************************************************************************************
gchar *com_get_filename(gchar *path)
{
	gchar *str=path;
	gchar *c=str+strlen(str);
	while(*c!='.')
		c--;
	*c='\0';
	while(*c!='/')
		c--;
	c++;
	return c;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		com_get_file
//*	DISCRIPTION:	get filename
//*
//*****************************************************************************************************
gchar *com_get_file(gchar *path)
{
	gchar *str=path;
	gchar *c=str+strlen(str);
	while(*c!='/')
		c--;
	c++;
	return c;
}
