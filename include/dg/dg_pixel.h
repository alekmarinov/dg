/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_pixel.h                                         */
/* Description:   Color & Pixel Converstions API                     */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_PIXEL_H
#define __DG_PIXEL_H

#include <dg/dg.h>

/*!
	\addtogroup dg_pixel Pixels
	<b>This page is under construction</b>
  *  @{
*/

/*! 32 bit color type */
typedef unsigned int dg_color_t, *dg_color_p, dg_hsv_t, *dg_hsv_p;

/*! color packing information */
typedef struct _dg_pixel_format_t
{
	unsigned char bRedBits;
	unsigned char bGreenBits;
	unsigned char bBlueBits;
	unsigned char bAlphaBits;

	unsigned char bRedShift;
	unsigned char bGreenShift;
	unsigned char bBlueShift;
	unsigned char bAlphaShift;

	unsigned char bitsPerPixel;
} dg_pixel_format_t, *dg_pixel_format_p;

#define A_SHIFT 24
#define R_SHIFT 16
#define G_SHIFT 8
#define B_SHIFT 0

#define DG_SET_ALPHA(c, a) c = ((c) & 0x00FFFFFF) | ( ((a) & 0xFF) << A_SHIFT)
#define DG_SET_RED(c, r)   c = ((c) & 0xFF00FFFF) | ( ((r) & 0xFF) << R_SHIFT)
#define DG_SET_GREEN(c, g) c = ((c) & 0xFFFF00FF) | ( ((g) & 0xFF) << G_SHIFT)
#define DG_SET_BLUE(c, b)  c = ((c) & 0xFFFFFF00) | ( ((b) & 0xFF) << B_SHIFT)
#define DG_GET_ALPHA(c) ((((c) & 0xFF000000) >> A_SHIFT) & 0xFF)
#define DG_GET_RED(c)   ((((c) & 0x00FF0000) >> R_SHIFT) & 0xFF)
#define DG_GET_GREEN(c) ((((c) & 0x0000FF00) >> G_SHIFT) & 0xFF)
#define DG_GET_BLUE(c)  ((((c) & 0x000000FF) >> B_SHIFT) & 0xFF)
#define DG_ADD_RED(c, r) DG_SET_RED(c, DG_GET_RED(c)+(r))
#define DG_SUB_RED(c, r) DG_ADD_RED(c, -r)
#define DG_ADD_GREEN(c, g) DG_SET_GREEN(c, DG_GET_GREEN(c)+(g))
#define DG_SUB_GREEN(c, g) DG_ADD_GREEN(c, -g)
#define DG_ADD_BLUE(c, b) DG_SET_BLUE(c, DG_GET_BLUE(c)+(b))
#define DG_SUB_BLUE(c, b) DG_ADD_BLUE(c, -b)
#define DG_ADD_ALPHA(c, b) DG_SET_ALPHA(c, DG_GET_ALPHA(c)+(b))
#define DG_SUB_ALPHA(c, b) DG_ADD_ALPHA(c, -b)

#define DG_RGB_COLOR(r, g, b, a) \
	(((a) & 0xFF) << A_SHIFT) | \
	(((r) & 0xFF) << R_SHIFT) | \
	(((g) & 0xFF) << G_SHIFT) | \
	(((b) & 0xFF) << B_SHIFT)

#define DG_HSV_COLOR(h,s,v,a) DG_RGB_COLOR(h,s,v,a)
#define DG_GET_HUE            DG_GET_RED
#define DG_GET_SATURATION     DG_GET_GREEN
#define DG_GET_VELOCITY       DG_GET_BLUE

#define DG_COLOR_BLACK        (dg_color_t)0xFF000000
#define DG_COLOR_WHITE        (dg_color_t)0xFFFFFFFF
#define DG_COLOR_GRAY         (dg_color_t)0xFF808080
#define DG_COLOR_LIGHT_GRAY   (dg_color_t)0xFFC0C0C0
#define DG_COLOR_DARK_GRAY    (dg_color_t)0xFF606060
#define DG_COLOR_RED          (dg_color_t)0xFFFF0000
#define DG_COLOR_GREEN        (dg_color_t)0xFF00FF00
#define DG_COLOR_BLUE         (dg_color_t)0xFF0000FF
#define DG_COLOR_CYAN         (dg_color_t)0xFF00FFFF
#define DG_COLOR_MAGENTA      (dg_color_t)0xFFFF00FF
#define DG_COLOR_YELLOW       (dg_color_t)0xFFFFFF00

DG_API unsigned long dg_pixel_pack(dg_pixel_format_p pixel_format, dg_color_t);
DG_API dg_color_t dg_pixel_unpack(dg_pixel_format_p, unsigned long);

/* Compare two pixel formats */
int dg_pixel_format_equals(dg_pixel_format_p, dg_pixel_format_p);

/* Convert buffer from one pixel format to another */
DG_API void dg_pixel_format_convert(
	struct dg_pixel_format_t*, /* destination pixel format */
	unsigned char*,            /* destination buffer */
	unsigned long,             /* destination pitch */
	struct dg_pixel_format_t*, /* source pixel format */
	unsigned char*,            /* source buffer */
	unsigned long,             /* source pitch */
	int,                       /* vertical lines (actual width) */
	int                        /* horizontal lines (height) */);

DG_API dg_hsv_t dg_rgb_to_hsv(dg_color_t);
DG_API dg_color_t dg_hsv_to_rgb(dg_hsv_t);

/*! @} */

#endif
