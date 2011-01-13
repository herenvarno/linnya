

//HEADERS
#include "library.h"

//VARIABLES
GtkWidget *assistant=NULL;
GtkWidget *bar=NULL;
gboolean percentage=0.0;

//FUNCTIONS
//*****************************************************************************************************
//*
//*	FUNCTION:		lib_read_lib_start
//*	DISCRIPTION:	parse the start of xml makeup, serve for func. lib_load
//*
//*****************************************************************************************************
static void lib_read_lib_start(GMarkupParseContext *context,const gchar *element_name,const gchar **attribute_names,const gchar **attribute_values,gpointer data,GError **error)
{
	_gl_metadata *md=(_gl_metadata *)g_malloc(sizeof(_gl_metadata));
	md->title = NULL;
	md->artist = NULL;
	md->codec=NULL;
	md->start=NULL;
	md->duration=NULL;
	md->uri=NULL;
	GList *list;

	const gchar **name_cursor = attribute_names;
	const gchar **value_cursor = attribute_values;
	
	while (*name_cursor)
	{
	if (strcmp (*name_cursor, "title") == 0)
		md->title = g_strdup (*value_cursor);
	if (strcmp (*name_cursor, "artist") == 0)
		md->artist = g_strdup (*value_cursor);
	if (strcmp (*name_cursor, "album") == 0)
		md->album = g_strdup (*value_cursor);
	if (strcmp (*name_cursor, "codec") == 0)
		md->codec = g_strdup (*value_cursor);
	if (strcmp (*name_cursor, "start") == 0)
		md->start = g_strdup (*value_cursor);
	if (strcmp (*name_cursor, "duration") == 0)
		md->duration = g_strdup (*value_cursor);
	if (strcmp (*name_cursor, "uri") == 0)
	{
		list=((_gl_playlist *)(gl_lib->data))->list;
		md->uri = g_strdup (*value_cursor);
		list=g_list_append (list,md);
		((_gl_playlist *)(gl_lib->data))->list=list;
	}
	
	name_cursor++;
	value_cursor++;
	} 
}
//********************************************************************************
//*
//*	FUNCTION:		lib_read_lib
//*	DISCRIPTION:	read lib from database
//*
//********************************************************************************
gboolean lib_read_lib()
{
	gl_lib=NULL;
	
	_gl_playlist *pl=(_gl_playlist*)g_malloc(sizeof(_gl_playlist));
	pl->name=_("All Tracks");
	pl->list=NULL;
	gl_lib=g_list_append(gl_lib,pl);
	
	GMarkupParser parser = {
		.start_element = lib_read_lib_start,
		.end_element = NULL,
		.text = NULL,
		.passthrough = NULL,
		.error = NULL
		};
		
	
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	
	g_file_get_contents(gl_file_lib, &buf, &length,NULL);
	context = g_markup_parse_context_new(&parser, 0,NULL, NULL);
	
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		de_print(__DE_WARNNING,__FUNCTION__,_("Couldn't load xml,create a new one"));
		return FALSE;
	}
	g_markup_parse_context_free(context);
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_check_lib
//*	DISCRIPTION:	update lib
//*
//********************************************************************************
gboolean	lib_check_lib ()
{
	_gl_metadata *md;
	
	gchar *location=NULL;
	
	GList *p=NULL;
	_gl_metadata *plink;
	GList *q=NULL;
	gchar *qlink;
	GList *pbk=NULL;
	GList *qbk=NULL;
	
	GList *list1;
	GList *list2;
	
	//获取两个list
	list1=((_gl_playlist*)(gl_lib->data))->list;
	list2=com_get_uri_list(gl_conf->lib, 5, FALSE, TRUE);
	
	q=list2;
	while(q)
	{
		qbk=q->next;
		if(com_get_filetype(q->data)!=com_type_music)
			list2=g_list_delete_link(list2,q);
		q=qbk;
	}
	
	p=list1;
	while(p)
	{
		pbk=p->next;
		plink=p->data;
		q=list2;
		while(q)
		{
			qlink=q->data;
			if((g_strcmp0(plink->start,"00:00:000")==0)&&(g_strcmp0(plink->uri,qlink)==0))
			{
				list2=g_list_delete_link(list2,q);
				break;
			}
			q=q->next;
		}
		if(q==NULL)
		{
			//确认是无效数据
			plink=p->data;
			location=g_filename_from_uri(plink->uri,NULL,NULL);
			if(fopen(location,"r")==NULL)
			{
				//已确认是无效数据，删除
				((_gl_playlist*)(gl_lib->data))->list=g_list_delete_link(list1,p);
			}
			g_free(location);
		}
		p=pbk;
	}
	
	//插入新数据(list2)
	q=list2;
	while(q)
	{
		qlink=q->data;
		md=md_get_metadata(qlink);
		if(md)
		  ((_gl_playlist*)(gl_lib->data))->list = g_list_append(((_gl_playlist*)(gl_lib->data))->list,md);
		q=q->next;
	}
	
	//释放列表内存
	gl_free_list_string(list2);
	
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_check_lib_first
//*	DISCRIPTION:	update lib at the first time.
//*
//********************************************************************************
gboolean lib_callback_timer(gpointer data)
{
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bar),0.5);
	return TRUE;
}
gboolean lib_callback_change_entry(GtkWidget *widget,gpointer data)
{
	if(gl_conf->lib)
		g_free(gl_conf->lib);
	gl_conf->lib=g_strconcat(gtk_entry_get_text(GTK_ENTRY(widget)),NULL);
	return FALSE;
}
gboolean lib_callback_change_button(GtkWidget *widget,gpointer data)
{
	GtkWidget *dialog=gtk_file_chooser_dialog_new(	_("Select library"),
													GTK_WINDOW(assistant),
													GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
													GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
													GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
													NULL);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		gchar *path=gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
		gchar *str=g_strconcat(path,"/",NULL);
		g_free(path);
		gtk_entry_set_text(GTK_ENTRY(data),str);
		if(gl_conf->lib)
			g_free(gl_conf->lib);
		gl_conf->lib=str;
	}
	gtk_widget_destroy(dialog);
	return FALSE;
}
gboolean lib_callback_load(GtkWidget *widget,gpointer data)
{
	_gl_metadata *md;
	
	gchar *location=NULL;
	
	GList *p=NULL;
	_gl_metadata *plink;
	GList *q=NULL;
	gchar *qlink;
	GList *pbk=NULL;
	GList *qbk=NULL;
	
	GList *list1;
	GList *list2;
	
	//获取两个list
	list1=((_gl_playlist*)(gl_lib->data))->list;
	list2=com_get_uri_list(gl_conf->lib, 5, FALSE, TRUE);
	
	q=list2;
	while(q)
	{
		qbk=q->next;
		if(com_get_filetype(q->data)!=com_type_music)
			list2=g_list_delete_link(list2,q);
		q=qbk;
	}
	
	p=list1;
	while(p)
	{
		pbk=p->next;
		plink=p->data;
		q=list2;
		while(q)
		{
			qlink=q->data;
			if((g_strcmp0(plink->start,"00:00:000")==0)&&(g_strcmp0(plink->uri,qlink)==0))
			{
				list2=g_list_delete_link(list2,q);
				break;
			}
				q=q->next;
		}
		if(q==NULL)
		{
			//确认是无效数据
			plink=p->data;
			location=g_filename_from_uri(plink->uri,NULL,NULL);
			if(fopen(location,"r")==NULL)
			{
				//已确认是无效数据，删除
				((_gl_playlist*)(gl_lib->data))->list =g_list_delete_link(list1,p);
			}
			g_free(location);
		}
		p=pbk;
	}
	
	//插入新数据(list2)
	gdouble step=1.0/g_list_length(list2);
	q=list2;
	while(q)
	{
		qlink=q->data;
		md=md_get_metadata(qlink);
		if(md)
		  ((_gl_playlist*)(gl_lib->data))->list = g_list_append(((_gl_playlist*)(gl_lib->data))->list,md);
		percentage+=step;
		if(percentage>1)percentage=1;
		gtk_progress_bar_update(GTK_PROGRESS_BAR(bar),percentage);
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(bar),qlink);
		q=q->next;
	}
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bar),1.0);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(bar),_("Complete"));
	GtkWidget *page=gtk_assistant_get_nth_page(GTK_ASSISTANT(assistant),1);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant),page,TRUE);
	
	//释放列表内存
	gl_free_list_string(list2);
	return FALSE;
}
gboolean	lib_check_lib_first ()
{
	percentage=0;
	
	GtkWidget *page;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	GtkWidget *button;
	
	GdkPixbuf *pixbuf;
	gchar *path;
		
	assistant=gtk_assistant_new();
	gtk_window_set_default_size(GTK_WINDOW(assistant),500,400);
	gtk_window_set_position(GTK_WINDOW(assistant),GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(assistant),"delete-event",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(assistant),"delete-event",G_CALLBACK(gtk_widget_destroy),assistant);
	g_signal_connect(G_OBJECT(assistant),"cancel",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(assistant),"cancel",G_CALLBACK(gtk_widget_destroy),assistant);
	g_signal_connect(G_OBJECT(assistant),"close",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(assistant),"close",G_CALLBACK(gtk_widget_destroy),assistant);

	page=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(page), _("<b>It seems that this is your first time using Linnya</b>\n<b>Let's start your music discovery tour NOW !</b>"));
	gtk_assistant_append_page(GTK_ASSISTANT(assistant),page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant),page,_("WELCOME"));
	path=g_strconcat(gl_dir_icon,"wizard.png",NULL);
	pixbuf=gdk_pixbuf_new_from_file_at_size(path,64,64,NULL);
	g_free(path);
	gtk_assistant_set_page_header_image(GTK_ASSISTANT(assistant),page,pixbuf);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant),page,TRUE);
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant),page,GTK_ASSISTANT_PAGE_INTRO);
	page=gtk_vbox_new(FALSE,0);
	gtk_assistant_append_page(GTK_ASSISTANT(assistant),page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant),page,_("SETTING (library)"));
	path=g_strconcat(gl_dir_icon,"lib.png",NULL);
	gtk_container_set_border_width(GTK_CONTAINER(page),10);
	pixbuf=gdk_pixbuf_new_from_file_at_size(path,64,64,NULL);
	g_free(path);
	gtk_assistant_set_page_header_image(GTK_ASSISTANT(assistant),page,pixbuf);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant),page,FALSE);
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>First,</b> Choose the directory in which you store your music\n<b>Then,</b> press (Load) button to load all of them"));
	gtk_box_pack_start(GTK_BOX(page),label,TRUE,FALSE,0);
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(page),hbox,TRUE,FALSE,0);
	label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label),_("<b>Library Location:</b>"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entry=gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry),gl_conf->lib);
	g_signal_connect(G_OBJECT(entry),"changed",G_CALLBACK(lib_callback_change_entry),NULL);
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,0);
	button=gtk_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(lib_callback_change_button),entry);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	button=gtk_button_new_with_label(_("Load"));
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(lib_callback_load),NULL);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	bar=gtk_progress_bar_new();
	gtk_progress_bar_set_ellipsize(GTK_PROGRESS_BAR(bar),PANGO_ELLIPSIZE_START);
	gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(bar),GTK_PROGRESS_LEFT_TO_RIGHT);
	gtk_box_pack_start(GTK_BOX(page),bar,FALSE,FALSE,0);
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant),page,GTK_ASSISTANT_PAGE_CONTENT);
	page=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(page), _("<b>All Settings are complete. Have fun with Linnya</b>"));
	gtk_assistant_append_page(GTK_ASSISTANT(assistant),page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant),page,_("SUMMARY"));
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant),page,TRUE);
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant),page,GTK_ASSISTANT_PAGE_SUMMARY);
	gtk_widget_show_all(assistant);
