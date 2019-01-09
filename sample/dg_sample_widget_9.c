/*
	dg_sample_widget_9.c
	Tests widget rendering performance
*/

#include "dg_sample_shell.h"

#if defined(DEBUG) && defined(WIN32)
#include <crtdbg.h>
#endif

char *alpha_channel;

#define W_WIDTH   80
#define W_HEIGHT  80
#define WIDGETS   10

#define COL(c, s) (((c) >> s) & 0xFF)
#define FIRE_COLOR1(c1, c2, c3, c4, s) ((( COL(c1, s) +  COL(c2, s) + COL(c3, s) + COL(c4, s))/4)&(0xFF))
#define FIRE_COLOR(c1, c2, c3, c4, s) (FIRE_COLOR1(c1, c2, c3, c4, s)>255?255:FIRE_COLOR1(c1, c2, c3, c4, s));


void render_fire(dg_widget_p self)
{
	int i, j;
	if (self->surface->lock(self->surface, 1))
	{
		for (i=0; i<self->surface->width; i++)
		{
			dg_color_t c;
			switch (rand()%5)
			{
			case 0:
			case 1:c=DG_COLOR_YELLOW; break;
			case 2:c=DG_COLOR_RED; break;
			case 3:
			case 4:c=DG_COLOR_BLACK; break;
				
			}
			DGSurfaceSetPixel(self->surface, i, DGSurfaceHeight(self->surface)-1, c);
		}

		for (i=self->surface->height-2; i>=0; i--)
		{
			for (j=1; j<self->surface->width-1; j++)
			{
				dg_color_t color=0;
				unsigned char r,g,b;
				dg_color_t color1=DGSurfacePixel(self->surface, -1+j, i+1);
				dg_color_t color2=DGSurfacePixel(self->surface, 0+j, i+1);
				dg_color_t color3=DGSurfacePixel(self->surface, 1+j, i+1);
				dg_color_t color4=DGSurfacePixel(self->surface, 0+j, i);

				//color.red=((color1.red/2 + 4*color2.red + color3.red/2 + color4.red)/4)&0xFF;
				r=FIRE_COLOR(color1, color2, color3, color4, R_SHIFT);
				DG_SET_RED(color, r);
				if (DG_GET_RED(color)>0) DG_SUB_RED(color, 1);
				if (DG_GET_RED(color)>0) DG_SUB_RED(color, 1);
				//if (color.red<255)color.red++;

				//color.green=(color1.green/2 + 4*color2.green + color3.green/2 + color4.green)/4;
				g=FIRE_COLOR(color1, color2, color3, color4, G_SHIFT);
				DG_SET_GREEN(color, g);
				if (DG_GET_GREEN(color)>0) DG_SUB_GREEN(color, 1);
				if (DG_GET_GREEN(color)>0) DG_SUB_GREEN(color, 1);
				//if (color.green<255)color.green++;

				//color.blue=(color1.blue/2 + 4*color2.blue + color3.blue/2 + color4.blue)/4;
				b=FIRE_COLOR(color1, color2, color3, color4, B_SHIFT);
				DG_SET_BLUE(color, b);
				if (DG_GET_BLUE(color)>0) DG_SUB_BLUE(color, 1);
				if (DG_GET_BLUE(color)>0) DG_SUB_BLUE(color, 1);
				//if (color.blue<255)color.blue++;

				DG_SET_ALPHA(color, 0);

				DGSurfaceSetPixel(self->surface, j, i, color);

			}
		}
		self->surface->unlock(self->surface);
	}
}
dg_surface_p image;
void test_render(dg_widget_p self)
{
	unsigned char* dest_alpha_channel = (unsigned char*)malloc(self->surface->width * self->surface->height);
	dg_rect_p srect = dg_rect_create(0, 0, image->width, image->height);
	dg_rect_p drect = dg_rect_create(0, 0, self->surface->width, self->surface->height);

	self->surface->clear(self->surface, DG_COLOR_WHITE);

	memset(dest_alpha_channel, 255, self->surface->width * self->surface->height);
	self->surface->set_alpha_channel(self->surface, dest_alpha_channel);
	free(dest_alpha_channel);

	self->surface->blit(self->surface, drect, image, srect, 0, 0, 0, DG_OPT_BLIT_DEFAULT);

	srect->destroy(srect);
	drect->destroy(drect);

	//dg_color_t color;
	//unsigned long addr=(unsigned long)self;
	//addr=( (addr^(addr<<16))>>16)|(((addr&0xFFFF)^(addr>>16))<<16);
	//color=addr;
	//self->surface->clear(self->surface, color);
	////color.red=color.green=color.blue=color.alpha=0;
	////self->surface->fill_rect(self->surface, 20, 20, 40, 40, color);
	////self->surface->set_color_key(self->surface, color);
	//if (!self->surface->alpha_channel)
	//	self->surface->set_alpha_channel(self->surface, alpha_channel);
}

