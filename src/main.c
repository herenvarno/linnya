#include "main.h"

gboolean out(lyMsgMessage *message)
{
	puts(message->msg);
	return FALSE;
}

int main(int argc, char *argv[])
{
	ly_global_init(argc, argv);
	ly_msg_init();
	ly_conf_init();

	ly_msg_bind("core_info",G_CALLBACK(out));

	gchar str[300];
	ly_conf_set("test1","haha:%s","eeeeeeeer");
	ly_conf_set("test2","haha2:%d:%lf",1,2.5);
	ly_conf_set("test1","haha3:%lf",3.3);
	ly_conf_set("test1","haha4:%lf",3.3);

	int i=0;
	double j=0;
	if(ly_conf_get("audiao_volume","%s",str))
	{
		puts("TRUE");
		puts(str);
	}
	else
		puts("FALSE");
	
	if(ly_conf_get("test2","haha2:%d:%lf",&i,&j))
		printf("%d::%lf\n",i,j);

	ly_conf_set("audio_volume","%lf",34.646);
	ly_conf_delete("test2");
//	ly_global_run();

	ly_conf_finalize();
	ly_msg_finalize();
	ly_global_finalize();
	return 0;
}
