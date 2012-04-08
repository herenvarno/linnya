/*
 * cfg.h
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


#ifndef LY_CFG_H
#define LY_CFG_H

/*
 * HEADERS
 */
#include "glb.h"
#include "dbg.h"
#include "gud.h"
#include "key.h"
#include "thm.h"
#include "win.h"
#include "pli.h"
#include "sss.h"

/*
 * MACROS
 */
#define LY_CFG_DIALOG_TYPE	(ly_cfg_dialog_get_type())
#define LY_CFG_DIALOG(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), LY_CFG_DIALOG_TYPE, LyCfgDialog))
#define LY_CFG_PAGE_TYPE	(ly_cfg_page_get_type())
#define LY_CFG_PAGE(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), LY_CFG_PAGE_TYPE, LyCfgPage))
#define LY_CFG_ITEM_TYPE 	(ly_cfg_item_get_type())
#define LY_CFG_ITEM(obj) 	(G_TYPE_CHECK_INSTANCE_CAST((obj), LY_CFG_ITEM_TYPE, LyCfgItem))

/*
 * TYPES
 */
/*
 * class define: LyCfgDialog
 */
typedef struct _LyCfgDialog LyCfgDialog;
typedef struct _LyCfgDialogClass LyCfgDialogClass;
 
struct _LyCfgDialog {
 GtkDialog	parent;		/* parent type is GtkDialog */
 GtkWidget *navagation_area; /* GtkTreeView type widget for navagation */
 gboolean navagation_on;/* Flag for navagation on or off */
 GtkWidget *content_area;	/* GtkNotebook type widget for pages */
};
struct _LyCfgDialogClass {
 GtkDialogClass parent_class;
};

/*
 * class define: LyCfgPage
 */
typedef struct _LyCfgPage LyCfgPage;
typedef struct _LyCfgPageClass LyCfgPageClass;
 
struct _LyCfgPage {
	GtkVBox parent;		/* parent type is GtkVbox */
	GtkWidget *label;
	GtkWidget *image;
	GtkWidget *content_area;
};
struct _LyCfgPageClass {
 GtkVBoxClass parent_class;
};

/*
 * class define: LyCfgPage
 */
typedef struct _LyCfgItem LyCfgItem;
typedef struct _LyCfgItemClass LyCfgItemClass;
 
struct _LyCfgItem {
	GtkVBox parent;		/* parent type is GtkVbox */
	gchar name[128];
	GtkWidget *content_area;
};
struct _LyCfgItemClass {
 GtkVBoxClass parent_class;
};



/*
 * FUNCTIONS
 */
void			ly_cfg_init					();
void			ly_cfg_fina					();

GType			ly_cfg_dialog_get_type		();
GtkWidget*		ly_cfg_dialog_new			(gchar *name);
void			ly_cfg_dialog_append		(LyCfgDialog *dialog, GtkWidget *widget);
void			ly_cfg_dialog_set_nav_visible		(LyCfgDialog *dialog, gboolean visible);

GType			ly_cfg_page_get_type		();
GtkWidget*		ly_cfg_page_new				(gchar *name, gchar *desc, gchar *image);
gchar*			ly_cfg_page_get_name		(LyCfgPage *page);
void			ly_cfg_page_append			(LyCfgPage *page, GtkWidget *widget);
GtkWidget*		ly_cfg_page_get_content_area		(LyCfgPage *page);

GType			ly_cfg_item_get_type		();
GtkWidget*		ly_cfg_item_new				(gchar *name);
gchar*			ly_cfg_item_get_name		(LyCfgItem *item);
void			ly_cfg_item_append			(LyCfgItem *item, GtkWidget *widget);
GtkWidget*		ly_cfg_item_get_content_area		(LyCfgItem *item);
 
#endif	//LY_CFG_H
