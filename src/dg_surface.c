/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_surface.c                                       */
/* Description:   DG video surface implementation                    */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <dg/dg_debug.h>
#include <dg/dg_surface.h>
#include <dg/dg_blit.h>

void dg_surface_resize(dg_surface_p self, int width, int height)
{
	if (width>0 && height>0 && (self->width != width || self->height != height))
	{
		if (self->options & DG_OPT_SURFACE_ALPHA_CHANNEL)
		{
			free(self->alpha_channel);
			self->alpha_channel=0;
			self->options&=~DG_OPT_SURFACE_ALPHA_CHANNEL;
			///* resize the alpha channel */
			//char* alpha_channel = (char*)malloc(width * height);
			//dg_stretch_pixel_buffer(8, alpha_channel, width, height, self->alpha_channel, self->width, self->height);
			//free(self->alpha_channel);
			//self->alpha_channel=alpha_channel;
		}

		self->width=width;
		self->height=height;
	}
}

static int dg_surface_get_width(dg_surface_p self)
{
	return self->width;
}

static int dg_surface_get_height(dg_surface_p self)
{
	return self->height;
}

static void dg_surface_clear(dg_surface_p self, dg_color_t color)
{
}

static void dg_surface_fill_rect(dg_surface_p self, int x, int y, int width, int height, dg_color_t color)
{
}

static void dg_surface_flip(dg_surface_p self, int x, int y, dg_rect_p rect)
{
}

static unsigned char dg_surface_get_alpha(dg_surface_p self)
{
	return self->alpha;
}

static void dg_surface_set_alpha(dg_surface_p self, unsigned char alpha)
{
	if (alpha == 255)
	{
		self->options&=~DG_OPT_SURFACE_ALPHA;
	}
	else
	{
		self->options|=DG_OPT_SURFACE_ALPHA;
	}
	self->alpha = alpha;
}

static dg_color_t dg_surface_get_color_key(dg_surface_p self)
{
	return self->color_key;
}

int dg_surface_set_color_key(dg_surface_p self, dg_color_t color_key)
{
	self->options|=DG_OPT_SURFACE_COLOR_KEY;
	self->color_key = color_key;
	return 1;
}

static unsigned char* dg_surface_get_alpha_channel(dg_surface_p self)
{
	return self->alpha_channel;
}

static void dg_surface_set_alpha_channel(dg_surface_p self, unsigned char* alpha_channel)
{
	int channel_size=self->width * self->height;
	if (self->alpha_channel)
	{
		free(self->alpha_channel);
	}
	if (alpha_channel)
	{
		self->options|=DG_OPT_SURFACE_ALPHA_CHANNEL;
		self->alpha_channel = malloc(channel_size);
		dg_blit_copy(8, self->alpha_channel, alpha_channel, channel_size);
	}
	else
	{
		self->options&=~DG_OPT_SURFACE_ALPHA_CHANNEL;
		self->alpha_channel = 0;
	}
}

static int dg_surface_lock(dg_surface_p self, int is_pixels_converted)
{
	return 0;
}

static int dg_surface_unlock(dg_surface_p self)
{
	return 0;
}

void dg_surface_destroy(dg_surface_p self)
{
	if (self->alpha_channel)
	{
		free(self->alpha_channel);
		self->alpha_channel=0;
	}
	//if (self->alpha_channel_initial)
	//{
	//	free(self->alpha_channel_initial);
	//	self->alpha_channel_initial=0;
	//}	
	free(self);
}

dg_surface_p dg_surface_create(dg_video_p device, int width, int height, int options)
{
	dg_surface_p self=(dg_surface_p)calloc(1, sizeof(dg_surface_t));

	self->width=width;
	self->height=height;
	self->options=options;
	self->alpha_channel=0;
	//self->alpha_channel_initial=0;
	//self->alpha_channel_width=0;
	//self->alpha_channel_height=0;
	self->alpha=0;
	self->device=device;
	DG_SET_ALPHA(self->color_key, 255); /* undefined color key */
	self->lock_count=0;
	self->pixels=0;
	self->pitch=0;
	self->is_pixels_converted=0;

	self->resize             = dg_surface_resize;
	self->get_width          = dg_surface_get_width;
	self->get_height         = dg_surface_get_height;
	self->clear              = dg_surface_clear;
	self->fill_rect          = dg_surface_fill_rect;
	self->flip               = dg_surface_flip;
	self->blit               = dg_surface_blit;
	self->set_alpha          = dg_surface_set_alpha;
	self->get_alpha          = dg_surface_get_alpha;
	self->set_color_key      = dg_surface_set_color_key;
	self->get_color_key      = dg_surface_get_color_key;
	self->set_alpha_channel  = dg_surface_set_alpha_channel;
	self->get_alpha_channel  = dg_surface_get_alpha_channel;
	self->lock               = dg_surface_lock;
	self->unlock             = dg_surface_unlock;
	self->destroy            = dg_surface_destroy;

	return self;
}
