/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      vnone_video.h                                      */
/* Description:   DG/None Video interface                            */
/*                                                                   */
/*********************************************************************/

#ifndef __VNONE_VIDEO_H
#define __VNONE_VIDEO_H

#include <dg/dg_video.h>
#include <dg/dg_surface.h>

typedef struct _vnone_video_t
{
	dg_surface_p  screen;             /* main screen surface */
	int           options;            /* video device options */
} vnone_video_t, *vnone_video_p;

int vnone_video_create(struct dg_video_t*);

#endif
