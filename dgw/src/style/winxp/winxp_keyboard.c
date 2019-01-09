/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      winxp_keyboard.c                                   */
/* Description:   Keyboard with Windows XP look & feel               */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <dg/dg_utf8.h>
#include <dg/dg_pixel.h>
#include <dg/dgw_draw.h>
#include "winxp_keyboard.h"
#include "winxp_draw.h"

extern dgw_skin_t _dgw_skin_entries[SKIN_ITEM_LAST];

static const int border_size                  = 2;
static const dg_color_t border_color          = DG_RGB_COLOR(0, 0, 128, 255);
static const dg_color_t back_color            = DG_RGB_COLOR(30, 124, 227, 255);
static const char* keyboard_font_face         = "Trebuchet MS Bold Italic";
static const int keyboard_font_size           = 14;
static const dg_color_t keyboard_font_color   = DG_RGB_COLOR(255, 255, 255, 192);

static const dg_color_t key_hilite_color1     = DG_RGB_COLOR(30, 60, 120, 255);
static const dg_color_t key_hilite_color2     = DG_RGB_COLOR(0, 84, 227, 255);
static const dg_color_t key_shadow_color1     = DG_RGB_COLOR(61, 149, 255, 255);
static const dg_color_t key_shadow_color2     = DG_RGB_COLOR(0, 84, 227, 255);

