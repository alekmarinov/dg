/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      ffmpeg_movie.h                                     */
/* Description:   ffmpeg movie context definition                    */
/*                                                                   */
/*********************************************************************/

#ifndef __FFMPEG_MOVIE_H
#define __FFMPEG_MOVIE_H

#define HAVE_AV_CONFIG_H
#include <avformat.h>
#include <dg/dg_pixel.h>

typedef struct _ffmpeg_movie_context_t
{
	AVFormatContext *avFormatContext;
	AVCodecContext *avCodecContext;
	AVCodec *avCodec;
	int video_index;

	AVFormatParameters params;
	AVPicture* pict;
	AVPacket pkt;
	AVFrame *frame;

	dg_pixel_format_t pixel_format;

} ffmpeg_movie_context_t, *ffmpeg_movie_context_p;

#endif
