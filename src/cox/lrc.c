/*
 * lrc.c
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * HEADERS
 */
#include "lrc.h"

/*
 * VARIABLES
 */
LyLrcLyric	*ly_lrc_lyrics_array[10240];
gint		ly_lrc_lyrics_length=0;
gint		ly_lrc_lyrics_index=0;
gint		ly_lrc_timeout=0;
gboolean	ly_lrc_flag_update_state=TRUE;

/*
 * FUNCTIONS
 */
void ly_lrc_read(FILE *fp);
void ly_lrc_sort();

static gint ly_lrc_sort_cb(gconstpointer c, gconstpointer d, gpointer data);
void	ly_lrc_on_md_changed_cb(LyMbsMessage *message, gpointer data);
gboolean	ly_lrc_on_update_cb(gpointer data);

void		ly_lrc_init		()
{
	ly_mbs_bind("meta_changed", "core:", ly_lrc_on_md_changed_cb, NULL);
	ly_mbs_bind("lrc_update", "", ly_lrc_on_md_changed_cb, NULL);
	ly_lrc_timeout=g_timeout_add(100, ly_lrc_on_update_cb, NULL);
}

void		ly_lrc_fina		()
{
	ly_mbs_unbind("meta_changed", "core:", ly_lrc_on_md_changed_cb);
	ly_mbs_unbind("lrc_update", "", ly_lrc_on_md_changed_cb);
	g_source_remove(ly_lrc_timeout);
}

char*		ly_lrc_build_path	(LyMdhMetadata *md)
{
	if(!md)
		return NULL;

	gchar dir[1024]="./";
	gchar *lrcfile=NULL;
	gchar *path=NULL;
	gchar *filename=NULL;

	if(!ly_reg_get("lrc_dir", "%s", dir))
	{
		ly_reg_set("lrc_dir", "%s", dir);
	}

	if(g_str_equal(dir,"")||g_str_equal(dir,"./"))
	{
		path=ly_gla_uri_get_dir(md->uri);
		filename=ly_gla_uri_get_filename(md->uri);

		if(md->flag==1)
		{
			lrcfile=g_strconcat(path, filename, "-", md->start,".lrc", NULL);
		}
		else
		{
			lrcfile=g_strconcat(path, filename, ".lrc", NULL);
		}
	}
	else
	{
		if(md->flag==1)
			lrcfile=g_strconcat(dir, md->title, "-", md->artist, "-", md->start,".lrc", NULL);
		else
			lrcfile=g_strconcat(dir, md->title, "-", md->artist, ".lrc", NULL);
	}
	g_free(path);
	g_free(filename);
	return lrcfile;
}

gboolean ly_lrc_load(char *path)
{
	LyMdhMetadata *md=NULL;
	md=ly_pqm_get_current_md();
	if(!md)
		return FALSE;
	if(!path)
	{
		return FALSE;
	}

	ly_lrc_lyrics_length=0;
	ly_lrc_lyrics_index=0;

	FILE *fp=NULL;
	fp=fopen(path,"r");
	if(fp==NULL)
	{
		return FALSE;
	}
	ly_lrc_read(fp);
	ly_lrc_sort();
	return FALSE;
}

