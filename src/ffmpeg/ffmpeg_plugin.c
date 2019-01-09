/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      ffmpeg_plugin.c                                    */
/* Description:   ffmpeg based plugin to read/write movie files      */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_plugin.h>
#include "ffmpeg_movie.h"
#include "ffmpeg_reader.h"

static const char* plugin_name = "ffmpeg";

DG_API int dg_plugin_init( dg_plugin_info_p plugin_info, int initializing)
{
	if (initializing)
	{
		/* Register - only once */
		av_register_all();

		strcpy(plugin_info->plugin_name, plugin_name);
		plugin_info->provide_mask=PLUGIN_MOVIE_READER | PLUGIN_MOVIE_WRITER;
		plugin_info->movie_reader=(dg_movie_reader_p)malloc(sizeof(dg_movie_reader_t));
		plugin_info->movie_reader->open_movie_file   = ffmpeg_open_read_movie_file;
		plugin_info->movie_reader->open_movie_stream = ffmpeg_open_read_movie_stream;
		plugin_info->movie_reader->open_movie_memory = ffmpeg_open_read_movie_memory;
		plugin_info->movie_reader->read_next_frame   = ffmpeg_read_next_frame;
		plugin_info->movie_reader->close_movie       = ffmpeg_close_movie;

		plugin_info->movie_writer=(dg_movie_writer_p)malloc(sizeof(dg_movie_writer_t));
	}
	else
	{
		free(plugin_info->movie_reader);
		free(plugin_info->movie_writer);
	}
	return 1;
}
