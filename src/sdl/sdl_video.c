/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      sdl_video.c                                        */
/* Description:   DG/SDL Video implementation                        */
/*                                                                   */
/*********************************************************************/

#include <string.h>
#include <malloc.h>
#include <dg/dg_debug.h>
#include <dg/dg_widget.h>
#include "sdl_video.h"
#include "sdl_surface.h"
#include "sdl_event.h"
#include "sdl_blit.h"

const char *sdl_app_name="DG/SDL App";

static void sdl_pack_mask(unsigned long, unsigned char*, unsigned char*);
static int sdl_set_mode(dg_video_p device);
static void sdl_clear_buffers(dg_video_p);
static void sdl_master_flip(dg_surface_p, int, int, dg_rect_p);
static int sdl_master_blit(dg_surface_p, dg_rect_p, dg_surface_p, dg_rect_p, unsigned char, unsigned char*, dg_color_t, int);
static void sdl_master_resize(dg_surface_p, int, int);
static void sdl_master_fill_rect(dg_surface_p, int, int, int, int, dg_color_t);
static void sdl_master_clear(dg_surface_p, dg_color_t);
static int  sdl_master_lock(dg_surface_p, void**, unsigned long*);
static int  sdl_master_unlock(dg_surface_p);
static void sdl_destroy(dg_video_p);
static void sdl_video_init_master_widget(dg_video_p);
static void sdl_video_set_mouse_capture(dg_video_p, int);
static unsigned long sdl_pack_color(dg_video_p self, dg_color_t color);
static dg_color_t sdl_unpack_color(dg_video_p self, unsigned long color);

static void sdl_pack_mask(unsigned long mask, unsigned char* bits, unsigned char* shifts)
{
	int maxBits=32;
	*bits=0;
	*shifts=0;
	while (maxBits>0 && (mask & 1) == 0)
	{
		(*shifts)++;
		mask>>=1;
		maxBits--;
	}

	while (maxBits>0 && (mask & 1) == 1)
	{
		(*bits)++;
		mask>>=1;
		maxBits--;
	}
}

static int sdl_set_mode(dg_video_p device)
{
	sdl_video_p context=device->context;
	sdl_surface_p screen_context;
	int sdl_options=0;

	if (device->options & DG_OPT_FULLSCREEN)
		sdl_options|=SDL_FULLSCREEN;

	if (device->options & DG_OPT_DOUBLE_BUFFERING)
		sdl_options|=SDL_DOUBLEBUF;

	screen_context = (sdl_surface_p)context->screen->context;
	screen_context->lpSDSBack=SDL_SetVideoMode(device->xres, device->yres, device->bpp, sdl_options);
	if (screen_context->lpSDSBack)
	{
		/* set color info */
		sdl_pack_mask(screen_context->lpSDSBack->format->Rmask, &device->pixel_format.bRedBits, &device->pixel_format.bRedShift);
		sdl_pack_mask(screen_context->lpSDSBack->format->Gmask, &device->pixel_format.bGreenBits, &device->pixel_format.bGreenShift);
		sdl_pack_mask(screen_context->lpSDSBack->format->Bmask, &device->pixel_format.bBlueBits, &device->pixel_format.bBlueShift);
		sdl_pack_mask(screen_context->lpSDSBack->format->Amask, &device->pixel_format.bAlphaBits, &device->pixel_format.bAlphaShift);
		device->pixel_format.bitsPerPixel = (unsigned char)screen_context->lpSDSBack->format->BitsPerPixel;
		return 1;
	}
	return 0;
}

static void sdl_master_flip(dg_surface_p self, int x, int y, dg_rect_p rect)
{
	if (self && self->context)
	{
		sdl_video_p context=self->context;
		sdl_surface_p screen_context=(sdl_surface_p)context->screen->context;

		SDL_Flip(screen_context->lpSDSBack);
	}
}

static int sdl_master_blit(dg_surface_p self, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect, unsigned char alpha, unsigned char* alphaChannel, dg_color_t colorKey, int options)
{
	if (self && self->context)
	{
		sdl_video_p context=self->context;
		return sdl_surface_blit(context->screen, dstRect, sSurface, srcRect, alpha, alphaChannel, colorKey, options);
	}
	return 0;
}

static void sdl_master_resize(dg_surface_p self, int width, int height)
{
	sdl_surface_resize(self, width, height);
}

static void sdl_master_fill_rect(dg_surface_p self, int x, int y, int width, int height, dg_color_t color)
{
	sdl_video_p context=self->context;
	sdl_surface_fill_rect(context->screen, x, y, width, height, color);
}

