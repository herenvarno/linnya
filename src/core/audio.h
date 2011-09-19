#ifndef AUDIO_H
#define AUDIO_H

#include "global.h"
#include "msg.h"
#include "db.h"

/* *
 *  MICROS
 * */

/* *
 *  DATASTRUCTURE [PRIVATE]
 * */
typedef struct lyAudioCore
{
	GstElement* play;
	GstElement* spectrum;
	GstElement* volume;
	GstElement* equalizer;
}lyAudioCore;

/* *
 *  VARS[PRIVATE]
 * */
lyAudioCore*     ly_audio_core;
lyDbMetadata* ly_audio_meta;

/* *
 *  FUNCS [PUBLIC]
 * */
gboolean ly_audio_set_meta(gint id);
gboolean ly_audio_prev();
gboolean ly_audio_next();
gboolean ly_audio_play();
gboolean ly_audio_pause();
gboolean ly_audio_stop(); 
gboolean ly_audio_set_volume(gdouble volume);
gdouble ly_audio_get_volume();
gboolean ly_audio_set_position(gdouble position);
gdouble ly_audio_get_position();
gint64 ly_audio_get_position_abs();
gboolean ly_audio_set_equalizer(gdouble band0,gdouble band1,gdouble band2,gdouble band3,gdouble band4,gdouble band5,gdouble band6,gdouble band7,gdouble band8,gdouble band9);
gboolean ly_audio_get_equalizer(gdouble* band0,gdouble* band1,gdouble* band2,gdouble* band3,gdouble* band4,gdouble* band5,gdouble* band6,gdouble* band7,gdouble* band8,gdouble* band9);

 /* *
  *  FUNCS [PRIVATE]
  * */
gboolean ly_audio_callback(GstBus *bus,GstMessage *message,gpointer data);
gboolean ly_audio_init(void);
GstState ly_audio_get_state();
gboolean ly_audio_finalize(void);


#endif
