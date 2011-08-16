

//HEADERS
#include "playlist.h"

//VARIABLES
_gl_playlist *tmp_pl=NULL;
_gl_playinglist *tmp_pgl=NULL;

_lib_uri *tmp_uri=NULL;
gint playing;

//FUNCTIONS
//************************************************************************
//*
//*	FUNCTION:		pl_read_playlist_start
//*	DISCRIPTION:	parse the start of xml makeup, serve for func. pl_read_playinglist
//*
//************************************************************************
static void pl_read_playlist_start(
					GMarkupParseContext *context,
					const gchar *element_name,
					const gchar **attribute_names,
					const gchar **attribute_values,
					gpointer data,GError **error)
{
	const gchar **name_cursor = attribute_names;
	const gchar **value_cursor = attribute_values;
	
	while (*name_cursor)
	{
		if (strcmp (*name_cursor, "name") == 0)
		{
			tmp_pl=(_gl_playlist *)g_malloc(sizeof(_gl_playlist));
			tmp_pl->list=NULL;
			tmp_pl->name = g_strdup (*value_cursor);
			gl_playlist=g_list_append (gl_playlist,tmp_pl);
		}
		
		if (strcmp (*name_cursor, "start") == 0)
		{
			if(tmp_uri&&tmp_uri->start)
			{
				g_free(tmp_uri->start);
			}
			tmp_uri->start = g_strdup (*value_cursor);
		}
		if (strcmp (*name_cursor, "duration") == 0)
		{
			if(tmp_uri&&tmp_uri->duration)
			{
				g_free(tmp_uri->duration);
			}
			tmp_uri->duration = g_strdup (*value_cursor);
		}
		if (strcmp (*name_cursor, "uri") == 0)
		{
			if(tmp_uri&&tmp_uri->uri)
				g_free(tmp_uri->uri);
			tmp_uri->uri = g_strdup (*value_cursor);
		}
		
		name_cursor++;
		value_cursor++;
	}
}
//************************************************************************
//*
//*	FUNCTION:		pl_read_playlist_end
//*	DISCRIPTION:	parse the start of xml makeup, serve for func. pl_read_playinglist
//*
//************************************************************************
static void pl_read_playlist_end(
					GMarkupParseContext *context,
					const gchar *element_name,
					gpointer user_data,
					GError **error)
{
	if (g_strcmp0(element_name,"metadata")==0)
	{
		GList *q=NULL;
		q=lib_find_md_by_uri(((_gl_playlist*)(gl_lib->data))->list, tmp_uri->start,tmp_uri->duration, tmp_uri->uri);
		if(q&&q->data)
		{
			tmp_pl->list=g_list_append(tmp_pl->list,q->data);
		}
	}
}

//********************************************************************************
//*
//*	FUNCTION:		pl_read_playlist
//*	DISCRIPTION:	read playlist
//*
//********************************************************************************
gboolean pl_read_playlist()
{
	gl_playlist=NULL;
	if(!tmp_uri)
	{
		tmp_uri=(_lib_uri *)g_malloc(sizeof(_lib_uri));
		tmp_uri->start=NULL;
		tmp_uri->duration=NULL;
		tmp_uri->uri=NULL;
	}
	
	GMarkupParser parser = {
		.start_element = pl_read_playlist_start,
		.end_element = pl_read_playlist_end,
		.text = NULL,
		.passthrough = NULL,
		.error = NULL
		};
		
		
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	
	g_file_get_contents(gl_file_playlist, &buf, &length,NULL);
	context = g_markup_parse_context_new(&parser, 0, NULL, NULL);
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		de_print(__DE_WARNNING,__FUNCTION__,_("Couldn't load xml,create a new one."));
		return FALSE;
	}
	g_markup_parse_context_free(context);
	return TRUE;
}



