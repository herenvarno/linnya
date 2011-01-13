


//HEADERS
#include"configure.h"

//FUCTIONS
//*****************************************************************************************************
//*
//*	FUNCTION:		cf_start
//*	DISCRIPTION:	parse the start of xml makeup, serve for func. cf_load
//*
//*****************************************************************************************************
static void cf_start(	GMarkupParseContext *context,
						const gchar *element_name,
						const gchar **attribute_names,
						const gchar **attribute_values,
						gpointer data,
						GError **error)
{
	gl_conf->element=(gchar*)element_name;
}

//*****************************************************************************************************
//*
//*	FUNCTION:	conf_text
//*	DISCRIPTION:	parse the text of xml makeup, serve for func. conf_load
//*
//*****************************************************************************************************
static void cf_text(	GMarkupParseContext *context,
						const gchar *text,
						gsize text_len,
						gpointer data,
						GError **error)
{
	gchar *p=NULL;

	//basic setting
	if (g_strcmp0 (gl_conf->element, "version") == 0)
		gl_conf->version=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "random") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->random));
		}
		else
			gl_conf->random=0;
	}
	if (g_strcmp0 (gl_conf->element, "single") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->single));
		}
		else
			gl_conf->single=0;
	}
	if (g_strcmp0 (gl_conf->element, "volume") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->volume));

		}
		else
			gl_conf->volume=30;
	}
	if (g_strcmp0 (gl_conf->element, "theme") == 0)
		gl_conf->theme=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "equalizer") == 0)
		gl_conf->equalizer=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "autoeq") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->autoeq));
		}
		else
			gl_conf->autoeq=1;
	}
	if (g_strcmp0 (gl_conf->element, "sublib") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->sublib));
		}
		else
			gl_conf->sublib=1;
	}
	if (g_strcmp0 (gl_conf->element, "autocheck") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->autocheck));
		}
		else
			gl_conf->autocheck=0;
	}
	if (g_strcmp0 (gl_conf->element, "encode") == 0)
		gl_conf->encode=g_strconcat(text,NULL);
		
	//last time information
	if (g_strcmp0 (gl_conf->element, "mainwindow") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->mainwindow));
		}
		else
			gl_conf->mainwindow=0;
	}
	if (g_strcmp0 (gl_conf->element, "desktoplyrics") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->desktoplyrics));
		}
		else
			gl_conf->desktoplyrics=0;
	}
	if (g_strcmp0 (gl_conf->element, "desktoplyrics_fix") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->desktoplyrics_fix));
		}
		else
			gl_conf->desktoplyrics_fix=1;
	}
	if (g_strcmp0 (gl_conf->element, "desktoplyrics_x") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->desktoplyrics_x));
		}
		else
			gl_conf->desktoplyrics_x=100;
	}
	if (g_strcmp0 (gl_conf->element, "desktoplyrics_y") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->desktoplyrics_y));
		}
		else
			gl_conf->desktoplyrics_y=30;
	}


	//directory configuration
	if (g_strcmp0 (gl_conf->element, "lib") == 0)
	{
		p=(gchar*)text;
		if(*p=='~')
		{
			p=p+2;
			p=g_strconcat(gl_dir_home,p,NULL);
		}
		gl_conf->lib=g_strconcat(p,NULL);
	}

	//lyrics
	if (g_strcmp0 (gl_conf->element, "lrc_location") == 0)
		gl_conf->lrc_location=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "lrc_download_notify") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->lrc_download_notify));
		}
		else
			gl_conf->lrc_download_notify=0;
	}
	if (g_strcmp0 (gl_conf->element, "custom_lrc") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->custom_lrc));
		}
		else
			gl_conf->custom_lrc=0;
	}
	if (g_strcmp0 (gl_conf->element, "lrcgap") == 0)
	{
		if(text_len)
		{
			sscanf(text,"%d",&(gl_conf->lrcgap));
		}
		else
			gl_conf->lrcgap=0;
	}
	if (g_strcmp0 (gl_conf->element, "font_highlight") == 0)
		gl_conf->font_highlight=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "font_normal") == 0)
		gl_conf->font_normal=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "font_desktop") == 0)
		gl_conf->font_desktop=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "color_highlight") == 0)
		gl_conf->color_highlight=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "color_normal") == 0)
		gl_conf->color_normal=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "color_spectrum") == 0)
		gl_conf->color_spectrum=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "color_desktop_bg1") == 0)
		gl_conf->color_desktop_bg1=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "color_desktop_bg2") == 0)
		gl_conf->color_desktop_bg2=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "color_desktop_fg1") == 0)
		gl_conf->color_desktop_fg1=g_strconcat(text,NULL);
	if (g_strcmp0 (gl_conf->element, "color_desktop_fg2") == 0)
		gl_conf->color_desktop_fg2=g_strconcat(text,NULL);
}

