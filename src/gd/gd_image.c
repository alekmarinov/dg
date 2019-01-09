/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      gd_image.c                                         */
/* Description:   Image read/write implementation over GD library    */
/*                                                                   */
/*********************************************************************/

#include <gd.h>
#include "gd_image.h"
#include "gd_format.h"

/* FIXME: allow customization of image saving options for the various image formats */
#define DEFAULT_JPEG_QUALITY 90

#define MIN_HEADER_SIZE 10

typedef struct
{
	struct gdIOCtx ctx;
	dg_stream_p stream;
} gdIOStreamCtx;

static int ioctx_get_buf(struct gdIOCtx* ctx, void* buffer, int size)
{
	dg_stream_p stream = ((gdIOStreamCtx*)ctx)->stream;
	return stream->read(stream, buffer, size);
}

static int ioctx_get_char(struct gdIOCtx* ctx)
{
	dg_stream_p stream = ((gdIOStreamCtx*)ctx)->stream;
	int c = 0;
	if (0 == stream->readbyte(stream, (unsigned char*)&c))
	{
		return EOF;
	}
	return c;
}

static int ioctx_put_buf(struct gdIOCtx* ctx, const void* buffer, int size)
{
	dg_stream_p stream = ((gdIOStreamCtx*)ctx)->stream;
	return stream->write(stream, buffer, size);
}

static void ioctx_put_char(struct gdIOCtx* ctx, int c)
{
	dg_stream_p stream = ((gdIOStreamCtx*)ctx)->stream;
	stream->writebyte(stream, (unsigned char)c);
}

static int ioctx_seek(struct gdIOCtx* ctx, const int offset)
{
	dg_stream_p stream = ((gdIOStreamCtx*)ctx)->stream;
	return stream->seek(stream, offset, SEEK_FROM_START);
}

static long ioctx_tell(struct gdIOCtx* ctx)
{
	dg_stream_p stream = ((gdIOStreamCtx*)ctx)->stream;
	return (long)stream->tell(stream);
}

static dg_image_format_t guess_image_type(char header[MIN_HEADER_SIZE])
{
	if (header[0] == 'G' && header[1] == 'I' && header[2] == 'F')
		return IMAGE_FORMAT_GIF;
	else if (header[6] == 'J' && header[7] == 'F' && header[8] == 'I' && header[9] == 'F')
		return IMAGE_FORMAT_JPEG;
	else if (header[1] == 'P' && header[2] == 'N' && header[3] == 'G')
		return IMAGE_FORMAT_PNG;
	return 0;
}

struct dg_surface_t* gd_read_image_file(struct dg_video_t* video, const char* filename)
{
	struct dg_surface_t* surface=0;
	dg_stream_p file_stream = dg_stream_file(filename, FILE_ACCESS_READ, FILE_MODE_BINARY);
	if (file_stream)
	{
		surface = gd_read_image_stream(video, file_stream);
		file_stream->destroy(file_stream);
	}
	return surface;
}

struct dg_surface_t* gd_read_image_memory(struct dg_video_t* video, dg_memory_p memory)
{
	gdImagePtr image=0;
	if (memory->size >= MIN_HEADER_SIZE)
	{
		switch (guess_image_type((char*)memory->memory))
		{
		case IMAGE_FORMAT_GIF:  image = gdImageCreateFromGifPtr(memory->size, memory->memory);  break;
		case IMAGE_FORMAT_JPEG: image = gdImageCreateFromJpegPtr(memory->size, memory->memory); break;
		case IMAGE_FORMAT_PNG:  image = gdImageCreateFromPngPtr(memory->size, memory->memory);  break;
		}
	}
	if (image)
	{
		struct dg_surface_t* surface;
		image = gd_make_truecolor(image);
		surface = (struct dg_surface_t*)create_dg_from_gd_image((dg_video_p)video, image);
		gdImageDestroy(image);
		return surface;
	}
	return 0;
}

