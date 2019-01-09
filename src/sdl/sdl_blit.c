/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      sdl_blit.c                                         */
/* Description:   DG/SDL blittings                                   */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_blit.h>
#include "sdl_blit.h"
#include "sdl_video.h"
#include "sdl_surface.h"

static int sdl_blit_copy(dg_surface_p, dg_rect_p, dg_surface_p, dg_rect_p);
static int sdl_blit_color_key(dg_surface_p, dg_rect_p, dg_surface_p, dg_rect_p, dg_color_t);

/* 
* SDL stretched copy
* dSurface - the destination surface
* dstRect - the destination rectangle
* sSurface - the source surface
* srcRect - the source rectangle
*/
static int sdl_blit_copy(dg_surface_p dSurface, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect)
{
	SDL_Rect srect={srcRect->x, srcRect->y, srcRect->w, srcRect->h};
	SDL_Rect drect={dstRect->x, dstRect->y, dstRect->w, dstRect->h};

	/* FIXME: Check for NULL pointer */
	SDL_Surface* lpSrcSDS=((sdl_surface_p)sSurface->context)->lpSDSBack;
	SDL_Surface* lpDstSDS=((sdl_surface_p)dSurface->context)->lpSDSBack;

	if (srcRect->w == dstRect->w && srcRect->h == dstRect->h)
	{
		/* fast blit */		
		SDL_BlitSurface(lpSrcSDS, &srect, lpDstSDS, &drect);
		return 1;
	}
	else
	{
		/* Scaling the surface */
		SDL_Surface* resizedSurface;
		SDL_Surface* srcSubSurface;
		if (sdl_create_physical_surface(dstRect->w, dstRect->h, &resizedSurface, 0))
		{
			if (sdl_create_physical_surface(srcRect->w, srcRect->h, &srcSubSurface, 0))
			{
				SDL_BlitSurface(lpSrcSDS, &srect, srcSubSurface, 0);

				SDL_LockSurface(srcSubSurface);
				SDL_LockSurface(resizedSurface);
				dg_scale_pixel_buffer(32, resizedSurface->pixels, dstRect->w, dstRect->h, srcSubSurface->pixels, srcRect->w, srcRect->h);
				SDL_UnlockSurface(resizedSurface);
				SDL_UnlockSurface(srcSubSurface);

				SDL_BlitSurface(resizedSurface, 0, lpDstSDS, &drect);
			}
		}
	}
	return 0;
}

/* 
* SDL stretched copy with color key
* dSurface - the destination surface
* dstRect - the destination rectangle
* sSurface - the source surface
* srcRect - the source rectangle
* colorKey - color key
*/
static int sdl_blit_color_key(dg_surface_p dSurface, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect, dg_color_t colorKey)
{
	/* set color key */
	sSurface->set_color_key(sSurface, colorKey);

	/* SDL blit with color key */
	return sdl_blit_copy(dSurface, dstRect, sSurface, srcRect);
}

/* 
* dx5_surface_blit copies a bitmap from a source rectangle into destination rectangle
* dSurface - the destination surface
* dstRect - the destination rectangle
* sSurface - the source surface
* srcRect - the source rectangle
* alpha - transparency level changing from 0 to 255 changes from invisible to visible
* alphaChannel - transparency level changing from 0 to 255 changes from invisible to visible
* colorKey - transparent color
* options - enable or disable any of alpha, alphaChannel, colorKey parameters
*/
int sdl_surface_blit(dg_surface_p dSurface, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect, unsigned char alpha, unsigned char* alphaChannel, dg_color_t colorKey, int options)
{
	int result;
	dg_video_p device=dSurface->device;
	unsigned char _alpha;
	unsigned char* _alphaChannel;
	dg_color_t _colorKey;

	if (options == DG_OPT_BLIT_DEFAULT)
	{
		_alpha        = sSurface->alpha;
		_alphaChannel = sSurface->alpha_channel;
		_colorKey     = sSurface->color_key;

		options = 0;
		if (sSurface->options & DG_OPT_SURFACE_COLOR_KEY)
		{
			options|=DG_OPT_BLIT_COLOR_KEY;
		}
		if (sSurface->options & DG_OPT_SURFACE_ALPHA)
		{
			options|=DG_OPT_BLIT_ALPHA;
		}
		if (sSurface->options & DG_OPT_SURFACE_ALPHA_CHANNEL)
		{
			options|=DG_OPT_BLIT_ALPHA_CHANNEL;
		}

		if (!options) 
		{
			options=DG_OPT_BLIT_COPY;
		}
	}
	else
	{
		_alpha=alpha;
		_alphaChannel=alphaChannel;
		_colorKey=colorKey;
	}

	/* Using SDL HW accelerated blits with or without color key */
	/* but software alpha & alpha channel blending */

	if (options & ~(DG_OPT_BLIT_COPY | DG_OPT_BLIT_COLOR_KEY))
	{
		/* delegate to SW blit */
		result = dg_surface_blit(dSurface, dstRect, sSurface, srcRect, _alpha, _alphaChannel, _colorKey, options);
	}
	else
	{
		/* HW blit */
		if (options & DG_OPT_BLIT_COLOR_KEY)
		{
			/* delegate to HW blit with color key */
			result = sdl_blit_color_key(dSurface, dstRect, sSurface, srcRect, _colorKey);
		}
		else
		{
			/* delegate to HW blit */
			result = sdl_blit_copy(dSurface, dstRect, sSurface, srcRect);
		}
	}

	return result;
}
