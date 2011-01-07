#include "main.h"

gboolean printhello(mmMsgMessage* m)
{
	printf("hello:%s\n",m->msg);
	return TRUE;
}
gboolean printhello1(mmMsgMessage* m)
{
	printf("hello1:%s\n",m->msg);
	return TRUE;
}
gboolean printmm(mmMsgMessage* m)
{
	printf("mm:%s\n",m->msg);
	return TRUE;
}

int main(int argc, char *argv[])
{
	mm_gl_init();
	mm_msg_init();
	
	mm_msg_bind("hello",G_CALLBACK(printhello));
	mm_msg_bind("hello",G_CALLBACK(printhello1));
	mm_msg_bind("mm",G_CALLBACK(printmm));
	gchar type[100];
	gchar msg[100];
	gint i=0;
	for(i=0;i<2;i++)
	{
		printf("输入任意信号和消息,用空格隔开:");
		scanf("%s %s",type,msg);
		mm_msg_put(type,"main",msg);
	}
	g_main_loop_run(mm_gl_mainloop);
	return 0;
}
