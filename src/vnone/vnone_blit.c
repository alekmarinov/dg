/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      vnone_blit.c                                       */
/* Description:   DG/VNone blittings                                 */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_blit.h>
#include "vnone_blit.h"
#include "vnone_video.h"
#include "vnone_surface.h"
#include "vnone_error.h"

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
int vnone_surface_blit(dg_surface_p dSurface, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect, unsigned char alpha, unsigned char* alphaChannel, dg_color_t colorKey, int options)
{
	dg_video_p device = dSurface->device;
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

	/* delegate to SW blit */
	return dg_surface_blit(dSurface, dstRect, sSurface, srcRect, _alpha, _alphaChannel, _colorKey, options);
}
