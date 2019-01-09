/*
	dg_sample_surface_1.c
	Tests color keyed surfaces
*/
#include "dg_sample_shell.h"

#define BOX_WIDTH  100
#define BOX_HEIGHT 100

int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event) return 0;
	if (event->type == EVT_TYPE_MOUSE)
	{
		printf("Got mouse message: %d %d %d\n", event->e.mouse.x, event->e.mouse.y, event->e.mouse.mouse_event);
		if (event->e.mouse.mouse_event & EVT_MOUSE_LBUTTON)
		{
			dg_surface_p surface = device->create_surface(device, BOX_WIDTH, BOX_HEIGHT, 0);
			if (surface)
			{
				dg_color_t color=DG_RGB_COLOR(255, 0, 0, 0);

				surface->clear(surface, color);
				color=DG_RGB_COLOR(0, 0, 255, 0);
				surface->fill_rect(surface, BOX_WIDTH/4, BOX_HEIGHT/4, BOX_WIDTH/2, BOX_HEIGHT/2, color);
				surface->set_color_key(surface, color);
				surface->flip(surface, event->e.mouse.x, event->e.mouse.y, 0);
				surface->destroy(surface);
			}
			swap_buffers(device);
		} else
		if (event->e.mouse.mouse_event & EVT_MOUSE_RBUTTON)
		{
			dg_surface_p surface = device->create_surface(device, BOX_WIDTH, BOX_HEIGHT, 0);
			if (surface)
			{
				dg_color_t color=DG_RGB_COLOR(0, 0, 255, 0);
				surface->clear(surface, color);
				color=DG_RGB_COLOR(255, 0, 0, 0);
				surface->fill_rect(surface, BOX_WIDTH/4, BOX_HEIGHT/4, BOX_WIDTH/2, BOX_HEIGHT/2, color);
				surface->set_color_key(surface, color);

				surface->flip(surface, event->e.mouse.x, event->e.mouse.y, 0);
				surface->destroy(surface);
			}
			swap_buffers(device);
		}
	}
	return 1;
}

int main()
{
	start_sample(event_handler);
	return 0;
}

