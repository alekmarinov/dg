/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      gd_image.h                                         */
/* Description:   Image read/write implementation over GD library    */
/*                                                                   */
/*********************************************************************/

#ifndef GD_IMAGE_H
#define GD_IMAGE_H

#include <gd.h>
#include <dg/dg_plugin.h>

gdImagePtr gd_make_truecolor(gdImagePtr);

struct dg_surface_t* gd_read_image_file(struct dg_video_t*, const char*);
struct dg_surface_t* gd_read_image_memory(struct dg_video_t*, dg_memory_p);
struct dg_surface_t* gd_read_image_stream(struct dg_video_t*, dg_stream_p);

int gd_write_image_file(struct dg_surface_t*, dg_image_format_t, const char*);
int gd_write_image_memory(struct dg_surface_t*, dg_image_format_t, dg_memory_p*);
int gd_write_image_stream(struct dg_surface_t*, dg_image_format_t, dg_stream_p);

#endif
