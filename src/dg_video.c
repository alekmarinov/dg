/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_video.c                                         */
/* Description:   Common Video Device                                */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <dg/dg_debug.h>
#include <dg/dg_pixel.h>
#include <dg/dg_video.h>
#include <dg/dg_surface.h>
#include <dg/dg_window.h>
#include <dg/dg_widget.h>
#include <dg/dg_plugin.h>

#define SOURCE_TYPE_FILE    0
#define SOURCE_TYPE_MEMORY  1
#define SOURCE_TYPE_STREAM  2

typedef struct
{
	int is_read;                     /* 1 for reading, 0 for writing image */
	int source_type;                 /* I/O data source type */
	union
	{
		const char* filename;
		dg_memory_p memory;
		dg_stream_p stream;
	} source;                         /* I/O data source */
	dg_video_p video;                 /* video driver */
	dg_image_format_t image_format;   /* image format to write to */
	dg_surface_p surface;             /* read/write from/to surface */
} dg_image_format_handler_t, *dg_image_format_handler_p;

/* global definitions */

/*! current logging level */
int _loglevel=LOG_LEVEL_IMPORTANT;

/* local function prototypes */

static dg_widget_p dg_video_create_widget(dg_video_p, int, int, int, int, int);
static int dg_video_get_xres(dg_video_p);
static int dg_video_get_yres(dg_video_p);
static void dg_video_flip(dg_video_p, dg_rect_p);
static unsigned long dg_video_pack_color(dg_video_p, dg_color_t);
static dg_color_t dg_video_unpack_color(dg_video_p, unsigned long);
static void dg_video_set_mouse_capture(dg_video_p, int);
static dg_surface_p dg_video_load_surface_file(dg_video_p, const char*);
static dg_surface_p dg_video_load_surface_memory(dg_video_p, dg_memory_p);
static dg_surface_p dg_video_load_surface_stream(dg_video_p, dg_stream_p);
static int dg_video_save_surface_file(dg_video_p, dg_surface_p, dg_image_format_t, const char*);
static int dg_video_save_surface_memory(dg_video_p, dg_surface_p, dg_image_format_t, dg_memory_p*);
static int dg_video_save_surface_stream(dg_video_p, dg_surface_p, dg_image_format_t, dg_stream_p);

static dg_widget_p dg_video_create_widget(dg_video_p self, int type_size, int x, int y, int width, int height)
{
	return dg_widget_create(type_size, self, x, y, width, height, self->master_widget);
}

static int dg_video_get_xres(dg_video_p self)
{
	return self->xres;
}

static int  dg_video_get_yres(dg_video_p self)
{
	return self->yres;
}

static void dg_video_flip(dg_video_p self, dg_rect_p rect)
{
	self->master_widget->surface->flip(self->master_widget->surface, 0, 0, rect);
}

static unsigned long dg_video_pack_color(dg_video_p self, dg_color_t color)
{
	return dg_pixel_pack(&self->pixel_format, color);
}

static dg_color_t dg_video_unpack_color(dg_video_p self, unsigned long color)
{
	return dg_pixel_unpack(&self->pixel_format, color);
}

static void dg_video_set_mouse_capture(dg_video_p self, int is_capture)
{
}

/* exported function implementation */

void dg_video_destroy(dg_video_p self)
{
	free(self);
	dg_plugin_destroy();
}

static dg_video_driver_p video_driver=0;

static int plugin_video_driver_enum_callback(dg_video_driver_p* video_driver, dg_plugin_info_p plugin_info)
{
	*video_driver=plugin_info->video_driver;

	/* video driver found, stop plugin enumeration */
	return 1;
}

