/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      gd_format.c                                        */
/* Description:   DG to/from GD surface format converters            */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include "gd_format.h"

dg_pixel_format_t gd_pixel_format = { 8, 8, 8, 0, 16, 8, 0, 32, 32 };

gdImagePtr gd_make_truecolor(gdImagePtr gd_image)
{
	if (gd_image->trueColor)
	{
		return gd_image;
	}
	else
	{
		int i, j;
		gdImagePtr gd_image_truecolor = gdImageCreateTrueColor(gd_image->sx, gd_image->sy);
		for (i=0; i<gd_image->sy; i++)
			for (j=0; j<gd_image->sx; j++)
			{
				int color = gdImageGetTrueColorPixel(gd_image, j, i);
                gdImageSetPixel(gd_image_truecolor, j, i, color);
			}
		gdImageDestroy(gd_image);
		return gd_image_truecolor;
	}
}

/* copy GD image to locked DG surface */
void copy_gd_to_dg_image(gdImagePtr gd_image, dg_surface_p dg_image)
{
	int y;
	unsigned char* pline=(unsigned char*)(dg_image->pixels);

	/* convert GD image to DG surface */
	for (y=0; y < dg_image->height; y++)
	{
		dg_pixel_format_convert( 
			(struct dg_pixel_format_t*)&dg_image->device->pixel_format,/* DG surface pixel format */
			pline,                                                     /* DG surface buffer */
			dg_image->width * sizeof(int),                             /* DG surface pitch */
			(struct dg_pixel_format_t*)&gd_pixel_format,               /* GD image pixel format */
			(unsigned char*)(gd_image->tpixels[y]),                    /* GD image buffer */
			dg_image->width * sizeof(int),                             /* GD image pitch */
			dg_image->width,                                           /* DG surface width */
			1);                                                        /* DG surface height */
		pline += dg_image->pitch;
	}

	/* convert alpha channel from GD image to DG surface */
	if (gd_image->alphaBlendingFlag)
	{
		unsigned char* alpha_channel = (unsigned char*)malloc(dg_image->width*dg_image->height);
		int has_alpha = 0;
		pline=alpha_channel;
		if (pline)
		{
			for (y=0; y < dg_image->height; y++)
			{
				int x;
				for (x=0; x<dg_image->width; x++)
				{
					*pline = (((127-(gd_image->tpixels[y][x] >> 24)) << 1) + 1) & 0xFF;
					has_alpha = has_alpha || (*pline != 255);
					pline++;
				}
			}
		}

		if (has_alpha)
			dg_image->set_alpha_channel(dg_image, alpha_channel);

		free(alpha_channel);
	}
}

/* copy locked DG image to GD surface */
void copy_dg_to_gd_image(dg_surface_p dg_image, gdImagePtr gd_image)
{
	int y;
	unsigned char* pline=(unsigned char*)(dg_image->pixels);

	/* convert DG image to GD surface */
	for (y=0; y < dg_image->height; y++)
	{
		dg_pixel_format_convert(
			(struct dg_pixel_format_t*)&gd_pixel_format,               /* GD image pixel format */
			(unsigned char*)(gd_image->tpixels[y]),                    /* GD image buffer */
			dg_image->width * sizeof(int),                             /* GD image pitch */
			(struct dg_pixel_format_t*)&dg_image->device->pixel_format,/* DG surface pixel format */
			pline,                                                     /* DG surface buffer */
			dg_image->width * sizeof(int),                             /* DG surface pitch */
			dg_image->width,                                           /* DG surface width */
			1);                                                        /* DG surface height */
		pline += dg_image->pitch;
	}

	/* convert alpha channel from DG surface to GD image */
	pline=dg_image->alpha_channel;
	if (pline)
	{
		for (y=0; y < dg_image->height; y++)
		{
			int x;
			for (x=0; x < dg_image->width; x++)
			{
				gd_image->tpixels[y][x] |= ( (127 - ((*pline++) >> 1)) << 24 );
			}
		}
	}
}

/* converts GD to DG surface */
dg_surface_p create_dg_from_gd_image(dg_video_p device, gdImagePtr gd_image)
{
	dg_surface_p dg_image=device->create_surface(device, gd_image->sx, gd_image->sy, 0);
	if (gd_image->transparent != (-1))
	{
		dg_image->set_color_key(dg_image, GD_DG_COLOR(gd_image->transparent));
	}
	dg_image->lock(dg_image, 0);
	copy_gd_to_dg_image(gd_image, dg_image);
	dg_image->unlock(dg_image);
	return dg_image;
}

/* converts DG to GD surface */
gdImagePtr create_gd_from_dg_image(dg_surface_p dg_image)
{
	gdImagePtr gd_image = gdImageCreateTrueColor(dg_image->width, dg_image->height);
	dg_image->lock(dg_image, 0);
	copy_dg_to_gd_image(dg_image, gd_image);
	dg_image->unlock(dg_image);
	return gd_image;
}
