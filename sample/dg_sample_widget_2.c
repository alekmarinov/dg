/*
	dg_sample_widget_2.c
	Tests widget creation, rendering and resizing
*/
#include "dg_sample_shell.h"

dg_widget_p widget=0;
int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event) return 0;

	if (event->type == EVT_TYPE_MOUSE)
	{
		printf("Got mouse message: %d %d %d\n", event->e.mouse.x, event->e.mouse.y, event->e.mouse.mouse_event);
		if (event->e.mouse.mouse_event & EVT_MOUSE_LBUTTON && event->e.mouse.mouse_event & EVT_PRESSED)
		{
			int x = event->e.mouse.x;
			int y = event->e.mouse.y;

			widget = device->create_widget(device, sizeof(dg_widget_t), x, y, 100, 100);
			widget->render=default_widget_render;
			widget->render(widget);
			widget->invalidate(widget, 0);

			swap_buffers(device);
		}
		else
		if (event->e.mouse.mouse_event & EVT_MOUSE_RBUTTON && event->e.mouse.mouse_event & EVT_PRESSED)
		{
			if (widget)
			{
				int x=widget->window->rect->x;
				int y=widget->window->rect->y;
				int w=widget->window->rect->w;
				int h=widget->window->rect->h;
				widget->set_rect(widget, x+w/5, y+h/5, w-2*w/5, h-2*h/5, 1);

				swap_buffers(device);
			}
		}
	}
	return 1;
}

int main()
{
	start_sample(event_handler);
	return 0;
}
