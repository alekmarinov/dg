/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      winxp_draw.c                                       */
/* Description:   Common render functions for Windows XP look & feel */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <dg/dgw_draw.h>
#include <dg/dg_utf8.h>
#include "winxp_draw.h"

void winxp_draw_caption(dg_gdc_p gdc, int x, int y, int width, int height, int options, 
						const char* text, dg_color_t color1, dg_color_t color2, 
						const char* font_face, int font_size, dg_color_t font_color)
{
	int size, i;
	dg_color_p colors;

	size = MIN(width, height) >> 1;
	colors = (dg_color_p)malloc(sizeof(dg_color_t) * (size + 2));

	gdc->gradient_colors(gdc, color2, color1, colors, size + 2);
	for (i = 0; i < size; i++)
	{
		gdc->set_pen(gdc, dg_pen_create(1, colors[1+i], BRUSH_SOLID));
        gdc->draw_rect(gdc, x + size - i - 1, y + size - i - 1, width - 2 * (size - i - 1), height - 2 * (size - i - 1));
	}
	free(colors);
	
	/* draw caption text */
	gdc->set_font(gdc, font_face, font_size);
	gdc->set_brush(gdc, 0);

	gdc->set_align(gdc, ALIGN_CENTER);
	gdc->set_pen(gdc, dg_pen_create(1, font_color, PEN_SOLID));
	gdc->draw_text_rect(gdc, x, y, width, height, text, (int)utf8_strlen(text));
}

void winxp_draw_frame(dg_gdc_p gdc, int x, int y, int width, int height, int options, 
					  int border_size, dg_color_t border_color, dg_color_t back_color, 
					  dg_pen_p pen, dg_brush_p brush)
{
	gdc->set_brush(gdc, brush?brush:dg_brush_create(back_color, BRUSH_SOLID));
	gdc->draw_fillrect(gdc, x, y, width, height);

	gdc->set_pen(gdc, pen?pen:dg_pen_create(border_size, border_color, PEN_SOLID));
	gdc->draw_rect(gdc, x, y, width, height);
}

void winxp_draw_label(dg_gdc_p gdc, int x, int y, int width, int height, int options, const char* text, const char* font_face, int font_size, dg_color_t font_color, int border_size, dg_color_t border_color, dg_color_t back_color)
{
	winxp_draw_frame(gdc, x, y, width, height, options, border_size, border_color, back_color, 0, 0);

	gdc->set_pen(gdc, dg_pen_create(1, font_color, PEN_SOLID));
	gdc->set_brush(gdc, 0);
	gdc->set_font(gdc, font_face, font_size);	
	gdc->set_align(gdc, ALIGN_LEFT_CENTER);
	gdc->draw_text_rect(gdc, border_size, border_size, width - (border_size << 1), height - (border_size << 1), text, (int)utf8_strlen(text));
}

