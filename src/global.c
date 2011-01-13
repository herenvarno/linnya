

//HEADERS
#include"global.h"


//FUNCTIONS
gboolean gl_free_list_string(GList *list)
{
	if(!list)
		return	TRUE;

	GList *p;
	
	p=list;
	while(p)
	{
		if(p->data)
			g_free(p->data);
		p->data=NULL;
		p=p->next;
	}
	g_list_free(p);
	list=NULL;
	return TRUE;
}

gboolean gl_free_list_metadata(GList *list)
{
	if(!list)
		return	TRUE;

	GList *p;
	_gl_metadata *md;
	
	p=list;
	while(p)
	{
		if(p->data)
		{
			md=((_gl_metadata*)(p->data));
			gl_free_metadata(md);
		}
		p->data=NULL;
		p=p->next;
	}
	g_list_free(p);
	list=NULL;
	return TRUE;
}


gboolean gl_free_list_playlist(GList *list)
{
	if(!list)
		return	TRUE;

	GList *p;
	_gl_playlist *pl;
	
	p=list;
	while(p)
	{
		if(p->data)
		{
			pl=p->data;
			gl_free_playlist(pl);
		}
		p->data=NULL;
		p=p->next;
	}
	g_list_free(p);
	list=NULL;
	return TRUE;
}
gboolean gl_free_list_playinglist(GList *list)
{
	if(!list)
		return	TRUE;

	GList *p;
	_gl_playinglist *pgl;
	
	p=list;
	while(p)
	{
		if(p->data)
		{
			pgl=p->data;
			gl_free_playinglist(pgl);
		}
		p->data=NULL;
		p=p->next;
	}
	g_list_free(p);
	list=NULL;
	return TRUE;
}
gboolean gl_free_list_equalizer(GList *list)
{
	if(!list)
		return	TRUE;

	GList *p;
	_gl_equalizer *eq;
	
	p=list;
	while(p)
	{
		if(p->data)
		{
			eq=p->data;
			gl_free_equalizer(eq);
		}
		p->data=NULL;
		p=p->next;
	}
	g_list_free(p);
	list=NULL;
	return TRUE;
}
gboolean gl_free_list_lyrics(GList *list)
{
	if(!list)
		return	TRUE;

	GList *p;
	_gl_lyrics *lrc;
	
	p=list;
	while(p)
	{
		if(p->data)
		{
			lrc=p->data;
			gl_free_lyrics(lrc);
		}
		p->data=NULL;
		p=p->next;
	}
	g_list_free(p);
	list=NULL;
	return TRUE;
}
gboolean gl_free_playlist(_gl_playlist *pl)
{
	if(!pl)
		return	TRUE;


	if(pl->name)
		g_free(pl->name);
	if(pl->list)
		g_list_free(pl->list);
	g_free(pl);
	pl=NULL;
	return TRUE;
}

gboolean gl_free_metadata(_gl_metadata *md)
{
	if(!md)
		return	TRUE;

	if(md->title)
		g_free(md->title);
	if(md->artist)
		g_free(md->artist);
	if(md->album)
		g_free(md->album);
	if(md->codec)
		g_free(md->codec);
	if(md->start)
		g_free(md->start);
	if(md->duration)
		g_free(md->duration);
	if(md->uri)
		g_free(md->uri);
	
	g_free(md);
	md=NULL;
	return TRUE;
}

gboolean gl_free_playinglist(_gl_playinglist *pgl)
{
	if(!pgl)
		return	TRUE;

	if(pgl->name)
		g_free(pgl->name);
	pgl->md=NULL;
	
	g_free(pgl);
	pgl=NULL;
	return TRUE;
}
gboolean gl_free_equalizer(_gl_equalizer *eq)
{
	if(!eq)
		return	TRUE;

	if(eq->name)
		g_free(eq->name);
	return TRUE;
}
gboolean gl_free_lyrics(_gl_lyrics *lrc)
{
	if(!lrc)
		return	TRUE;

	if(lrc->text)
		g_free(lrc->text);
	return TRUE;
}
