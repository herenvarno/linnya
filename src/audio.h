#ifndef AUDIO_H
#define AUDIO_H

#include "global.h"
#include "msg.h"
#include "db.h"

/* *
 *  MICRO
 * */

/* *
 *  STRUCT [PRIVATE]
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
	gchar *track;
	gchar *genre;
	gchar *comment;
	gchar *start;
	gchar *duration;
	gchar *uri;
	gint tmpflag;
	gint cueflag;

}ly_type_audio_playmeta;

/* *
 *  VARS[PRIVATE]
 * */
ly_type_audio_core     audio_core;
ly_type_audio_playmod  audio_playmod;
ly_type_audio_playmeta audio_playmeta;

/* *
 *  FUNC [PUBLIC]
 * */



#endif
