/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      winxp_button.c                                      */
/* Description:   Label with Windows XP look & feel                  */
/*                                                                   */
/*********************************************************************/

#include "winxp_button.h"
#include "winxp_draw.h"

extern dgw_skin_t _dgw_skin_entries[SKIN_ITEM_LAST];

static const int border_size               = 2;
static const dg_color_t border_color       = DG_RGB_COLOR(0, 0, 128, 255);
static const dg_color_t back_color         = DG_RGB_COLOR(0, 84, 227, 255);
static const char* button_font_face         = "Arial";
static const int button_font_size           = 10;
static const dg_color_t button_font_color   = DG_RGB_COLOR(255, 255, 255, 192);

void winxp_render_button(dg_widget_p widget)
{
	dg_event_t event;
	dgw_button_p self = (dgw_button_p)widget;
	dg_gdc_p    gdc    = DGWidgetCreateGDC(self);
	dg_window_p window = DGWidgetWindow(self);
	int         width  = DGWidgetWidth(self);
	int         height = DGWidgetHeight(self);
	int         is_focus = DGWidgetIsFocus(self);

	event.type = EVT_TYPE_WIDGET;
	event.e.widget.widget_event = EVT_WIDGET_RENDER;
	event.e.widget.p1=event.e.widget.p2=0;

	window->client_left   = border_size;
	window->client_top    = border_size;
	window->client_right  = border_size;
	window->client_bottom = border_size;

	winxp_draw_button(gdc, 0, 0, width, height, 
					self->options, 
					self->is_push,
					self->text, 
					button_font_face, 
					button_font_size, 
					button_font_color,
					border_size,
					border_color,
					back_color);

	if (is_focus && !(self->is_push))
	{
		gdc->set_pen(gdc, dg_pen_create(1, border_color, PEN_DASHED));
		gdc->draw_rect(gdc, border_size, border_size, width - (border_size << 1), height - (border_size << 1));
	}

	gdc->destroy(gdc);
	widget->event_manager->trigger_event(widget->event_manager, &event);
}
