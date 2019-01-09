/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      winxp_scrollbar.c                                  */
/* Description:   Scrollbar with Windows XP look & feel              */
/*                                                                   */
/*********************************************************************/

#include <dg/dgw_draw.h>
#include <dg/dgw_scrollbar.h>
#include "winxp_scrollbar.h"
#include "winxp_draw.h"

static const dg_color_t scroll_bar_color1  = DG_RGB_COLOR(255, 139, 100, 255);
static const dg_color_t scroll_bar_color2  = DG_RGB_COLOR(199, 100, 50, 255);

static void winxp_draw_vert_scrollbar(dgw_scrollbar_p self, dg_gdc_p gdc, int x, int y, int width, int height)
{
	dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
	int bar_size = SCROLLBAR_SIZE;

	/* draw scroll background */
	gdc->set_brush(gdc, dg_brush_create(scroll_bar_color1, BRUSH_SOLID));
	gdc->draw_fillrect(gdc, x, y, width, height);

	if (height < 2*SCROLLBAR_SIZE)
	{
		bar_size = (height - 1) >> 1;
	}

	/* draw scroll bar buttons */
	gdc->set_brush(gdc, dg_brush_create(scroll_bar_color2, BRUSH_SOLID));
	gdc->draw_fillrect(gdc, x, y, width, bar_size);
	gdc->draw_fillrect(gdc, x, y+height-bar_size, width, bar_size);
		
	if (height > SCROLLBAR_SIZE)
	{
		int arrow_points_up[6], arrow_points_down[6];
		arrow_points_up[0] = x + 3;
		arrow_points_up[1] = y + bar_size - 5;
		arrow_points_up[2] = x + width - 4;
		arrow_points_up[3] = arrow_points_up[1];
		arrow_points_up[4] = x + (width >> 1);
		arrow_points_up[5] = y + 3;

		arrow_points_down[0] = x + 3;
		arrow_points_down[1] = y + height - bar_size + 4;
		arrow_points_down[2] = x + width - 4;
		arrow_points_down[3] = arrow_points_down[1];
		arrow_points_down[4] = x + (width >> 1);
		arrow_points_down[5] = y + height - 4;

		gdc->set_brush(gdc, dg_brush_create(scroll_bar_color1, BRUSH_SOLID));
		gdc->draw_fillpolygon(gdc, arrow_points_up, 3);
		gdc->draw_fillpolygon(gdc, arrow_points_down, 3);
	}

	dgw_draw_groove_rectangle(gdc, x, y, width, bar_size, scroll_bar_color2, scroll_bar_color1);
	dgw_draw_groove_rectangle(gdc, x, y+height-bar_size, width, bar_size, scroll_bar_color2, scroll_bar_color1);

	if (height > 3*SCROLLBAR_SIZE)
	{
		/* draw scroll bar cursor */
		dgw_scrollbar_get_cursor_rect(self, rect);
		gdc->set_brush(gdc, dg_brush_create(scroll_bar_color2, BRUSH_SOLID));
		gdc->draw_fillrects(gdc, &rect, 1);
		gdc->set_pen(gdc, dg_pen_create(1, scroll_bar_color1, PEN_SOLID));
		gdc->draw_line(gdc, rect->x + 3, rect->y + (rect->h >> 1) - 2, rect->x + rect->w - 2, rect->y + (rect->h >> 1) - 2);
		gdc->draw_line(gdc, rect->x + 3, rect->y + (rect->h >> 1) - 0, rect->x + rect->w - 2, rect->y + (rect->h >> 1) - 0);
		gdc->draw_line(gdc, rect->x + 3, rect->y + (rect->h >> 1) + 2, rect->x + rect->w - 2, rect->y + (rect->h >> 1) + 2);
		dgw_draw_groove_rectangle(gdc, rect->x, rect->y, rect->w, rect->h, scroll_bar_color2, scroll_bar_color1);
	}

	rect->destroy(rect);
}

void winxp_render_scrollbar(dg_widget_p widget)
{
	dg_event_t event;
	dgw_scrollbar_p self = (dgw_scrollbar_p)widget;
	dg_gdc_p    gdc    = widget->create_gdc(widget);
	dg_window_p window = widget->window;
	int         width  = DGWidgetWidth(widget);
	int         height = DGWidgetHeight(widget);

	event.type = EVT_TYPE_WIDGET;
	event.e.widget.widget_event = EVT_WIDGET_RENDER;
	event.e.widget.p1=event.e.widget.p2=0;

	winxp_draw_vert_scrollbar(self, gdc, 0, 0, width, height);

	gdc->destroy(gdc);
	widget->event_manager->trigger_event(widget->event_manager, &event);
}
