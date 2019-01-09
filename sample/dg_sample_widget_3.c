/*
	dg_sample_widget_3.c
	Tests widget creation, rendering and resizing performance
*/
#include "dg_sample_shell.h"

#define W_WIDTH	 300
#define W_HEIGHT 200

dg_widget_p widget=0;
int dx=1;
int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event)
	{
		widget = device->create_widget(device, sizeof(dg_widget_t), 0, 0, W_WIDTH, W_HEIGHT);
		widget->render(widget);
	}
	else
	if (event->type == EVT_TYPE_MOUSE)
	{
		int x = event->e.mouse.x;
		int y = event->e.mouse.y;
		printf("Got mouse message: %d %d %d\n", event->e.mouse.x, event->e.mouse.y, event->e.mouse.mouse_event);
		if (event->e.mouse.mouse_event & EVT_MOUSE_LBUTTON && event->e.mouse.mouse_event & EVT_PRESSED)
		{
			widget = device->create_widget(device, sizeof(dg_widget_t), x, y, widget->surface->width, widget->surface->height);
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
				widget->invalidate(widget, 0);

				swap_buffers(device);
			}
		} 
		else
		{
			if (widget)
			{
				int w=widget->surface->width;
				int h=widget->surface->height;
				w+=dx;
				h+=dx;
				widget->set_rect(widget, x, y, w, h, 1);
				swap_buffers(device);
				if (w>300) dx=-dx;
				if (w<150) dx=-dx;
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
