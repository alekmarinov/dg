/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      vnone_surface.h                                    */
/* Description:   DG/None Surface interface                          */
/*                                                                   */
/*********************************************************************/

#ifndef __VNONE_SURFACE_H
#define __VNONE_SURFACE_H

#include <dg/dg_surface.h>
#include <dg/dg_video.h>

typedef struct _vnone_surface_t
{
	dg_video_p device;             /* video device reference */
	dg_color_p pixels;  
} vnone_surface_t, *vnone_surface_p;

dg_surface_p vnone_create_surface(dg_video_p device, int width, int height, int options);
int vnone_surface_lock(dg_surface_p, int);
int vnone_surface_unlock(dg_surface_p);
int vnone_surface_fill_rect(dg_surface_p, int, int, int, int, dg_color_t);
int vnone_surface_clear(dg_surface_p, dg_color_t);
int vnone_surface_set_color_key(dg_surface_p, dg_color_t);

#endif
