#ifndef AUDIO_H
#define AUDIO_H

#include "global.h"
#include "msg.h"
//#include "db.h"

/* *
 *  MICROS
 * */

/* *
 *  DATASTRUCTURE [PRIVATE]
 * */
typedef struct ly_type_audio_core
{
	GstElement* play;
	GstElement* volumn;
	GstElement* equalizer;
}ly_type_audio_core;

typedef struct ly_type_audio_playmod
{
	gint random;
	gint singleloop;
	gint single;
	gint listloop;
	gint list;
	gint count;

}ly_type_audio_playmod;

typedef struct ly_type_audio_playmeta
{
	gint *id;
	gchar *title;
	gchar *artist;
	gchar *album;
	gchar *codec;
	gchar *start;
	gchar *duration;
	gchar *uri;
	gint cueflag;
	gint tmpflag;

}ly_type_audio_playmeta;

/* *
 *  VARS[PRIVATE]
 * */
ly_type_audio_core*     audio_core;
ly_type_audio_playmod*  audio_playmod;
ly_type_audio_playmeta* audio_meta;

/* *
 *  FUNCS [PUBLIC]
 * */
gboolean ly_func_audio_setrand();
gboolean ly_func_audio_setsingleloop();
gboolean ly_func_audio_setlist();
gboolean ly_func_audio_setsingle();
gboolean ly_func_audio_setlistloop();
gboolean ly_func_audio_previous();
gboolean ly_func_audio_next();
gboolean ly_func_audio_play();
gboolean ly_func_audio_pause();
gboolean ly_func_audio_stop(); 
gboolean ly_func_audio_setvolumn(gdouble pvolumn);
gdouble ly_func_audio_getvolumn();
gboolean ly_func_audio_setposition(gdouble playperc);
gdouble ly_func_audio_getpositionp();
gint ly_func_audio_getpositioni();
gboolean ly_func_audio_setequalizer(gdouble band0,gdouble band1,gdouble band2,gdouble band3,gdouble band4,gdouble band5,gdouble band6,gdouble band7,gdouble band8,gdouble band9);
gboolean ly_func_audio_getequalizer(gdouble* band0,gdouble* band1,gdouble* band2,gdouble* band3,gdouble* band4,gdouble* band5,gdouble* band6,gdouble* band7,gdouble* band8,gdouble* band9);

 /* *
  *  FUNCS [PRIVATE]
  * */
gint ly_func_audio_rand(gint listlength);
gint ly_func_audio_singleloop(gint position);
gint ly_func_audio_nlistloop(gint position,gint listlength);
gint ly_func_audio_plistloop(gint position,gint listlength);
gboolean ly_func_audio_callback(GstBus *bus,GstMessage *message,gpointer data);
gboolean ly_audio_init();
GstState ly_func_audio_getstate();
gboolean ly_audio_finalize();


  


#endif
