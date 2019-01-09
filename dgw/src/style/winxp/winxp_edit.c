/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      winxp_edit.c                                      */
/* Description:   Label with Windows XP look & feel                  */
/*                                                                   */
/*********************************************************************/

#include <string.h>
#include <dg/dg_utf8.h>
#include "winxp_edit.h"
#include "winxp_draw.h"

extern dgw_skin_t _dgw_skin_entries[SKIN_ITEM_LAST];

static const int border_size               = 2;
static const dg_color_t border_color       = DG_RGB_COLOR(0, 0, 128, 255);
static const dg_color_t back_color         = DG_RGB_COLOR(0, 84, 227, 255);
static const char* edit_font_face         = "Arial";
static const int edit_font_size           = 20;
static const dg_color_t edit_font_color   = DG_RGB_COLOR(255, 255, 255, 192);

void winxp_render_edit(dg_widget_p widget)
{
	dg_event_t event;
	dgw_edit_p self = (dgw_edit_p)widget;
	dg_gdc_p    gdc    = widget->create_gdc(widget);
	dg_window_p window = widget->window;
	int         width  = DGWidgetWidth(widget);
	int         height = DGWidgetHeight(widget);
	int         is_focus = widget->is_focus(widget);
	dg_font_p   font;
	int         cursor_x, cursor_offset;
	const char* text;
	int         visible_width = width - DGWidgetWidth(self->cursor) - 2 * border_size;

	event.type = EVT_TYPE_WIDGET;
	event.e.widget.widget_event = EVT_WIDGET_RENDER;
	event.e.widget.p1=event.e.widget.p2=0;

	gdc->set_font(gdc, edit_font_face, edit_font_size);
	font = gdc->get_font(gdc);

	window->client_left   = border_size;
	window->client_top    = border_size;
	window->client_right  = border_size;
	window->client_bottom = border_size;

	text = self->get_text(self);

	if (self->scroll > self->position)
		self->scroll = self->position;

	while (font->get_text_width(font, utf8_strpos(text, self->scroll), self->position - self->scroll) > visible_width)
		self->scroll++;

	if (self->position < utf8_strlen(text) - 1)
	{
		if ( visible_width - 
			font->get_text_width(font, utf8_strpos(text, self->scroll), self->position - self->scroll) < 
			font->get_text_width(font, utf8_strpos(text, self->position + 1), 1) / 2)
		{
			self->scroll++;
		}
		cursor_offset = 0;
	}
	else
		cursor_offset = 1;

	cursor_x = cursor_offset + 
			   font->get_text_width(font, utf8_strpos(text, self->scroll), self->position - self->scroll);

	winxp_draw_edit(gdc, 0, 0, width, height, 
					self->options,
					self->scroll,
					self->text, 
					edit_font_face, 
					edit_font_size, 
					edit_font_color,
					border_size,
					border_color,
					back_color);

	self->cursor->set_rect(self->cursor, cursor_x, self->cursor->window->rect->y, self->cursor->window->rect->w, self->cursor->window->rect->h, 0);

	if (is_focus)
	{
		gdc->set_pen(gdc, dg_pen_create(1, border_color, PEN_DASHED));
		gdc->draw_rect(gdc, border_size, border_size, width - (border_size << 1), height - (border_size << 1));
	}

	gdc->destroy(gdc);
	widget->event_manager->trigger_event(widget->event_manager, &event);
}
