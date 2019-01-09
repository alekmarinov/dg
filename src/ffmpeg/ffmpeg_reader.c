/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      ffmpeg_reader.c                                    */
/* Description:   Movie reader implementation via ffmpeg             */
/*                                                                   */
/*********************************************************************/

#define _CRTIMP
#include <malloc.h>
#include <dg/dg_pixel.h>
#include <dg/dg_movie.h>
#include "ffmpeg_reader.h"
#include "ffmpeg_movie.h"

struct dg_movie_t* ffmpeg_open_read_movie_file(struct dg_video_t* _device, const char* filename)
{
	int err, i;
	dg_movie_p movie = dg_movie_create((dg_video_p)_device, 0, 0);
	ffmpeg_movie_context_p movie_ctx = (ffmpeg_movie_context_p)malloc(sizeof(ffmpeg_movie_context_t));
	memset(movie_ctx, 0, sizeof(ffmpeg_movie_context_t));
	
	movie_ctx->frame = avcodec_alloc_frame();

	/* Open the input file */
	err = av_open_input_file(&movie_ctx->avFormatContext, filename, NULL, 0, &movie_ctx->params);
    if (err < 0) 
		goto movie_open_file_err;

    /* Find the stream info */
    err = av_find_stream_info(movie_ctx->avFormatContext);

    /* Find the first video stream */
    for (i=0; i<movie_ctx->avFormatContext->nb_streams; i++)
	{
		if(movie_ctx->avFormatContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) 
		{
			movie_ctx->avCodecContext = movie_ctx->avFormatContext->streams[i]->codec;
			break;
		}
    }
    movie_ctx->video_index = i;

    if(movie_ctx->video_index >= 0)
	{
		movie_ctx->avCodec = avcodec_find_decoder(movie_ctx->avCodecContext->codec_id);
		if(movie_ctx->avCodec)
		{
			err = avcodec_open(movie_ctx->avCodecContext, movie_ctx->avCodec);
			if(err < 0)
				goto movie_open_err;
		}
		else
			goto movie_open_err;
    }
    else 
		goto movie_open_err;

	movie_ctx->pixel_format.bRedBits = 8;
	movie_ctx->pixel_format.bGreenBits = 8;
	movie_ctx->pixel_format.bBlueBits = 8;
	movie_ctx->pixel_format.bAlphaBits = 0;
	movie_ctx->pixel_format.bRedShift = 0;
	movie_ctx->pixel_format.bGreenShift = 8;
	movie_ctx->pixel_format.bBlueShift = 16;
	movie_ctx->pixel_format.bAlphaShift = 24;
	movie_ctx->pixel_format.bitsPerPixel = 24;

	movie->context = movie_ctx;
	movie->width = movie_ctx->avCodecContext->width;
	movie->height = movie_ctx->avCodecContext->height;
	movie->device = (dg_video_p)_device;
	movie->surface = ((dg_video_p)_device)->create_surface((dg_video_p)_device, movie->width, movie->height, 0);

	return (struct dg_movie_t*)movie;

movie_open_err:
	av_close_input_file(movie_ctx->avFormatContext);
movie_open_file_err:
	free(movie_ctx);
	movie->destroy(movie);
	return 0;
}

struct dg_movie_t* ffmpeg_open_read_movie_stream(struct dg_video_t* device, dg_stream_p stream)
{
	/* not implemented */
	return 0;
}

struct dg_movie_t* ffmpeg_open_read_movie_memory(struct dg_video_t* device, dg_memory_p memory)
{
	/* not implemented */
	return 0;
}

int ffmpeg_read_next_frame(struct dg_movie_t* _movie)
{
	dg_movie_p movie = (dg_movie_p)_movie;
	if (movie && movie->context)
	{
		ffmpeg_movie_context_p movie_ctx = (ffmpeg_movie_context_p)movie->context;
		int got_picture = 0;
		do
		{
			/* Read a frame/packet */
			if (0 > av_read_frame(movie_ctx->avFormatContext, &movie_ctx->pkt))
			{
				/* end of stream */
				return 0;
			}

			/* If it's a video packet from our video stream... */
			if(movie_ctx->pkt.stream_index == movie_ctx->video_index)
			{
				int len;

				/* Decode the packet */
				len = avcodec_decode_video(movie_ctx->avCodecContext, movie_ctx->frame, 
										&got_picture, movie_ctx->pkt.data, movie_ctx->pkt.size);

				if (got_picture) 
				{
					/* Allocate RGB AVPicture the first time through */
					if (!movie_ctx->pict)
					{
						movie_ctx->pict = (AVPicture*)malloc(sizeof(AVPicture));
						avpicture_alloc(movie_ctx->pict, PIX_FMT_RGB24, movie_ctx->avCodecContext->width, 
										movie_ctx->avCodecContext->height);
					}

					/* Create RGB image */
					img_convert(movie_ctx->pict, PIX_FMT_RGB24, (AVPicture*)movie_ctx->frame, movie_ctx->avCodecContext->pix_fmt, 
								movie_ctx->avCodecContext->width, movie_ctx->avCodecContext->height);
					
					/* Fill DG surface compatible with the device */
					movie->surface->lock(movie->surface, 0);				
					dg_pixel_format_convert(&movie->device->pixel_format, 
						(unsigned char*)movie->surface->pixels, 
						movie->surface->pitch, 
						&movie_ctx->pixel_format,
						(unsigned char*)movie_ctx->pict->data[0],
						movie->width * 3,
						movie->width,
						movie->height);
					movie->surface->unlock(movie->surface);
				}

				av_free_packet(&movie_ctx->pkt);
			}
		} while (!got_picture);

		return 1;
	}
	/* invalid params or error reading frame */
	return -1;
}

void ffmpeg_close_movie(struct dg_movie_t* _movie)
{
	dg_movie_p movie = (dg_movie_p)_movie;
	if (movie)
	{
		ffmpeg_movie_context_p movie_ctx = (ffmpeg_movie_context_p)movie->context;
		if (movie_ctx)
		{
			if (movie_ctx->pict)
				avpicture_free(movie_ctx->pict);

			if (movie_ctx->frame)
				av_free(movie_ctx->frame);

			av_close_input_file(movie_ctx->avFormatContext);
			free(movie_ctx);
		}
		movie->destroy(movie);
	}
}
