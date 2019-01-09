/*
	dg_sample_screen_1.c
	Very simple GDC test
*/

#include <string.h>
#include <dg/dg_gdc.h>
#include <dg/dg_pixel.h>
#include "dg_sample_shell.h"

const char* demo_text = "Hello, GDC!";

#define BOX_WIDTH  400
#define BOX_HEIGHT 400

void gdc_renderer(dg_widget_p self)
{
	dg_color_t color=DG_COLOR_BLUE;
	dg_gdc_p gdc;
	//self->surface->clear(self->surface, color);
	//self->surface->set_color_key(self->surface, color);
	gdc=self->create_gdc(self);

#if 1
	color=DG_COLOR_WHITE;
	gdc->set_pen(gdc, dg_pen_create(1, color, PEN_SOLID));
	color=DG_COLOR_BLUE;
	gdc->set_brush(gdc, dg_brush_create(color, BRUSH_SOLID));
	gdc->set_font(gdc, "Arial", 24);
	gdc->draw_fillrect(gdc, 10, 10, 80, 80);
	color=DG_COLOR_RED;
	color=DG_COLOR_GREEN;
	gdc->set_brush(gdc, dg_brush_create(color, BRUSH_SOLID));
	gdc->draw_fillrect(gdc, 20, 20, 50, 50);
	color=DG_COLOR_WHITE;
	gdc->set_pen(gdc, dg_pen_create(1, color, PEN_SOLID));
	gdc->draw_line(gdc, 20, 20, 50, 30);
	gdc->draw_text(gdc, 60, 180, demo_text, strlen(demo_text));
#endif

#if 0
	gdc->set_brush(gdc, dg_brush_create(DG_COLOR_GREEN, BRUSH_SOLID));
	gdc->draw_fillrect(gdc, 0, 0, BOX_WIDTH, BOX_HEIGHT);
#endif

	gdc->destroy(gdc);
}

int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event) 
	{
		dg_widget_p widget;

		widget = device->create_widget(device, sizeof(dg_widget_t), 0, 0, 640, 480);
		widget->render(widget);
		widget->invalidate(widget, 0);

		widget = device->create_widget(device, sizeof(dg_widget_t), 100, 100, BOX_WIDTH, BOX_HEIGHT);
		widget->render=gdc_renderer;
		widget->render(widget);
		widget->invalidate(widget, 0);

		swap_buffers(device);
	}
	return 1;
}

int main()
{
	start_sample(event_handler);
	return 0;
}

