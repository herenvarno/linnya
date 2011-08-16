#ifndef CORE_H
#define CORE_H

//HEADERS
#include "global.h"
#include "debug.h"
#include "common.h"
#include "configure.h"
#include "metadata.h"
#include "library.h"
#include "playlist.h"
#include "equalizer.h"

//VARIABLES
gdouble co_mag[40];
gdouble co_phase[40];

//FUNCTIONS
gboolean	co_new();		//新建core
gboolean	co_play();		//播放
gboolean	co_pause();		//暂停
gboolean	co_stop();		//停止
gboolean	co_set_next();	//设置下一首要播放的歌曲，改变gl_playing游标指向他
gboolean	co_set_prev();	//设置上一首要播放的歌曲，改变gl_playing游标指向他
gboolean	co_quit();		//软件退出时core模块的清理工作

gdouble		co_get_position();
gboolean	co_set_position(gdouble percentage);		//设定播放位置
gdouble		co_get_volume();
gboolean	co_set_volume(gdouble percentage);			//设定音量
GstState	co_get_state();

extern	gboolean gui_window_update_display();
extern gboolean gui_lrc_read_lyrics();
extern	GtkWidget *gui_window_image_cd;
#endif






