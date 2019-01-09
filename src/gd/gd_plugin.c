/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      gd_plugin.c                                        */
/* Description:   GD based plugin for GDC (graphics device context)  */
/*                                                                   */
/*********************************************************************/

#include <string.h>
#include <dg/dg_plugin.h>
#include <malloc.h>
#include "gd_gdc.h"
#include "gd_image.h"

#define SUPPORTED_IMAGE_FORMATS IMAGE_FORMAT_GIF | IMAGE_FORMAT_JPEG | IMAGE_FORMAT_PNG

static const char* plugin_name="gd";

DG_API int dg_plugin_init( dg_plugin_info_p plugin_info, int initializing)
{
	if (initializing)
	{
		gdFontCacheSetup();

		strcpy(plugin_info->plugin_name, plugin_name);
		plugin_info->provide_mask=PLUGIN_GDC_DRIVER | PLUGIN_IMAGE_READER | PLUGIN_IMAGE_WRITER;
		plugin_info->gdc_driver=(dg_gdc_driver_p)malloc(sizeof(dg_gdc_driver_t));
		plugin_info->gdc_driver->gdc_create=gd_gdc_create;

		plugin_info->image_reader=(dg_image_reader_p)malloc(sizeof(dg_image_reader_t));
		plugin_info->image_reader->supported_image_formats = SUPPORTED_IMAGE_FORMATS;
		plugin_info->image_reader->read_image_file    = gd_read_image_file;
		plugin_info->image_reader->read_image_memory  = gd_read_image_memory;
		plugin_info->image_reader->read_image_stream  = gd_read_image_stream;

		plugin_info->image_writer=(dg_image_writer_p)malloc(sizeof(dg_image_writer_t));
		plugin_info->image_writer->supported_image_formats = SUPPORTED_IMAGE_FORMATS;
		plugin_info->image_writer->write_image_file   = gd_write_image_file;
		plugin_info->image_writer->write_image_memory = gd_write_image_memory;
		plugin_info->image_writer->write_image_stream = gd_write_image_stream;
	}
	else
	{
		free(plugin_info->gdc_driver);
		free(plugin_info->image_reader);
		free(plugin_info->image_writer);
		gdFontCacheShutdown();
	}
	return 1;
}