int started=0;

struct 
{
	int dx, dy;
	int dw, dh;
	dg_widget_p widget;
} widgets[WIDGETS];

int init_pos=5;

int get_rand_pos()
{
	init_pos+=5;
	return rand();
}

int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event)
	{
		int x=0, y=0, i;
		dg_widget_p fire_widget;
		dg_rect_p rect;
		image = device->load_surface_file(device, "c:\\temp\\img\\test0.png");
		//image = device->create_surface(device, 100, 100, 0);
		//image->clear(image, DG_COLOR_YELLOW);
		device->master_widget->layout_manager=dg_layout_anchor_manager;
		fire_widget=device->create_widget(device, sizeof(dg_widget_t), 0, device->yres-80, device->xres, 80);
		rect=dg_rect_create(0, 0, 0 ,0);
		fire_widget->layout_properties=dg_layout_anchor_properties_create(DG_LAYOUT_ANCHOR_LEFT_RIGHT | DG_LAYOUT_ANCHOR_BOTTOM);
		fire_widget->render=render_fire;
		/* init widgets */
		for (i=0; i<WIDGETS; i++)
		{
			widgets[i].dx=rand()%3-1;
			widgets[i].dy=rand()%3-1;
			widgets[i].dw=rand()%3-1;
			widgets[i].dh=rand()%3-1;
			if (!widgets[i].dx) widgets[i].dx=1;
			if (!widgets[i].dy) widgets[i].dy=1;
			if (!widgets[i].dw) widgets[i].dw=1;
			if (!widgets[i].dh) widgets[i].dh=1;

			widgets[i].widget = device->create_widget(device, sizeof(dg_widget_t), 1+get_rand_pos()%(device->xres-W_WIDTH-1), 1+get_rand_pos()%(device->yres-W_HEIGHT-1), W_WIDTH, W_HEIGHT);

			widgets[i].widget->render=test_render;
			widgets[i].widget->render(widgets[i].widget);
			widgets[i].widget->invalidate(widgets[i].widget, 0);
		}

		swap_buffers(device);

		started=1;
		do {
			test_performance();

			for (i=0; i<WIDGETS; i++)
			{
				widgets[i].widget->window->get_rect(widgets[i].widget->window, rect);
				if (rect->x>device->xres || rect->x<-rect->w)
				{
					widgets[i].dx=-widgets[i].dx;
				}

				if (rect->y>device->yres || rect->y<-rect->h)
				{
					widgets[i].dy=-widgets[i].dy;
				}

				if (rect->w < 2 || rect->w > 3*W_WIDTH)
				{
					widgets[i].dw=-widgets[i].dw;
				}

				if (rect->h < 2 || rect->h > 3*W_HEIGHT)
				{
					widgets[i].dh=-widgets[i].dh;
				}

				widgets[i].widget->set_rect(widgets[i].widget, rect->x+widgets[i].dx, rect->y+widgets[i].dy, rect->w+widgets[i].dw, rect->h+widgets[i].dh, 1);
			}
			fire_widget->render(fire_widget);
			fire_widget->invalidate(fire_widget, 0);
			swap_buffers(device);
			device->master_widget->event_manager->idle(device->master_widget->event_manager);
		} while (device->master_widget->event_manager->active);
		started=0;
		image->destroy(image);
		rect->destroy(rect);
	}
	return 1;
}

int main()
{
	int i,j;
#if defined(DEBUG) && defined(WIN32)
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	alpha_channel=malloc(W_WIDTH * W_HEIGHT);
	for (i=0; i<W_HEIGHT; i++)
		for (j=0; j<W_WIDTH; j++)
		{
			alpha_channel[i*W_WIDTH+j] = (char)(255-(255-((i+j)>255?255-(i+j):(i+j)))&0xFF);
			//alpha_channel[i*W_WIDTH+j] = 40;
		}
	start_sample(event_handler);
	free(alpha_channel);
#if defined(DEBUG) && defined(WIN32)
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
