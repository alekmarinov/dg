/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      gd_format.h                                        */
/* Description:   DG to/from GD surface format converters            */
/*                                                                   */
/*********************************************************************/

#ifndef GD_FORMAT_H
#define GD_FORMAT_H

#include <gd.h>
#include <dg/dg_surface.h>

#define DG_GD_COLOR(c) gdTrueColorAlpha( \
	DG_GET_RED(c),                       \
	DG_GET_GREEN(c),                     \
	DG_GET_BLUE(c),                      \
	127-(DG_GET_ALPHA(c)>>1))

#define GD_DG_COLOR(c) DG_RGB_COLOR(     \
	gdTrueColorGetRed(c),                \
	gdTrueColorGetGreen(c),              \
	gdTrueColorGetBlue(c),               \
	gdTrueColorGetAlpha(c))

extern dg_pixel_format_t gd_pixel_format;

/*! Copy GD image to locked DG surface */
void copy_gd_to_dg_image(gdImagePtr, dg_surface_p);

/*! Copy locked DG image to GD surface */
void copy_dg_to_gd_image(dg_surface_p, gdImagePtr);

/* Converts GD to DG surface */
dg_surface_p create_dg_from_gd_image(dg_video_p, gdImagePtr);

/* Converts DG to GD surface */
gdImagePtr create_gd_from_dg_image(dg_surface_p);

#endif
