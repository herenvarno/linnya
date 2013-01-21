/*
 * cfg.obj.c
 * This file is part of linnya
 *
 * Copyright (C) 2010-2012 - Edward Yang
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
#include "cfg.obj.h"

/*
 * FUNCTIONS
 */

static void	ly_cfg_dialog_init			(LyCfgDialog *obj);
static void	ly_cfg_dialog_class_init	(LyCfgDialogClass *klass);
gboolean	ly_cfg_dialog_on_selection_changed_cb(GtkTreeSelection *selection, gpointer data);
static void	ly_cfg_page_init			(LyCfgPage *obj);
static void	ly_cfg_page_class_init		(LyCfgPageClass *klass);
static void	ly_cfg_item_init			(LyCfgItem *obj);
static void	ly_cfg_item_class_init		(LyCfgItemClass *klass);
gboolean 	ly_cfg_item_on_expose_cb	(GtkWidget *widget, cairo_t *cr, gpointer data);

GType ly_cfg_dialog_get_type()
{
	static GType ly_cfg_dialog_type = 0;
	if(!ly_cfg_dialog_type)
	{
		static const GTypeInfo ly_cfg_dialog_info = {
			sizeof(LyCfgDialogClass),
			NULL,
			NULL,
			(GClassInitFunc)ly_cfg_dialog_class_init,
			NULL,
			NULL,
			sizeof(LyCfgDialog),
			0,
			(GInstanceInitFunc)ly_cfg_dialog_init,
			NULL
		};
		ly_cfg_dialog_type = g_type_register_static(GTK_TYPE_DIALOG,"LyCfgDialog",&ly_cfg_dialog_info,0);
	}
	return ly_cfg_dialog_type;
}
static void ly_cfg_dialog_init(LyCfgDialog *obj)
{
	gtk_window_set_title(GTK_WINDOW(obj), _("Preference"));
	gtk_window_set_default_size(GTK_WINDOW(obj),600,500);

	GtkWidget *hpaned=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_container_set_border_width(GTK_CONTAINER(hpaned), 10);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(obj))),hpaned,TRUE,TRUE,0);

	obj->navagation_area=gtk_tree_view_new();
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(obj->navagation_area),FALSE);
	gtk_container_add(GTK_CONTAINER(hpaned), obj->navagation_area);

	GtkTreeStore *store= gtk_tree_store_new (2,G_TYPE_INT, G_TYPE_STRING);
	GtkCellRenderer *cell_renderer = gtk_cell_renderer_text_new ();
	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes( "Options", cell_renderer,"text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (obj->navagation_area), column);
	gtk_tree_view_set_model(GTK_TREE_VIEW (obj->navagation_area), GTK_TREE_MODEL(store));
	GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(obj->navagation_area));

	obj->content_area = gtk_notebook_new();
	gtk_notebook_set_show_border(GTK_NOTEBOOK(obj->content_area),FALSE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(obj->content_area),FALSE);
	gtk_container_add(GTK_CONTAINER(hpaned), obj->content_area);

	g_signal_connect(G_OBJECT(selection), "changed", G_CALLBACK(ly_cfg_dialog_on_selection_changed_cb), hpaned);
}
static void ly_cfg_dialog_class_init(LyCfgDialogClass *klass)
{
}
GtkWidget *ly_cfg_dialog_new(gchar *name)
{
	GtkWidget *dialog;
	dialog = g_object_new(LY_CFG_DIALOG_TYPE, NULL);
	if(dialog && name)
	{
		gtk_window_set_title(GTK_WINDOW(dialog), name);
	}
	return dialog;
}
void			ly_cfg_dialog_append		(LyCfgDialog *dialog, GtkWidget *widget)
{
	GtkTreeIter iter;
	gtk_notebook_append_page(GTK_NOTEBOOK(LY_CFG_DIALOG(dialog)->content_area), widget, gtk_label_new(NULL));
	gint id=gtk_notebook_get_n_pages(GTK_NOTEBOOK(LY_CFG_DIALOG(dialog)->content_area));
	GtkTreeStore *store=GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(dialog->navagation_area)));
	gtk_tree_store_append (store, &iter,NULL);
	char *name=ly_cfg_page_get_name(LY_CFG_PAGE(widget));
	gtk_tree_store_set(store, &iter, 0, id-1, 1, name, -1);
	g_free(name);
}

