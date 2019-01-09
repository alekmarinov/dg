/*
	dg_sample_surface_3.c
	Tests direct buffer access and pixel format conversion
*/
#include "dg_sample_shell.h"

#define BOX_WIDTH  320
#define BOX_HEIGHT 200

int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event) 
	{
		int i,j, width=BOX_WIDTH, height=BOX_HEIGHT;
		dg_surface_p surface = device->create_surface(device, BOX_WIDTH, BOX_HEIGHT, 0);
		surface->clear(surface, DG_COLOR_BLUE);

		do {
			surface->lock(surface, 1);
			for (i=0; i<height; i++)
				for (j=0; j<width; j++)
				{
					dg_color_t c;
					switch (rand()%12)
					{
					case 0: c=DG_COLOR_RED; break;
					case 1: c=DG_COLOR_BLACK; break;
					case 2: c=DG_COLOR_WHITE; break;
					case 3: c=DG_COLOR_GRAY; break;
					case 4: c=DG_COLOR_LIGHT_GRAY; break;
					case 5: c=DG_COLOR_DARK_GRAY; break;
					case 6: c=DG_COLOR_RED; break;
					case 7: c=DG_COLOR_GREEN; break;
					case 8: c=DG_COLOR_BLUE; break;
					case 9: c=DG_COLOR_CYAN; break;
					case 10: c=DG_COLOR_MAGENTA; break;
					case 11: c=DG_COLOR_YELLOW; break;
					}

					surface->pixels[i*width+j] = c;
				}
			surface->unlock(surface);
			surface->flip(surface, 0, 0, 0);
			swap_buffers(device);
			device->master_widget->event_manager->idle(device->master_widget->event_manager);
			surface->resize(surface, BOX_WIDTH+50, BOX_HEIGHT+50);
			width=BOX_WIDTH+50;
			height=BOX_HEIGHT+50;

		} while (device->master_widget->event_manager->active);
	}
	return 1;
}

int main()
{
	start_sample(event_handler);
	return 0;
}

