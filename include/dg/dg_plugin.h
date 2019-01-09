/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_plugin.h                                        */
/* Description:   DG Plugin interface                                */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_PLUGIN_H
#define __DG_PLUGIN_H

#include <dg/dg_stream.h>
#include <dg/dg_pixel.h>

/*!
	\addtogroup dg_plugin Plugin Management
	<b>This page is under construction</b>
  *  @{
*/

#define MAX_PLUGIN_NAME 50

#define PLUGIN_VIDEO_DRIVER  (1 << 0)
#define PLUGIN_FONT_RENDERER (1 << 1)
#define PLUGIN_IMAGE_READER  (1 << 2)
#define PLUGIN_IMAGE_WRITER  (1 << 3)
#define PLUGIN_MOVIE_READER  (1 << 4)
#define PLUGIN_MOVIE_WRITER  (1 << 5)
#define PLUGIN_INPUT_MESSAGE (1 << 6)
#define PLUGIN_PIXEL_FORMAT  (1 << 7)
#define PLUGIN_GDC_DRIVER    (1 << 8)

struct dg_video_t;
struct dg_surface_t;
struct dg_movie_t;
struct dg_gdc_t;
struct dg_font_t;
/*!
	Plugin interface for video drivers
*/
typedef struct _dg_video_driver_t
{
	int (*video_create)(struct dg_video_t*);
} dg_video_driver_t, *dg_video_driver_p;

/*!
	Plugin interface for font renderers
*/
typedef struct _dg_font_renderer_t
{
	void (*set_font_path)(int, const char**);
	struct dg_font_t* (*font_create)(const char*, int);
	void (*draw_text)(struct dg_gdc_t*, int, int, const char*, int);
} dg_font_renderer_t, *dg_font_renderer_p;

typedef enum
{
	IMAGE_FORMAT_GIF=1,
	IMAGE_FORMAT_JPEG=2,
	IMAGE_FORMAT_PNG=4,
} dg_image_format_t;

/*!
	Plugin interface for image readers
*/
typedef struct _dg_image_reader_t
{
	unsigned int supported_image_formats;
	struct dg_surface_t* (*read_image_file)(struct dg_video_t*, const char*);
	struct dg_surface_t* (*read_image_memory)(struct dg_video_t*, dg_memory_p);
	struct dg_surface_t* (*read_image_stream)(struct dg_video_t*, dg_stream_p);
} dg_image_reader_t, *dg_image_reader_p;

/*!
	Plugin interface for image writers
*/
typedef struct _dg_image_writer_t
{
	unsigned int supported_image_formats;
	int (*write_image_file)(struct dg_surface_t*, dg_image_format_t, const char*);
	int (*write_image_memory)(struct dg_surface_t*, dg_image_format_t, dg_memory_p*);
	int (*write_image_stream)(struct dg_surface_t*, dg_image_format_t, dg_stream_p);
} dg_image_writer_t, *dg_image_writer_p;

/*!
	Plugin interface for movie readers
*/
typedef struct _dg_movie_reader_t
{
	struct dg_movie_t* (*open_movie_file)(struct dg_video_t*, const char*);
	struct dg_movie_t* (*open_movie_memory)(struct dg_video_t*, dg_memory_p);
	struct dg_movie_t* (*open_movie_stream)(struct dg_video_t*, dg_stream_p);
	int (*read_next_frame)(struct dg_movie_t*);
	void (*close_movie)(struct dg_movie_t*);
} dg_movie_reader_t, *dg_movie_reader_p;

/*!
	Plugin interface for movie writers
*/
typedef struct _dg_movie_writer_t
{
	int not_implemented;
} dg_movie_writer_t, *dg_movie_writer_p;

/*!
	Plugin interface for input messages
*/
typedef struct _dg_input_message_t
{
	int (*peek_event)(struct dg_event_t*);
} dg_input_message_t, *dg_input_message_p;

/*!
	Plugin interface for pixel format converters
*/
typedef struct _dg_pixel_format_convert_t
{
	/* Convert buffer from one pixel format to another */
	void (*pixel_format_convert)(
		struct dg_pixel_format_t*, /* destination pixel format */
		unsigned char*,            /* destination buffer */
		unsigned long,             /* destination pitch */
		struct dg_pixel_format_t*, /* source pixel format */
		unsigned char*,            /* source buffer */
		unsigned long,             /* source pitch */
		int,                       /* vertical lines (actual width) */
		int                        /* horizontal lines (height) */);
} dg_pixel_format_convert_t, *dg_pixel_format_convert_p;

/*!
	Plugin interface for GDC
*/
typedef struct _dg_gdc_driver_t
{
	struct dg_gdc_t* (*gdc_create)(struct dg_surface_t*);
} dg_gdc_driver_t, *dg_gdc_driver_p;

/*!
	Plugin information
*/
typedef struct _dg_plugin_info_t
{
	char plugin_name[MAX_PLUGIN_NAME];
	unsigned int provide_mask;

	dg_video_driver_p video_driver;
	dg_font_renderer_p font_renderer;
	dg_image_reader_p image_reader;
	dg_image_writer_p image_writer;
	dg_movie_reader_p movie_reader;
	dg_movie_writer_p movie_writer;
	dg_input_message_p input_message;
	dg_pixel_format_convert_p pixel_format_converter;
	dg_gdc_driver_p gdc_driver;

} dg_plugin_info_t, *dg_plugin_info_p;

/*!
	prototype for the plugin initializer
*/
typedef int (*dg_plugin_init_t)( dg_plugin_info_p, int );

/*!
	List of plugins
*/
typedef struct _dg_plugin_t
{
	dg_plugin_info_p plugin_info;
	dg_plugin_init_t plugin_init;
	struct _dg_plugin_t* next;
} dg_plugin_t, *dg_plugin_p;

/*!
	Registers a new plugin
*/
void dg_plugin_register(dg_plugin_init_t, dg_plugin_info_p);

/*!
	Loads plugin from dynamic library
*/
int dg_plugin_load(const char*);

/*!
	Plugin enumeration callback
*/
typedef int (*plugin_enum_callback_t)(void*, dg_plugin_info_p);

/*!
	Calls a callback function for each plugin which provides any of the requested features
*/
DG_API int dg_plugin_enum(int, plugin_enum_callback_t, void*);

/*!
	Destroys all loaded plugins
*/
void dg_plugin_destroy( void );

/*! @} */

#endif
