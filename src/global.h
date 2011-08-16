


#ifndef _GLOBAL_H_
#define _GLOBAL_H_

//HEADER FILES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <dirent.h>
#include <locale.h>
#include <libintl.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <pango/pangocairo.h>
#include <iconv.h>


//MACROS

#define __GL_VERSION 1.0

#define _(String)		gettext (String)
#define __GL_LOCALEDIR	g_strconcat(LOCALEDIR,NULL)
#define __GL_PACKAGE	"linnya"


//GLOBAL STRUCT TYPES 
typedef struct _gl_metadata
{
	gchar		*title;
	gchar		*artist;
	gchar		*album;

	gchar		*codec;		//歌曲格式
	gchar		*start;		//歌曲起始时间（对CUE分割的文件有用,默认为"00:00:00"）
	gchar		*duration;	//歌曲总时长（不一定就是文件的总时长）
	gchar		*uri;		//歌曲位置
}_gl_metadata;

typedef struct _gl_playlist
{
	gchar		*name;
	GList		*list;
}_gl_playlist;

typedef struct _gl_playinglist
{
	gchar		*name;	//列表的名字
	gint		flag;	//正数代表用户自定义列表，0代表库，负数代表子库
	_gl_metadata *md;	//元数据
}_gl_playinglist;

typedef struct _gl_conf
{
	gchar		*version;
	
	gint		volume;
	gint		random;
	gint		single;
	gchar		*equalizer;
	gint		autoeq;
	gchar		*encode;
	
	gchar		*lib;
	gint		sublib;
	gint		autocheck;
	
	gchar		*theme;
	gint		mainwindow;
	gint		desktoplyrics;
	gint		desktoplyrics_fix;
	gint		desktoplyrics_x;
	gint		desktoplyrics_y;
	gchar		*lrc_location;
	gint		lrc_download_notify;
	gint		custom_lrc;
	gint		lrcgap;
	gchar		*font_highlight;
	gchar		*font_normal;
	gchar		*font_desktop;
	gchar		*color_highlight;
	gchar		*color_normal;
	gchar		*color_spectrum;
	gchar		*color_desktop_bg1;
	gchar		*color_desktop_bg2;
	gchar		*color_desktop_fg1;
	gchar		*color_desktop_fg2;
	
	gchar		*element;
}_gl_conf;

typedef struct _gl_equalizer
{
	gchar		*name;
	gdouble		band[10];
	
}_gl_equalizer;

typedef struct _gl_core
{
	GstElement	*play;
	GstElement	*equalizer;
	GstElement	*vol;
	GstElement	*audiosink;
}_gl_core;

typedef struct _gl_theme
{
	gchar *name;
	gint x;
	gint y;
	gint w;
	gint h;
	gchar *path;
	gchar *font;
	gchar *color_fg1;
	gchar *color_fg2;
	gchar *color_bg1;
	gchar *color_bg2;
}_gl_theme;

typedef struct _gl_lyrics
{
	gint	time;
	gchar	*text;
}_gl_lyrics;

//GLOBAL VARIABLES

//路径变量
const gchar		*gl_dir_prog;			//程序目录
const gchar		*gl_dir_home;			//用户目录
const gchar		*gl_dir_conf1;			//全局配置目录
const gchar		*gl_dir_conf2;			//用户配置目录
const gchar		*gl_dir_icon;			//程序图标目录
const gchar		*gl_file_conf1;			//全局配置
const gchar		*gl_file_conf2;			//用户配置
const gchar		*gl_file_lib;			//库文件
const gchar		*gl_file_sublib;		//子库文件
const gchar		*gl_file_playlist;		//用户播放列表
const gchar		*gl_file_playinglist;	//上次关闭时的正在播放列表
const gchar		*gl_file_equalizer1;	//全局均衡器配置文件
const gchar		*gl_file_equalizer2;	//用户均衡器配置文件
const gchar		*gl_file_playingicon;	//正在播放标志图片

//全局变量
_gl_conf		*gl_conf;		//全局配置
_gl_core		*gl_core;		//播放核心

GList	*gl_lib;				//库根指针
GList	*gl_sublib;				//子库根指针
GList	*gl_playlist;			//播放列表根指针
GList	*gl_playinglist;		//正在播放列表根指针
GList	*gl_playing;			//正在播放的音乐,属于gl_playinglist
GList	*gl_played;				//上次播放的音乐,属于gl_playlist/gl_lib/gl_sublib
GList	*gl_selecting;			//选中的列表,属于gl_playlist/gl_lib/gl_sublib

GList	*gl_theme;				//主题项根指针
GList	*gl_equalizer;			//均衡器列表根指针
GList	*gl_eq;					//均衡器游标
GList	*gl_lyrics;				//歌词列表根指针
GList	*gl_lrc;				//歌词游标

GdkPixbuf *gl_pixbuf_cd;		//默认cd封面图片
GdkPixbuf *gl_pixbuf_playing;	//播放标志图片
gboolean gl_firstflag;			//第一次播放标志

GThread	*gl_thread_download;	//下载歌词线程
GMutex	*gl_mutex_download;		//下载歌词线程锁

//FUNCTION
gboolean gl_free_list_string(GList *list);
gboolean gl_free_list_playlist(GList *list);
gboolean gl_free_list_playinglist(GList *list);
gboolean gl_free_list_metadata(GList *list);
gboolean gl_free_list_equalizer(GList *list);
gboolean gl_free_list_lyrics(GList *list);
gboolean gl_free_playlist(_gl_playlist* pl);
gboolean gl_free_playinglist(_gl_playinglist* pgl);
gboolean gl_free_metadata(_gl_metadata* md);
gboolean gl_free_equalizer(_gl_equalizer* eq);
gboolean gl_free_lyrics(_gl_lyrics *lrc);
#endif
