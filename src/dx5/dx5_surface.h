/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_surface.h                                      */
/* Description:   DG/DirectX Surface interface                       */
/*                                                                   */
/*********************************************************************/

#ifndef __DX5_SURFACE_H
#define __DX5_SURFACE_H

#include <ddraw.h>
#include <dg/dg_surface.h>
#include <dg/dg_video.h>

typedef struct _dx5_surface_t
{
    LPDIRECTDRAWSURFACE lpDDSBack;          /* this is the one where all the blits go to */
    LPDIRECTDRAWSURFACE lpDDSPrimary;       /* only filled in case of a backbuffered surface */
    DDSURFACEDESC       ddsd;               /* store the surface description */
    DDPIXELFORMAT       ddpf;               /* pixel format */
	dg_video_p   device;             /* video device reference */
} dx5_surface_t, *dx5_surface_p;

dg_surface_p dx5_create_surface(dg_video_p device, int width, int height, int options);
HRESULT dx5_create_physical_surface(LPDIRECTDRAW, int, int, LPDIRECTDRAWSURFACE*, LPDDSURFACEDESC, int*);
int dx5_surface_lock(dg_surface_p, int);
int dx5_surface_unlock(dg_surface_p);
int dx5_surface_fill_rect(dg_surface_p, int, int, int, int, dg_color_t);
int dx5_surface_clear(dg_surface_p, dg_color_t);
int dx5_surface_set_color_key(dg_surface_p, dg_color_t);

#endif