static int plugin_image_handler_enum_callback(dg_image_format_handler_p image_handler, 
											  dg_plugin_info_p plugin_info)
{
	if (image_handler->is_read)
	{
		/* read an image from input source */
		if (plugin_info->provide_mask & PLUGIN_IMAGE_READER)
		{
			switch (image_handler->source_type)
			{
			case SOURCE_TYPE_FILE:
			{
				image_handler->surface = (dg_surface_p)plugin_info->image_reader->read_image_file(
					(struct dg_video_t*)image_handler->video, image_handler->source.filename);
				if (image_handler->surface)
					return 1; /* image successfully read, stop enumeration */
			}
			break;
			case SOURCE_TYPE_MEMORY:
			{
				image_handler->surface = (dg_surface_p)plugin_info->image_reader->read_image_memory(
					(struct dg_video_t*)image_handler->video, image_handler->source.memory);
				if (image_handler->surface)
					return 1; /* image successfully read, stop enumeration */
			}
			break;
			case SOURCE_TYPE_STREAM:
			{
				image_handler->surface = (dg_surface_p)plugin_info->image_reader->read_image_stream(
					(struct dg_video_t*)image_handler->video, image_handler->source.stream);
				if (image_handler->surface)
					return 1; /* image successfully read, stop enumeration */
			}
			break;
			}
		}
	}
	else
	{
		/* write an image to output source */
		if (plugin_info->provide_mask & PLUGIN_IMAGE_WRITER)
		{
			switch (image_handler->source_type)
			{
			case SOURCE_TYPE_FILE:
			{
				if (plugin_info->image_writer->write_image_file(
					(struct dg_surface_t*)image_handler->surface, image_handler->image_format, image_handler->source.filename))
					return 1; /* image successfully written, stop enumeration */
			}
			break;
			case SOURCE_TYPE_MEMORY:
			{
				if (plugin_info->image_writer->write_image_memory(
					(struct dg_surface_t*)image_handler->surface, image_handler->image_format, &image_handler->source.memory))
					return 1; /* image successfully written, stop enumeration */
			}
			break;
			case SOURCE_TYPE_STREAM:
			{
				if (plugin_info->image_writer->write_image_stream(
					(struct dg_surface_t*)image_handler->surface, image_handler->image_format, image_handler->source.stream))
					return 1; /* image successfully written, stop enumeration */
			}
			break;
			}
		}
	}
	return 0; /* continue image reader/write enumeration */
}

static dg_surface_p dg_video_load_surface_file(dg_video_p self, const char* filename)
{
	dg_image_format_handler_t image_handler;
	image_handler.image_format=0;
	image_handler.source_type=SOURCE_TYPE_FILE;
	image_handler.source.filename=filename;
	image_handler.surface=0;
	image_handler.is_read=1;
	image_handler.video=self;

	if (dg_plugin_enum(PLUGIN_IMAGE_READER, 
					   plugin_image_handler_enum_callback, &image_handler))
	{
		return image_handler.surface;
	}
	return 0;
}

static dg_surface_p dg_video_load_surface_memory(dg_video_p self, dg_memory_p memory)
{
	dg_image_format_handler_t image_handler;
	image_handler.image_format=0;
	image_handler.source_type=SOURCE_TYPE_MEMORY;
	image_handler.source.memory=memory;
	image_handler.surface=0;
	image_handler.is_read=1;
	image_handler.video=self;

	if (dg_plugin_enum(PLUGIN_IMAGE_READER, 
					   plugin_image_handler_enum_callback, &image_handler))
	{
		return image_handler.surface;
	}
	return 0;
}

static dg_surface_p dg_video_load_surface_stream(dg_video_p self, dg_stream_p stream)
{
	dg_image_format_handler_t image_handler;
	image_handler.image_format=0;
	image_handler.source_type=SOURCE_TYPE_STREAM;
	image_handler.source.stream=stream;
	image_handler.surface=0;
	image_handler.is_read=1;
	image_handler.video=self;

	if (dg_plugin_enum(PLUGIN_IMAGE_READER, 
					   plugin_image_handler_enum_callback, &image_handler))
	{
		return image_handler.surface;
	}
	return 0;
}

static int dg_video_save_surface_file(dg_video_p self, dg_surface_p surface, 
									   dg_image_format_t image_format, const char* filename)
{
	dg_image_format_handler_t image_handler;
	image_handler.image_format=image_format;
	image_handler.source_type=SOURCE_TYPE_FILE;
	image_handler.source.filename=filename;
	image_handler.surface=surface;
	image_handler.is_read=0;
	image_handler.video=self;

	return (dg_plugin_enum(PLUGIN_IMAGE_WRITER, plugin_image_handler_enum_callback, &image_handler));
}

