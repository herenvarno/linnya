#include "main.h"

gboolean printhello(lyMsgMessage* m)
{
	printf("hello:%s\n",m->msg);
	return TRUE;
}
gboolean printhello1(lyMsgMessage* m)
{
	printf("hello1:%s\n",m->msg);
	return TRUE;
}
gboolean printly(lyMsgMessage* m)
{
	printf("ly:%s\n",m->msg);
	return TRUE;
}

int main(int argc, char *argv[])
{
	ly_gl_init();
	ly_msg_init();
	
	ly_msg_bind("hello",G_CALLBACK(printhello));
	ly_msg_bind("hello",G_CALLBACK(printhello1));
	ly_msg_bind("ly",G_CALLBACK(printly));
	gchar type[100];
	gchar msg[100];
	gint i=0;
	for(i=0;i<2;i++)
	{
		printf("输入任意信号和消息,用空格隔开:");
		scanf("%s %s",type,msg);
		ly_msg_put(type,"main",msg);
	}
	g_main_loop_run(ly_gl_mainloop);
	return 0;
}
