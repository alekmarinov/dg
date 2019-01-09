/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      winxp_list.c                                       */
/* Description:   List with Windows XP look & feel                   */
/*                                                                   */
/*********************************************************************/

#include <string.h>
#include <dg/dg_rect.h>
#include <dg/dg_utf8.h>
#include <dg/dgw_draw.h>
#include "winxp_list.h"
#include "winxp_draw.h"

static const int border_size               = 1;
static const dg_color_t border_color       = DG_RGB_COLOR(61, 149, 255, 255);
static const dg_color_t back_color         = DG_RGB_COLOR(0, 84, 227, 255);
static const dg_color_t select_back_color  = DG_RGB_COLOR(255, 139, 0, 255);
static const char* list_font_face          = "Arial Bold";
static const int list_font_size            = 14;
static const dg_color_t list_font_color    = DG_RGB_COLOR(255, 255, 255, 192);

void winxp_render_list(dg_widget_p widget)
{
	dg_event_t event;
	dgw_list_p self = (dgw_list_p)widget;
	dg_gdc_p    gdc;
	dg_window_p window = widget->window;
	int         width  = DGWidgetWidth(widget);
	int         height = DGWidgetHeight(widget);
	int         i, pen_x, pen_y, inc_y;
	int         max_visible_count;
	int         is_focus = widget->is_focus(widget);
	int         bsize  = is_focus * border_size;
	dg_color_t  font_color = list_font_color;
	int         visible_row_count = self->get_visible_row_count(self);
	int         nrows = self->get_row_count(self);

	event.type = EVT_TYPE_WIDGET;
	event.e.widget.widget_event = EVT_WIDGET_RENDER;
	event.e.widget.p1=event.e.widget.p2=0;

	gdc = widget->create_gdc(widget);
	gdc->set_antialiaze(gdc, 1);
	window->client_left   = bsize;
	window->client_top    = bsize;
	window->client_right  = bsize;
	window->client_bottom = bsize;

	/* draw WinXP styled list */
	winxp_draw_frame(gdc, 0, 0, width, height, self->options, bsize, border_color, back_color, dg_pen_create(bsize, border_color, PEN_DASHED), 0);

	pen_y = border_size + self->padding;
	inc_y = self->row_height + self->spacing + self->padding;
    
	if (DGWidgetVisible(self->vscroll))
	{
		gdc->set_clip_rect(gdc, border_size, pen_y, width - (border_size << 2) - DGWidgetWidth(self->vscroll), height - pen_y);
	}
	else
	{
		gdc->set_clip_rect(gdc, border_size, pen_y, width - (border_size << 2), height - pen_y);
	}
	
	if (is_focus)
	{
		DG_SET_ALPHA(font_color, 220);
	}
	
	max_visible_count = MIN(visible_row_count, nrows - self->get_scroll(self));

	gdc->set_pen(gdc, dg_pen_create(1, font_color, PEN_SOLID));	
	gdc->set_font(gdc, list_font_face, list_font_size);	
	gdc->set_align(gdc, ALIGN_LEFT_CENTER);
	for (i = 0; i < max_visible_count; i++)
	{
		const dgw_list_row_p row = self->get_row(self, i + self->scroll);
		if (!row->is_separator)
		{
			int col, ncols = self->get_column_count(self);
			if (self->is_cursor_visible && (self->cursor == (i + self->scroll)))
			{
				/* draw selection bar */
				if (is_focus)
				{
					gdc->set_brush(gdc, dg_brush_create(select_back_color, BRUSH_SOLID));
					gdc->draw_fillrect(gdc, border_size, pen_y, width - (border_size << 2), inc_y);
					gdc->set_pen(gdc, dg_pen_create(1, font_color, PEN_SOLID));
				}
				else
				{
					//dg_font_p font = gdc->get_font(gdc);
					//int text_width = 1 + font->get_text_width(font, item->text, utf8_strlen(item->text));
					dg_color_t rect_color = select_back_color;
					DG_SET_ALPHA(rect_color, 128);
					gdc->set_pen(gdc, dg_pen_create(1, rect_color, PEN_DASHED));
					gdc->draw_rect(gdc, border_size, pen_y, width - (border_size << 2) - border_size, inc_y);
					//gdc->draw_rect(gdc, border_size, pen_y, text_width, inc_y);
					gdc->set_pen(gdc, dg_pen_create(1, font_color, PEN_SOLID));
				}
			}

			gdc->set_brush(gdc, 0);

			pen_x = border_size;
			for (col = 0; col < ncols; col++)
			{
				if (self->cols[col].column_type == LIST_COLUMN_TEXT)
					gdc->draw_text_rect(gdc, pen_x, pen_y, width - pen_x - border_size, inc_y, row->cells[col].text, utf8_strlen(row->cells[col].text));
				else if (self->cols[col].column_type == LIST_COLUMN_IMAGE)
					gdc->draw_image_rect(gdc, pen_x, pen_y, width - pen_x - border_size, inc_y, 
										 self->image_list->get(self->image_list, row->cells[col].image));

				pen_x += self->cols[col].width;
			}
		}

		pen_y += inc_y;
	}

	gdc->destroy(gdc);
	widget->event_manager->trigger_event(widget->event_manager, &event);
}