static int dg_video_save_surface_memory(dg_video_p self, dg_surface_p surface, 
										 dg_image_format_t image_format, dg_memory_p* pmemory)
{
	dg_image_format_handler_t image_handler;
	image_handler.image_format=image_format;
	image_handler.source_type=SOURCE_TYPE_MEMORY;
	image_handler.source.memory=0;
	image_handler.surface=surface;
	image_handler.is_read=0;
	image_handler.video=self;

	if (dg_plugin_enum(PLUGIN_IMAGE_WRITER, plugin_image_handler_enum_callback, &image_handler))
	{
		*pmemory = image_handler.source.memory;
		return 1;
	}
	return 0;
}

static int dg_video_save_surface_stream(dg_video_p self, dg_surface_p surface, 
										 dg_image_format_t image_format, dg_stream_p stream)
{
	dg_image_format_handler_t image_handler;
	image_handler.image_format=image_format;
	image_handler.source_type=SOURCE_TYPE_STREAM;
	image_handler.source.stream=stream;
	image_handler.surface=surface;
	image_handler.is_read=0;
	image_handler.video=self;

	return (dg_plugin_enum(PLUGIN_IMAGE_WRITER, plugin_image_handler_enum_callback, &image_handler));
}

static void dg_video_set_background_image(dg_video_p self, dg_surface_p background_image)
{
	if (self->background_image)
	{
		self->background_image->destroy(self->background_image);
	}
	self->background_image = background_image;
	DGWidgetInvalidate(self->master_widget, 0);
}

static void dg_video_set_background_color(dg_video_p self, dg_color_t background_color)
{
	self->background_color = background_color;
	DGWidgetInvalidate(self->master_widget, 0);
}

dg_video_p dg_video_create(int type_size, int xres, int yres, int bpp, int options)
{
	dg_video_driver_p video_driver=0;
	dg_video_p self=(dg_video_p)malloc(sizeof(dg_video_t));

	self->xres=xres;
	self->yres=yres;
	self->bpp=bpp;
	self->options=options;
	self->master_widget = dg_widget_create(type_size, self, 0, 0, xres, yres, 0);
	self->active=0;
	self->background_image=0;
	self->background_color=0;

	self->create_widget = dg_video_create_widget;
	self->get_xres=dg_video_get_xres;
	self->get_yres=dg_video_get_yres;
	self->flip=dg_video_flip;
	self->destroy=dg_video_destroy;
	self->pack_color=dg_video_pack_color;
	self->unpack_color=dg_video_unpack_color;
	self->set_mouse_capture=dg_video_set_mouse_capture;
	self->load_surface_file=dg_video_load_surface_file;
	self->load_surface_memory=dg_video_load_surface_memory;
	self->load_surface_stream=dg_video_load_surface_stream;
	self->save_surface_file=dg_video_save_surface_file;
	self->save_surface_memory=dg_video_save_surface_memory;
	self->save_surface_stream=dg_video_save_surface_stream;
	self->set_background_image=dg_video_set_background_image;
	self->set_background_color=dg_video_set_background_color;

	//dg_plugin_load("dg_vnone.dll");
	dg_plugin_load("dg_dx5.dll");
	//dg_plugin_load("dg_sdl.dll");
	//dg_plugin_load("dg_hermes.dll");
	dg_plugin_load("dg_gd.dll");
	dg_plugin_load("dg_freetype.dll");
	dg_plugin_load("dg_ffmpeg.dll");

	LOG_IMPORTANT("Create video\n");

#if defined(DEBUG) && defined(_WIN32)
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	if (dg_plugin_enum(PLUGIN_VIDEO_DRIVER, plugin_video_driver_enum_callback, &video_driver))
	{
		if (video_driver->video_create((struct dg_video_t*)self))
		{
			return self;
		}
	}

	self->destroy(self);
	return 0;
}