void			ly_cfg_dialog_set_nav_visible		(LyCfgDialog *dialog, gboolean visible)
{
	gtk_widget_set_visible(dialog->navagation_area, visible);
}

gboolean ly_cfg_dialog_on_selection_changed_cb(GtkTreeSelection *selection, gpointer data)
{
	GtkWidget *hpaned=(GtkWidget *)data;
	GtkWidget *notebook=gtk_paned_get_child2(GTK_PANED(hpaned));
	GtkTreeModel *model;
	GtkTreeIter iter;
	gint id=0;
	gtk_tree_selection_get_selected(selection ,&model, &iter);
	gtk_tree_model_get(model, &iter, 0, &id, -1);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), id);
	return FALSE;
}



GType ly_cfg_page_get_type()
{
	static GType ly_cfg_page_type = 0;
	if(!ly_cfg_page_type)
	{
		static const GTypeInfo ly_cfg_page_info = {
			sizeof(LyCfgPageClass),
			NULL,
			NULL,
			(GClassInitFunc)ly_cfg_page_class_init,
			NULL,
			NULL,
			sizeof(LyCfgPage),
			0,
			(GInstanceInitFunc)ly_cfg_page_init,
			NULL
		};
		ly_cfg_page_type = g_type_register_static(GTK_TYPE_VBOX,"LyCfgPage",&ly_cfg_page_info,0);
	}
	return ly_cfg_page_type;
}
static void ly_cfg_page_init(LyCfgPage *obj)
{
	GtkWidget	*hbox;
	GtkWidget	*scrolled_window;

	hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_pack_start(GTK_BOX(obj), hbox, FALSE, FALSE, 0);
	obj->label=gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(obj->label),0.1,0.5);
	gtk_box_pack_start(GTK_BOX(hbox),obj->label,TRUE,TRUE,0);
	obj->image=gtk_image_new();
	gtk_box_pack_end(GTK_BOX(hbox),obj->image,FALSE,FALSE,0);
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(obj),scrolled_window,TRUE,TRUE,0);
	obj->content_area=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_container_set_border_width(GTK_CONTAINER(obj->content_area), 6);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),obj->content_area);
}
static void ly_cfg_page_class_init(LyCfgPageClass *klass)
{
}
GtkWidget *ly_cfg_page_new(gchar *name, gchar *desc, gchar *image)
{
	GtkWidget *page;
	page = g_object_new(LY_CFG_PAGE_TYPE, NULL);
	char str[1024]="";
	g_snprintf(str, sizeof(str), "<b>%s</b> \n%s", name, desc);
	gtk_label_set_markup(GTK_LABEL(LY_CFG_PAGE(page)->label),str);
	gtk_image_set_from_pixbuf(GTK_IMAGE(LY_CFG_PAGE(page)->image), gdk_pixbuf_new_from_file_at_size(image,64,64,NULL));
	return page;
}

gchar*			ly_cfg_page_get_name		(LyCfgPage *page)
{
	const gchar *str=gtk_label_get_text(GTK_LABEL(page->label));
	gchar *name=(gchar *)g_malloc(128*sizeof(gchar));
	sscanf(str, "%128[^\n]\n", name);
	return name;
}

void			ly_cfg_page_append		(LyCfgPage *page, GtkWidget *widget)
{
	gtk_box_pack_start(GTK_BOX(page->content_area), widget, FALSE, FALSE, 0);
}

GtkWidget*			ly_cfg_page_get_content_area		(LyCfgPage *page)
{
	return page->content_area;
}

/*
 * ITEM
 */
