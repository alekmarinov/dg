/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      sdl_surface.h                                      */
/* Description:   DG/SDL Surface interface                           */
/*                                                                   */
/*********************************************************************/

#ifndef __SDL_SURFACE_H
#define __SDL_SURFACE_H

#include <SDL.h>
#include <dg/dg_surface.h>
#include <dg/dg_video.h>

typedef struct _sdl_surface_t
{
	SDL_Surface *lpSDSBack;
} sdl_surface_t, *sdl_surface_p;

dg_surface_p sdl_create_surface(dg_video_p device, int width, int height, int options);
int sdl_create_physical_surface(int, int, SDL_Surface **, int*);
int sdl_surface_resize(dg_surface_p, int, int);
int sdl_surface_lock(dg_surface_p, int);
int sdl_surface_unlock(dg_surface_p);
int sdl_surface_fill_rect(dg_surface_p, int, int, int, int, dg_color_t);
int sdl_surface_clear(dg_surface_p, dg_color_t);
int sdl_surface_set_color_key(dg_surface_p, dg_color_t);

#endif
