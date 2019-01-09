/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_movie.c                                         */
/* Description:   DG video movie implementation                      */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <dg/dg_debug.h>
#include <dg/dg_movie.h>

void dg_movie_destroy(dg_movie_p self)
{
	if (self->surface)
		self->surface->destroy(self->surface);

	free(self);
}

dg_movie_p dg_movie_create(dg_video_p device, int width, int height)
{
	dg_movie_p self=(dg_movie_p)calloc(1, sizeof(dg_movie_t));
	self->context = 0;
	self->device = device;
	self->surface = 0;
	self->width = width;
	self->height = height;

	self->destroy = dg_movie_destroy;
	return self;
}