void winxp_render_keyboard_key(dg_widget_p widget)
{
	dg_event_t event;
	dgw_key_p self = (dgw_key_p)widget;
	dg_widget_p parent = DGWidgetParent(widget);
	dgw_keyboard_p keyboard = (dgw_keyboard_p)parent;
	dg_gdc_p    gdc;
	dg_window_p window = widget->window;
	int         width  = DGWidgetWidth(widget);
	int         height = DGWidgetHeight(widget);
	int         is_focus = widget->is_focus(widget);
	int         i, border_size = 3;
	int         back_colors_count = width - 2 * border_size + self->is_push;
	dg_color_p  back_colors = (dg_color_p)malloc(back_colors_count * sizeof(dg_color_t));
	dg_color_p  hilite_colors = (dg_color_p)malloc(border_size * sizeof(dg_color_t));
	dg_color_p  shadow_colors = (dg_color_p)malloc(border_size * sizeof(dg_color_t));
	
	event.type = EVT_TYPE_WIDGET;
	event.e.widget.widget_event = EVT_WIDGET_RENDER;
	event.e.widget.p1=event.e.widget.p2=0;

	gdc = widget->create_gdc(widget);

	if (self->is_push)
		gdc->gradient_colors(gdc, back_color, key_hilite_color2, back_colors, back_colors_count);
	else
		gdc->gradient_colors(gdc, key_hilite_color2, back_color, back_colors, back_colors_count);

	for (i = 0; i < back_colors_count; i++)
	{
		gdc->set_pen(gdc, dg_pen_create(1, back_colors[i], PEN_SOLID));
		gdc->draw_line(gdc, self->is_push + border_size + i, border_size, self->is_push + border_size + i, height);

		if (self->is_push)
			gdc->set_pen(gdc, dg_pen_create(1, key_shadow_color2, PEN_SOLID));
		else
			gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_BLACK, PEN_SOLID));

		if (self->is_push)
			gdc->draw_line(gdc, border_size + i, border_size, border_size + i, border_size + 1);
		else
			gdc->draw_line(gdc, border_size + i, height - border_size + 1, border_size + i, height - border_size + 1);
	}

	if (self->is_push)
		gdc->set_brush(gdc, dg_brush_create(key_shadow_color2, BRUSH_SOLID));
	else
		gdc->set_brush(gdc, dg_brush_create(DG_COLOR_BLACK, BRUSH_SOLID));

	if (self->is_push)
	{
		gdc->draw_fillrect(gdc, border_size, border_size, 1, height - border_size);
	}
	else
	{
		gdc->draw_fillrect(gdc, width - border_size, border_size, border_size, height - border_size);
	}

	gdc->gradient_colors(gdc, key_hilite_color1, key_hilite_color2, hilite_colors, border_size);
	gdc->gradient_colors(gdc, key_shadow_color1, key_shadow_color2, shadow_colors, border_size);
	for (i = 0; i < border_size; i++)
	{
		if (self->is_push)
			dgw_draw_groove_rectangle(gdc, i, i, width - i, height - i, hilite_colors[i], shadow_colors[i]);
		else
			dgw_draw_raised_rectangle(gdc, i, i, width - i, height - i, hilite_colors[i], shadow_colors[i]);
	}
	free(hilite_colors);
	free(shadow_colors);
	free(back_colors);

	if (self->key < KEYB_COUNT)
	{
		gdc->set_antialiaze(gdc, 1);
		gdc->set_brush(gdc, dg_brush_create(keyboard_font_color, BRUSH_SOLID));
		switch (self->key)
		{
		case KEYB_BACKSPACE:
		{
			int points[6];
			points[0] = self->is_push + width / 4;                  /* x1 */
			points[1] = self->is_push + (height >> 1);                /* y1 */
			points[2] = points[0] + width / 6;      /* x2 */
			points[3] = self->is_push + (height >> 1) - height / 6; /* y2 */
			points[4] = points[0] + width / 6;      /* x3 */
			points[5] = self->is_push + (height >> 1) + height / 6; /* y3 */
			gdc->draw_fillpolygon(gdc, points, 3);
			gdc->draw_fillrect(gdc, self->is_push + width / 4 + width / 6, self->is_push + (height >> 1) - height / 12,
							   width / 3, height / 6);
		}
		break;
		case KEYB_SHIFT:
		{
			int points[6];
			points[0] = self->is_push + (width >> 1);                 /* x1 */
			points[1] = self->is_push + height / 4;                 /* y1 */
			points[2] = points[0] + height / 4;      /* x2 */
			points[3] = points[1] + height / 6; /* y2 */
			points[4] = points[0] - height / 4 + height / 24;      /* x3 */
			points[5] = points[1] + height / 6; /* y3 */
			gdc->draw_fillpolygon(gdc, points, 3);
			gdc->draw_fillrect(gdc, points[0] - height / 12, points[3],
							   height / 4, height / 3);
		}
		break;
		case KEYB_LANGUAGE:
		{
			const char* lang_id = keyboard->get_language_id(keyboard);
			gdc->set_brush(gdc, 0);
			gdc->set_font(gdc, keyboard_font_face, keyboard_font_size);
			gdc->set_pen(gdc, dg_pen_create(1, keyboard_font_color, PEN_SOLID));
			gdc->set_align(gdc, ALIGN_CENTER);
			gdc->draw_text_rect(gdc, self->is_push, self->is_push, width - self->is_push, height - self->is_push, lang_id, utf8_strlen(lang_id));
		}
		break;
		case KEYB_SPACE:
		{
			const char* space_id = keyboard->get_space_id(keyboard);
			gdc->set_brush(gdc, 0);
			gdc->set_font(gdc, keyboard_font_face, keyboard_font_size);
			gdc->set_pen(gdc, dg_pen_create(1, keyboard_font_color, PEN_SOLID));
			gdc->set_align(gdc, ALIGN_CENTER);
			gdc->draw_text_rect(gdc, self->is_push, self->is_push, width - self->is_push, height - self->is_push, space_id, utf8_strlen(space_id));
		}
		break;
		case KEYB_ENTER:
		{
			int points[6];
			points[0] = self->is_push + width / 4;                  /* x1 */
			points[1] = self->is_push + (height >> 1);                /* y1 */
			points[2] = points[0] + width / 6;      /* x2 */
			points[3] = self->is_push + (height >> 1) - height / 6; /* y2 */
			points[4] = points[0] + width / 6;      /* x3 */
			points[5] = self->is_push + (height >> 1) + height / 6; /* y3 */
			gdc->draw_fillpolygon(gdc, points, 3);
			gdc->draw_fillrect(gdc, self->is_push + width / 4 + width / 6, self->is_push + (height >> 1) - height / 12,
							   width / 3, height / 6);
			gdc->draw_fillrect(gdc, self->is_push + width / 4 + width / 6 + width / 3 - height / 6, self->is_push + (height >> 1) - height / 12 - height / 6,
							   height / 6, height / 6);
		}
		break;
		case KEYB_ESC:
		{
			const char* esc_id = keyboard->get_esc_id(keyboard);
			gdc->set_brush(gdc, 0);
			gdc->set_font(gdc, keyboard_font_face, keyboard_font_size);
			gdc->set_pen(gdc, dg_pen_create(1, keyboard_font_color, PEN_SOLID));
			gdc->set_align(gdc, ALIGN_CENTER);
			gdc->draw_text_rect(gdc, self->is_push, self->is_push, width, height, esc_id, utf8_strlen(esc_id));
		}
		break;
		}
	}
	else
	{
		gdc->set_brush(gdc, 0);
		gdc->set_font(gdc, keyboard_font_face, keyboard_font_size);
		gdc->set_pen(gdc, dg_pen_create(1, keyboard_font_color, PEN_SOLID));
		gdc->set_align(gdc, ALIGN_CENTER);
		gdc->draw_text_rect(gdc, self->is_push, self->is_push, width, height, utf8_strpos(self->text, keyboard->is_caps(keyboard)), 1);
	}

	if (self->is_selected)
	{
		
		dg_color_t selection_color = DG_RGB_COLOR(255-DG_GET_RED(border_color), 255-DG_GET_GREEN(border_color), 255-DG_GET_BLUE(border_color), DG_GET_ALPHA(border_color));
		dgw_draw_border_rectangle(gdc, 1, 1, width-3, height-2, selection_color);
	}
	
	gdc->destroy(gdc);
	widget->event_manager->trigger_event(widget->event_manager, &event);
}

void winxp_render_keyboard(dg_widget_p widget)
{
	dg_event_t event;
	dgw_keyboard_p self = (dgw_keyboard_p)widget;
	dg_gdc_p    gdc;
	dg_window_p window = widget->window;
	int         width  = DGWidgetWidth(widget);
	int         height = DGWidgetHeight(widget);
	int         is_focus = widget->is_focus(widget);

	event.type = EVT_TYPE_WIDGET;
	event.e.widget.widget_event = EVT_WIDGET_RENDER;
	event.e.widget.p1=event.e.widget.p2=0;

	widget->surface->set_alpha(widget->surface, 164);

	gdc = widget->create_gdc(widget);

	window->client_left   = border_size;
	window->client_top    = border_size;
	window->client_right  = border_size;
	window->client_bottom = border_size;

	winxp_draw_frame(gdc, 0, 0, width, height, self->options, border_size, border_color, back_color, 0, 0);

	if (is_focus)
	{
		gdc->set_pen(gdc, dg_pen_create(1, border_color, PEN_DASHED));
		gdc->draw_rect(gdc, border_size, border_size, width - (border_size << 1), height - (border_size << 1));
	}

	gdc->destroy(gdc);
	widget->event_manager->trigger_event(widget->event_manager, &event);
}
