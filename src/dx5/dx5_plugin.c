/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_plugin.c                                       */
/* Description:   DG video driver plugin                             */
/*                                                                   */
/*********************************************************************/

#include <dg/dg.h>
#include <dg/dg_plugin.h>
#include "dx5_video.h"

static const char* plugin_name="directx5";

DG_API int dg_plugin_init( dg_plugin_info_p plugin_info, int initializing)
{
	if (initializing)
	{
		strcpy(plugin_info->plugin_name, plugin_name);
		//plugin_info->input_message
		plugin_info->provide_mask=PLUGIN_VIDEO_DRIVER;
		plugin_info->video_driver = (dg_video_driver_p)malloc(sizeof(dg_video_driver_t));
		plugin_info->video_driver->video_create=dx5_video_create;
	}
	else
	{
		free(plugin_info->video_driver);
	}
	return 1;
}