void ly_lrc_read(FILE *fp)
{
	ly_lrc_lyrics_length=0;

	/*
	 * Extra Encoding
	 */
	gchar extra_encoding[1024]="GB18030";
	if(!ly_reg_get("dbm_extra_encoding", "%*[^\n(](%1023[^\n)]", extra_encoding))
	{
		ly_reg_set("dbm_extra_encoding", "Chinese Simplified (GB18030)");
	}

	LyLrcLyric *lrc=NULL;
	LyLrcLyric *tmplrc[1024];

	gint64 time;
	gint64 time_delay=0;
	gchar line[1024]="";

	gchar *p,*q;
	gchar label[1024];
	gchar text[1024];

	gchar *tmpstr;

	gint i=0;
	gint j=0;

	while(fgets(line, sizeof(line), fp))
	{
		j=0;
		if(line[strlen(line)-1]=='\n')
			line[strlen(line)-1]='\0';
		lrc=NULL;
		p=line;
		while(*p)
		{
			while(*p=='[')
			{
				p++;
				if((*p<='9'&&*p>='0')||(*p=='-'&&*(p+1)<='9'&&*(p+1)>='0'))		//时间标签
				{
					//读取时间
					label[0]='\0';
					q=label;
					while(*p!=']')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';
					p++;
					//读取文本
					text[0]='\0';
					q=text;
					while(*p!='['&&*p!='\0')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';

					//读取完成，处理数据
					lrc=(LyLrcLyric*)malloc(sizeof(LyLrcLyric));
					lrc->time=0;
					g_strlcpy(lrc->text, "", sizeof(lrc->text));
					time=(gint64)(ly_mdh_time_str2int(label))+(time_delay*1000000);
					if(time<0)
						time=0;
					lrc->time=time;

					if(*p=='['&&g_str_equal(text,""))
					{
						tmplrc[j]=lrc;
						j++;
					}
					else
					{
						for (i = 0; i <j; i++)
						{
							if(g_utf8_validate(text,-1,NULL))
							{
								g_utf8_strncpy(tmplrc[i]->text,text,sizeof(tmplrc[i]->text));
							}
							else	//非UTF-8字符
							{
								tmpstr=g_convert(text, strlen(text),"UTF-8",extra_encoding,NULL,NULL,NULL);
								g_strlcpy(tmplrc[i]->text,tmpstr, sizeof(tmplrc[i]->text));
								g_free(tmpstr);
							}
							if(ly_lrc_lyrics_array[ly_lrc_lyrics_length]!=NULL)
							{
								g_free(ly_lrc_lyrics_array[ly_lrc_lyrics_length]);
							}
							ly_lrc_lyrics_array[ly_lrc_lyrics_length]=tmplrc[i];
							ly_lrc_lyrics_length++;
						}
						j=0;
						if(g_utf8_validate(text,-1,NULL))
						{
							g_utf8_strncpy(lrc->text,text,sizeof(lrc->text));
						}
						else	//非UTF-8字符
						{
							tmpstr=g_convert(text, strlen(text),"UTF-8",extra_encoding,NULL,NULL,NULL);
							g_strlcpy(lrc->text,tmpstr, sizeof(lrc->text));
							g_free(tmpstr);
						}
						if(ly_lrc_lyrics_array[ly_lrc_lyrics_length]!=NULL)
						{
							g_free(ly_lrc_lyrics_array[ly_lrc_lyrics_length]);
						}
						ly_lrc_lyrics_array[ly_lrc_lyrics_length]=lrc;
						ly_lrc_lyrics_length++;
					}
				}
				else	//其他标签
				{
					label[0]='\0';
					q=label;
					while(*p!=':'&&*p!='\0')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';
					p++;

					text[0]='\0';
					q=text;
					while(*p!=']'&&*p!='\0')
					{
						*q=*p;
						q++;
						p++;
					}
					*q='\0';

					if(g_str_equal(label,"offset")||g_str_equal(label,"OFFSET"))
					{
						if(!g_str_equal(text,""))
						{
							sscanf(text,"%lld",&time_delay);
						}
					}
					p++;
					break;
				}
			}
			p++;
		}
	}
	fclose(fp);
}

static gint ly_lrc_sort_cb(gconstpointer c, gconstpointer d, gpointer data)
{
	LyLrcLyric *a=*((LyLrcLyric**)c);
	LyLrcLyric *b=*((LyLrcLyric**)d);

	if(a->time<b->time)
		return -1;
	else
		return 1;
}
void ly_lrc_sort()
{
	g_qsort_with_data(ly_lrc_lyrics_array, ly_lrc_lyrics_length, sizeof(LyLrcLyric *), ly_lrc_sort_cb, NULL);
}

LyLrcLyric*	ly_lrc_new()
{
	return (LyLrcLyric*)g_malloc(sizeof(LyLrcLyric));
}
LyLrcLyric*	ly_lrc_new_full(gint64 time, gchar *text)
{
	if(!text)
		return NULL;

	LyLrcLyric *lyric=NULL;
	lyric=ly_lrc_new();
	if(!lyric)
		return NULL;
	lyric->time=time;
	g_strlcpy(lyric->text, text, sizeof(lyric->text));
	return lyric;
}
void		ly_lrc_free(LyLrcLyric *lyric)
{
	if(lyric)
	{
		g_free(lyric);
	}
}


