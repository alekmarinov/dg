/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_blit.c                                         */
/* Description:   DG/DirectX blittings                               */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_blit.h>
#include "dx5_blit.h"
#include "dx5_video.h"
#include "dx5_surface.h"
#include "dx5_error.h"

static int dx5_blit_copy(dg_surface_p, dg_rect_p, dg_surface_p, dg_rect_p);
static int dx5_blit_color_key(dg_surface_p, dg_rect_p, dg_surface_p, dg_rect_p, dg_color_t);

/* 
* HW stretched copy
* dSurface - the destination surface
* dstRect - the destination rectangle
* sSurface - the source surface
* srcRect - the source rectangle
*/
static int dx5_blit_copy(dg_surface_p dSurface, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect)
{
	HRESULT hResult=-1;
	RECT srect, drect;
	int swidth, sheight, dwidth, dheight;

	/* FIXME: Check for NULL pointer */
	LPDIRECTDRAWSURFACE lpSrcDDS=((dx5_surface_p)sSurface->context)->lpDDSBack;
	LPDIRECTDRAWSURFACE lpDstDDS=((dx5_surface_p)dSurface->context)->lpDDSBack;

	if (srcRect)
	{
		SetRect(&srect, srcRect->x, srcRect->y, srcRect->x+srcRect->w, srcRect->y+srcRect->h); 
		swidth = srcRect->w;
		sheight = srcRect->h;
	}
	else
	{
		SetRect(&srect, 0, 0, sSurface->width, sSurface->height); 
		swidth = sSurface->width;
		sheight = sSurface->height;
	}

	if (dstRect)
	{
		SetRect(&drect, dstRect->x, dstRect->y, dstRect->x+dstRect->w, dstRect->y+dstRect->h); 
		dwidth = dstRect->w;
		dheight = dstRect->h;
	}
	else
	{
		SetRect(&drect, 0, 0, dSurface->width, dSurface->height); 
		dwidth = dSurface->width;
		dheight = dSurface->height;
	}

	if (swidth == dwidth && sheight == dheight)
	{
		/* fast blit */		
		hResult=IDirectDrawSurface_BltFast(lpDstDDS, drect.left, drect.top, lpSrcDDS, &srect, DDBLTFAST_DONOTWAIT | DDBLTFAST_NOCOLORKEY /*DDBLTFAST_WAIT*/);
	}
	/* Note: problems has been introduced while fast bliting over the primary surface */
	if (FAILED(hResult))
	{
		/* stretched blit */
		hResult=IDirectDrawSurface_Blt(lpDstDDS, &drect, lpSrcDDS, &srect, DDBLT_DONOTWAIT | DDBLT_ASYNC /*DDBLT_WAIT*/, NULL);
	}
	return (SUCCEEDED(hResult))?1:0;
}

/* 
* HW stretched copy with color key
* dSurface - the destination surface
* dstRect - the destination rectangle
* sSurface - the source surface
* srcRect - the source rectangle
* colorKey - color key
*/
static int dx5_blit_color_key(dg_surface_p dSurface, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect, dg_color_t colorKey)
{
	HRESULT hResult=DD_FALSE;
	RECT srect, drect;

	/* FIXME: Check for NULL pointer */
	LPDIRECTDRAWSURFACE lpSrcDDS=((dx5_surface_p)sSurface->context)->lpDDSBack;
	LPDIRECTDRAWSURFACE lpDstDDS=((dx5_surface_p)dSurface->context)->lpDDSBack;

	SetRect(&srect, srcRect->x, srcRect->y, srcRect->x+srcRect->w, srcRect->y+srcRect->h); 
	SetRect(&drect, dstRect->x, dstRect->y, dstRect->x+dstRect->w, dstRect->y+dstRect->h); 

	/* set color key */
	sSurface->set_color_key(sSurface, colorKey);

	if (srcRect->w == dstRect->w && srcRect->h == dstRect->h)
	{
		/* fast blit with source color key */
		hResult=IDirectDrawSurface_BltFast(lpDstDDS, drect.left, drect.top, lpSrcDDS, &srect, DDBLTFAST_DONOTWAIT | DDBLTFAST_SRCCOLORKEY /*DDBLTFAST_WAIT */);
	}
	/* Note: problems has been introduced while fast bliting over the primary surface */
	if (FAILED(hResult))
	{
		/* stretched blit with source color key */
		hResult=IDirectDrawSurface_Blt(lpDstDDS, &drect, lpSrcDDS, &srect, DDBLT_DONOTWAIT | DDBLT_ASYNC | DDBLT_KEYSRC /*DDBLT_WAIT*/, NULL);	
	}
	return (SUCCEEDED(hResult))?1:0;
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
int dx5_surface_blit(dg_surface_p dSurface, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect, unsigned char alpha, unsigned char* alphaChannel, dg_color_t colorKey, int options)
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

	/* Using Direct Draw HW accelerated blits with or without color key */
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
			result = dx5_blit_color_key(dSurface, dstRect, sSurface, srcRect, _colorKey);
		}
		else
		{
			/* delegate to HW blit */
			result = dx5_blit_copy(dSurface, dstRect, sSurface, srcRect);
		}
	}

	return result;
}