//************************************************************************
//*
//*	FUNCTION:		pl_read_playlist_start
//*	DISCRIPTION:	parse the start of xml makeup, serve for func. pl_read_playlist
//*
//************************************************************************
static void pl_read_playinglist_start(
					GMarkupParseContext *context,
					const gchar *element_name,
					const gchar **attribute_names,
					const gchar **attribute_values,
					gpointer data,GError **error)
{
	
	const gchar **name_cursor = attribute_names;
	const gchar **value_cursor = attribute_values;
	
	if(g_strcmp0(element_name,"metadata")==0)
	{
		tmp_pgl=(_gl_playinglist*)g_malloc(sizeof(_gl_playinglist));
		tmp_pgl->name=NULL;
		tmp_pgl->flag=0;
		tmp_pgl->md=NULL;
	}
	while (*name_cursor)
	{
	
		if (strcmp (*name_cursor, "start") == 0)
		{
			if(tmp_uri&&tmp_uri->start)
				g_free(tmp_uri->start);
			tmp_uri->start = g_strdup (*value_cursor);
		}
		if (strcmp (*name_cursor, "duration") == 0)
		{
			if(tmp_uri&&tmp_uri->duration)
				g_free(tmp_uri->duration);
			tmp_uri->duration = g_strdup (*value_cursor);
		}
		if (strcmp (*name_cursor, "uri") == 0)
		{
			if(tmp_uri&&tmp_uri->uri)
				g_free(tmp_uri->uri);
			tmp_uri->uri = g_strdup (*value_cursor);
		}
		if (strcmp (*name_cursor, "name") == 0)
		{
			if(tmp_pgl&&tmp_pgl->name)
				g_free(tmp_pgl->name);
			tmp_pgl->name = g_strdup (*value_cursor);
		}
		if (strcmp (*name_cursor, "flag") == 0)
		{
			sscanf(*value_cursor,"%d",&tmp_pgl->flag);
		}
		if (strcmp (*name_cursor, "playing") == 0)
		{
			sscanf(*value_cursor,"%d",&playing);
		}
		
		name_cursor++;
		value_cursor++;
	}
}
//************************************************************************
//*
//*	FUNCTION:		pl_read_playinglist_end
//*	DISCRIPTION:	parse the start of xml makeup, serve for func. pl_read_playinglist
//*
//************************************************************************
static void pl_read_playinglist_end(
					GMarkupParseContext *context,
					const gchar *element_name,
					gpointer user_data,
					GError **error)
{
	if (g_strcmp0(element_name,"metadata")==0)
	{
		GList *q=NULL;
		q=lib_find_md_by_uri(((_gl_playlist*)(gl_lib->data))->list, tmp_uri->start,tmp_uri->duration, tmp_uri->uri);
		if(q&&q->data)
		{
			tmp_pgl->md=q->data;
			gl_playinglist=g_list_append(gl_playinglist,tmp_pgl);
			if(playing!=0)
				gl_playing=g_list_last(gl_playinglist);
			playing=0;
		}
		else
		{
			gl_free_playinglist(tmp_pgl);
			tmp_pgl=NULL;
		}
	}
}
//********************************************************************************
//*
//*	FUNCTION:		pl_read_playinglist
//*	DISCRIPTION:	read playlist
//*
//********************************************************************************
gboolean pl_read_playinglist()
{
	gl_playinglist=NULL;
	if(!tmp_uri)
	{
		tmp_uri=(_lib_uri *)g_malloc(sizeof(_lib_uri));
		tmp_uri->start=NULL;
		tmp_uri->duration=NULL;
		tmp_uri->uri=NULL;
	}
	
	GMarkupParser parser = {
		.start_element = pl_read_playinglist_start,
		.end_element = pl_read_playinglist_end,
		.text = NULL,
		.passthrough = NULL,
		.error = NULL
		};
		
		
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	
	g_file_get_contents(gl_file_playinglist, &buf, &length,NULL);
	context = g_markup_parse_context_new(&parser, 0, NULL, NULL);
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		de_print(__DE_WARNNING,__FUNCTION__,_("Couldn't load xml,create a new one."));
		return FALSE;
	}
	g_markup_parse_context_free(context);
	if(!gl_playing)
		gl_playing=g_list_first(gl_playinglist);
	gl_played=NULL;
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		pl_write_playlist
//*	DISCRIPTION:	write playlist
//*
//********************************************************************************
gboolean	pl_write_playlist ()
{
	FILE *fp;
	fp=fopen(gl_file_playlist,"w+");
	if(!fp)
	{
		de_print(__DE_WARNNING, __FUNCTION__, _("write playlist to file failed!"));
		return FALSE;
	}
	
	GList *p;
	_gl_playlist *plink;
	GList *q;
	_gl_metadata *qlink;
	
	gchar *line=NULL;
	
	line=g_markup_printf_escaped("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<playlist>\n");
	fputs(line,fp);
	g_free(line);
	
	p=gl_playlist;
	while(p)
	{
		plink=p->data;
		line=g_markup_printf_escaped("\t<pl name=\"%s\">\n",plink->name);
		fputs(line,fp);
		g_free(line);
		
		q=plink->list;
		while(q)
		{
			qlink=q->data;
			line=g_markup_printf_escaped("\t\t<metadata start=\"%s\" duration=\"%s\" uri=\"%s\" />\n",qlink->start, qlink->duration, qlink->uri);
			fputs(line,fp);
			g_free(line);
			q=q->next;
		}
		line=g_markup_printf_escaped("\t</pl>\n");
		fputs(line,fp);
		g_free(line);
		
		p=p->next;
	}
	
	line=g_markup_printf_escaped("</playlist>");
	fputs(line,fp);
	g_free(line);
	
	fclose(fp);
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		pl_write_playinglist
//*	DISCRIPTION:	write playinglist
//*
//********************************************************************************
gboolean	pl_write_playinglist ()
{
	FILE *fp;
	fp=fopen(gl_file_playinglist,"w+");
	if(!fp)
	{
		de_print(__DE_WARNNING, __FUNCTION__, _("write playinglist to file failed!"));
		return FALSE;
	}
	
	GList *p;
	_gl_playinglist *plink;
	gint playing=0;
	
	gchar *line=NULL;
	
	line=g_markup_printf_escaped("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<playinglist>\n");
	fputs(line,fp);
	g_free(line);
	
	p=gl_playinglist;
	while(p)
	{
		plink=p->data;
		if(p==gl_playing)
			playing=1;
		else
			playing=0;
		line=g_markup_printf_escaped("\t<metadata name=\"%s\" flag=\"%d\" start=\"%s\" duration=\"%s\" uri=\"%s\" playing=\"%d\" />\n",plink->name, plink->flag, plink->md->start, plink->md->duration, plink->md->uri, playing);
		fputs(line,fp);
		g_free(line);
		
		p=p->next;
	}
	
	line=g_markup_printf_escaped("</playinglist>");
	fputs(line,fp);
	g_free(line);
	
	fclose(fp);
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		pl_add_playlist
//*	DISCRIPTION:	add_playlist
//*
//********************************************************************************
gboolean	pl_add_playlist(gchar *name)
{
	int i=1;
	
	if(!name)
	{
		name=(gchar *)g_malloc(64*sizeof(gchar));
		g_snprintf(name,63,_("NEW PLAYLIST %d"), i);
		while(lib_find_pl_by_name(gl_playlist,name)!=NULL)
		{
			g_free(name);
			i++;
			name=(gchar *)g_malloc(64*sizeof(gchar));
			g_snprintf(name,63,_("NEW PLAYLIST %d"), i);
		}
	}
	else
	{
		if(lib_find_pl_by_name(gl_playlist,name)!=NULL)
		{
			de_print(__DE_ERROR, __FUNCTION__, _("A playlist with same name are exist! Add failed!"));
			return	FALSE;
		}
	}

	_gl_playlist *pl;
	pl=(_gl_playlist *)g_malloc(sizeof(_gl_playlist));
	pl->name=g_strconcat(name,NULL);
	pl->list=NULL;
	gl_playlist=g_list_append(gl_playlist,pl);
	
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		pl_add_to_playlist
//*	DISCRIPTION:	add metadata to playlist
//*
//********************************************************************************
gboolean	pl_add_to_playlist(GList *p, _gl_metadata *md)
{
	if((!md)&&(!p))
	{
		de_print(__DE_ERROR, __FUNCTION__, _("pl or md are null! Add failed!"));
		return FALSE;
	}
	
	_gl_playlist *plink=p->data;
	
	if(lib_find_md_by_uri(plink->list,md->start, md->duration, md->uri)!=NULL)
	{
		de_print(__DE_ERROR, __FUNCTION__, _("A music with same name are exist! Add failed!"));
		return	FALSE;
	}
	
	GList *q;
	_gl_metadata *qlink;
	
	q=lib_find_md_by_uri(((_gl_playlist*)(gl_lib->data))->list,md->start, md->duration, md->uri);
	if(q!=NULL)
	{
		qlink=q->data;
		plink->list=g_list_append(plink->list,qlink);
	}
	else
	{
		lib_add_md(md);
		plink->list=g_list_append(plink->list,md);
	}
	
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		pl_add_to_playinglist
//*	DISCRIPTION:	add metadata to playinglist
//*
//********************************************************************************
gboolean	pl_add_to_playinglist(_gl_playinglist *pgl)
{
	if(!pgl)
	{
		de_print(__DE_ERROR, __FUNCTION__, _("pgl are null! Add failed!"));
		return FALSE;
	}
	
	if(lib_find_pgl_by_uri(gl_playinglist,pgl->md->start, pgl->md->duration, pgl->md->uri,pgl->name,pgl->flag)!=NULL)
	{
		de_print(__DE_ERROR, __FUNCTION__, _("A music with same name are exist! Add failed!"));
		return	FALSE;
	}
	
	gl_playinglist=g_list_append(gl_playinglist,pgl);
	
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		pl_read_m3u
//*	DISCRIPTION:	analyse a m3u file
//*
//********************************************************************************
gboolean pl_read_m3u(GList *list, gchar *path)
{
	FILE *fp;
	fp=fopen(path,"r");
	if(!fp)
	{
		de_print(__DE_ERROR, __FUNCTION__, _("Cannot open the m3u file!"));
		return FALSE;
	}

	gchar *str;
	
	//解析文件
	GList *p;
	gchar *line;
	FILE *tmp_fp;
	
	gchar *uri;
	_gl_metadata *md;
	
	gint i=0;
	
	
	line=(gchar *)g_malloc(256*sizeof(gchar));
	while(fgets(line,256,fp)!=NULL)
	{
		line[strlen(line)-1]='\0';	//去掉'\n'
		for(i=0;i<strlen(line);i++)
		{
			if(line[i]!='#')
			{
				
				if(line[i]!=' '&&line[i]!='\t'&&line[i]!='\n')
				{
					uri=g_strconcat("file://",line,NULL);
					p=lib_find_md_by_uri(((_gl_playlist*)(gl_lib->data))->list,"00:00:000",NULL,uri);
					if(p)
					{//成功找到
						pl_add_to_playlist(list,p->data);
					}
					else
					{//未成功找到,则核对物理音乐文件是否存在，如果存在则添加到库，否则删除列表对应项
						tmp_fp=fopen(uri+7,"r");
						if(tmp_fp)
						{
							md=md_get_metadata(uri);
							pl_add_to_playlist(list,md);
						}
						else
						{
							str=g_strconcat("music ",uri,"has been deleted, clear it from playlist!",NULL);
							de_print(__DE_WARNNING, __FUNCTION__,str);
							g_free(str);
							g_free(uri);
							break;
						}
					}
					g_free(uri);
					break;
				}
			}
			else
				break;
		}
		g_free(line);
		line=(gchar *)g_malloc(256*sizeof(gchar));
	}
	g_free(line);
	fclose(fp);

	return TRUE;
}
//********************************************************************************
//*
//*	FUNCTION:		pl_read_cue
//*	DISCRIPTION:	analyse a cue file
//*
//********************************************************************************
gboolean pl_read_cue(GList *list, gchar *path)
{
	FILE *fp;
	fp=fopen(path,"r");
	if(!fp)
	{
		de_print(__DE_ERROR, __FUNCTION__, _("Cannot open the m3u file!"));
		return FALSE;
	}
	
	_gl_playlist *pl=list->data;
	gchar *c,*d;
	gchar *str;
	
	//解析文件
	gchar *line;
	gchar *temp;
	gsize r,w;
	gint i=0;

	gint duration;
	gint start;
	gint min;
	gint sec;
	gint frm;
	
	gboolean lock_global=TRUE;	//用来区分记录是全局的还是各个歌曲的
	gboolean lock_start=TRUE;	//用来确定该段是否INDEX 00 这个字段
	gchar *artist_global=g_strconcat("unknown",NULL);
	gchar *album_global=g_strconcat("unknown",NULL);
	gchar *uri_global=g_strconcat("unknown",NULL);
	gchar *duration_global=g_strconcat("00:00:01",NULL);
	_gl_metadata *md=NULL;
	_gl_metadata *md_bk=NULL;
	_gl_metadata *tmp_md=NULL;
	
	line=(gchar *)g_malloc(256*sizeof(gchar));
	while(fgets(line,256,fp)!=NULL)
	{
		//如果不是UTF－8，转换编码
		if(!g_utf8_validate(line,-1,NULL))
		{
			temp = g_convert(line, -1, "UTF-8", gl_conf->encode, &r, &w, NULL);
			if(line!=NULL)
				g_free(line);
			line = temp;
		}
		while(line[strlen(line)-1]=='\n'||line[strlen(line)-1]==' '||line[strlen(line)-1]=='\t')
		{
			line[strlen(line)-1]='\0';	//去掉'\n'和空格
		}
		for(i=0;i<strlen(line);i++)
		{
			if(line[i]!='#')
			{
				
				if(line[i]!=' '&&line[i]!='\t'&&line[i]!='\n')
				{
					if(	(line[i]=='T'||line[i]=='t')&&
						(line[i+1]=='I'||line[i+1]=='i')&&
						(line[i+2]=='T'||line[i+2]=='t')&&
						(line[i+3]=='L'||line[i+3]=='l')&&
						(line[i+4]=='E'||line[i+4]=='e'))		//TITLE标签
					{
						i=i+5;
						while(line[++i]!='\"');
						i++;
						c=&line[i];
						while(line[++i]!='\"');
						line[i]='\0';
						
						if(lock_global)
						{
							if(album_global)
								g_free(album_global);
							album_global=g_strconcat(c,NULL);
						}
						else
						{
							if(md->title)
								g_free(md->title);
							md->title=g_strconcat(c,NULL);
						}
						break;
					}
					if(	(line[i]=='P'||line[i]=='p')&&
						(line[i+1]=='E'||line[i+1]=='e')&&
						(line[i+2]=='R'||line[i+2]=='r')&&
						(line[i+3]=='F'||line[i+3]=='f')&&
						(line[i+4]=='O'||line[i+4]=='o')&&
						(line[i+5]=='R'||line[i+5]=='r')&&
						(line[i+6]=='M'||line[i+6]=='m')&&
						(line[i+7]=='E'||line[i+7]=='e')&&
						(line[i+8]=='R'||line[i+8]=='r'))		//PERFORMER标签
					{
						i=i+9;
						while(line[++i]!='\"');
						i++;
						c=&line[i];
						while(line[++i]!='\"');
						line[i]='\0';
						
						if(lock_global)
						{
							if(artist_global)
								g_free(artist_global);
							artist_global=g_strconcat(c,NULL);
						}
						else
						{
							if(md->artist)
								g_free(md->artist);
							md->artist=g_strconcat(c,NULL);
						}
						break;
					}
					if(	(line[i]=='F'||line[i]=='f')&&
						(line[i+1]=='I'||line[i+1]=='i')&&
						(line[i+2]=='L'||line[i+2]=='l')&&
						(line[i+3]=='E'||line[i+3]=='e'))		//FILE标签  仅支持绝对路径的读取
					{
						i=i+4;
						while(line[++i]!='\"');
						i++;
						c=&line[i];
						while(line[++i]!='\"');
						line[i]='\0';
						
						lock_start=TRUE;
						str=g_strconcat(path,NULL);
						while(str[strlen(str)-1]!='/')
							str[strlen(str)-1]='\0';
						str[strlen(str)-1]='\0';
						
						tmp_md=md_get_metadata(g_strconcat("file://",str,"/",c,NULL));
						if(!tmp_md)
						{
							g_free(line);
							g_free(str);
							return FALSE;
						}
						
						if(lock_global)
						{
							if(uri_global)
								g_free(uri_global);
							uri_global=g_strconcat("file://",str,"/",c,NULL);
						}
						else
						{
							if(md->uri)
								g_free(md->uri);
							md->uri=g_strconcat("file://",str,"/",c,NULL);
						}
						if(duration_global)
							g_free(duration_global);
						
						tmp_md=md_get_metadata(g_strconcat("file://",str,"/",c,NULL));
						duration_global=g_strconcat(tmp_md->duration,NULL);
						g_free(str);
						tmp_md=NULL;
						
						break;
					}
					if(	(line[i]=='T'||line[i]=='t')&&
						(line[i+1]=='R'||line[i+1]=='r')&&
						(line[i+2]=='A'||line[i+2]=='a')&&
						(line[i+3]=='C'||line[i+3]=='c')&&
						(line[i+4]=='K'||line[i+4]=='k'))		//TRACK标签
					{	
						i=i+5;
						md_bk=md;
						md=(_gl_metadata *)g_malloc(sizeof(_gl_metadata));
						md->title=g_strconcat(pl->name,NULL);
						md->artist=g_strconcat(artist_global,NULL);
						md->album=g_strconcat(album_global,NULL);
						md->codec=g_strconcat("FLAC",NULL);		//不关注这个字段，随意写一个，它会在播放时自动更正
						md->start=g_strconcat("00:00:00",NULL);
						md->duration=g_strconcat(duration_global,NULL);
						md->uri=g_strconcat(uri_global,NULL);
						
						lock_global=FALSE;
						break;
					}
					if(	(line[i]=='I'||line[i]=='i')&&
						(line[i+1]=='N'||line[i+1]=='n')&&
						(line[i+2]=='D'||line[i+2]=='d')&&
						(line[i+3]=='E'||line[i+3]=='e')&&
						(line[i+4]=='X'||line[i+4]=='x'))		//INDEX标签
					{
						i=i+5;
						while(line[i]==' '||line[i]=='\t'||line[i]=='\0')
							i++;
						if(	line[i]=='0'&&
							line[i+1]=='0')
						{
							i=i+2;
							lock_start=TRUE;
							
							while(line[i]==' '||line[i]=='\t')
								i++;
							c=&line[i];
							while(line[i]!=' '&&line[i]!='\r'&&line[i]!='\t'&&line[i]!='\0')
								i++;
							line[i]='\0';
							
							//填充上一首的duration
							tmp_md=md_bk;
							
							sscanf(tmp_md->start,"%d:%d:%d",&min,&sec,&frm);
							start=(min*60+sec)*100+frm;
							sscanf(c,"%d:%d:%d",&min,&sec,&frm);
							duration=(min*60+sec)*100+frm-start;
							frm=duration%100;
							duration=duration/100;
							sec=duration%60;
							min=duration/60;
							
							if(tmp_md->duration)
								g_free(tmp_md->duration);
							tmp_md->duration=(gchar *)g_malloc(64*sizeof(gchar));
							g_snprintf(tmp_md->duration,63,"%02d:%02d:%02d",min,sec,frm);
							tmp_md=NULL;
							
							// 添加进列表
							pl_add_to_playlist(list,md_bk);
						}
						if(	line[i]=='0'&&
							line[i+1]=='1')
						{
							i=i+2;
							while(line[i]==' '||line[i]=='\t')
								i++;
							c=&line[i];
							while(line[i]!=' '&&line[i]!='\r'&&line[i]!='\t'&&line[i]!='\0')
								i++;
							line[i]='\0';

							d=c;
							while(*d!='\0')
							{
								putchar(*d);
								d++;
							}
							putchar('^');
							putchar('\n');
							
							//填充本首歌曲的start
							if(md&&md->start)
								g_free(md->start);
							md->start=g_strconcat(c,NULL);
							
							//确定本首歌曲的默认duration
							sscanf(c,"%d:%d:%d",&min,&sec,&frm);
							start=(min*60+sec)*100+frm;
							sscanf(duration_global,"%d:%d:%d",&min,&sec,&frm);
							duration=(min*60+sec)*100+frm-start;
							frm=duration%100;
							duration=duration/100;
							sec=duration%60;
							min=duration/60;
							
							if(md->duration)
								g_free(md->duration);
							md->duration=(gchar *)g_malloc(64*sizeof(gchar));
							g_snprintf(md->duration,63,"%02d:%02d:%02d",min,sec,frm);
							
							//如果没有INDEX 00 则更新上一首歌曲的duration
							if(!lock_start)
							{
								tmp_md=md_bk;
							
								sscanf(tmp_md->start,"%d:%d:%d",&min,&sec,&frm);
								start=(min*60+sec)*100+frm;
								sscanf(c,"%d:%d:%d",&min,&sec,&frm);
								duration=(min*60+sec)*100+frm-start;
								frm=duration%100;
								duration=duration/100;
								sec=duration%60;
								min=duration/60;
							
								if(tmp_md->duration)
									g_free(tmp_md->duration);
								tmp_md->duration=(gchar *)g_malloc(64*sizeof(gchar));
								g_snprintf(tmp_md->duration,63,"%02d:%02d:%02d",min,sec,frm);
								tmp_md=NULL;
								
								// 添加进列表
								pl_add_to_playlist(list,md_bk);
							}
							lock_start=FALSE;
							
						}
						break;
					}
				}
			}
			else
				break;
		}
		g_free(line);
		line=(gchar *)g_malloc(256*sizeof(gchar));
	}
	if(line)
		g_free(line);
	fclose(fp);
	pl_add_to_playlist(list,md);
	return TRUE;
}


//********************************************************************************
//*
//*	FUNCTION:		pl_import_playlist
//*	DISCRIPTION:	import a playinglist
//*
//********************************************************************************
gboolean pl_import_playlist(gchar *path)
{
	//新建列表并以文件名命名列表
	gchar *str=g_strconcat(path,NULL);
	gchar *c=com_get_filename(str);
	if(!pl_add_playlist(c))
		pl_add_playlist(NULL);
	g_free(str);
	GList *list=g_list_last(gl_playlist);

	switch(com_get_filetype(path))
	{
		case com_type_m3u:
			pl_read_m3u(list, path);
			break;
		case com_type_cue:
			pl_read_cue(list, path);
			break;
		default:
			de_print(__DE_ERROR, __FUNCTION__, _("Linnya don't know how to deal with the file!!"));
			gl_free_playlist(list->data);
			gl_playlist=g_list_delete_link(gl_playlist,list);
			break;
	}
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		pl_write_m3u
//*	DISCRIPTION:	write to a m3u file
//*
//********************************************************************************
gboolean pl_write_m3u(GList *p, gchar *path)
{
	if(((_gl_playlist*)(p->data))->list==NULL)
		return FALSE;
	FILE *fp;
	fp=fopen(path,"w+");
	if(!fp)
	{
		de_print(__DE_ERROR, __FUNCTION__, _("Cannot create the m3u file!"));
		return FALSE;
	}
	_gl_playlist *plink=p->data;
	GList *q;
	_gl_metadata *qlink;
	gchar *line;
	
	fputs("#EXTM3U\n",fp);
	q=plink->list;
	while(q)
	{
		qlink=q->data;
		line=g_strconcat("#EXTINF:",qlink->title,"\n",NULL);
		fputs(line,fp);
		g_free(line);
		line=(gchar*)qlink->uri;
		line=line+7;
		line=g_strconcat(line,"\n",NULL);
		fputs(line,fp);
		g_free(line);
		line=NULL;
		q=q->next;
	}
	fclose(fp);
	return TRUE;
}
//********************************************************************************
//*
//*	FUNCTION:		pl_write_cue
//*	DISCRIPTION:	write to a cue file
//*
//********************************************************************************
gboolean pl_write_cue(GList *p, gchar *path)
{
	if(((_gl_playlist*)(p->data))->list==NULL)
		return FALSE;
	FILE *fp;
	fp=fopen(path,"w+");
	if(!fp)
	{
		de_print(__DE_ERROR, __FUNCTION__, _("Cannot create the cue file!"));
		return FALSE;
	}
	_gl_playlist *plink=p->data;
	GList *q;
	_gl_metadata *qlink;
	gchar *line;
	gint i=0;
	gint min;
	gint sec;
	gint frm;
	gint start;
	gint end;
	gchar *str=NULL;
	
	//写入全局属性
	q=plink->list;
	qlink=q->data;
	gchar *artist_global=g_strconcat(qlink->artist,NULL);
	gchar *uri_global=g_strconcat(qlink->uri,NULL);
	line=(gchar *)g_malloc(256*sizeof(gchar));
	g_snprintf(line,255,"PERFORMER \"%s\"\nTITLE \"%s\"\nFILE \"%s\" WAVE\n", qlink->artist, qlink->album, com_get_file(qlink->uri));
	fputs(line,fp);
	g_free(line);
	
	//分别写入
	while(q)
	{
		i++;
		qlink=q->data;
		line=(gchar *)g_malloc(256*sizeof(gchar));
		g_snprintf(line,255,"\tTRACK %02d AUDIO\n",i);
		fputs(line,fp);
		g_free(line);
		line=(gchar *)g_malloc(256*sizeof(gchar));
		g_snprintf(line,255,"\t\tTITLE \"%s\"\n",qlink->title);
		fputs(line,fp);
		g_free(line);
		if(g_strcmp0(qlink->artist,artist_global)!=0)
		{
			line=(gchar *)g_malloc(256*sizeof(gchar));
			g_snprintf(line,255,"\t\tPERFORMER \"%s\"\n",qlink->title);
			fputs(line,fp);
			g_free(line);
		}
		if(g_strcmp0(qlink->uri,uri_global)!=0)
		{
			line=(gchar *)g_malloc(256*sizeof(gchar));
			g_snprintf(line,255,"\t\tFILE \"%s\"\n",com_get_file(qlink->uri));
			fputs(line,fp);
			g_free(line);
		}
		if(i!=1)
		{
			sscanf(((_gl_metadata*)(q->prev->data))->start,"%d:%d:%d",&min,&sec,&frm);
			start=(min*60+sec)*100+frm;
			sscanf(((_gl_metadata*)(q->prev->data))->duration,"%d:%d:%d",&min,&sec,&frm);
			end=(min*60+sec)*100+frm+start;
			frm=end%100;
			end=end/100;
			sec=end%60;
			min=end/60;
			
			str=(gchar*)g_malloc(64*sizeof(gchar));
			g_snprintf(str,63,"%02d:%02d:%02d",min,sec,frm);
			line=(gchar *)g_malloc(256*sizeof(gchar));
			g_snprintf(line,255,"\t\tINDEX 00 %s\n",str);
			fputs(line,fp);
			g_free(line);
			g_free(str);
		}
		line=(gchar *)g_malloc(256*sizeof(gchar));
		g_snprintf(line,255,"\t\tINDEX 01 %s\n",qlink->start);
		fputs(line,fp);
		g_free(line);
		
		q=q->next;
	}
	if(artist_global)
		g_free(artist_global);
	if(uri_global)
		g_free(uri_global);
	fclose(fp);
	return TRUE;
}


//********************************************************************************
//*
//*	FUNCTION:		pl_export_playlist
//*	DISCRIPTION:	export to a file
//*
//********************************************************************************
gboolean pl_export_playlist(GList *p, gchar *path)
{
	switch(com_get_filetype(path))
	{
		case com_type_m3u:
			pl_write_m3u(p, path);
			break;
		case com_type_cue:
			pl_write_cue(p, path);
			break;
		default:
			de_print(__DE_ERROR, __FUNCTION__, _("Linnya don't know how to deal with the file!!"));
			break;
	}
	return TRUE;
}
//********************************************************************************
//*
//*	FUNCTION:		pl_rempve_playlist
//*	DISCRIPTION:	remove a playinglist
//*
//********************************************************************************
gboolean	pl_remove_playlist(GList *p)
{
	gl_free_playlist(p->data);
	p->data=NULL;
	gl_playlist=g_list_delete_link(gl_playlist,p);
	return TRUE;
}
//********************************************************************************
//*
//*	FUNCTION:		pl_rename_playlist
//*	DISCRIPTION:	rename a playinglist
//*
//********************************************************************************
gboolean	pl_rename_playlist(GList *p, gchar *name)
{
	if(!name)
		return FALSE;
	
	_gl_playlist *plink;
	plink=p->data;
	if(plink->name)
		g_free(plink->name);
	plink->name=g_strconcat(name,NULL);
	return TRUE;
}
//********************************************************************************
//*
//*	FUNCTION:		pl_remove_md_from_playlist
//*	DISCRIPTION:	remove a metadata from playlist
//*
//********************************************************************************
gboolean	pl_remove_md_from_playlist(GList *p, GList *q)
{
	if(!p||!q)
		return FALSE;
	
	_gl_playlist *plink;
	plink=p->data;
	
	plink->list=g_list_delete_link(plink->list,q);
	return TRUE;
}
//********************************************************************************
//*
//*	FUNCTION:		pl_remove_md_from_playinglist
//*	DISCRIPTION:	remove a metadata from playinglist
//*
//********************************************************************************
gboolean	pl_remove_md_from_playinglist(GList *q)
{
	if(!q)
		return FALSE;
	
	gl_playinglist=g_list_delete_link(gl_playinglist,q);
	return TRUE;
}

//********************************************************************************
//*
//*	FUNCTION:		pl_find_pl_by_playing
//*	DISCRIPTION:	return the playlist that playing song belongs to
//*
//********************************************************************************
GList*	pl_find_pl_by_playing()
{
	if(!gl_playing)
		return NULL;
	
	_gl_playinglist *pgl=gl_playing->data;
	
	GList *p=NULL;
	_gl_playlist *plink;
	GList *q=NULL;
	switch (pgl->flag)
	{
		case 0:
			p=gl_lib;
			break;
		case 1:
			p=lib_find_pl_by_name(gl_sublib,pgl->name);
			break;
		case 2:
			p=lib_find_pl_by_name(gl_playlist,pgl->name);
			break;
		default:
			break;
	}
	if(!p)
	{
		if(pgl->name)
			g_free(pgl->name);
		pgl->name=g_strconcat("All Tracks",NULL);
		pgl->flag=0;
		p=gl_lib;
	}
	plink=p->data;
	q=plink->list;
	q=g_list_find(q,pgl->md);
	if(!q)
	{
		if(pgl->name)
			g_free(pgl->name);
		pgl->name=g_strconcat("All Tracks",NULL);
		pgl->flag=0;
		p=gl_lib;
	}
	
	return p;
}
