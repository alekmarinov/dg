/*
	dg_sample_widget_6.c
	Tests widget events
*/

#include "dg_sample_shell.h"

#define W_WIDTH  255
#define W_HEIGHT 255

char *alpha_channel;
dg_widget_p parent_widget=0;

void test_render(dg_widget_p self)
{
	dg_color_t color;
	unsigned long addr=(unsigned long)self;
	addr=( (addr^(addr<<16))>>16)|(((addr&0xFFFF)^(addr>>16))<<16);
	color=(dg_color_t)addr;
	self->surface->clear(self->surface, color);

	color=0;
	self->surface->fill_rect(self->surface, W_WIDTH/4, W_HEIGHT/4, W_WIDTH/2, W_HEIGHT/2, color);
	self->surface->set_color_key(self->surface, color);

	self->surface->set_alpha_channel(self->surface, alpha_channel);
}

int widget_event_handler(dg_widget_p widget, dg_event_p event)
{
	if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAGGING)
	{
		dg_rect_p rect=dg_rect_create(0, 0, 0, 0);
		test_performance();
		widget->get_rect(widget, rect);
		widget->set_rect(widget, rect->x+DG_WORD_LO(event->e.widget.p2), rect->y+DG_WORD_HI(event->e.widget.p2), rect->w, rect->h, 1);
		swap_buffers(widget->surface->device);
		rect->destroy(rect);
		return 1;
	} 
	else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_START)
	{
		widget->set_capture(widget, 1);
		return 1;
	} 
	else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_STOP)
	{
		widget->set_capture(widget, 0);
		return 1;
	}
	return 0;
}

int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event || event->type == EVT_TYPE_MOUSE && (event->e.mouse.mouse_event & EVT_MOUSE_RBUTTON) && (event->e.mouse.mouse_event & EVT_PRESSED))
	{
		int x = event?event->e.mouse.x:rand()%(device->xres-W_WIDTH);
		int y = event?event->e.mouse.y:rand()%(device->yres-W_WIDTH);
		dg_widget_p widget;

		if (!parent_widget)
		{
			parent_widget = widget = device->create_widget(device, sizeof(dg_widget_t), x, y, W_WIDTH, W_HEIGHT);
		}
		else
		{
			widget = parent_widget->create(parent_widget, sizeof(dg_widget_t), x%W_WIDTH, y%W_HEIGHT, W_WIDTH, W_HEIGHT);
		}
		widget->event_manager->add_listener(widget->event_manager, widget_event_handler, EVT_TYPE_WIDGET, widget);

		widget->render=test_render;
		widget->render(widget);
		widget->invalidate(widget, 0);

		swap_buffers(device);
		return 1;
	}
	return 0;
}

int main()
{
	int i,j;
	alpha_channel=malloc(W_WIDTH * W_HEIGHT);
	for (i=0; i<W_HEIGHT; i++)
		for (j=0; j<W_WIDTH; j++)
		{
			alpha_channel[i*W_WIDTH+j] = (char)((255-((i+j)>255?255-(i+j):(i+j)))&0xFF);
			//alpha_channel[i*W_WIDTH+j] = 40;
		}
	start_sample(event_handler);
	free(alpha_channel);
	return 0;
}
