/*
	dg_sample_widget_1.c
	Tests widget creation and rendering
*/

#include "dg_sample_shell.h"

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
			dg_widget_p widget;

			widget = device->create_widget(device, sizeof(dg_widget_t), x, y, 100, 100);
			widget->render=default_widget_render;
			widget->render(widget);
			widget->invalidate(widget, 0);

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
