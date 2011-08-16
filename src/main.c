


#include "main.h"

int main(int argc, char *argv[])
{
	//全局初始化
	com_init(argc,argv);
	
	//载入配置
	cf_read();
	
	//载入库和子库
	lib_read_lib();
	if(gl_firstflag)
		lib_check_lib_first();
	else
		lib_check_lib();
	lib_read_sublib();
	lib_check_sublib();
	
	//载入播放列表和活跃列表
	pl_read_playlist();
	pl_read_playinglist();
	
	//创建播放核心
	co_new();
	
	//载入主题
	th_read_theme();
	
	//创建gui
	gui_window_new();
	
	//绑定事件
	con_connect_signal();

	//循环开始
	GMainLoop *loop;
	gtk_main();
	loop = g_main_loop_new(NULL,FALSE);//创建主循环，在执行 g_main_loop_run后正式开始循环
	g_main_loop_run(loop);
	
	return 0;
}

