

#ifndef CONNECT_H
#define CONNECT_H

//HEADERS
#include "global.h"
#include "debug.h"
#include "common.h"
#include "configure.h"
#include "metadata.h"
#include "library.h"
#include "playlist.h"
#include "core.h"
#include "theme.h"
#include "gui_window.h"
#include "gui_treeview.h"
#include "gui_lrc.h"
#include "gui_dialog.h"


//FUNCTIONS
gboolean	con_connect_signal();
gboolean	timeout();

#endif
