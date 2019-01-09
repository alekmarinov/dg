/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_video.h                                        */
/* Description:   DG/DirectX Video interface                         */
/*                                                                   */
/*********************************************************************/

#ifndef __DX5_VIDEO_H
#define __DX5_VIDEO_H

#include <dg/dg_video.h>
#include <dg/dg_surface.h>
#include <windows.h>
#include <ddraw.h>

typedef struct _dx5_video_t
{
	LPDIRECTDRAW  lpDD;               /* ddraw object */
	dg_surface_p  screen;             /* main screen surface */
	HWND          hwndMain;           /* main window handle */
	RECT          rectClient;         /* main window rectangle in screen coords */
	int           options;            /* video device options */
	int           initialized;        /* true if the video device is initialized */
} dx5_video_t, *dx5_video_p;

int dx5_video_create(struct dg_video_t*);

#endif