LyLrcLyric**	ly_lrc_get_array()
{
	return ly_lrc_lyrics_array;
}
int			ly_lrc_get_index()
{
	return ly_lrc_lyrics_index;
}

int			ly_lrc_get_length()
{
	return ly_lrc_lyrics_length;
}

void		ly_lrc_set_index(int index)
{
	if(index<0||index>=ly_lrc_lyrics_length)
		return;
	ly_lrc_lyrics_index=index;
}


void ly_lrc_subtitle_load()
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return;
	GstElement *play=NULL;
	play=ly_ppl_get_playbin();
	if(!play)
		return;

	char extra_encoding[1024]="GB18030";
	if(!ly_reg_get("dbm_extra_encoding", "%*[^\n(](%1023[^\n)]", extra_encoding))
	{
		ly_reg_set("dbm_extra_encoding", "Chinese Simplified (GB18030)");
	}
	g_object_set(G_OBJECT(play), "subtitle-encoding", extra_encoding, NULL);
	g_object_set(G_OBJECT(play),"suburi", NULL, NULL);

	gchar *path;
	gchar *filename;
	gchar *contents;
	path=ly_gla_uri_get_dir(md->uri);
	filename=ly_gla_uri_get_filename(md->uri);
	GList *list=NULL;
	GList *p=NULL;
	gchar *str_path=g_regex_escape_string(path, -1);
	gchar *str_filename=g_regex_escape_string(filename, -1);
	gchar str[1024]="";
	g_snprintf(str, sizeof(str), "%s%s.*\\.(?i:srt|sub)$", str_path, str_filename);
	list=ly_gla_traverse_dir(path, str, 1, 1000, FALSE);
	p=list;
	if(list)
	{
		g_object_set(G_OBJECT(play),"suburi", p->data, NULL);
		g_file_get_contents(p->data+7, &contents, NULL, NULL);
		if(g_utf8_validate(contents, -1, NULL))
		{
			g_object_set(G_OBJECT(play), "subtitle-encoding", "UTF8", NULL);
		}
		g_free(contents);
	}
	p=list;
	while(p)
	{
		g_free(p->data);
		p->data=NULL;
		p=p->next;
	}
	g_list_free(list);
	g_free(path);
	g_free(filename);
	g_free(str_path);
	g_free(str_filename);
}

void	ly_lrc_on_md_changed_cb(LyMbsMessage *message, gpointer data)
{
	LyMdhMetadata *md=ly_pqm_get_current_md();
	if(!md)
		return;

	ly_lrc_lyrics_length=0;
	// FOR AUDIO
	if(md->flag>=0 && md->flag<=9)
	{
		char *path=ly_lrc_build_path(ly_pqm_get_current_md());
		if(!path)
			return;
		ly_lrc_load(path);
	}

	// FOR VIDEO
	if(md->flag>=10 && md->flag<=19)
	{
		ly_lrc_subtitle_load();
	}
}

gboolean ly_lrc_on_update_cb(gpointer data)
{
	if(!ly_lrc_flag_update_state)
		return TRUE;

	if(ly_lrc_lyrics_length<=0)
		return TRUE;

	LyMdhMetadata *md=NULL;
	md=ly_pqm_get_current_md();
	if(!md)
		return TRUE;

	gint64 time=0;
	time=ly_aud_get_position_abs();

	int min=0;
	int max=ly_lrc_lyrics_length;
	int index=(max-min)/2;

	while(max-min>1)
	{
		if(ly_lrc_lyrics_array[index]->time<time)
		{
			min=index;
			index=(max+min)/2;
		}
		else if(ly_lrc_lyrics_array[index]->time>time)
		{
			max=index;
			index=(max+min)/2;
		}
		else
		{
			ly_lrc_lyrics_index=index;
			return TRUE;
		}
	}
	ly_lrc_lyrics_index=(max-index>=index-min)?min:max;
	return TRUE;
}

void		ly_lrc_set_update_state(gboolean state)
{
	ly_lrc_flag_update_state=state;
}