void winxp_draw_button(dg_gdc_p gdc, int x, int y, int width, int height, int options, int is_push, const char* text, const char* font_face, int font_size, dg_color_t font_color, int border_size, dg_color_t border_color, dg_color_t back_color)
{
	int text_x = border_size + is_push;
	int text_y = border_size + is_push;
	dg_color_t hilite_color = DG_RGB_COLOR(255-DG_GET_RED(border_color), 255-DG_GET_GREEN(border_color), 255-DG_GET_BLUE(border_color), DG_GET_ALPHA(border_color));
	dg_color_t base_color = DG_RGB_COLOR( 
		(DG_GET_RED(back_color)   + DG_GET_RED(hilite_color))   >> 1,
		(DG_GET_GREEN(back_color) + DG_GET_GREEN(hilite_color)) >> 1,
		(DG_GET_BLUE(back_color)  + DG_GET_BLUE(hilite_color))  >> 1,
		(DG_GET_ALPHA(back_color) + DG_GET_ALPHA(hilite_color)) >> 1
	);
	dg_color_t shadow_color = DG_RGB_COLOR( 
		(DG_GET_RED(back_color)   + DG_GET_RED(border_color))   >> 1,
		(DG_GET_GREEN(back_color) + DG_GET_GREEN(border_color)) >> 1,
		(DG_GET_BLUE(back_color)  + DG_GET_BLUE(border_color))  >> 1,
		(DG_GET_ALPHA(back_color) + DG_GET_ALPHA(border_color)) >> 1
	);

	/* draw background */
	gdc->set_brush(gdc, dg_brush_create(back_color, BRUSH_SOLID));
	gdc->draw_fillrect(gdc, x, y, width, height);

	/* draw border */
	if (is_push)
	{
		dgw_draw_double_sunken_rectangle(gdc, x, y, width, height, 
										 shadow_color, hilite_color, border_color, base_color);
	}
	else
	{
		dgw_draw_double_raised_rectangle(gdc, x, y, width, height, 
										 shadow_color, hilite_color, border_color, back_color);
	}

	gdc->set_pen(gdc, dg_pen_create(1, font_color, PEN_SOLID));
	gdc->set_brush(gdc, 0);
	gdc->set_font(gdc, font_face, font_size);	
	gdc->set_align(gdc, ALIGN_CENTER);
	gdc->draw_text_rect(gdc, text_x, text_y, width - (border_size << 1), height - (border_size << 1), text, (int)utf8_strlen(text));
}

void winxp_draw_edit(dg_gdc_p gdc, int x, int y, int width, int height, int options, int scroll, const char* text, const char* font_face, int font_size, dg_color_t font_color, int border_size, dg_color_t border_color, dg_color_t back_color)
{
	int text_x = border_size + 1;
	int text_y = border_size + 1;
	dg_color_t hilite_color = DG_RGB_COLOR(255-DG_GET_RED(border_color), 255-DG_GET_GREEN(border_color), 255-DG_GET_BLUE(border_color), DG_GET_ALPHA(border_color));
	dg_color_t base_color = DG_RGB_COLOR( 
		(DG_GET_RED(back_color)   + DG_GET_RED(hilite_color))   >> 1,
		(DG_GET_GREEN(back_color) + DG_GET_GREEN(hilite_color)) >> 1,
		(DG_GET_BLUE(back_color)  + DG_GET_BLUE(hilite_color))  >> 1,
		(DG_GET_ALPHA(back_color) + DG_GET_ALPHA(hilite_color)) >> 1
	);
	dg_color_t shadow_color = DG_RGB_COLOR( 
		(DG_GET_RED(back_color)   + DG_GET_RED(border_color))   >> 1,
		(DG_GET_GREEN(back_color) + DG_GET_GREEN(border_color)) >> 1,
		(DG_GET_BLUE(back_color)  + DG_GET_BLUE(border_color))  >> 1,
		(DG_GET_ALPHA(back_color) + DG_GET_ALPHA(border_color)) >> 1
	);

	/* draw background */
	gdc->set_brush(gdc, dg_brush_create(back_color, BRUSH_SOLID));
	gdc->draw_fillrect(gdc, x, y, width, height);

	dgw_draw_double_sunken_rectangle(gdc, x, y, width, height, 
										shadow_color, hilite_color, border_color, base_color);

	gdc->set_pen(gdc, dg_pen_create(1, font_color, PEN_SOLID));
	gdc->set_brush(gdc, 0);
	gdc->set_font(gdc, font_face, font_size);	
	gdc->set_align(gdc, ALIGN_LEFT | ALIGN_CENTER_Y );
	gdc->set_clip_rect(gdc, border_size + 1, border_size + 1, width - (border_size << 1) - 2, height - (border_size << 1) - 2);
	gdc->draw_text_rect(gdc, text_x, text_y, scroll + width - (border_size << 1), height - (border_size << 1), utf8_strpos(text, scroll), (int)utf8_strlen(text) - scroll);
	gdc->set_clip_rect(gdc, 0, 0, width, height);
}
