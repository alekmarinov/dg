/*
	dg_sample_screen_2.c
	Tests various GDC graphics primitives
*/

#include <string.h>
#include <dg/dg_gdc.h>
#include <dg/dg_pixel.h>
#include "dg_sample_shell.h"

const char* demo_text = "Hello, GDC!";

#define BOX_WIDTH  400
#define BOX_HEIGHT 400

void draw_rects(dg_gdc_p gdc, int y_ofs)
{
	gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_RED, PEN_SOLID));
	gdc->draw_rect(gdc, 10, y_ofs+10, 50, 50);

	gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_GREEN, PEN_SOLID));
	gdc->draw_rect(gdc, 20, y_ofs+20, 50, 50);

	gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_BLUE, PEN_SOLID));
	gdc->draw_rect(gdc, 30, y_ofs+30, 50, 50);
}

void draw_fillrects(dg_gdc_p gdc, int y_ofs, dg_brush_style_t brush_style)
{
	int i;
	dg_rect_p* rects=(dg_rect_p*)malloc(4*sizeof(dg_rect_p));
	gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_BLACK, PEN_SOLID));

	gdc->set_brush(gdc, dg_brush_create(DG_COLOR_MAGENTA, brush_style));
	for (i=0; i<4; i++)
	{
		rects[i]=dg_rect_create(130+i*10, y_ofs+10+i*10, 50, 50);
	}
	gdc->draw_fillrects(gdc, rects, 4);

	for (i=0; i<4; i++)
		rects[i]->destroy(rects[i]);
	free(rects);

	gdc->set_brush(gdc, dg_brush_create(DG_COLOR_RED, brush_style));
	gdc->draw_fillrect(gdc, 110, y_ofs+10, 51, 51);

	gdc->set_brush(gdc, dg_brush_create(DG_COLOR_GREEN, brush_style));
	gdc->draw_fillrect(gdc, 120, y_ofs+20, 51, 51);

	gdc->set_brush(gdc, dg_brush_create(DG_COLOR_BLUE, brush_style));
	gdc->draw_fillrect(gdc, 130, y_ofs+30, 51, 51);
}

void draw_lines(dg_gdc_p gdc, int y_ofs)
{
	int i;
	int coords[8*4];
	gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_RED, PEN_SOLID));
	
	for (i=0; i<8; i++)
	{
		coords[i*4+0] = 250+i;
		coords[i*4+1] = y_ofs+10+i*i;
		coords[i*4+2] = 300+i;
		coords[i*4+3] = y_ofs+50+i*i;
		gdc->draw_line(gdc, 200+i, y_ofs+10+i*i, 250+i, y_ofs+50+i*i);
	}
	gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_BLUE, PEN_SOLID));
	gdc->draw_lines(gdc, coords, 8);
}

void draw_texts(dg_gdc_p gdc, int y_ofs)
{
	int i;
	gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_WHITE, PEN_SOLID));
	gdc->set_brush(gdc, 0);
	gdc->set_font(gdc, "Arial", 14);
	for (i=0; i<5; i++)
	{
		gdc->set_font(gdc, "Arial", 12+2*i);
		gdc->draw_text(gdc, 320, y_ofs+10+(12+2*i)*i, demo_text, strlen(demo_text));
	}
}

void draw_polygons(dg_gdc_p gdc, int y_ofs, dg_brush_style_t brush_style)
{
	int i;
	int points[6];
	for (i=0; i<2; i++)
	{
		points[0]=i*80+420;
		points[1]=y_ofs+10;
		if (i==0)
		{
			gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_WHITE, PEN_SOLID));
			gdc->draw_polygon(gdc, points, 1);
		}
		else
		{
			gdc->set_brush(gdc, dg_brush_create(DG_COLOR_WHITE, brush_style));
			gdc->draw_fillpolygon(gdc, points, 1);
		}

		points[0]+=5;
		points[1]+=5;
		points[2]=i*80+430;
		points[3]=y_ofs+40;

		if (i==0)
		{
			gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_YELLOW, PEN_SOLID));
			gdc->draw_polygon(gdc, points, 2);
		}
		else
		{
			gdc->set_brush(gdc, dg_brush_create(DG_COLOR_YELLOW, brush_style));
			gdc->draw_fillpolygon(gdc, points, 2);
		}

		points[0]=i*80+430;
		points[1]=y_ofs+20;
		points[2]=i*80+440;
		points[3]=y_ofs+80;
		points[4]=i*80+470;
		points[5]=y_ofs+40;
		if (i==0)
		{
			gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_CYAN, PEN_SOLID));
			gdc->draw_polygon(gdc, points, 3);
		}
		else
		{
			gdc->set_brush(gdc, dg_brush_create(DG_COLOR_CYAN, brush_style));
			gdc->draw_fillpolygon(gdc, points, 3);
		}
	}
}

void draw_arcs(dg_gdc_p gdc, int y_ofs, dg_brush_style_t brush_style)
{
	gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_BLUE, PEN_SOLID));
	gdc->draw_rect(gdc, 580-25, y_ofs+50-25, 50, 50);

	gdc->set_pen(gdc, dg_pen_create(1, DG_COLOR_YELLOW, PEN_SOLID));
	gdc->draw_arc(gdc, 580, y_ofs+50, 50, 50, 180, 270);

	gdc->set_brush(gdc, dg_brush_create(DG_COLOR_RED, brush_style));
	gdc->draw_fillarc(gdc, 580, y_ofs+50, 50, 50, 270, 360, ARC_STYLE);

	gdc->set_brush(gdc, dg_brush_create(DG_COLOR_GREEN, brush_style));
	gdc->draw_fillarc(gdc, 580, y_ofs+50, 50, 50, 0, 90, CHORD_STYLE);

	gdc->set_brush(gdc, dg_brush_create(DG_COLOR_CYAN, brush_style));
	gdc->draw_fillarc(gdc, 580, y_ofs+50, 50, 50, 90, 180, ARC_STYLE | ARC_EDGED | ARC_NOFILL);
}

void gdc_renderer(dg_widget_p self)
{
	dg_color_t color=DG_COLOR_BLUE;
	dg_gdc_p gdc=self->create_gdc(self);

	draw_rects(gdc, 0);
	draw_fillrects(gdc, 0, BRUSH_SOLID);
	draw_lines(gdc, 0);
	draw_texts(gdc, 0);
	draw_polygons(gdc, 0, BRUSH_SOLID);
	draw_arcs(gdc, 0, BRUSH_SOLID);

	gdc->set_antialiaze(gdc, 1);
	draw_rects(gdc, 100);
	draw_fillrects(gdc, 100, BRUSH_SOLID);
	draw_lines(gdc, 100);
	draw_texts(gdc, 100);
	draw_polygons(gdc, 100, BRUSH_SOLID);
	draw_arcs(gdc, 100, BRUSH_SOLID);

	gdc->destroy(gdc);
}

int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event) 
	{
		dg_widget_p widget = device->create_widget(device, sizeof(dg_widget_t), 0, 0, 640, 480);
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

