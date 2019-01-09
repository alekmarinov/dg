/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_surface.h                                       */
/* Description:   DG video surface                                   */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_SURFACE_H
#define __DG_SURFACE_H

/*!
	\addtogroup dg_surface Surface
  *  @{
*/

#include <dg/dg.h>
#include <dg/dg_pixel.h>
#include <dg/dg_video.h>
#include <dg/dg_window.h>

#define DG_OPT_SURFACE_VIDEO         (1<<0)
#define DG_OPT_SURFACE_COLOR_KEY     (1<<1)
#define DG_OPT_SURFACE_ALPHA         (1<<2)
#define DG_OPT_SURFACE_ALPHA_CHANNEL (1<<3)

/*! video surface type */
typedef struct _dg_surface_t
{
/*! Platform specific surface type */
	void* context;

/*! Surface width */
	int width;

/*! Surface height */
	int height;

/*! Surface pixels in RGBA format (available after lock) */
	dg_color_p pixels;

/*! Is pixels are converted to dg_color_t */
	int is_pixels_converted;

/*! Surface pitch */
	unsigned long pitch;

/*! Surface locking count */
	int lock_count;

/*! Surface options */
	int options;

/*! Alpha factor */
	unsigned char alpha;

/*! Color Key */
	dg_color_t color_key;

/*! Alpha channel */
	unsigned char* alpha_channel;
	//unsigned char* alpha_channel_initial;
	//int alpha_channel_width;
	//int alpha_channel_height;

/*! The video device */
	dg_video_p device;

	void (*resize)(struct _dg_surface_t*, int, int);
	int  (*get_width)(struct _dg_surface_t*);
	int  (*get_height)(struct _dg_surface_t*);
	void (*fill_rect)(struct _dg_surface_t*, int, int, int, int, dg_color_t);
	void (*clear)(struct _dg_surface_t*, dg_color_t);
	void (*flip)(struct _dg_surface_t*, int, int, dg_rect_p);
	int  (*blit) (struct _dg_surface_t*, dg_rect_p, struct _dg_surface_t*, dg_rect_p, unsigned char, unsigned char*, dg_color_t, int);
	void (*set_alpha) (struct _dg_surface_t*, unsigned char);
	unsigned char (*get_alpha) (struct _dg_surface_t*);
	int (*set_color_key) (struct _dg_surface_t*, dg_color_t);
	dg_color_t (*get_color_key) (struct _dg_surface_t*);
	void (*set_alpha_channel) (struct _dg_surface_t*, unsigned char* alpha_channel);
	unsigned char* (*get_alpha_channel) (struct _dg_surface_t*);
	int  (*lock)(struct _dg_surface_t*, int);
	int  (*unlock)(struct _dg_surface_t*);
	void (*destroy)(struct _dg_surface_t*);

} dg_surface_t, *dg_surface_p;

DG_API dg_surface_p dg_surface_create(dg_video_p, int, int, int);
DG_API int dg_surface_set_color_key(dg_surface_p, dg_color_t);
DG_API void dg_surface_resize(dg_surface_p, int, int);
DG_API void dg_surface_destroy(dg_surface_p);

#define DGSurfaceResize(surface, width, height) surface->resize(surface, width, height)
#define DGSurfaceWidth(surface) surface->get_width(surface)
#define DGSurfaceHeight(surface) surface->get_height(surface)
#define DGSurfaceFillRect(surface, x, y, width, height, color) surface->fill_rect(surface, x, y, width, height, color)
#define DGSurfaceClear(surface, color) surface->clear(surface, color)
#define DGSurfaceFlip(surface, x, y, rect) surface->flip(surface, x, y, rect)
#define DGSurfaceBlit(dst, drect, src, srect, alpha, alpha_channel, color_key, options) \
	surface->blit(surface, dst, drect, src, srect, alpha, alpha_channel, color_key, options)
#define DGSurfaceSetAlpha(surface, alpha) surface->set_alpha(surface, alpha)
#define DGSurfaceAlpha(surface) surface->get_alpha(surface)
#define DGSurfaceSetColorKey(surface, color_key) surface->set_color_key(surface, color_key)
#define DGSurfaceColorKey(surface) surface->get_color_key(surface)
#define DGSurfaceSetAlphaChannel(surface, alpha_channel) surface->set_alpha_channel(surface, alpha_channel)
#define DGSurfaceAlphaChannel(surface) surface->get_alpha_channel(surface)
#define DGSurfaceLock(surface, is_pixels_converted) surface->lock(surface, is_pixels_converted)
#define DGSurfaceUnlock(surface) surface->unlock(surface)
#define DGSurfaceGDC(surface) surface->create_gdc(surface)
#define DGSurfaceDestroy(surface) surface->destroy(surface)
#define DGSurfaceSetPixel(surface, x, y, color) { assert(x>=0 && y>=0 && x<surface->width && y<surface->height); surface->pixels[(x)+(y)*surface->width]=color; }
#define DGSurfacePixel(surface, x, y) surface->pixels[(x)+(y)*surface->width]

/*! @} */

#endif
