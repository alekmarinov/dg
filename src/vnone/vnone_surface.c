/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      vnone_surface.c                                    */
/* Description:   DG/None Surface implementation                     */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_video.h>
#include "vnone_surface.h"
#include "vnone_video.h"
#include "vnone_blit.h"
#include "vnone_error.h"

/* surface methods */
static int vnone_surface_resize(dg_surface_p, int, int);
static int vnone_surface_flip(dg_surface_p, int x, int y, dg_rect_p);
static void vnone_surface_destroy(dg_surface_p);

/* surface constructor */
dg_surface_p vnone_create_surface(dg_video_p device, int width, int height, int options);

static int vnone_surface_resize(dg_surface_p self, int width, int height)
{
	assert(self->lock_count == 0);
	if (self && self->context && width>0 && height>0 && (self->width != width || self->height != height))
	{
		vnone_surface_p surface = (vnone_surface_p)self->context; /* None surface context */
		vnone_video_p context  = surface->device->context; /* None device context */

		if (surface->pixels)
			free(surface->pixels);

		surface->pixels = (dg_color_p)malloc(width * height * sizeof(dg_color_t));
	}
	dg_surface_resize(self, width, height);
	return 1;
}

int vnone_surface_clear(dg_surface_p self, dg_color_t color)
{
	if (self && self->context)
	{
		vnone_surface_p surface = (vnone_surface_p)self->context;
		int count = self->width * self->height;
		int i;
		for (i=0; i<count; i++)
			surface->pixels[i] = color;
		return 1;
	}
	return 0;
}

int vnone_surface_fill_rect(dg_surface_p self, int x, int y, int width, int height, dg_color_t color)
{
	if (self && self->context)
	{
		vnone_surface_p surface = (vnone_surface_p)self->context;
		dg_color_p addr = surface->pixels + x + y * self->width;
		int next_line = self->width - width;
		int w;
		while (height--)
		{
			w = width;
			while (w--) *addr++ = color;
			addr += next_line;
		}
		return 1;
	}
	return 0;
}

static int vnone_surface_flip(dg_surface_p self, int x, int y, dg_rect_p rect)
{
	int result = 0;
	if (self && self->context)
	{
		dg_rect_p srcRect=rect;
		dg_rect_p dstRect;
		vnone_surface_p surface = (vnone_surface_p)self->context;
		vnone_video_p context = (vnone_video_p)self->device->context;

		if (rect)
		{
			dstRect=dg_rect_create(rect->x+x, rect->y+y, rect->w, rect->h);
		}
		else
		{
			dstRect=dg_rect_create(x, y, self->width, self->height);
		}

		if (!srcRect)
		{
			srcRect=dg_rect_create(0, 0, self->width, self->height);
		}

		result = vnone_surface_blit(context->screen, dstRect, self, srcRect, 0, 0, 0, DG_OPT_BLIT_DEFAULT);

		if (srcRect != rect)
		{
			srcRect->destroy(srcRect);
		}
		dstRect->destroy(dstRect);
	}
	return result;
}

int vnone_surface_lock(dg_surface_p self, int is_pixels_converted)
{
	if (self && self->context)
	{
		self->lock_count++;
		self->is_pixels_converted=1;
		self->pitch=self->width * sizeof(dg_color_t);
		self->pixels=((vnone_surface_p)self->context)->pixels;
		return 1;
	}
	return 0;
}

int vnone_surface_unlock(dg_surface_p self)
{
	if (self->lock_count>0)
	{
		self->lock_count--;
		return 1;
	}
	return 0;
}

static void vnone_surface_destroy(dg_surface_p self)
{
	if (self)
	{
		if (self->context)
		{
			vnone_surface_p surface = (vnone_surface_p)self->context;
			if (surface->pixels)
			{
				free(surface->pixels);
			}
			free(self->context);
		}
		dg_surface_destroy(self);
	}
}

dg_surface_p vnone_create_surface(dg_video_p device, int width, int height, int options)
{
	if (device && device->context)
	{
		/* allocate general surface */
		dg_surface_p self=dg_surface_create(device, width, height, options);

		/* allocate private surface */
		vnone_surface_p surface=(vnone_surface_p)self->context = malloc(sizeof(vnone_surface_t));
		memset(surface, 0, sizeof(vnone_surface_t));

		/* set video device to this surface */
		surface->device = device;

		/* set surface methods */
		self->resize        = vnone_surface_resize;
		self->clear         = vnone_surface_clear;
		self->fill_rect     = vnone_surface_fill_rect;
		self->flip          = vnone_surface_flip;
		self->blit          = vnone_surface_blit;
		self->lock          = vnone_surface_lock;
		self->unlock        = vnone_surface_unlock;
		self->destroy       = vnone_surface_destroy;

		/* create Null surface */
		surface->pixels = (dg_color_p)malloc(width * height * sizeof(dg_color_t));

		if (surface->pixels)
			return self;

		/* unable to create surface */
		free(surface);
		free(self);
	}
	return 0;
}
