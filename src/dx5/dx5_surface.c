/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_surface.c                                      */
/* Description:   DG/DirectX Surface implementation                  */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_video.h>
#include "dx5_surface.h"
#include "dx5_video.h"
#include "dx5_blit.h"
#include "dx5_error.h"

/*
* The pixel format as seen by the user
* amask = 0x00000000
* rmask = 0x00ff0000
* gmask = 0x0000ff00
* bmask = 0x000000ff
*/
static dg_pixel_format_t default_pixel_format={ 8, 8, 8, 0, 16, 8, 0, 32, 32 };


/* surface methods */
static int dx5_surface_resize(dg_surface_p, int, int);
static int dx5_surface_flip(dg_surface_p, int x, int y, dg_rect_p);
static void dx5_surface_destroy(dg_surface_p);

/* surface constructor */
dg_surface_p dx5_create_surface(dg_video_p device, int width, int height, int options);


static int dx5_surface_resize(dg_surface_p self, int width, int height)
{
	assert(self->lock_count == 0);
	if (self && self->context && width>0 && height>0 && (self->width != width || self->height != height))
	{
		LPDIRECTDRAWSURFACE lpOldDDSurface;
		dx5_surface_p surface = (dx5_surface_p)self->context; /* DX5 surface context */
		dx5_video_p context  = surface->device->context; /* DX5 device context */

		lpOldDDSurface=surface->lpDDSBack;

		/* create new direct draw surface */
		if (SUCCEEDED(dx5_create_physical_surface(context->lpDD, width, height, &(surface->lpDDSBack), &surface->ddsd, &(self->options))))
		{
			/* release the old surface */
			IDirectDrawSurface_Release(lpOldDDSurface);
		} 
		else
		{
			/* Failed to create the new surface, restore the old */
			surface->lpDDSBack=lpOldDDSurface;
		}
	}
	dg_surface_resize(self, width, height);
	return 1;
}

int dx5_surface_clear(dg_surface_p self, dg_color_t color)
{
	if (self && self->context)
	{
		HRESULT hResult;
		dx5_surface_p surface = (dx5_surface_p)self->context;
	    DDBLTFX ddBltFx;
		ddBltFx.dwSize = sizeof(DDBLTFX);
		ddBltFx.dwFillColor = self->device->pack_color(self->device, color);

		hResult = IDirectDrawSurface_Blt(surface->lpDDSBack, NULL, NULL, NULL, /*DDBLT_WAIT*/ DDBLT_DONOTWAIT | DDBLT_COLORFILL, &ddBltFx);
		if (FAILED(hResult))
		{
			/* FIXME: Handle this error */
			return 0;
		}
	}
	return 1;
}

int dx5_surface_fill_rect(dg_surface_p self, int x, int y, int width, int height, dg_color_t color)
{
	if (self && self->context)
	{
		DDBLTFX ddBltFx;
		dx5_surface_p surface = (dx5_surface_p)self->context;
		RECT rect;
		HRESULT hResult;
		ddBltFx.dwSize = sizeof(DDBLTFX);
		ddBltFx.dwFillColor = self->device->pack_color(self->device, color);

		SetRect(&rect, x, y, x+width, y+height);

		hResult=IDirectDrawSurface_Blt(surface->lpDDSBack, &rect, NULL, NULL, /*DDBLT_WAIT*/DDBLT_DONOTWAIT | DDBLT_COLORFILL, &ddBltFx);
		if (FAILED(hResult))
		{
			/* FIXME: Handle this error */
			return 0;
		}
	}
	return 1;
}

static int dx5_surface_flip(dg_surface_p self, int x, int y, dg_rect_p rect)
{
	int result=0;
	if (self && self->context)
	{
		dg_rect_p srcRect=rect;
		dg_rect_p dstRect;
		dx5_surface_p surface = (dx5_surface_p)self->context;
		dx5_video_p context  = (dx5_video_p)self->device->context;
		dg_color_t dummy={0};

		if (rect)
		{
			dstRect=dg_rect_create(rect->x+x, rect->y+y, rect->w, rect->h);
		}
		else
		{
			dstRect=dg_rect_create(x, y, self->width, self->height);
		}

		if (!(surface->device->options & (DG_OPT_DOUBLE_BUFFERING | DG_OPT_FULLSCREEN)))
		{
			dstRect->move(dstRect, dstRect->x+context->rectClient.left, dstRect->y+context->rectClient.top);
		}

		if (!srcRect)
		{
			srcRect=dg_rect_create(0, 0, self->width, self->height);
		}

		result = dx5_surface_blit(context->screen, dstRect, self, srcRect, 0, 0, dummy, DG_OPT_BLIT_DEFAULT);

		if (srcRect != rect)
		{
			srcRect->destroy(srcRect);
		}
		dstRect->destroy(dstRect);
	}
	return result;
}

int dx5_surface_lock(dg_surface_p self, int is_pixels_converted)
{
	if (self && self->context)
	{
		self->lock_count++;
		if (self->lock_count == 1)
		{
			HRESULT hResult;
			dx5_surface_p surface = (dx5_surface_p)self->context;
			memset(&surface->ddsd, 0, sizeof(DDSURFACEDESC));
			surface->ddsd.dwSize = sizeof(DDSURFACEDESC);
			hResult=IDirectDrawSurface_Lock(surface->lpDDSBack, NULL, &surface->ddsd, DDLOCK_READONLY | DDLOCK_WRITEONLY | DDLOCK_WAIT, 0);
			if (SUCCEEDED(hResult))
			{
				if (is_pixels_converted)
				{
					self->pitch=self->width * sizeof(dg_color_t);
					self->pixels=(dg_color_p)malloc(self->height * self->pitch);
					dg_pixel_format_convert((struct dg_pixel_format_t*)&default_pixel_format, (void*)self->pixels, self->pitch, (struct dg_pixel_format_t*)&self->device->pixel_format, surface->ddsd.lpSurface, surface->ddsd.lPitch, self->width, self->height);
				}
				else
				{
					self->pitch=surface->ddsd.lPitch;
					self->pixels=(dg_color_p)surface->ddsd.lpSurface;
				}

				self->is_pixels_converted=is_pixels_converted;
				return 1;
			}
			else
			{
				self->lock_count--;
				DX5_CHECK_RESULT(hResult);
				return 0;
			}
		}
	}
	return 0;
}

