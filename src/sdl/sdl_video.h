/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      sdl_video.h                                        */
/* Description:   DG/SDL Video interface                             */
/*                                                                   */
/*********************************************************************/

#ifndef __SDL_VIDEO_H
#define __SDL_VIDEO_H

#include <dg/dg_video.h>
#include <dg/dg_surface.h>

typedef struct _sdl_video_t
{
	dg_surface_p  screen;             /* main screen surface */
} sdl_video_t, *sdl_video_p;

int sdl_video_create(struct dg_video_t* device);

#endif