//*****************************************************************************************************
//*
//*	FUNCTION:		cf_read
//*	DISCRIPTION:	load the conf file
//*
//*****************************************************************************************************
gboolean cf_read()
{
	gl_conf=(_gl_conf *)g_malloc(sizeof(_gl_conf));
	gl_conf->version=NULL;
	gl_conf->lib=NULL;
	gl_conf->equalizer=NULL;
	gl_conf->encode=NULL;
	gl_conf->theme=NULL;
	gl_conf->lrc_location=NULL;
	gl_conf->font_highlight=NULL;
	gl_conf->font_normal=NULL;
	gl_conf->font_desktop=NULL;
	gl_conf->color_highlight=NULL;
	gl_conf->color_normal=NULL;
	gl_conf->color_spectrum=NULL;
	gl_conf->color_desktop_bg1=NULL;
	gl_conf->color_desktop_bg2=NULL;
	gl_conf->color_desktop_fg1=NULL;
	gl_conf->color_desktop_fg2=NULL;
	
	cf_write_default();
	
	GMarkupParser parser = {.start_element = cf_start,
				.end_element = NULL,
				.text = cf_text,
				.passthrough = NULL,
				.error = NULL
				};
				
	gchar *buf;
	gsize length;
	GMarkupParseContext *context;
	
	gl_firstflag=FALSE;
	
	//使用用户配置
	g_file_get_contents(gl_file_conf2, &buf, &length,NULL);
	context = g_markup_parse_context_new(&parser, 0,NULL, NULL);
	//若出错，使用全局配置,并运行配置向导
	if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
	{
		cf_free();
		gl_firstflag=TRUE;
		//打印错误
		de_print(__DE_ERROR, __FUNCTION__,
				 _("Couldn't load user's configuration file, use global configuration !\n"));
		
		//重新载入gl_conf
		g_file_get_contents(gl_file_conf1, &buf, &length,NULL);
		context = g_markup_parse_context_new(&parser, 0,NULL, NULL);
		
		//若再次出错
		if (g_markup_parse_context_parse(context, buf, length, NULL) == FALSE)
		{
			cf_free();
			//打印错误
			de_print(__DE_ERROR, __FUNCTION__,
					 _("Couldn't load global configuration file, create a new one !\n"));
		
			//写入默认gl_conf
			cf_write_default();
		}
	}
	
	//配置文件版本过低 改成默认gl_conf
	if(gl_conf->version)
	{
		gdouble ver=-1;
		sscanf(gl_conf->version,"%lf",&ver);
		if(ver<__GL_VERSION)
		{
			gl_firstflag=TRUE;
			//打印错误
			de_print(__DE_ERROR, __FUNCTION__,
					 _("The Version of Configuration file is low, Use default preferences !\n"));
			cf_free();
			cf_write_default();
		}
	}
	else
	{
		gl_firstflag=TRUE;
		//打印错误
		de_print(__DE_ERROR, __FUNCTION__,
				 _("The Version of Configuration file is low, Use default preferences !\n"));
		cf_free();
		cf_write_default();
	}
	
	g_markup_parse_context_free(context);
	return TRUE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		cf_write
//*	DISCRIPTION:	write the configure to file
//*
//*****************************************************************************************************
gboolean cf_write()
{
	gchar *line=NULL;

	FILE *fp;
	fp=fopen(gl_file_conf2,"w+");
	if(fp==NULL)
	{
		de_print(__DE_ERROR, __FUNCTION__, _("Fail to write configuration file\n"));
		return FALSE;
	}
	
	line=g_markup_printf_escaped(
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<configuration>\n\
\n\
\t<basic>\n\
\t\t<version>%s</version>\n\
\t</basic>\n\
\n\
\t<library>\n\
\t\t<lib>%s</lib>\n\
\t\t<autocheck>%d</autocheck>\n\
\t\t<sublib>%d</sublib>\n\
\t</library>\n\
\n\
\t<play>\n\
\t\t<volume>%d</volume>\n\
\t\t<random>%d</random>\n\
\t\t<single>%d</single>\n\
\t\t<equalizer>%s</equalizer>\n\
\t\t<autoeq>%d</autoeq>\n\
\t\t<encode>%s</encode>\n\
\t</play>\n\
\n\
\t<style>\n\
\t\t<theme>%s</theme>\n\
\t\t<mainwindow>%d</mainwindow>\n\
\t\t<desktoplyrics>%d</desktoplyrics>\n\
\t\t<desktoplyrics_fix>%d</desktoplyrics_fix>\n\
\t\t<desktoplyrics_x>%d</desktoplyrics_x>\n\
\t\t<desktoplyrics_y>%d</desktoplyrics_y>\n\
\t\t<lrc_location>%s</lrc_location>\n\
\t\t<lrc_download_notify>%d</lrc_download_notify>\n\
\t\t<custom_lrc>%d</custom_lrc>\n\
\t\t<lrcgap>%d</lrcgap>\n\
\t\t<font_highlight>%s</font_highlight>\n\
\t\t<font_normal>%s</font_normal>\n\
\t\t<font_desktop>%s</font_desktop>\n\
\t\t<color_highlight>%s</color_highlight>\n\
\t\t<color_normal>%s</color_normal>\n\
\t\t<color_spectrum>%s</color_spectrum>\n\
\t\t<color_desktop_bg1>%s</color_desktop_bg1>\n\
\t\t<color_desktop_bg2>%s</color_desktop_bg2>\n\
\t\t<color_desktop_fg1>%s</color_desktop_fg1>\n\
\t\t<color_desktop_fg2>%s</color_desktop_fg2>\n\
\t</style>\n\
</configuration>",
	gl_conf->version,
	gl_conf->lib,
	gl_conf->autocheck,
	gl_conf->sublib,
	gl_conf->volume,
	gl_conf->random,
	gl_conf->single,
	gl_conf->equalizer,
	gl_conf->autoeq,
	gl_conf->encode,
	gl_conf->theme,
	gl_conf->mainwindow,
	gl_conf->desktoplyrics,
	gl_conf->desktoplyrics_fix,
	gl_conf->desktoplyrics_x,
	gl_conf->desktoplyrics_y,
	gl_conf->lrc_location,
	gl_conf->lrc_download_notify,
	gl_conf->custom_lrc,
	gl_conf->lrcgap,
	gl_conf->font_highlight,
	gl_conf->font_normal,
	gl_conf->font_desktop,
	gl_conf->color_highlight,
	gl_conf->color_normal,
	gl_conf->color_spectrum,
	gl_conf->color_desktop_bg1,
	gl_conf->color_desktop_bg2,
	gl_conf->color_desktop_fg1,
	gl_conf->color_desktop_fg2
	);
	
	fputs(line,fp);
	g_free(line);
	fclose(fp);
	return TRUE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:	conf_write_default
//*	DISCRIPTION:	write the default configure to file.
//*
//*****************************************************************************************************
gboolean cf_write_default()
{
	cf_free();
	gl_conf->version=g_strconcat("1.0",NULL);
	
	gl_conf->lib=g_strconcat(gl_dir_home,"music/",NULL);
	gl_conf->autocheck=1;
	gl_conf->sublib=1;

	
	gl_conf->random=0;
	gl_conf->single=0;
	gl_conf->volume=30;
	gl_conf->equalizer=g_strconcat("default",NULL);
	gl_conf->autoeq=1;
	gl_conf->encode=g_strconcat("GB18030",NULL);

	gl_conf->theme=g_strconcat("Aero-Fly",NULL);
	gl_conf->mainwindow=1;
	gl_conf->desktoplyrics=1;
	gl_conf->desktoplyrics_fix=1;
	gl_conf->desktoplyrics_x=100;
	gl_conf->desktoplyrics_y=30;
	gl_conf->lrc_location=g_strconcat("./",NULL);
	gl_conf->lrc_download_notify=0;
	gl_conf->custom_lrc=0;
	gl_conf->lrcgap=16;
	gl_conf->font_highlight=g_strconcat("Sans Regular 10",NULL);
	gl_conf->font_normal=g_strconcat("Sans Regular 10",NULL);
	gl_conf->font_desktop=g_strconcat("Sans Regular 25",NULL);
	gl_conf->color_highlight=g_strconcat("25523:0:65535",NULL);
	gl_conf->color_normal=g_strconcat("14270:14270:14270",NULL);
	gl_conf->color_spectrum=g_strconcat("65535:65535:65535",NULL);
	gl_conf->color_desktop_bg1=g_strconcat("65535:10449:0",NULL);
	gl_conf->color_desktop_bg2=g_strconcat("65535:61062:0",NULL);
	gl_conf->color_desktop_fg1=g_strconcat("19532:65535:65535",NULL);
	gl_conf->color_desktop_fg2=g_strconcat("5539:0:65535",NULL);

	return TRUE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:	conf_write_default
//*	DISCRIPTION:	write the default configure to file.
//*
//*****************************************************************************************************
gboolean cf_free()
{
	if(!gl_conf)
		return TRUE;
	
	if(gl_conf->version)
		g_free(gl_conf->version);
	if(gl_conf->lib)
		g_free(gl_conf->lib);
	if(gl_conf->equalizer)
		g_free(gl_conf->equalizer);
	if(gl_conf->encode)
		g_free(gl_conf->encode);
	if(gl_conf->theme)
		g_free(gl_conf->theme);
	if(gl_conf->lrc_location)
		g_free(gl_conf->lrc_location);
	if(gl_conf->font_highlight)
		g_free(gl_conf->font_highlight);
	if(gl_conf->font_normal)
		g_free(gl_conf->font_normal);

	if(gl_conf->font_desktop)
		g_free(gl_conf->font_desktop);
	if(gl_conf->color_highlight)
		g_free(gl_conf->color_highlight);
	if(gl_conf->color_normal)
		g_free(gl_conf->color_normal);
	if(gl_conf->color_spectrum)
		g_free(gl_conf->color_spectrum);
	if(gl_conf->color_desktop_bg1)
		g_free(gl_conf->color_desktop_bg1);
	if(gl_conf->color_desktop_bg2)
		g_free(gl_conf->color_desktop_bg2);	
	if(gl_conf->color_desktop_fg1)
		g_free(gl_conf->color_desktop_fg1);
	if(gl_conf->color_desktop_fg2)
		g_free(gl_conf->color_desktop_fg2);

	gl_conf->version=NULL;
	gl_conf->lib=NULL;
	gl_conf->equalizer=NULL;
	gl_conf->encode=NULL;
	gl_conf->theme=NULL;
	gl_conf->lrc_location=NULL;
	gl_conf->font_highlight=NULL;
	gl_conf->font_normal=NULL;
	gl_conf->font_desktop=NULL;
	gl_conf->color_highlight=NULL;
	gl_conf->color_normal=NULL;
	gl_conf->color_spectrum=NULL;
	gl_conf->color_desktop_bg1=NULL;
	gl_conf->color_desktop_bg2=NULL;
	gl_conf->color_desktop_fg1=NULL;
	gl_conf->color_desktop_fg2=NULL;
	return TRUE;
}
