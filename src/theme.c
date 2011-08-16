

//HEADERS
#include "theme.h"

//GLOBAL VARIBALES
gchar *temp;

//FUNCTIONS
//*****************************************************************************************************
//*
//*	FUNCTION:		theme_load_start
//*	DISCRIPTION:	get the start of theme index.xml, serve for theme_load
//*
//*****************************************************************************************************	
static void th_read_theme_start(GMarkupParseContext *context,const gchar *element_name,const gchar **attribute_names,const gchar **attribute_values,gpointer data,GError **error)
{
	const gchar **name_cursor = attribute_names;
	const gchar **value_cursor = attribute_values;
	const gchar *ele_name=element_name;
	
	if(g_strcmp0(ele_name,"theme")!=0)
	{
		_gl_theme *th=(_gl_theme*)g_malloc(sizeof(_gl_theme));
		th->name=g_strconcat(ele_name,NULL);
		th->x=th->y=th->w=th->h=-1;
		th->path=NULL;
		th->font=NULL;
		th->color_fg1=NULL;
		th->color_fg2=NULL;
		th->color_bg1=NULL;
		th->color_bg2=NULL;
		while(*name_cursor)
		{
			if (strcmp (*name_cursor, "x") == 0)
			{
				if((temp=g_strdup(*value_cursor))!=NULL)
				{
					sscanf(temp,"%d",&th->x);
					g_free(temp);
				}
			}
			else if (strcmp (*name_cursor, "y") == 0)
			{
				if((temp=g_strdup(*value_cursor))!=NULL)
			{
					sscanf(temp,"%d",&th->y);
					g_free(temp);
				}
			}
			else if (strcmp (*name_cursor, "w") == 0)
			{
				if((temp=g_strdup(*value_cursor))!=NULL)
				{
					sscanf(temp,"%d",&th->w);
					g_free(temp);
				}
			}
			else if (strcmp (*name_cursor, "h") == 0)
			{
				if((temp=g_strdup(*value_cursor))!=NULL)
				{
					sscanf(temp,"%d",&th->h);
					g_free(temp);
				}
			}
			else if (strcmp (*name_cursor, "path") == 0)
			{
				temp=g_strdup(*value_cursor);
				if(g_strcmp0(th->name,"label_volume")==0)
				{
					th->path=temp;
				}
				else
				{
					gchar *path=g_strconcat(gl_dir_conf2,"themes/",gl_conf->theme,"/",NULL);
					DIR *dir=opendir(path);
					g_free(path);
					if(dir)
					{
						th->path=g_strconcat(gl_dir_conf2,"themes/",gl_conf->theme,"/",temp,NULL);
					}
					else
					{
						th->path=g_strconcat(gl_dir_conf1,"themes/",gl_conf->theme,"/",temp,NULL);
					}
					closedir(dir);
					g_free(temp);
				}
			}
			else if (strcmp (*name_cursor, "font") == 0)
			{
				th->font=g_strdup(*value_cursor);
			}
			else if (strcmp (*name_cursor, "color_fg1") == 0)
			{
				th->color_fg1=g_strdup(*value_cursor);
			}
			else if (strcmp (*name_cursor, "color_fg2") == 0)
			{
				th->color_fg2=g_strdup(*value_cursor);
			}
			else if (strcmp (*name_cursor, "color_bg1") == 0)
			{
				th->color_bg1=g_strdup(*value_cursor);
			}
			else if (strcmp (*name_cursor, "color_bg2") == 0)
			{
				th->color_bg2=g_strdup(*value_cursor);
			}
			name_cursor++;
			value_cursor++;
		}
		gl_theme=g_list_append(gl_theme,th);
	}
}
//*****************************************************************************************************
//*
//*	FUNCTION:	theme_load
//*	DISCRIPTION:	get the detail theme setting from xml file
//*
//*****************************************************************************************************	
gboolean th_read_theme()
{
	gchar *path=g_strconcat(gl_dir_conf2,"themes/",gl_conf->theme,"/",NULL);
	DIR *dir=opendir(path);
	g_free(path);
	if(dir)
	{
		path=g_strconcat(gl_dir_conf2,"themes/",gl_conf->theme,"/index.xml",NULL);
	}
	else
	{
		path=g_strconcat(gl_dir_conf1,"themes/",gl_conf->theme,"/index.xml",NULL);
	}
	closedir(dir);
	
	GMarkupParser parser = {
		.start_element = th_read_theme_start,
		.end_element = NULL,
		.text = NULL,
		.passthrough = NULL,
		.error = NULL
		};
		
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	
	g_file_get_contents(path, &buf, &length,NULL);
	g_free(path);
	context = g_markup_parse_context_new(&parser, 0, NULL, NULL);
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		de_print(__DE_WARNNING,__FUNCTION__,_("Couldn't load xml,No theme found"));
		return FALSE;
	}
	g_markup_parse_context_free(context);
	return TRUE;
}
//*****************************************************************************************************
//*
//*	FUNCTION:	theme_get_by_name
//*	DISCRIPTION:	get the theme value by value name
//*
//*****************************************************************************************************	
_gl_theme *th_get_th_by_name(gchar *name)
{
	GList *p;
	_gl_theme *plink;
	
	p=gl_theme;
	while(p)
	{
		plink=p->data;
		if(g_strcmp0(plink->name,name)==0)
		{
			return plink;
		}
		p=p->next;
	}
	return NULL;
}

//*****************************************************************************************************
//*
//*	FUNCTION:	theme_get_name_list
//*	DISCRIPTION:	get the exist theme name list
//*
//*****************************************************************************************************		
GList *th_get_name_list()
{
	GList *list=NULL;
	GList *list1=NULL;
	GList *p,*q;
	gchar *path;
	path=g_strconcat(gl_dir_conf2,"themes/",NULL);
	list=com_get_dir_list(path,FALSE);
	g_free(path);
	path=g_strconcat(gl_dir_conf1,"themes/",NULL);
	list1=com_get_dir_list(path,FALSE);
	g_free(path);
	
	p=list1;
loop:	while(p)
	{
		q=list;
		while(q)
		{
			if(g_strcmp0(p->data,q->data)==0)
			{
				p=p->next;
				goto loop;
				break;
			}
			q=q->next;
		}
		list=g_list_append(list,g_strconcat(p->data,NULL));
		p=p->next;
	}
	gl_free_list_string(list1);
	return list;
}
	
	
	
	
	
	
	
	
	