//	g_timeout_add(100,lib_callback_timer,NULL);
	gtk_main();
	
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:		lib_read_sublib_start
//*	DISCRIPTION:	parse the start of xml makeup, serve for func. lib_load
//*
//*****************************************************************************************************
static void lib_read_sublib_start(GMarkupParseContext *context,const gchar *element_name,const gchar **attribute_names,const gchar **attribute_values,gpointer data,GError **error)
{
	_gl_playlist *pl=(_gl_playlist *)g_malloc(sizeof(_gl_playlist));
	pl->name=NULL;
	pl->list=NULL;

	const gchar **name_cursor = attribute_names;
	const gchar **value_cursor = attribute_values;
	
	while (*name_cursor)
	{
	
	if (strcmp (*name_cursor, "name") == 0)
	{
		pl->name = g_strdup (*value_cursor);
		gl_sublib=g_list_append (gl_sublib,pl);
	}
	
	name_cursor++;
	value_cursor++;
	} 
}
//********************************************************************************
//*
//*	FUNCTION:		lib_read_sublib
//*	DISCRIPTION:	read sublib
//*
//********************************************************************************
gboolean lib_read_sublib()
{
	gl_sublib=NULL;
	if(gl_conf->sublib==0)
		return FALSE;

	GMarkupParser parser = {
		.start_element = lib_read_sublib_start,
		.end_element = NULL,
		.text = NULL,
		.passthrough = NULL,
		.error = NULL
		};
		
		
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	
	g_file_get_contents(gl_file_sublib, &buf, &length,NULL);
	context = g_markup_parse_context_new(&parser, 0, NULL, NULL);
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		de_print(__DE_WARNNING,__FUNCTION__,_("Couldn't load xml,create a new one"));
		return FALSE;
	}
	g_markup_parse_context_free(context);
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_check_sublib
//*	DISCRIPTION:	update sublib
//*
//********************************************************************************
gboolean	lib_check_sublib ()
{
	if(gl_conf->sublib==0)
	{
		gl_free_list_playlist(gl_sublib);
		return FALSE;
	}

	_gl_playlist *pl;
	
	GList *p=NULL;
	_gl_metadata *plink;
	GList *q=NULL;
	_gl_playlist *qlink;
	
	GList *qbk=NULL;
	
	p=((_gl_playlist*)(gl_lib->data))->list;
	
loop:
	while(p)
	{
		plink=p->data;
		if(plink==NULL||plink->title==NULL)
		{
			p=p->next;
			continue;
		}
		q=gl_sublib;
		while(q)
		{
			qlink=q->data;
			switch(gl_conf->sublib)
			{
				case 1:	//按artist显示sublib
					if(g_strcmp0(plink->artist,qlink->name)==0)
					{
						qlink->list=g_list_append(qlink->list,plink);
						p=p->next;
						goto loop;
					}
					break;
				case 2:	//按album显示sublib
					if(g_strcmp0(plink->album,qlink->name)==0)
					{
						qlink->list=g_list_append(qlink->list,plink);
						p=p->next;
						goto loop;
					}
					break;
				default:
					break;
			}
			q=q->next;
		}
		if(q==NULL)
		{
			pl=(_gl_playlist*)g_malloc(sizeof(_gl_playlist));
			

			switch(gl_conf->sublib)
			{
				case 1:	//按artist显示sublib
					pl->name=g_strconcat(plink->artist,NULL);
					break;
				case 2:	//按album显示sublib
					pl->name=g_strconcat(plink->album,NULL);
					break;
				default:
					break;
			}
			pl->list=NULL;
			pl->list=g_list_append(pl->list,plink);
			gl_sublib=g_list_append(gl_sublib,pl);
			
		}
		p=p->next;
	}
	
	//删除空的子库项目
	q=gl_sublib;
	while(q)
	{
		qbk=q->next;
		qlink=q->data;
		if(qlink->list==NULL)
		{
			gl_sublib=g_list_delete_link(gl_sublib,q);
		}
		q=qbk;
	}
	
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_write_sublib
//*	DISCRIPTION:	write sublib
//*
//********************************************************************************
gboolean	lib_write_sublib ()
{
	FILE *fp;
	fp=fopen(gl_file_sublib,"w+");
	if(!fp)
	{
		de_print(__DE_WARNNING, __FUNCTION__, _("write sublib to file failed!"));
		return FALSE;
	}
	
	GList *p;
	_gl_playlist *plink;
	
	gchar *line=NULL;
	
	line=g_markup_printf_escaped("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<sublibrary>\n");
	fputs(line,fp);
	g_free(line);
	
	p=gl_sublib;
	while(p)
	{
		plink=p->data;
		line=g_markup_printf_escaped("\t<sublib name=\"%s\"/>\n",plink->name);
		fputs(line,fp);
		g_free(line);
		p=p->next;
	}
	
	line=g_markup_printf_escaped("</sublibrary>");
	fputs(line,fp);
	g_free(line);
	
	fclose(fp);
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_write_lib
//*	DISCRIPTION:	write sublib
//*
//********************************************************************************
gboolean	lib_write_lib ()
{
	FILE *fp;
	fp=fopen(gl_file_lib,"w+");
	if(!fp)
	{
		de_print(__DE_WARNNING, __FUNCTION__, _("write lib to file failed!"));
		return FALSE;
	}
	
	GList *p;
	_gl_metadata *plink;
	
	gchar *line=NULL;
	
	line=g_markup_printf_escaped("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<library>\n");
	fputs(line,fp);
	g_free(line);
	
	p=((_gl_playlist*)(gl_lib->data))->list;
	while(p)
	{
		plink=p->data;
		line=g_markup_printf_escaped("\t<lib title=\"%s\" artist=\"%s\" album=\"%s\" codec=\"%s\" start=\"%s\" duration=\"%s\" uri=\"%s\" />\n",plink->title,plink->artist,plink->album,plink->codec,plink->start,plink->duration,plink->uri);
		fputs(line,fp);
		g_free(line);
		p=p->next;
	}
	
	line=g_markup_printf_escaped("</library>");
	fputs(line,fp);
	g_free(line);
	
	fclose(fp);
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_callback_find_pl_by_name
//*	DISCRIPTION:	a callback
//*
//********************************************************************************
gint lib_callback_find_pl_by_name(gconstpointer a, gconstpointer b)
{
	_gl_playlist *p=(_gl_playlist*)a;
	gchar *q=(gchar*)b;
	
	return g_strcmp0(p->name,q);
}
//********************************************************************************
//*
//*	FUNCTION:		lib_find_pl_by_name
//*	DISCRIPTION:	find a sublib by name
//*
//********************************************************************************
GList* lib_find_pl_by_name(GList *list, gchar *name)
{
	if(!name)
		return NULL;
	GList *l;
	l=g_list_find_custom(list,name,lib_callback_find_pl_by_name);
	return l;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_find_pl_by_index
//*	DISCRIPTION:	find a sublib by index
//*
//********************************************************************************
GList* lib_find_pl_by_index(GList *list, gint index)
{
	GList *l;
	l=g_list_nth(list,index);
	return l;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_callback_lib_find_md_by_uri
//*	DISCRIPTION:	a callback
//*
//********************************************************************************
gint lib_callback_find_md_by_uri(gconstpointer a, gconstpointer b)
{
	_gl_metadata *p=(_gl_metadata *)a;
	_lib_uri *q=(_lib_uri *)b;
	if(q->duration==NULL)
		return (g_strcmp0(p->uri,q->uri))||(g_strcmp0(p->start,q->start));
	return (g_strcmp0(p->uri,q->uri))||(g_strcmp0(p->start,q->start))||(g_strcmp0(p->duration,q->duration));
}
//********************************************************************************
//*
//*	FUNCTION:		lib_find_md_by_uri
//*	DISCRIPTION:	find a metadata by uri
//*
//********************************************************************************
GList* lib_find_md_by_uri(GList *list, gchar *start, gchar *duration, gchar *uri)
{
	//注意：duration字段可以为空，为空表示匹配任意duration
	if(!uri)
		return NULL;
	if(!start)
		start=g_strconcat("00:00:000",NULL);
	_lib_uri *lib_uri=(_lib_uri *)g_malloc(sizeof(_lib_uri));
	lib_uri->start=start;
	lib_uri->duration=duration;
	lib_uri->uri=uri;
	
	GList *l;
	l=g_list_find_custom(list,lib_uri,lib_callback_find_md_by_uri);
	
	lib_uri->start=NULL;
	lib_uri->duration=NULL;
	lib_uri->uri=NULL;
	g_free(lib_uri);
	
	return l;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_find_md_by_index
//*	DISCRIPTION:	find a metadata by uri
//*
//********************************************************************************
GList* lib_find_md_by_index(GList *list, gint index)
{
	GList *l;
	l=g_list_nth (list,index);
	return l;
}
//********************************************************************************
//*
//*	FUNCTION:		lib_callback_lib_find_pgl_by_uri
//*	DISCRIPTION:	a callback
//*
//********************************************************************************
gint lib_callback_find_pgl_by_uri(gconstpointer a, gconstpointer b)
{
	_gl_playinglist *p=(_gl_playinglist *)a;
	_lib_playing *q=(_lib_playing *)b;
	if(	(g_strcmp0(p->md->uri,q->uri)==0)&&
		(g_strcmp0(p->md->start,q->start)==0)&&
		(g_strcmp0(p->md->duration,q->duration)==0)&&
		(g_strcmp0(p->name,q->name)==0)&&
		(p->flag*q->flag)>=0)
		return 0;
	else
		return 1;
}
//********************************************************************************
//*
//*	FUNCTION:		lib_find_pgl_by_uri
//*	DISCRIPTION:	find a pgl by uri
//*
//********************************************************************************
GList* lib_find_pgl_by_uri(GList *list, gchar *start, gchar *duration, gchar *uri, gchar *name, gint flag)
{
	if(!uri)
		return NULL;
	if(!name)
		return NULL;
	if(!start)
		start=g_strconcat("00:00:000",NULL);
	_lib_playing *lib_playing=(_lib_playing *)g_malloc(sizeof(_lib_playing));
	lib_playing->start=start;
	lib_playing->duration=duration;
	lib_playing->uri=uri;
	lib_playing->name=name;
	lib_playing->flag=flag;
	
	GList *l;
	l=g_list_find_custom(list,lib_playing,lib_callback_find_pgl_by_uri);
	
	lib_playing->start=NULL;
	lib_playing->duration=NULL;
	lib_playing->uri=NULL;
	lib_playing->name=NULL;
	g_free(lib_playing);
	
	return l;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_find_pgl_by_index
//*	DISCRIPTION:	find a metadata by uri
//*
//********************************************************************************
GList* lib_find_pgl_by_index(GList *list, gint index)
{
	GList *l;
	l=g_list_nth (list,index);
	return l;
}


//********************************************************************************
//*
//*	FUNCTION:		lib_add_md
//*	DISCRIPTION:	add a metadata to library
//*
//********************************************************************************
gboolean lib_add_md(_gl_metadata *md)
{
	GList *p=NULL;
	_gl_playlist *plink=NULL;
	
	//添加进lib
	if(lib_find_md_by_uri(((_gl_playlist*)(gl_lib->data))->list, md->start, md->duration, md->uri))
		return FALSE;
	((_gl_playlist*)(gl_lib->data))->list=g_list_append(((_gl_playlist*)(gl_lib->data))->list,md);
	
	//更新sublib
	switch(gl_conf->sublib)
	{
		case 0:
			return TRUE;
			break;
		case 1:
			p=lib_find_pl_by_name(gl_sublib,md->artist);
			break;
		case 2:
			p=lib_find_pl_by_name(gl_sublib,md->album);
			break;
		default:
			break;
	}
	if(p)
	{
		plink=p->data;
	}
	else
	{
		plink=(_gl_playlist*)g_malloc(sizeof(_gl_playlist));
		switch(gl_conf->sublib)
		{
			case 0:
				return TRUE;
				break;
			case 1:
				plink->name=g_strconcat(md->artist,NULL);
				break;
			case 2:
				plink->name=g_strconcat(md->album,NULL);
				break;
			default:
				break;
		}
		plink->list=NULL;
	}
	plink->list=g_list_append(plink->list,md);
	return TRUE;
}
//********************************************************************************
//*
//*	FUNCTION:		lib_delete_md
//*	DISCRIPTION:	delete a metadata from library
//*
//********************************************************************************
gboolean lib_delete_md(GList *p)
{
	if(!p)
		return FALSE;
	
	_gl_metadata *md;
	md=p->data;
	
	if(g_str_has_prefix(md->uri+7,gl_conf->lib))
	{
		//如果在物理库文件夹中，则删除文件
		gchar *cmd;
		gint result;
		cmd=g_strconcat("rm \"", (md->uri)+7,"\"", NULL);
		puts(cmd);
		result=system(cmd);
	}
	
	//移除p
	GList *q=NULL;
	_gl_playlist *qlink;
	GList *r=NULL;
	
	//删除sublib中的
	switch(gl_conf->sublib)
	{
		case 0:
			break;
		case 1:
			q=lib_find_pl_by_name(gl_sublib,md->artist);
			qlink=q->data;
			r=lib_find_md_by_uri(qlink->list,md->start,md->duration, md->uri);
			qlink->list=g_list_delete_link(qlink->list,r);
			break;
		case 2:
			q=lib_find_pl_by_name(gl_sublib,md->album);
			qlink=q->data;
			r=lib_find_md_by_uri(qlink->list,md->start,md->duration, md->uri);
			qlink->list=g_list_delete_link(qlink->list,r);
			break;
		default:
			break;
	}
	
	//删除playlist中的
	q=gl_playlist;
	while(q)
	{
		qlink=q->data;
		r=lib_find_md_by_uri(qlink->list,md->start, md->duration, md->uri);
		if(r)
			qlink->list=g_list_delete_link(qlink->list,r);
		q=q->next;
	}
	
	//释放md
	gl_free_metadata(p->data);
	((_gl_playlist*)(gl_lib->data))->list=g_list_delete_link(((_gl_playlist*)(gl_lib->data))->list,p);
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		lib_refresh_sublib
//*	DISCRIPTION:	refresh sublib, This function will make the list mis-order, use it carefully
//*
//********************************************************************************
gboolean lib_refresh_sublib()
{
	gl_selecting=gl_lib;
	gl_played=NULL;
	//释放sublib
	gl_free_list_playlist(gl_sublib);
	gl_sublib=NULL;
	//重建sublib
	lib_check_sublib();
	return TRUE;
}



//********************************************************************************
//*
//*	FUNCTION:		lib_callback_order_pl_by_name
//*	DISCRIPTION:	a callback
//*
//********************************************************************************
gboolean	lib_callback_order_pl_by_name(gpointer a, gpointer b)
{
	_gl_playlist *c=(_gl_playlist*)a;
	_gl_playlist *d=(_gl_playlist*)b;
	
	return g_strcmp0(c->name,d->name);
}
//********************************************************************************
//*
//*	FUNCTION:		lib_callback_order_md_by_title
//*	DISCRIPTION:	a callback
//*
//********************************************************************************
gboolean	lib_callback_order_md_by_title(gpointer a, gpointer b)
{
	_gl_metadata *c=(_gl_metadata*)a;
	_gl_metadata *d=(_gl_metadata*)b;
	
	return g_strcmp0(c->title,d->title);
}
//********************************************************************************
//*
//*	FUNCTION:		lib_callback_order_md_by_artist
//*	DISCRIPTION:	a callback
//*
//********************************************************************************
gboolean	lib_callback_order_md_by_artist(gpointer a, gpointer b)
{
	_gl_metadata *c=(_gl_metadata*)a;
	_gl_metadata *d=(_gl_metadata*)b;
	
	return g_strcmp0(c->artist,d->artist);
}
//********************************************************************************
//*
//*	FUNCTION:		lib_callback_order_md_by_album
//*	DISCRIPTION:	a callback
//*
//********************************************************************************
gboolean	lib_callback_order_md_by_album(gpointer a, gpointer b)
{
	_gl_metadata *c=(_gl_metadata*)a;
	_gl_metadata *d=(_gl_metadata*)b;
	
	return g_strcmp0(c->album,d->album);
}
//********************************************************************************
//*
//*	FUNCTION:		lib_order_pl_by_name
//*	DISCRIPTION:	order a playlist by name
//*
//********************************************************************************
GList*	lib_order_pl_by_name(GList *p)
{
	p=g_list_sort(p,(GCompareFunc)lib_callback_order_pl_by_name);
	return	p;
}
//********************************************************************************
//*
//*	FUNCTION:		lib_order_md_by_title
//*	DISCRIPTION:	order a playlist by title
//*
//********************************************************************************
GList*	lib_order_md_by_title(GList *q)
{
	q=g_list_sort(q,(GCompareFunc)lib_callback_order_md_by_title);
	return 	q;
}
//********************************************************************************
//*
//*	FUNCTION:		lib_order_pl_by_artist
//*	DISCRIPTION:	order a playlist by artist
//*
//********************************************************************************
GList*	lib_order_md_by_artist(GList *q)
{
	q=g_list_sort(q,(GCompareFunc)lib_callback_order_md_by_artist);
	return 	q;
}
//********************************************************************************
//*
//*	FUNCTION:		lib_order_pl_by_album
//*	DISCRIPTION:	order a playlist by album
//*
//********************************************************************************
GList*	lib_order_md_by_album(GList *q)
{
	q=g_list_sort(q,(GCompareFunc)lib_callback_order_md_by_album);
	return 	q;
}