GType ly_cfg_item_get_type()
{
	static GType ly_cfg_item_type = 0;
	if(!ly_cfg_item_type)
	{
		static const GTypeInfo ly_cfg_item_info = {
			sizeof(LyCfgItemClass),
			NULL,
			NULL,
			(GClassInitFunc)ly_cfg_item_class_init,
			NULL,
			NULL,
			sizeof(LyCfgItem),
			0,
			(GInstanceInitFunc)ly_cfg_item_init,
			NULL
		};
		ly_cfg_item_type = g_type_register_static(GTK_TYPE_VBOX,"LyCfgItem",&ly_cfg_item_info,0);
	}
	return ly_cfg_item_type;
}
static void ly_cfg_item_init(LyCfgItem *obj)
{
	g_strlcpy(obj->name, "Untitled Item", sizeof(obj->name));
	GtkWidget	*drawing;
	drawing=gtk_drawing_area_new();
	gtk_widget_set_size_request(drawing,-1,30);
	gtk_box_pack_start(GTK_BOX(obj), drawing, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(drawing), "draw", G_CALLBACK(ly_cfg_item_on_expose_cb), obj);
	obj->content_area=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_box_pack_start(GTK_BOX(obj), obj->content_area, FALSE, FALSE, 0);
}

static void ly_cfg_item_class_init(LyCfgItemClass *klass)
{
}
GtkWidget *ly_cfg_item_new(gchar *name)
{
	GtkWidget *item;
	item = g_object_new(LY_CFG_ITEM_TYPE, NULL);
	g_strlcpy(LY_CFG_ITEM(item)->name, name, sizeof(LY_CFG_ITEM(item)->name));
	return item;
}

gchar*			ly_cfg_item_get_name		(LyCfgItem *item)
{
	return g_strconcat(item->name, NULL);
}

void			ly_cfg_item_append		(LyCfgItem *item, GtkWidget *widget)
{
	gtk_box_pack_start(GTK_BOX(item->content_area), widget, FALSE, FALSE, 5);
}

GtkWidget*			ly_cfg_item_get_content_area		(LyCfgItem *item)
{
	return item->content_area;
}

gboolean ly_cfg_item_on_expose_cb( GtkWidget * widget, cairo_t *cr, gpointer data)
{
	gchar *text=LY_CFG_ITEM(data)->name;

	GtkStyleContext *context=gtk_widget_get_style_context(widget);
	gint width=0,height=0,x,y,r;
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	GdkRGBA *color=(GdkRGBA *)g_malloc(sizeof(GdkRGBA));
	gtk_style_context_get_background_color(GTK_STYLE_CONTEXT(context), GTK_STATE_FLAG_FOCUSED, color);
	gdk_cairo_set_source_rgba (cr, color);
	cairo_set_line_width (cr, 2);

	x =8;
	width=width-8;
	y =8;
	width = width-2*x+4;
	height = height-2*y+4;
	r = height / 4.0;

	cairo_move_to (cr, x + r, y);
	cairo_line_to (cr, x + width - r, y);
	cairo_move_to (cr, x + width, y + r);
	cairo_line_to (cr, x + width, y + height - r);
	cairo_move_to (cr, x + width - r, y + height);
	cairo_line_to (cr, x + r, y + height);
	cairo_move_to (cr, x, y + height - r);
	cairo_line_to (cr, x, y + r);
	cairo_arc (cr, x + r, y + r, r, M_PI, 3 * M_PI / 2.0);
	cairo_arc (cr, x + width - r, y + r, r, 3 * M_PI / 2, 2 * M_PI);
	cairo_arc (cr, x + width - r, y + height - r, r, 0, M_PI / 2);
	cairo_arc (cr, x + r, y + height - r, r, M_PI / 2, M_PI);
	cairo_stroke_preserve (cr);
	cairo_fill (cr);

	gtk_style_context_get_color(GTK_STYLE_CONTEXT(context), GTK_STATE_FLAG_NORMAL, color);
	gdk_cairo_set_source_rgba (cr, color);
	cairo_move_to(cr,18,10);
	PangoLayout *layout;
	PangoFontDescription *desc;
	layout = pango_cairo_create_layout (cr);
	pango_layout_set_text (layout, text, -1);
	desc = pango_font_description_from_string ("Sans Regular 9");
	pango_layout_set_font_description (layout, desc);
	pango_font_description_free (desc);
	pango_cairo_update_layout (cr, layout);
	pango_cairo_show_layout (cr, layout);
	g_object_unref (layout);
	g_free(color);

	return TRUE;
}