/*
	dg_sample_widget_11.c
	Tests widget layout
*/

#include "dg_sample_shell.h"

#define W_WIDTH  512
#define W_HEIGHT 400

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
	//color.red=color.green=color.blue=color.alpha=0;
	//self->surface->fill_rect(self->surface, 20, 20, 40, 40, color);
	//self->surface->set_color_key(self->surface, color);
	/*if (!self->surface->alpha_channel)
		self->surface->set_alpha_channel(self->surface, alpha_channel);*/

	//self->surface->set_alpha(self->surface, 128);
}

dg_widget_p widget=0;
dg_widget_p child;
int dx=-1;
int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event)
	{
		int x=device->xres-W_WIDTH-20, y=20;
		device->master_widget->layout_manager=dg_layout_anchor_manager;

		widget = device->create_widget(device, sizeof(dg_widget_t), x, y, W_WIDTH, W_HEIGHT);
		widget->layout_manager=dg_layout_anchor_manager;
		widget->layout_properties=dg_layout_anchor_properties_create(DG_LAYOUT_ANCHOR_RIGHT | DG_LAYOUT_ANCHOR_TOP_BOTTOM);
		widget->render=test_render;
		widget->render(widget);

		child  = widget->create(sizeof(dg_widget_t), widget, W_WIDTH-100, 10, 90, W_HEIGHT-20);
		child->layout_properties=dg_layout_anchor_properties_create(DG_LAYOUT_ANCHOR_RIGHT_BOTTOM | DG_LAYOUT_ANCHOR_TOP);
		child->render=default_widget_render;
		child->render(child);

		child  = widget->create(sizeof(dg_widget_t), widget, 10, W_HEIGHT-40, W_WIDTH-120, 30);
		child->layout_properties=dg_layout_anchor_properties_create(DG_LAYOUT_ANCHOR_LEFT_RIGHT | DG_LAYOUT_ANCHOR_BOTTOM);
		child->render=default_widget_render;
		child->render(child);

		child  = widget->create(sizeof(dg_widget_t), widget, 10, 10, W_WIDTH-120, W_HEIGHT-60);
		child->layout_properties=dg_layout_anchor_properties_create(DG_LAYOUT_ANCHOR_LEFT_RIGHT | DG_LAYOUT_ANCHOR_TOP_BOTTOM);
		child->render=default_widget_render;
		child->render(child);

		widget->invalidate(widget, 0);

		swap_buffers(device);
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
			alpha_channel[i*W_WIDTH+j] = (char)(255-(255-(((i+j)/2)>255?255-(i+j)/2:(i+j)/2))&0xFF);
			//alpha_channel[i*W_WIDTH+j] = 40;
		}
	start_sample(event_handler);
	free(alpha_channel);
	return 0;
}
