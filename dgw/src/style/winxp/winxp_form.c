/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      winxp_form.c                                       */
/* Description:   Form with Windows XP look & feel                   */
/*                                                                   */
/*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <string.h>
#include "winxp_form.h"
#include "winxp_draw.h"

extern dgw_skin_t _dgw_skin_entries[SKIN_ITEM_LAST];

/* FIXME: hardcoded */
static const int border_size               = 2;
static const int caption_size              = 25;
static const dg_color_t border_color       = DG_RGB_COLOR(128, 128, 255, 255);
static const dg_color_t back_color         = DG_RGB_COLOR(0, 84, 227, 255);
static const dg_color_t caption_color1     = DG_RGB_COLOR(0, 84, 227, 255);
static const dg_color_t caption_color2     = DG_RGB_COLOR(61, 149, 255, 255);
static const char* caption_font_face       = "Trebuchet MS Bold";
static const int caption_font_size         = 14;
static const dg_color_t caption_font_color = DG_RGB_COLOR(255, 255, 255, 255);

void winxp_render_form(dg_widget_p widget)
{
	dg_event_t event;
	dgw_form_p  self   = (dgw_form_p)widget;
	dg_gdc_p    gdc    = DGWidgetCreateGDC(self);
	dg_window_p window = DGWidgetWindow(self);
	int         width  = DGWidgetWidth(self);
	int         height = DGWidgetHeight(self);

	event.type = EVT_TYPE_WIDGET;
	event.e.widget.widget_event = EVT_WIDGET_RENDER;
	event.e.widget.p1=event.e.widget.p2=0;

	window->client_left   = border_size;
	window->client_top    = caption_size;
	window->client_right  = border_size;
	window->client_bottom = border_size;

	winxp_draw_frame(gdc, 0, 0, width, height, self->options, border_size, border_color, back_color, 0, 0);
	winxp_draw_caption(gdc, border_size, border_size, width-2*border_size, caption_size - border_size, 
					   self->options, self->text, caption_color1, caption_color2, 
					   caption_font_face, caption_font_size, caption_font_color);
	gdc->destroy(gdc);

	widget->event_manager->trigger_event(widget->event_manager, &event);
#if 0
	/* just idea */
	{
		unsigned char* alpha_channel = (unsigned char*)malloc(width * height);
		unsigned char* alpha_line;
		int i;
		double step = 128.0 / height;
		double alpha = 255.0;

		for (alpha_line = alpha_channel, i = 0; i < height; i++, alpha_line += width)
		{
			memset(alpha_line, (int)ceil(alpha), width);
			alpha -= step;
		}
		((dg_widget_p)self)->surface->set_alpha_channel(((dg_widget_p)self)->surface, alpha_channel);
		free(alpha_channel);
	}
#endif
}
