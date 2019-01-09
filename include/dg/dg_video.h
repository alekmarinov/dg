/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_video.h                                         */
/* Description:   DG video device                                    */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_VIDEO_H
#define __DG_VIDEO_H

#include <dg/dg_window.h>
#include <dg/dg_event.h>
#include <dg/dg_pixel.h>
#include <dg/dg_plugin.h>
#include <dg/dg_stream.h>

/*!
	\addtogroup dg_video Desktop and Video Mode API
  *  @{
*/

/*!	Mask defining fullscreen mode, which can be passed to \ref dg_video_create */
#define DG_OPT_FULLSCREEN       (1 << 0)

/*!	Mask enabling double buffering, which can be passed to \ref dg_video_create */
#define DG_OPT_DOUBLE_BUFFERING (1 << 1)

struct _dg_widget_t;
struct _dg_surface_t;

/*! video device type */
typedef struct _dg_video_t
{
/*! Platform specific video type */
	void* context;

/*! Root widget */
	struct _dg_widget_t* master_widget;

/*! Color packing info */
	dg_pixel_format_t pixel_format;

/*! Video X resoltion */
	int xres;

/*! Video Y resoltion */
	int yres;

/*! Video bits per pixel */
	int bpp;

/*! Video options */
	int options;

/*! Is the main window active or inactive */
	int active;

/*! Background image */
	struct _dg_surface_t* background_image;

/*! Background color */
	dg_color_t background_color;

	int  (*get_xres)(struct _dg_video_t*);
	int  (*get_yres)(struct _dg_video_t*);
	struct _dg_surface_t* (*create_surface)(struct _dg_video_t*, int width, int height, int options);
	struct _dg_widget_t* (*create_widget)(struct _dg_video_t*, int, int x, int y, int width, int height);
	struct _dg_surface_t* (*load_surface_file)(struct _dg_video_t*, const char*);
	struct _dg_surface_t* (*load_surface_memory)(struct _dg_video_t*, dg_memory_p);
	struct _dg_surface_t* (*load_surface_stream)(struct _dg_video_t*, dg_stream_p);
	int (*save_surface_file)(struct _dg_video_t*, struct _dg_surface_t*, dg_image_format_t, const char*);
	int (*save_surface_memory)(struct _dg_video_t*, struct _dg_surface_t*, dg_image_format_t, dg_memory_p*);
	int (*save_surface_stream)(struct _dg_video_t*, struct _dg_surface_t*, dg_image_format_t, dg_stream_p);
	void (*flip) (struct _dg_video_t*, dg_rect_p);
	unsigned long (*pack_color)(struct _dg_video_t*, dg_color_t);
	dg_color_t (*unpack_color)(struct _dg_video_t*, unsigned long);
	void (*set_mouse_capture)(struct _dg_video_t*, int);
	void (*set_background_image)(struct _dg_video_t*, struct _dg_surface_t*);
	void (*set_background_color)(struct _dg_video_t*, dg_color_t);
	void (*destroy) (struct _dg_video_t*);

} dg_video_t, *dg_video_p;

/*! Initializes DG device and turn on specified video mode
	\arg \a driver_name is the low level graphic driver name

	\arg \a xres is the desired video mode resolution width

	\arg \a yres is the desired video mode resolution height

	\arg \a bpp is the desired video mode pixel depth

	\arg \a options is one or more of the options below:

		\c DG_OPT_FULLSCREEN turn on the video in exclusive fullscreen mode \n
		\c DG_OPT_DOUBLE_BUFFERING use double buffering
*/
DG_API dg_video_p dg_video_create(int, int, int, int, int);
DG_API void dg_video_destroy(dg_video_p);

#define DGCreate(type_size, xres, yres, bpp, options) dg_video_create(type_size, xres, yres, bpp, options)
#define DGDestroy(video) video->destroy(video)
#define DGFlip(video, rect) video->flip(video, rect)
#define DGCreateSurface(video, width, height, options)  video->create_surface(video, width, height, options)
#define DGCreateWidget(video, type_size, x, y, width, height)  video->create_widget(video, type_size, x, y, width, height)
#define DGPackColor(video, color)  video->pack_color(video, color)
#define DGUnpackColor(video, color)  video->unpack_color(video, color)
#define DGSetMouseCapture(video, is_capture) video->set_mouse_capture(video, is_capture)
#define DGVideoXRes(video) video->get_xres(video)
#define DGVideoYRes(video) video->get_yres(video)
#define DGVideoBpp(video)  video->get_bpp(video)

/*! @} */

#endif
