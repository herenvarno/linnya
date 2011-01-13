

//HEADERS
#include "equalizer.h"

_gl_equalizer *tmp_eq;
	
//FUNCTIONS
//*****************************************************************************************************
//*
//*	FUNCTION:		eq_read_equalizer_start
//*	DISCRIPTION:	parse the start of xml makeup, serve for func. eq_read_equalizer
//*
//*****************************************************************************************************
static void eq_read_equalizer_start(GMarkupParseContext *context,const gchar *element_name,const gchar **attribute_names,const gchar **attribute_values,gpointer data,GError **error)
{
	int i=0;
	if(g_strcmp0(element_name,"eq")==0)
	{
		tmp_eq=(_gl_equalizer *)g_malloc(sizeof(_gl_equalizer));
		tmp_eq->name=NULL;
		for(i=0;i<10;i++)
			tmp_eq->band[i]=0;
	}
	const gchar **name_cursor = attribute_names;
	const gchar **value_cursor = attribute_values;
	
	while (*name_cursor)
	{
		if (strcmp (*name_cursor, "name") == 0)
			tmp_eq->name= g_strdup (*value_cursor);
		if (strcmp (*name_cursor, "band") == 0)
		{
			sscanf(*value_cursor,"%lf:%lf:%lf:%lf:%lf:%lf:%lf:%lf:%lf:%lf",&tmp_eq->band[0],&tmp_eq->band[1],&tmp_eq->band[2],&tmp_eq->band[3],&tmp_eq->band[4],&tmp_eq->band[5],&tmp_eq->band[6],&tmp_eq->band[7],&tmp_eq->band[8],&tmp_eq->band[9]);
		}
	
		name_cursor++;
		value_cursor++;
	} 
}
//************************************************************************
//*
//*	FUNCTION:		eq_read_equalizer_end
//*	DISCRIPTION:	parse the start of xml makeup, serve for func. eq_read_equalizer
//*
//************************************************************************
static void eq_read_equalizer_end(
					GMarkupParseContext *context,
					const gchar *element_name,
					gpointer user_data,
					GError **error)
{
	if (g_strcmp0(element_name,"eq")==0)
	{
		GList *q=NULL;
		q=eq_find_eq_by_name(gl_equalizer, tmp_eq->name);
		if(!q)
			gl_equalizer=g_list_append(gl_equalizer,tmp_eq);
		else
			gl_free_equalizer(tmp_eq);
	}
}
//********************************************************************************
//*
//*	FUNCTION:		eq_read_equalizer
//*	DISCRIPTION:	read equalizer from file
//*
//********************************************************************************
gboolean eq_read_equalizer()
{
	int i=0;
	gl_equalizer=NULL;

	GMarkupParser parser = {
		.start_element = eq_read_equalizer_start,
		.end_element = eq_read_equalizer_end,
		.text = NULL,
		.passthrough = NULL,
		.error = NULL
		};
		
		
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	
	//载入用户均衡器配置
	g_file_get_contents(gl_file_equalizer2, &buf, &length,NULL);
	context = g_markup_parse_context_new(&parser, 0, NULL, NULL);
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		de_print(__DE_WARNNING,__FUNCTION__,_("Couldn't load xml,create a new one"));
	}
	if(context)
		g_markup_parse_context_free(context);
	//载入系统均衡器配置
	g_file_get_contents(gl_file_equalizer1, &buf, &length,NULL);
	context = g_markup_parse_context_new(&parser, 0, NULL, NULL);
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		de_print(__DE_WARNNING,__FUNCTION__,_("Couldn't load xml,create a new one"));
	}
	if(context)
		g_markup_parse_context_free(context);
	gl_eq=eq_find_eq_by_name(gl_equalizer,gl_conf->equalizer);
	if(!gl_eq)
		gl_eq=eq_find_eq_by_name(gl_equalizer,"default");
	if(!gl_eq)
	{
		_gl_equalizer *eq=(_gl_equalizer *)g_malloc(sizeof(_gl_equalizer));
		eq->name=g_strconcat("default",NULL);
		for(i=0;i<10;i++)
			eq->band[i]=0;
		gl_equalizer=g_list_append(gl_equalizer,eq);
	}
	return TRUE;
}
//********************************************************************************
//*
//*	FUNCTION:		eq_write_equalzier
//*	DISCRIPTION:	write equalizer
//*
//********************************************************************************
gboolean	eq_write_equalizer()
{
	FILE *fp;
	gchar *line;
	
	fp=fopen(gl_file_equalizer2,"w+");
	if(!fp)
	{
		de_print(__DE_WARNNING,__FUNCTION__,_("Cannot open equalizer file"));
		return FALSE;
	}
	
	line=g_markup_printf_escaped("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<equalizer>\n");
	fputs(line,fp);
	g_free(line);
	
	GList *p;
	_gl_equalizer *eq;
	p=gl_equalizer;
	while(p)
	{
		eq=p->data;
		line=g_markup_printf_escaped("\t<eq name=\"%s\" band=\"%lf:%lf:%lf:%lf:%lf:%lf:%lf:%lf:%lf:%lf\" />\n",eq->name,eq->band[0],eq->band[1],eq->band[2],eq->band[3],eq->band[4],eq->band[5],eq->band[6],eq->band[7],eq->band[8],eq->band[9]);
		fputs(line,fp);
		g_free(line);
		p=p->next;
	}
	line=g_strconcat("</equalizer>",NULL);
	fputs(line,fp);
	g_free(line);
	fclose(fp);
	return TRUE;
}
//********************************************************************************
//*
//*	FUNCTION:		eq_callback_find_eq_by_name
//*	DISCRIPTION:	a callback
//*
//********************************************************************************
gint eq_callback_find_eq_by_name(gconstpointer a, gconstpointer b)
{
	_gl_equalizer *p=(_gl_equalizer*)a;
	gchar *q=(gchar*)b;
	
	return g_strcmp0(p->name,q);
}
//********************************************************************************
//*
//*	FUNCTION:		eq_find_eq_by_name
//*	DISCRIPTION:	find a equalizer by name
//*
//********************************************************************************
GList* eq_find_eq_by_name(GList *list, gchar *name)
{
	if(!name)
		return NULL;
	GList *l;
	l=g_list_find_custom(list,name,eq_callback_find_eq_by_name);
	return l;
}
//********************************************************************************
//*
//*	FUNCTION:		eq_set_equalizer
//*	DISCRIPTION:	set equalizer
//*
//********************************************************************************
gboolean	eq_set_equalizer()
{
	if(!gl_eq)
		gl_eq=eq_find_eq_by_name(gl_equalizer,"default");
	_gl_equalizer *eq=gl_eq->data;
	g_object_set(G_OBJECT(gl_core->equalizer), "band0", eq->band[0], NULL);
	g_object_set(G_OBJECT(gl_core->equalizer), "band1", eq->band[1], NULL);
	g_object_set(G_OBJECT(gl_core->equalizer), "band2", eq->band[2], NULL);
	g_object_set(G_OBJECT(gl_core->equalizer), "band3", eq->band[3], NULL);
	g_object_set(G_OBJECT(gl_core->equalizer), "band4", eq->band[4], NULL);
	g_object_set(G_OBJECT(gl_core->equalizer), "band5", eq->band[5], NULL);
	g_object_set(G_OBJECT(gl_core->equalizer), "band6", eq->band[6], NULL);
	g_object_set(G_OBJECT(gl_core->equalizer), "band7", eq->band[7], NULL);
	g_object_set(G_OBJECT(gl_core->equalizer), "band8", eq->band[8], NULL);
	g_object_set(G_OBJECT(gl_core->equalizer), "band9", eq->band[9], NULL);
	
	if(gl_conf->equalizer)
		g_free(gl_conf->equalizer);
	gl_conf->equalizer=g_strconcat(eq->name,NULL);
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		eq_find_best
//*	DISCRIPTION:	find the best equalizer of the given name
//*
//********************************************************************************
GList* eq_find_best(GList *list, gchar *name)
{
	if(!name)
		return NULL;
	
	const gchar *name_down=g_utf8_strdown(name,-1);
	const gchar *temp_down=NULL;
	int i;
	int weight=0;
	GList *l=eq_find_eq_by_name(gl_equalizer,"default");
	
	GList *p=list;
	_gl_equalizer *plink;
	while(p)
	{
		plink=p->data;
		temp_down=g_utf8_strdown(plink->name,-1);
		if(g_strcmp0(temp_down,name_down)==0)	//全等
		{
			i=10000;
			if(i>weight)
			{
				l=p;
				weight=i;
				break;
			}
		}
		else if(g_strrstr(temp_down,name_down)||g_strrstr(name_down,temp_down))	//包含
		{
			i=500;
			if(i>weight)
			{
				l=p;
				weight=i;
			}
		}
		p=p->next;
	}

	return l;
}