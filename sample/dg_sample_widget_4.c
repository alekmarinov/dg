/*
	dg_sample_widget_4.c
	Tests child widget creation, rendering and movement
*/

#include "dg_sample_shell.h"

#define W_WIDTH  200
#define W_HEIGHT 200

dg_widget_p widget=0;
int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event || event->type == EVT_TYPE_MOUSE)
	{
		int x = event?event->e.mouse.x:0;
		int y = event?event->e.mouse.y:0;
		if (!event || event->e.mouse.mouse_event & EVT_MOUSE_LBUTTON && event->e.mouse.mouse_event & EVT_PRESSED)
		{
			dg_widget_p child;

			widget = device->create_widget(device, sizeof(dg_widget_t), x, y, W_WIDTH, W_HEIGHT);
			widget->render=default_widget_render;
			widget->render(widget);

			child  = widget->create(widget, sizeof(dg_widget_t), W_WIDTH/4, W_HEIGHT/4, W_WIDTH/2, W_HEIGHT/2);
			child->render=default_widget_render;
			child->render(child);

			widget->invalidate(widget, 0);
			swap_buffers(device);
		}
		else
		{
			if (widget)
			{
				widget->set_rect(widget, x, y, widget->surface->width, widget->surface->height, 1);
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
