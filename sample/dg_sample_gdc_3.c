/*
	dg_sample_screen_3.c
	Tests GDC with images
*/

#include <string.h>
#include <wchar.h>
#include <dg/dg_rect.h>
#include <dg/dg_utf8.h>
#include <dg/dg_blit.h>
#include <dg/dg_gdc.h>
#include <dg/dg_pixel.h>
#include "dg_sample_shell.h"

const char* IMAGE_NAME="c:\\temp\\img\\test0.png";
//const char* IMAGE_NAME="c:\\temp\\cpu_bug.png";

const char* text = "Hello, GDC!";

void gdc_renderer(dg_widget_p self)
{
	dg_color_t color=DG_COLOR_BLUE;
	dg_gdc_p gdc;
	dg_surface_p image;
	int height;
	int i;
	int from=0;
	int code;
	char* utftext;
	int utftextlen;
	FILE* file = fopen("c:\\utf8.txt", "r");
	fseek(file, 0, SEEK_END);
	utftextlen = ftell(file);
	utftext = malloc(utftextlen+1);
	fseek(file, 0, SEEK_SET);
	fread(utftext, 1, utftextlen, file);
	fclose(file);
	utftext[utftextlen] = 0;

	self->surface->clear(self->surface, DG_COLOR_BLACK);
	gdc=self->create_gdc(self);
	image=self->surface->device->load_surface_file(self->surface->device, IMAGE_NAME);
	//image=self->surface->device->create_surface(self->surface->device, 100, 100, 0);
	//image->clear(image, DG_COLOR_BLUE);
	if (image)
	{
		gdc->draw_image(gdc, 355, 355, image);
		gdc->draw_image(gdc, 350, 350, image);
	}
	else
		printf("Unable to load image %s\n", IMAGE_NAME);
	self->surface->device->save_surface_file(self->surface->device, image, IMAGE_FORMAT_JPEG, "c:\\temp\\new.jpg");
	self->surface->device->save_surface_file(self->surface->device, image, IMAGE_FORMAT_PNG, "c:\\temp\\new.png");
	self->surface->device->save_surface_file(self->surface->device, image, IMAGE_FORMAT_GIF, "c:\\temp\\new.gif");

	image->destroy(image);

	height = 20;
	gdc->set_font(gdc, "c:\\WINDOWS\\Fonts\\arial.ttf", height);
	gdc->set_pen(gdc, dg_pen_create(1, DG_RGB_COLOR(255, 255, 255, 255), PEN_SOLID));
	gdc->set_brush(gdc, dg_brush_create(DG_COLOR_BLUE, BRUSH_SOLID));

#if 11
	for (i=0; i<8; i++)
	{
		char text[17];
		int j;
		for (j=0; j<16; j++)
		{
			code = i*16+j;
			if (!code) code = 32;
			text[j] = code & 0x7F;
		}
		text[16]=0;
		gdc->draw_text(gdc, 0, i*height, text, 16);
	}
#endif

	//gdc->draw_text(gdc, 0, 0, utftext+3, utf8_strlen(utftext+3));

	//font = gdc->get_font(gdc);
	//height = font->get_height(font);
	//gdc->draw_text(gdc, 150, 1 + 150 + height, text, strlen(text));

	gdc->destroy(gdc);
}


void gdc_renderer2(dg_widget_p self)
{
	dg_color_t color=DG_COLOR_BLUE;
	dg_surface_p image;
	dg_rect_p srect;
	dg_rect_p drect;
	self->surface->clear(self->surface, DG_COLOR_GREEN);
	image=self->surface->device->load_surface_file(self->surface->device, IMAGE_NAME);
	srect = dg_rect_create(0, 0, image->width, image->height);
	drect = srect->clone(srect);
	drect->move(drect, 55, 55);
	self->surface->blit(self->surface, drect, image, srect, 0, 0, 0, DG_OPT_BLIT_DEFAULT);
	drect->move(drect, 50, 50);
	//self->surface->blit(self->surface, drect, image, srect, 0, 0, 0, DG_OPT_BLIT_DEFAULT);
	srect->destroy(srect);
	drect->destroy(drect);
	image->destroy(image);
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

