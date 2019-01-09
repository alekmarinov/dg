/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      vnone_video.c                                      */
/* Description:   DG/VNone Video implementation                       */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_widget.h>
#include "vnone_video.h"
#include "vnone_surface.h"
#include "vnone_event.h"
#include "vnone_blit.h"
#include "vnone_error.h"

/*
	Disable warnings ('LONG' differs in levels of indirection from 'HBRUSH')
	in SetClassLong and SetWindowLong when assigning pointer instead of long
*/
#pragma warning (push)
#pragma warning( disable : 4047 4312 )

/* global definitions */
const char *vnone_app_name="DG/None App";

/*
* The pixel format as seen by the user
* amask = 0x00000000
* rmask = 0x00ff0000
* gmask = 0x0000ff00
* bmask = 0x000000ff
*/
static dg_pixel_format_t default_pixel_format={ 8, 8, 8, 0, 16, 8, 0, 32, 32 };

static int  vnone_unset_mode(dg_video_p);
static void vnone_clear_buffers(dg_video_p);
static void vnone_master_flip(dg_surface_p, int, int, dg_rect_p);
static int vnone_master_blit(dg_surface_p, dg_rect_p, dg_surface_p, dg_rect_p, unsigned char, unsigned char*, dg_color_t, int);
static void vnone_master_resize(dg_surface_p, int, int);
static void vnone_master_fill_rect(dg_surface_p, int, int, int, int, dg_color_t);
static void vnone_master_clear(dg_surface_p, dg_color_t);
static void vnone_destroy(dg_video_p);
static void vnone_video_init_master_widget(dg_video_p);

/* Clear primary and backbuffer surfaces */
static void vnone_clear_buffers(dg_video_p device)
{
	if (device && device->context)
	{
		vnone_video_p context=device->context;
		vnone_surface_p screen_context=(vnone_surface_p)context->screen->context;

		memset(screen_context->pixels, 0, device->get_xres(device) * device->get_yres(device) * sizeof(dg_color_t));
	}
}

static void vnone_master_flip(dg_surface_p self, int x, int y, dg_rect_p rect)
{
	/* do nothing */
}

static int vnone_master_blit(dg_surface_p self, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect, unsigned char alpha, unsigned char* alphaChannel, dg_color_t colorKey, int options)
{
	if (self && self->context)
	{
		vnone_video_p context=(vnone_video_p)self->device->context;
		return vnone_surface_blit(context->screen, dstRect, sSurface, srcRect, alpha, alphaChannel, colorKey, options);
	}
	return 0;
}

static void vnone_master_resize(dg_surface_p self, int width, int height)
{
	dg_surface_resize(self, width, height);
}

static void vnone_master_fill_rect(dg_surface_p self, int x, int y, int width, int height, dg_color_t color)
{
	vnone_video_p context=(vnone_video_p)self->device->context;
	if (context)
	{
		vnone_surface_fill_rect(context->screen, x, y, width, height, color);
	}
}

static void vnone_master_clear(dg_surface_p self, dg_color_t color)
{
	vnone_master_fill_rect(self, 0, 0, self->width, self->height, color);
}

static void vnone_no_render(dg_widget_p self)
{
}

static void vnone_destroy(dg_video_p self)
{
	if (self)
	{
		vnone_video_p context=self->context;
		if (context)
		{
			vnone_surface_p screen_context=(vnone_surface_p)context->screen->context;

			if (self->background_image)
				self->background_image->destroy(self->background_image);

			/* Destroy master surface */
			dg_widget_destroy(self->master_widget);

			/* destroy screen context */
			free(context->screen->context);
			dg_surface_destroy(context->screen);

			/* destroy context */
			free(context);
		}

		/* destroy device */
		dg_video_destroy(self);
	}
}

static void vnone_video_set_mouse_capture(dg_video_p self, int is_capture)
{
	/* do nothing */
}

static void vnone_video_init_master_widget(dg_video_p device)
{
	device->master_widget->surface = dg_surface_create(device, device->xres, device->yres, 0);
	device->master_widget->surface->context=device->context;
	device->master_widget->surface->flip=vnone_master_flip;
	device->master_widget->surface->blit=vnone_master_blit;
	device->master_widget->surface->resize=vnone_master_resize;
	device->master_widget->surface->clear=vnone_master_clear;
	device->master_widget->surface->fill_rect=vnone_master_fill_rect;
	device->master_widget->surface->lock=vnone_surface_lock;
	device->master_widget->surface->unlock=vnone_surface_unlock;
	device->master_widget->render=vnone_no_render;
}

int vnone_video_create(struct dg_video_t* _device)
{
	dg_video_p device=(dg_video_p)_device;
	vnone_video_p self=(vnone_video_p)malloc(sizeof(vnone_video_t));
	memset(self, 0, sizeof(vnone_video_t));

	/* set vnone context */
	device->context=(void*)self;

	/* set device pixel format */
	device->pixel_format = default_pixel_format;

	/* initialize main null screen */
	self->screen=vnone_create_surface(device, device->xres, device->yres, 0);

	/* set options to vnone context */
	self->options = device->options;

	/* reset the video memory */
	vnone_clear_buffers(device);

	/* set null surface create method */
	device->create_surface = vnone_create_surface;

	/* set null event manager */
	device->master_widget->event_manager->destroy(device->master_widget->event_manager);
	device->master_widget->event_manager = vnone_create_event_manager(device);

	/* set null mouse capture */
	device->set_mouse_capture = vnone_video_set_mouse_capture;

	/* set video device destroyer */
	device->destroy = vnone_destroy;

	/* set video active */
	device->active = 1;

	/* init master widget */
	vnone_video_init_master_widget(device);

	return 1;
}

#pragma warning (pop)