struct dg_surface_t* gd_read_image_stream(struct dg_video_t* video, dg_stream_p stream)
{
	char header[MIN_HEADER_SIZE];
	size_t n = stream->read(stream, header, MIN_HEADER_SIZE);
	stream->seek(stream, 0, SEEK_FROM_START);
	if (n == MIN_HEADER_SIZE)
	{
		gdImagePtr image=0;
		gdIOStreamCtx* ctx = (gdIOStreamCtx*)malloc(sizeof(gdIOStreamCtx));
		ctx->ctx.getBuf = ioctx_get_buf;
		ctx->ctx.getC   = ioctx_get_char;
		ctx->ctx.seek   = ioctx_seek;
		ctx->ctx.tell   = ioctx_tell;
		ctx->stream     = stream;

		switch (guess_image_type(header))
		{
		case IMAGE_FORMAT_GIF:  image = gdImageCreateFromGifCtx((gdIOCtxPtr)ctx);  break;
		case IMAGE_FORMAT_JPEG: image = gdImageCreateFromJpegCtx((gdIOCtxPtr)ctx); break;
		case IMAGE_FORMAT_PNG:  image = gdImageCreateFromPngCtx((gdIOCtxPtr)ctx);  break;
		}
		free(ctx);
		if (image)
		{
			struct dg_surface_t* surface;
			image = gd_make_truecolor(image);
			surface = (struct dg_surface_t*)create_dg_from_gd_image((dg_video_p)video, image);
			gdImageDestroy(image);
			return surface;
		}
	}
	return 0;
}

int gd_write_image_file(struct dg_surface_t* surface, dg_image_format_t image_format, const char* filename)
{
	int status;
	dg_stream_p file_stream = dg_stream_file(filename, FILE_ACCESS_WRITE, FILE_MODE_BINARY);
	status = gd_write_image_stream(surface, image_format, file_stream);
	file_stream->destroy(file_stream);
	return status;
}

static void gd_memory_free(dg_memory_p self)
{
	gdFree(self->memory);
}

int gd_write_image_memory(struct dg_surface_t* surface, dg_image_format_t image_format, dg_memory_p* memory)
{
	int status=0;
	void* buffer;
	int size;
	gdImagePtr image = create_gd_from_dg_image((dg_surface_p)surface);
	*memory=0;
	if (image)
	{
		switch (image_format)
		{
		case IMAGE_FORMAT_GIF:  buffer=gdImageGifPtr(image, &size); status=1; break;
		case IMAGE_FORMAT_JPEG: buffer=gdImagePngPtr(image, &size); status=1; break;
		case IMAGE_FORMAT_PNG:  buffer=gdImageJpegPtr(image, &size, DEFAULT_JPEG_QUALITY); status=1; break;
		}

		if (status)
		{
			(*memory) = dg_memory_create(size);
			(*memory)->memory=buffer;
			(*memory)->free=gd_memory_free;
		}

		gdImageDestroy(image);
	}
	return status;
}

int gd_write_image_stream(struct dg_surface_t* surface, dg_image_format_t image_format, dg_stream_p stream)
{
	int status=0;
	gdImagePtr image = create_gd_from_dg_image((dg_surface_p)surface);
	if (image)
	{
		gdIOStreamCtx* ctx = (gdIOStreamCtx*)malloc(sizeof(gdIOStreamCtx));
		ctx->ctx.putBuf = ioctx_put_buf;
		ctx->ctx.putC   = ioctx_put_char;
		ctx->ctx.seek   = ioctx_seek;
		ctx->ctx.tell   = ioctx_tell;
		ctx->stream     = stream;

		switch (image_format)
		{
		case IMAGE_FORMAT_GIF:  
		{
			gdImageGifCtx(image, (gdIOCtxPtr)ctx); 
			status=1; 
		}
		break;
		case IMAGE_FORMAT_JPEG: 
		{
			gdImageJpegCtx(image, (gdIOCtxPtr)ctx, DEFAULT_JPEG_QUALITY); 
			status=1; 
		}
		break;
		case IMAGE_FORMAT_PNG:  
		{
			gdImagePngCtx(image, (gdIOCtxPtr)ctx); 
			status=1; 
		}
		break;
		}

		free(ctx);
		gdImageDestroy(image);
	}
	return status;
}