static void sdl_master_clear(dg_surface_p self, dg_color_t color)
{
	sdl_master_fill_rect(self, 0, 0, self->width, self->height, color);
}

static void sdl_no_render(dg_widget_p self)
{
}

static void sdl_destroy(dg_video_p self)
{
	if (self)
	{
		sdl_video_p context=self->context;
		if (context)
		{
			sdl_surface_p screen_context=(sdl_surface_p)context->screen->context;

			/* Destroy master surface */
			self->master_widget->destroy(self->master_widget);

			/* destroy screen context */
			free(context->screen->context);
			dg_surface_destroy(context->screen);

			/* destroy context */
			free(context);
		}
		/* destroy device */
		dg_video_destroy(self);

		SDL_Quit();
	}
}

static void sdl_video_set_mouse_capture(dg_video_p self, int is_capture)
{
	if (self && self->context)
	{
		if (is_capture)
		{
			SDL_WM_GrabInput(SDL_GRAB_ON);
		}
		else
		{
			SDL_WM_GrabInput(SDL_GRAB_OFF);
		}
	}
}

static unsigned long sdl_pack_color(dg_video_p self, dg_color_t color)
{
	sdl_video_p context=self->context;
	sdl_surface_p screen_context=(sdl_surface_p)context->screen->context;
	return SDL_MapRGBA(screen_context->lpSDSBack->format, DG_GET_RED(color), DG_GET_GREEN(color), DG_GET_BLUE(color), DG_GET_ALPHA(color));
}

static dg_color_t sdl_unpack_color(dg_video_p self, unsigned long color)
{
	unsigned char r, g, b, a;
	sdl_video_p context=self->context;
	sdl_surface_p screen_context=(sdl_surface_p)context->screen->context;
	SDL_GetRGBA(color, screen_context->lpSDSBack->format, &r, &g, &b, &a);
	return DG_RGB_COLOR(r, g, b, a);
}

static void sdl_video_init_master_widget(dg_video_p device)
{
	device->master_widget->surface = dg_surface_create(device, device->xres, device->yres, 0);
	device->master_widget->surface->context=device->context;
	device->master_widget->surface->flip=sdl_master_flip;
	device->master_widget->surface->blit=sdl_master_blit;
	device->master_widget->surface->resize=sdl_master_resize;
	device->master_widget->surface->clear=sdl_master_clear;
	device->master_widget->surface->fill_rect=sdl_master_fill_rect;
	device->master_widget->surface->lock=sdl_surface_lock;
	device->master_widget->surface->unlock=sdl_surface_unlock;
	device->master_widget->render=sdl_no_render;
}

int sdl_video_create(struct dg_video_t* _device)
{
	sdl_video_p self=(sdl_video_p)malloc(sizeof(sdl_video_t));
	dg_video_p device=(dg_video_p)_device;
	sdl_surface_p screen_context;
	memset(self, 0, sizeof(sdl_video_t));

	/* set SDL context */
	device->context=self;

	/* initialize main SDL screen */
	self->screen=dg_surface_create(device, device->xres, device->yres, 0);
	screen_context=self->screen->context=(sdl_surface_p)malloc(sizeof(sdl_surface_t));
	memset(screen_context, 0, sizeof(sdl_surface_t));

	self->screen->lock=sdl_surface_lock;
	self->screen->unlock=sdl_surface_unlock;
	self->screen->fill_rect=sdl_surface_fill_rect;
	self->screen->set_color_key=sdl_surface_set_color_key;

	/* Initialize SDL */
	SDL_Init(SDL_INIT_VIDEO);

	/* set video mode */
	if (!sdl_set_mode(device))
	{
		printf("Unable to set video mode!");
		return 0;
	}

	SDL_WM_SetCaption(sdl_app_name, 0);

	/* set SDL color packing */
	device->pack_color = sdl_pack_color;

	/* set SDL color unpacking */
	device->unpack_color = sdl_unpack_color;

	/* set SDL surface create method */
	device->create_surface = sdl_create_surface;

	/* set SDL event manager */
	device->master_widget->event_manager->destroy(device->master_widget->event_manager);
	device->master_widget->event_manager = sdl_create_event_manager(device);

	/* set SDL mouse capture */
	device->set_mouse_capture = sdl_video_set_mouse_capture;

	/* set video device destroyer */
	device->destroy = sdl_destroy;

	/* init master widget */
	sdl_video_init_master_widget(device);

	return 1;
}
