/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_blit.h                                          */
/* Description:   various buffer blittings                           */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_BLIT_H
#define __DG_BLIT_H

#include <dg/dg.h>
#include <dg/dg_pixel.h>
#include <dg/dg_surface.h>

/*!
	\addtogroup dg_blits Blits

	@{
*/

/*!	
* Use the default bliting operation as currently defined for the surface 
*/
#define DG_OPT_BLIT_DEFAULT       0

/*! 
* Copy surface disregarding the currently defined surface options 
* This flag disregards DG_OPT_BLIT_COLOR_KEY, DG_OPT_BLIT_ALPHA, DG_OPT_BLIT_ALPHA_CHANNEL
*/
#define DG_OPT_BLIT_COPY          (1 << 0)

/*!
* Copy surface with transparent color disregarding the currently defined surface options
* This flag can be combined with DG_OPT_BLIT_ALPHA, DG_OPT_BLIT_ALPHA_CHANNEL
*/
#define DG_OPT_BLIT_COLOR_KEY     (1 << 1)

/*!
* Copy surface with alpha level disregarding the currently defined surface options
* This flag can be combined with DG_OPT_BLIT_ALPHA_CHANNEL, DG_OPT_BLIT_COLOR_KEY
*/
#define DG_OPT_BLIT_ALPHA         (1 << 2)

/*!
* Use an alpha channel if available for the surface 
* This flag can be combined with DG_OPT_BLIT_ALPHA, DG_OPT_BLIT_COLOR_KEY
*/
#define DG_OPT_BLIT_ALPHA_CHANNEL (1 << 3)

/*!
* Blit surface from a source surface rectangle into destination surface rectangle
*/
DG_API int dg_surface_blit(
	/*! destination surface */
	dg_surface_p dSurface, 
	/*! destination rectangle */
	dg_rect_p dstRect, 
	/*! source surface */
	dg_surface_p sSurface, 
	/*! source rectangle */
	dg_rect_p srcRect, 
	/*! transparency level changing from 0 to 255 changes from invisible to visible */
	unsigned char alpha, 
	/*! transparency level changing from 0 to 255 changes from invisible to visible */
	unsigned char* alphaChannel, 
	/*! transparent color */
	dg_color_t colorKey, 
	/*! enable or disable any of alpha, alphaChannel, colorKey parameters */
	int options
	);

/*!
* Scales source pixel buffer to destination pixel buffer
*/
DG_API int dg_scale_pixel_buffer(
	/*! bits per pixel */
	int bitsPerPixel, 
	/*! destination buffer */
	unsigned char* dst, 
	/*! destination width */
	int dstWidth, 
	/*! destination height */
	int dstHeight, 
	/*! source buffer */
	unsigned char* src, 
	/*! source width */
	int srcWidth, 
	/*! source height */
	int srcHeight
	);

/*!
* Scales source pixel buffer to destination pixel buffer with color key
*/
DG_API int dg_scale_pixel_buffer_color_key(
	/*! bits per pixel */
	int bitsPerPixel, 
	/*! destination buffer */
	unsigned char* dst, 
	/*! destination width */
	int dstWidth, 
	/*! destination height */
	int dstHeight, 
	/*! source buffer */
	unsigned char* src, 
	/*! source width */
	int srcWidth, 
	/*! source height */
	int srcHeight,
	/*! color key */
	unsigned long color_key
	);

/*!
* Copy source pixel buffer to destination pixel buffer
*/
DG_API void dg_blit_copy(
	/*! bits per pixel */
	int bitsPerPixel,
	/*! destination buffer */
	unsigned char* dst,
	/*! source buffer */
	unsigned char* src,
	/*! buffer width */
	unsigned int width
	);

/*!
* Copy source pixel buffer to destination pixel buffer with color key
*/
DG_API void dg_blit_color_key(
	/*! bits per pixel */
	int bitsPerPixel,
	/*! destination buffer */
	unsigned char* dst,
	/*! source buffer */
	unsigned char* src,
	/*! buffer width */
	unsigned int width,
	/*! color key */
	unsigned long color_key
	);

/*!
* Copy source pixel buffer to destination pixel buffer with alpha factor
*/
DG_API void dg_blit_alpha(
	/*! bits per pixel */
	int bitsPerPixel,
	/*! destination buffer */
	unsigned char* dst,
	/*! source buffer */
	unsigned char* src,
	/*! buffer width */
	unsigned int width,
	/*! alpha factor */
	unsigned char alpha
	);

/*!
* Copy source pixel buffer to destination pixel buffer with alpha channel
*/
	void dg_blit_alpha_channel(
	/*! bits per pixel */
	int bitsPerPixel,
	/*! destination buffer */
	unsigned char* dst,
	/*! source buffer */
	unsigned char* src,
	/*! buffer width */
	unsigned int width,
	/*! alpha channel */
	unsigned char* alphaChannel
	);

/*!
* Copy source pixel buffer to destination pixel buffer with alpha factor and color key
*/
void dg_blit_alpha_color_key(
	/*! bits per pixel */
	int bitsPerPixel,
	/*! destination buffer */
	unsigned char* dst,
	/*! source buffer */
	unsigned char* src,
	/*! buffer width */
	unsigned int width,
	/*! alpha factor */
	unsigned char alpha,
	/*! color key */
	unsigned long color_key
	);

/*!
* Copy source pixel buffer to destination pixel buffer with alpha channel and alpha factor
*/
void dg_blit_alpha_channel_alpha(
	/*! bits per pixel */
	int bitsPerPixel,
	/*! destination buffer */
	unsigned char* dst,
	/*! source buffer */
	unsigned char* src,
	/*! buffer width */
	unsigned int width,
	/*! alpha factor */
	unsigned char alpha,
	/*! alpha channel */
	unsigned char* alphaChannel
	);

/*!
* Copy source pixel buffer to destination pixel buffer with alpha channel and color key
*/
void dg_blit_alpha_channel_color_key(
	/*! bits per pixel */
	int bitsPerPixel,
	/*! destination buffer */
	unsigned char* dst,
	/*! source buffer */
	unsigned char* src,
	/*! buffer width */
	unsigned int width,
	/*! alpha channel */
	unsigned char* alphaChannel,
	/*! color key */
	unsigned long color_key
	);

/*!
* Copy source pixel buffer to destination pixel buffer with alpha channel, alpha factor and color key
*/
void dg_blit_alpha_channel_alpha_color_key(
	/*! bits per pixel */
	int bitsPerPixel,
	/*! destination buffer */
	unsigned char* dst,
	/*! source buffer */
	unsigned char* src,
	/*! buffer width */
	unsigned int width,
	/*! alpha factor */
	unsigned char alpha,
	unsigned char* alphaChannel,
	/*! color key */
	unsigned long color_key
	);

/*! @} */

#endif
