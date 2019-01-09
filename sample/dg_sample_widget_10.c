/*
	dg_sample_widget_10.c
	Tests widget rendering with alpha channel
*/

#include "dg_sample_shell.h"

#define W_WIDTH 100
#define W_HEIGHT 100

char *alpha_channel;

void test_render(dg_widget_p self)
{
	unsigned char r, g, b, a;
	long addr=(long)self;
	addr = (addr & 0xFFFF) ^ ((addr >> 16) & 0xFFFF);
	a=0;
	r=addr & 0xFF; addr>>=8;
	g=addr & 0xFF; addr>>=8;
	b=addr & 0xFF;
	self->surface->clear(self->surface, DG_RGB_COLOR(r, g, b, a));
	self->surface->set_alpha_channel(self->surface, alpha_channel);
}

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

			widget = device->create_widget(device, sizeof(dg_widget_t), x, y, W_WIDTH, W_HEIGHT);

			widget->render=test_render;
			widget->render(widget);
			widget->invalidate(widget, 0);

			swap_buffers(device);
		}
	}
	return 1;
}

int main()
{
	int i,j;
	alpha_channel=malloc(W_WIDTH * W_HEIGHT);
	for (i=0; i<W_HEIGHT; i++)
		for (j=0; j<W_WIDTH; j++)
		{
			alpha_channel[i*W_WIDTH+j] = 255-i;
		}
	start_sample(event_handler);
	free(alpha_channel);
	return 0;
}
