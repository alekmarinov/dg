/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      sdl_surface.c                                      */
/* Description:   DG/SDL Surface implementation                      */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <dg/dg_blit.h>
#include <dg/dg_video.h>
#include "sdl_surface.h"
#include "sdl_video.h"
#include "sdl_blit.h"

/* surface methods */
static int sdl_surface_flip(dg_surface_p, int x, int y, dg_rect_p);
static void sdl_surface_destroy(dg_surface_p);

/* surface constructor */
dg_surface_p sdl_create_surface(dg_video_p device, int width, int height, int options);


int sdl_create_physical_surface(int width, int height, SDL_Surface **ppSurface, int* pOptions)
{
	Uint32 rmask, gmask, bmask, amask;

    amask = 0x00000000;
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;

	/* create SDL surface */
	if (pOptions && (*pOptions & DG_OPT_SURFACE_VIDEO))
	{
		/* create surface in video memory */
		*ppSurface=SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_ASYNCBLIT, 
			width, height, 32, rmask, gmask, bmask, amask);
		if ( !((*ppSurface)->flags & SDL_HWSURFACE) )
		{
			*pOptions&=~DG_OPT_SURFACE_VIDEO;
		}
	}
	else
	{
		/* create surface in system memory */
		*ppSurface=SDL_CreateRGBSurface(SDL_SWSURFACE, 
			width, height, 32, rmask, gmask, bmask, amask);
	}
	return (*ppSurface != 0);
}

int sdl_surface_resize(dg_surface_p self, int width, int height)
{
	if (self && self->context && width>0 && height>0 && (self->width != width || self->height != height))
	{
		SDL_Surface *lpOldSDSurface;
		sdl_surface_p surface = (sdl_surface_p)self->context; /* SDL surface context */
		sdl_video_p context  = (sdl_video_p)self->device->context; /* SDL device context */

		lpOldSDSurface=surface->lpSDSBack;

		/* create new SDL surface */
		if (sdl_create_physical_surface(width, height, &(surface->lpSDSBack), &(self->options)))
		{
			/* release the old surface */
			SDL_FreeSurface(lpOldSDSurface);
		} 
		else
		{
			/* Failed to create the new surface, restore the old */
			surface->lpSDSBack=lpOldSDSurface;
		}
	}
	dg_surface_resize(self, width, height);
	return 1;
}

int sdl_surface_clear(dg_surface_p self, dg_color_t color)
{
	if (self && self->context)
	{
		SDL_Rect rect={0, 0, self->width, self->height};
		sdl_surface_p surface = (sdl_surface_p)self->context;
		SDL_FillRect(surface->lpSDSBack, &rect, self->device->pack_color(self->device, color));
	}
	return 1;
}

int sdl_surface_fill_rect(dg_surface_p self, int x, int y, int width, int height, dg_color_t color)
{
	if (self && self->context)
	{
		SDL_Rect rect={x, y, width, height};
		sdl_surface_p surface = (sdl_surface_p)self->context;
		SDL_FillRect(surface->lpSDSBack, &rect, self->device->pack_color(self->device, color));
	}
	return 1;
}

static int sdl_surface_flip(dg_surface_p self, int x, int y, dg_rect_p rect)
{
	int result=0;
	if (self && self->context)
	{
		dg_rect_p srcRect=rect;
		dg_rect_p dstRect;
		sdl_surface_p surface = (sdl_surface_p)self->context;
		sdl_video_p context  = (sdl_video_p)self->device->context;
		dg_color_t dummy={0};

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

		result = sdl_surface_blit(context->screen, dstRect, self, srcRect, 0, 0, dummy, DG_OPT_BLIT_DEFAULT);

		if (srcRect != rect)
		{
			srcRect->destroy(srcRect);
		}
		dstRect->destroy(dstRect);
	}
	return result;
}

int sdl_surface_lock(dg_surface_p self, int is_pixels_converted)
{
	if (self && self->context)
	{
		sdl_surface_p surface = (sdl_surface_p)self->context;
		SDL_LockSurface(surface->lpSDSBack);
		self->pixels=(dg_color_p)surface->lpSDSBack->pixels;
		self->pitch=(unsigned long)surface->lpSDSBack->pitch;
		self->is_pixels_converted=1;
		return 1;
	}
	return 0;
}

int sdl_surface_unlock(dg_surface_p self)
{
	if (self && self->context)
	{
		sdl_surface_p surface = (sdl_surface_p)self->context;
		SDL_UnlockSurface(surface->lpSDSBack);
		return 1;
	}
	return 0;
}

int sdl_surface_set_color_key(dg_surface_p self, dg_color_t color_key)
{
	if (self && self->context)
	{
		sdl_surface_p surface = (sdl_surface_p)self->context;
		dg_color_t current_color_key=self->get_color_key(self);

		/* alpha != 0 in surface color key indicates the color key is unset */
		if (DG_GET_ALPHA(current_color_key) == 0)
		{
			if (DG_GET_RED(color_key) == DG_GET_RED(current_color_key)
				&& DG_GET_GREEN(color_key) == DG_GET_GREEN(current_color_key)
				&& DG_GET_BLUE(color_key) == DG_GET_BLUE(current_color_key))
			{
				/* not necessary to set the same color key again */
				return 1;
			}
		}

		/* alpha component is not used in the color key, */
		/* 0 indicates it is set to the surface */
		DG_SET_ALPHA(color_key, 0);

		SDL_SetColorKey(surface->lpSDSBack, SDL_SRCCOLORKEY | SDL_RLEACCEL, 
			self->device->pack_color(self->device, color_key));

		return dg_surface_set_color_key(self, color_key);
	}
	return 0;
}

static void sdl_surface_destroy(dg_surface_p self)
{
	if (self)
	{
		if (self->context)
		{
			sdl_surface_p surface = (sdl_surface_p)self->context;
			if (surface->lpSDSBack)
			{
				SDL_FreeSurface(surface->lpSDSBack);
				surface->lpSDSBack=0;
			}
			free(self->context);
		}
		dg_surface_destroy(self);
	}
}

dg_surface_p sdl_create_surface(dg_video_p device, int width, int height, int options)
{
	if (device && device->context)
	{
		sdl_video_p context=(sdl_video_p)device->context;
		sdl_surface_p screen_context=(sdl_surface_p)context->screen->context;

		/* allocate general surface */
		dg_surface_p self=dg_surface_create(device, width, height, options);

		/* allocate private surface */
		sdl_surface_p surface=(sdl_surface_p)self->context = malloc(sizeof(sdl_surface_t));
		memset(surface, 0, sizeof(sdl_surface_t));

		/* set surface methods */
		self->resize        = sdl_surface_resize;
		self->clear         = sdl_surface_clear;
		self->fill_rect     = sdl_surface_fill_rect;
		self->flip          = sdl_surface_flip;
		self->blit          = sdl_surface_blit;
		self->lock          = sdl_surface_lock;
		self->unlock        = sdl_surface_unlock;
		//self->create_gdc    = sdl_gdc_create;
		self->set_color_key = sdl_surface_set_color_key;
		self->destroy       = sdl_surface_destroy;

		/* create SDL surface */
		if (sdl_create_physical_surface(width, height, &(surface->lpSDSBack), &(self->options)))
		{
			return self;
		}

		/* unable to create surface */
		free(surface);
		free(self);
		printf("sdl_create_surface/sdl_create_physical_surface failed!");
		return 0;
	}
	return 0;
}
