/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_movie.h                                         */
/* Description:   DG video movie interface                           */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_MOVIE_H
#define __DG_MOVIE_H

#include <dg/dg.h>
#include <dg/dg_video.h>
#include <dg/dg_surface.h>

/*! video movie type */
typedef struct _dg_movie_t
{
/*! Implementation specific movie context */
	void* context;

/*! The video device */
	dg_video_p device;

/*! Single movie frame */
	dg_surface_p surface;

/*! Movie width */
	int width;

/*! Movie height */
	int height;

	void (*destroy)(struct _dg_movie_t*);

} dg_movie_t, *dg_movie_p;

DG_API dg_movie_p dg_movie_create(dg_video_p, int, int);
DG_API void dg_movie_destroy(dg_movie_p);

#endif
