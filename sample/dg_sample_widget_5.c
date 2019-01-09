/*
	dg_sample_widget_5.c
	Tests child widget creation, rendering, movement and child resizing
*/

#include "dg_sample_shell.h"

#define W_WIDTH  200
#define W_HEIGHT 200


dg_widget_p widget=0;
dg_widget_p child;
int dx=-1;
int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event) 
	{
		widget = device->create_widget(device, sizeof(dg_widget_t), 0, 0, W_WIDTH, W_HEIGHT);
		widget->render=default_widget_render;
		widget->render(widget);
		child  = widget->create(widget, sizeof(dg_widget_t), W_WIDTH/4, W_HEIGHT/4, W_WIDTH/2, W_HEIGHT/2);
		child->render(child);
	}
	else
	if (event->type == EVT_TYPE_MOUSE)
	{
		int x = event->e.mouse.x;
		int y = event->e.mouse.y;
		if (event->e.mouse.mouse_event & EVT_MOUSE_LBUTTON && event->e.mouse.mouse_event & EVT_PRESSED)
		{
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
		
				int w=child->surface->width;
				int h=child->surface->height;
				w+=2*dx;
				h+=2*dx;
				child->set_rect(child, child->window->rect->x-dx, child->window->rect->y-dx, w, h, 1);
				if (w>100) dx=-dx;
				if (w<50) dx=-dx;

				test_performance();
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
