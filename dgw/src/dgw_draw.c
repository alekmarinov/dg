/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_draw.c                                         */
/* Description:   Helper drawing functions                           */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_gdc.h>
#include <dg/dgw_draw.h>

/* Various rectangle drawers */

void dgw_draw_border_rectangle(dg_gdc_p gdc, int x, int y, int w, int h, 
							   dg_color_t border_color)
{
	gdc->set_pen(gdc, dg_pen_create(1, border_color, PEN_SOLID));
	gdc->draw_rect(gdc, x, y, w, h);
}

void dgw_draw_sunken_rectangle(dg_gdc_p gdc, int x, int y, int w, int h, 
							   dg_color_t shadow_color, dg_color_t hilite_color)
{
	if (w > 0 && h > 0)
	{
		gdc->set_brush(gdc, dg_brush_create(shadow_color, BRUSH_SOLID));;
		gdc->draw_fillrect(gdc, x, y, w, 1);
		gdc->draw_fillrect(gdc, x, y, 1, h);
		gdc->set_brush(gdc, dg_brush_create(hilite_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y + h - 1, w, 1);
		gdc->draw_fillrect(gdc, x + w - 1, y, 1, h);
	}
}

void dgw_draw_raised_rectangle(dg_gdc_p gdc, int x, int y, int w, int h, 
							   dg_color_t shadow_color, dg_color_t hilite_color)
{
	if (w > 0 && h > 0)
	{
		gdc->set_brush(gdc, dg_brush_create(shadow_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y + h - 1, w, 1);
		gdc->draw_fillrect(gdc, x + w - 1, y, 1, h);
		gdc->set_brush(gdc, dg_brush_create(hilite_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y, w, 1);
		gdc->draw_fillrect(gdc, x, y, 1, h);
	}
}

void dgw_draw_groove_rectangle(dg_gdc_p gdc, int x, int y, int w, int h, 
							   dg_color_t shadow_color, dg_color_t hilite_color)
{
	if (w > 0 && h > 0)
	{
		gdc->set_brush(gdc, dg_brush_create(shadow_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y, w, 1);
		gdc->draw_fillrect(gdc, x, y, 1, h);
		gdc->set_brush(gdc, dg_brush_create(hilite_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y + h - 1, w, 1);
		gdc->draw_fillrect(gdc, x + w - 1, y, 1, h);

		if (w > 1 && h > 1)
		{
			gdc->set_brush(gdc, dg_brush_create(shadow_color, BRUSH_SOLID));
			gdc->draw_fillrect(gdc, x + 1, y + h - 2, w - 2, 1);
			gdc->draw_fillrect(gdc, x + w - 2, y + 1, 1, h - 2);
			gdc->set_brush(gdc, dg_brush_create(hilite_color, BRUSH_SOLID));
			gdc->draw_fillrect(gdc, x + 1, y + 1, w - 3, 1);
			gdc->draw_fillrect(gdc, x + 1, y + 1, 1, h - 3);
		}
	}
}

void dgw_draw_ridge_rectangle(dg_gdc_p gdc, int x, int y, int w, int h, 
							  dg_color_t shadow_color, dg_color_t hilite_color)
{
	if (w > 0 && h > 0)
	{
		gdc->set_brush(gdc, dg_brush_create(hilite_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y, w, 1);
		gdc->draw_fillrect(gdc, x, y, 1, h);
		gdc->set_brush(gdc, dg_brush_create(shadow_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y + h - 1, w, 1);
		gdc->draw_fillrect(gdc, x + w - 1, y, 1, h);

		if (w > 1 && h > 1)
		{
			gdc->set_brush(gdc, dg_brush_create(hilite_color, BRUSH_SOLID));
			gdc->draw_fillrect(gdc, x + 1, y + h - 2, w - 2, 1);
			gdc->draw_fillrect(gdc, x + w - 2, y + 1, 1, h - 2);
			gdc->set_brush(gdc, dg_brush_create(shadow_color, BRUSH_SOLID));
			gdc->draw_fillrect(gdc, x + 1, y + 1, w - 3, 1);
			gdc->draw_fillrect(gdc, x + 1, y + 1, 1, h - 3);
		}
	}
}

void dgw_draw_double_sunken_rectangle(dg_gdc_p gdc, int x, int y, int w, int h, 
									  dg_color_t shadow_color, dg_color_t hilite_color, 
									  dg_color_t border_color, dg_color_t base_color)
{
	if (w > 0 && h > 0)
	{
		gdc->set_brush(gdc, dg_brush_create(hilite_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y + h - 1, w, 1);
		gdc->draw_fillrect(gdc, x + w - 1, y, 1, h);
		gdc->set_brush(gdc, dg_brush_create(shadow_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y, w - 1, 1);
		gdc->draw_fillrect(gdc, x, y, 1, h - 1);

		if (w > 1 && h > 1)
		{
			gdc->set_brush(gdc, dg_brush_create(border_color, BRUSH_SOLID));
			gdc->draw_fillrect(gdc, x + 1, y + 1, w - 3, 1);
			gdc->draw_fillrect(gdc, x + 1, y + 1, 1, h - 3);
			gdc->set_brush(gdc, dg_brush_create(base_color, BRUSH_SOLID));
			gdc->draw_fillrect(gdc, x + 1, y + h - 2, w - 2, 1);
			gdc->draw_fillrect(gdc, x + w - 2, y + 1, 1, h - 2);
		}
	}
}

void dgw_draw_double_raised_rectangle(dg_gdc_p gdc, int x, int y, int w, int h, 
									  dg_color_t shadow_color, dg_color_t hilite_color, 
									  dg_color_t border_color, dg_color_t base_color)
{
	if (w > 0 && h > 0)
	{
		gdc->set_brush(gdc, dg_brush_create(border_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y + h - 1, w, 1);
		gdc->draw_fillrect(gdc, x + w - 1, y, 1, h);
		gdc->set_brush(gdc, dg_brush_create(hilite_color, BRUSH_SOLID));
		gdc->draw_fillrect(gdc, x, y, w - 1, 1);
		gdc->draw_fillrect(gdc, x, y, 1, h - 1);

		if (w > 1 && h > 1)
		{
			gdc->set_brush(gdc, dg_brush_create(base_color, BRUSH_SOLID));
			gdc->draw_fillrect(gdc, x + 1, y + 1, w - 2, 1);
			gdc->draw_fillrect(gdc, x + 1, y + 1, 1, h - 2);
			gdc->set_brush(gdc, dg_brush_create(shadow_color, BRUSH_SOLID));
			gdc->draw_fillrect(gdc, x + 1, y + h - 2, w - 2, 1);
			gdc->draw_fillrect(gdc, x + w - 2, y + 1, 1, h - 2);
		}
	}
}
