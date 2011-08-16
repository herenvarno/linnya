

//HEADER FILES
#include "debug.h"

//FUNCTIONS
//********************************************************************************
//*	
//*	FUNCTION:		de_print
//*	DISCRIPTION:	output information to terminal
//*
//********************************************************************************
void	de_print(int type, const char *func, const char *msg)
{
	if(!func)
		func="UNKNOWN";
	switch(type)
	{
		case __DE_ERROR: printf(_("[Linnya ERROR] ( in function %s ):\n\t%s\n"),func,msg);break;
		case __DE_WARNNING: printf(_("[Linnya WARNNING] ( in function %s ):\n\t%s\n"),func,msg);break;
		case __DE_INFORMATION: printf(_("[Linnya INFORMATION] ( in function %s ):\n\t%s\n"),func,msg);break;
		default: break;
	}
}

//********************************************************************************
//*
//*	FUNCTION:		de_dialog_print
//*	DISCRIPTION:	output information by message_dialog
//*
//********************************************************************************
void	de_dialog_print (GtkWidget *parent, int type, const char *func, const char *msg)
{
	GtkWidget *dialog=NULL;
	
	if(!func)
		func="UNKNOWN";
	switch(type)
	{
		case __DE_ERROR:
			dialog=gtk_message_dialog_new_with_markup(
							GTK_WINDOW(parent),
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_OK,
							_("<b>(on function %s )</b>\n%s"),func,msg);
			break;
		case __DE_WARNNING:
			dialog=gtk_message_dialog_new_with_markup(
							GTK_WINDOW(parent),
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_OK,
							_("<b>(on function %s )</b>\n%s"),func,msg);
			break;
		case __DE_INFORMATION:
			dialog=gtk_message_dialog_new_with_markup(
							GTK_WINDOW(parent),
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_INFO,
							GTK_BUTTONS_OK,
							_("<b>(on function %s )</b>\n%s"),func,msg);
			break;
		default: break;
	}
	if(!dialog)
	{
		return;
	}
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