int dx5_surface_unlock(dg_surface_p self)
{
	if (self->lock_count>1)
	{
		self->lock_count--;
	}
	else
	if (self && self->context)
	{
		dx5_surface_p surface = (dx5_surface_p)self->context;

		if (self->is_pixels_converted)
		{
			dg_pixel_format_convert((struct dg_pixel_format_t*)&self->device->pixel_format, (unsigned char*)surface->ddsd.lpSurface, surface->ddsd.lPitch, (struct dg_pixel_format_t*)&default_pixel_format, (unsigned char*)self->pixels, self->pitch, self->width, self->height);
			free(self->pixels);
			self->pixels=0;
			self->is_pixels_converted=0;
		}

		self->lock_count=0;
		memset(&surface->ddsd, 0, sizeof(DDSURFACEDESC));
		surface->ddsd.dwSize = sizeof(DDSURFACEDESC);
		return IDirectDrawSurface_Unlock(surface->lpDDSBack, surface->ddsd.lpSurface) == DD_OK;
	}
	return 0;
}

int dx5_surface_set_color_key(dg_surface_p self, dg_color_t color_key)
{
	if (self)
	{
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

		if (self->context)
		{
			if (self->lock(self, 0))
			{
				DDCOLORKEY ddck;
				HRESULT hResult;
				dx5_surface_p surface = (dx5_surface_p)self->context;
				DWORD dwColor = self->device->pack_color(self->device, color_key);
				ddck.dwColorSpaceLowValue  = dwColor;
				ddck.dwColorSpaceHighValue = dwColor;
				hResult=IDirectDrawSurface_SetColorKey(surface->lpDDSBack, DDCKEY_SRCBLT, &ddck);
				DX5_CHECK_RESULT(hResult);
				self->unlock(self);
			}
		}
		return dg_surface_set_color_key(self, color_key);
	}
	return 0;
}

static void dx5_surface_destroy(dg_surface_p self)
{
	if (self)
	{
		if (self->context)
		{
			dx5_surface_p surface = (dx5_surface_p)self->context;
			if (surface->lpDDSBack)
			{
				IDirectDrawSurface_Release(surface->lpDDSBack);
			}
			free(self->context);
		}
		dg_surface_destroy(self);
	}
}

HRESULT dx5_create_physical_surface(LPDIRECTDRAW lpDD, int width, int height, LPDIRECTDRAWSURFACE* ppSurface, LPDDSURFACEDESC pDDSD, int* pOptions)
{
	HRESULT hResult;

	/* create direct draw surface */
	memset(pDDSD, 0, sizeof(DDSURFACEDESC));
	pDDSD->dwSize = sizeof(DDSURFACEDESC);
	pDDSD->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	pDDSD->dwWidth  = width;
	pDDSD->dwHeight = height;

	if (pOptions && (*pOptions & DG_OPT_SURFACE_VIDEO))
	{
		/* create surface in video memory */
		pDDSD->ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		hResult = IDirectDraw_CreateSurface(lpDD, pDDSD, ppSurface, NULL);
		if (SUCCEEDED(hResult))
		{
			return hResult;
	    }
	}

	/* create surface in system memory */
	pDDSD->ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	hResult = IDirectDraw_CreateSurface(lpDD, pDDSD, ppSurface, NULL);
	if (SUCCEEDED(hResult)) 
	{
		/* reset surface's video mem flag */
		if (pOptions)
		{
			*pOptions&=~DG_OPT_SURFACE_VIDEO;
		}
	}	
	return hResult;
}

dg_surface_p dx5_create_surface(dg_video_p device, int width, int height, int options)
{
	if (device && device->context)
	{
		HRESULT hResult;
		dx5_video_p context=device->context;
		dx5_surface_p screen_context=(dx5_surface_p)context->screen->context;

		/* allocate general surface */
		dg_surface_p self=dg_surface_create(device, width, height, options);

		/* allocate private surface */
		dx5_surface_p surface=(dx5_surface_p)self->context = malloc(sizeof(dx5_surface_t));
		memset(surface,0,sizeof(dx5_surface_t));

		/* set video device to this surface */
		surface->device = device;

		/* set surface methods */
		self->resize        = dx5_surface_resize;
		self->clear         = dx5_surface_clear;
		self->fill_rect     = dx5_surface_fill_rect;
		self->flip          = dx5_surface_flip;
		self->blit          = dx5_surface_blit;
		self->lock          = dx5_surface_lock;
		self->unlock        = dx5_surface_unlock;
		self->set_color_key = dx5_surface_set_color_key;
		self->destroy       = dx5_surface_destroy;

		surface->lpDDSPrimary=screen_context->lpDDSPrimary;

		/* create direct draw surface */
		hResult=dx5_create_physical_surface(context->lpDD, width, height, &(surface->lpDDSBack), &surface->ddsd, &(self->options));
		DX5_CHECK_RESULT(hResult);
		if (SUCCEEDED(hResult))
		{
			return self;
		}

		/* unable to create surface */
		free(surface);
		free(self);
		printf("dx5_create_surface/dx5_create_physical_surface failed!");
		return 0;
	}
	return 0;
}
