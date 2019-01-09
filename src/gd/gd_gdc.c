/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      gd_gdc.c                                           */
/* Description:   GDC implementation over GD library                 */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <dg/dg_pixel.h>
#include "gd_gdc.h"
#include "gd_format.h"

typedef struct _gd_gdc_context_t
{
	gdImagePtr gd_surface;
} gd_gdc_context_t, *gd_gdc_context_p;

static void gd_gdc_set_clip_rect(dg_gdc_p, int, int, int, int);
static void gd_gdc_set_pen(dg_gdc_p, dg_pen_p);
static void gd_gdc_set_brush(dg_gdc_p, dg_brush_p);
static void gd_gdc_set_pixel(dg_gdc_p, int, int, dg_color_t);
static dg_color_t gd_gdc_get_pixel(dg_gdc_p, int, int);
static void gd_gdc_draw_line(dg_gdc_p, int, int, int, int);
static void gd_gdc_draw_rect(dg_gdc_p, int, int, int, int);
static void gd_gdc_draw_fillrect(dg_gdc_p, int, int, int, int);
static void gd_gdc_draw_fillpolygon(dg_gdc_p, int*, int);
static void gd_gdc_draw_image(dg_gdc_p, int, int, dg_surface_p);
static void gd_gdc_draw_arc(dg_gdc_p, int, int, int, int, int, int);
static void gd_gdc_draw_fillarc(dg_gdc_p, int, int, int, int, int, int, dg_arc_style_t);
static void gd_gdc_set_antialiaze(dg_gdc_p, int);
static void gd_gdc_destroy(dg_gdc_p);

static void gd_gdc_set_clip_rect(dg_gdc_p self, int x, int y, int w, int h)
{
	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;
	gdImageSetClip(gd_context->gd_surface, x, y, x + w - 1, y + h - 1);
	dg_gdc_set_clip_rect(self, x, y, w, h);
}

static int gd_get_brush_color(dg_gdc_p gdc, dg_brush_p brush)
{
	if (brush)
	{
		if (brush->style == BRUSH_SOLID)
		{
			if (gdc->is_antialiaze(gdc))
				return gdAntiAliased;
			else
				return DG_GD_COLOR(brush->get_color(brush));
		}
		else
		if (brush->style == BRUSH_PATTERN)
		{
			return gdBrushed;
		}
		else assert(0);
	}
	return 0;
}

static int gd_get_pen_color(dg_gdc_p gdc, dg_pen_p pen)
{
	if (gdc->is_antialiaze(gdc))
	{
		return gdAntiAliased;
	}
	
	return DG_GD_COLOR(pen->get_color(pen));
}

static void gd_gdc_set_pen(dg_gdc_p self, dg_pen_p pen)
{
	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;
	dg_gdc_set_pen(self, pen);

	gdImageSetThickness(gd_context->gd_surface, pen->size);
}

static void gd_gdc_set_brush(dg_gdc_p self, dg_brush_p brush)
{
	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;
	dg_gdc_set_brush(self, brush);

	if (brush && brush->style == BRUSH_PATTERN)
	{
		gdImageSetTile(gd_context->gd_surface, create_gd_from_dg_image(brush->get_pattern(brush)));
		// FIXME: which of both - tile or brush?
		// gdImageSetBrush(gd_context->gd_surface, create_gd_from_dg_image(brush->get_pattern(brush)));
	}
}

static void gd_gdc_set_pixel(dg_gdc_p self, int x, int y, dg_color_t color)
{
	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;
	gdImageSetPixel(gd_context->gd_surface, x, y, DG_GD_COLOR(color));
}

static dg_color_t gd_gdc_get_pixel(dg_gdc_p self, int x, int y)
{
	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;
	return GD_DG_COLOR(gdImageGetTrueColorPixel(gd_context->gd_surface, x, y));
}

static void gd_gdc_draw_line(dg_gdc_p self, int x1, int y1, int x2, int y2)
{
	if (self->pen && self->pen->size>0)
	{
		gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;
		if (self->is_antialiaze(self))
			gdImageSetAntiAliased(gd_context->gd_surface, DG_GD_COLOR(self->pen->color));
		else
			gd_context->gd_surface->AA = 0;

		switch (self->pen->style)
		{
		case PEN_SOLID:  
		{
			gdImageLine(gd_context->gd_surface, x1, y1, x2, y2, gd_get_pen_color(self, self->pen));
		}
		break;
		case PEN_DOTTED: /* FIXME: not supported by GD */
		case PEN_DASHED: 
		{
			gdImageDashedLine(gd_context->gd_surface, x1, y1, x2, y2, gd_get_pen_color(self, self->pen));
		}
		break;
		default: assert(0);
		}
	}
}

static void gd_gdc_draw_rect(dg_gdc_p self, int x, int y, int w, int h)
{
	if (self->pen && self->pen->size>0)
	{
		int i;
		gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;

		if (self->is_antialiaze(self))
			gdImageSetAntiAliased(gd_context->gd_surface, DG_GD_COLOR(self->pen->color));
		else
			gd_context->gd_surface->AA = 0;

		gdImageSetThickness(gd_context->gd_surface, 1);
		for (i = 0; i < self->pen->size; i++)
		{
			self->draw_line(self, x + i, y + i, x + w - 1 - i, y + i);
			self->draw_line(self, x + i, y + i, x + i, y + h - 1 - i);
			self->draw_line(self, x + w - 1 - i, y + h - 1 - i, x + w - 1 - i, y + i);
			self->draw_line(self, x + w - 1 - i, y + h - 1 - i, x + i, y + h - 1 - i);
		}
		gdImageSetThickness(gd_context->gd_surface, self->pen->size);
	}
}

static void gd_gdc_draw_fillrect(dg_gdc_p self, int x, int y, int w, int h)
{
	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;

	if (self->is_antialiaze(self))
		gdImageSetAntiAliased(gd_context->gd_surface, DG_GD_COLOR(self->brush->color));
	else
		gd_context->gd_surface->AA = 0;

	gdImageFilledRectangle(gd_context->gd_surface, x, y, x + w - 1, y + h - 1, gd_get_brush_color(self, self->brush));
}

static void gd_gdc_draw_fillpolygon(dg_gdc_p self, int* coords, int ncoords)
{
	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;

	if (self->is_antialiaze(self))
		gdImageSetAntiAliased(gd_context->gd_surface, DG_GD_COLOR(self->brush->color));
	else
		gd_context->gd_surface->AA = 0;

	if (ncoords == 1)
	{
		self->set_pixel(self, coords[0], coords[1], gd_get_brush_color(self, self->brush));
	}
	else
		gdImageFilledPolygon(gd_context->gd_surface, (gdPointPtr)coords, ncoords, gd_get_brush_color(self, self->brush));
}
//
// Fast but disregards the alpha channel
//static void gd_gdc_draw_image(dg_gdc_p self, int x, int y, dg_surface_p image)
//{
//	int i;
//	unsigned char* pline;
//	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;
//	assert(x+image->width <= gd_context->gd_surface->sx);
//	assert(y+image->height <= gd_context->gd_surface->sy);
//	image->lock(image, 0);
//	pline=(unsigned char*)image->pixels;
//	for (i=0; i<image->height; i++)
//	{
//		dg_pixel_format_convert(
//			(struct dg_pixel_format_t*)&gd_pixel_format,           /* GD image pixel format */
//			(unsigned char*)(gd_context->gd_surface->tpixels[i+y]+x),/* GD image buffer */
//			0,                                                     /* GD image pitch */
//			(struct dg_pixel_format_t*)&self->surface->device->pixel_format,/* DG surface pixel format */
//			pline,                                                 /* DG surface buffer */
//			image->pitch,                                          /* DG surface pitch */
//			image->width,                                          /* DG surface width */
//			1);
//		pline+=image->pitch;
//	}
//	image->unlock(image);
//}

static void gd_gdc_draw_image(dg_gdc_p self, int x, int y, dg_surface_p image)
{
	int i;
	unsigned char* pline;
	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;
	//assert(x+image->width <= gd_context->gd_surface->sx);
	//assert(y+image->height <= gd_context->gd_surface->sy);
	image->lock(image, 0);
	pline=(unsigned char*)image->pixels;
	for (i=0; i<image->height; i++)
	{
		unsigned char* src=pline;
		int j;
		for (j=0; j<image->width; j++)
		{
			unsigned long c=0;
			switch (self->surface->device->pixel_format.bitsPerPixel)
			{
			case 15:
			case 16:
					c = *((unsigned short*)src);
					src+=2;
			break;
			case 24:
					c = *((unsigned short*)src);
					src+=2;
					c |= ((*src)<<16);
					src++;
			break;
			case 32:
					c = *((unsigned long*)src);
					src+=4;
			break;
			}
			c = dg_pixel_unpack(&self->surface->device->pixel_format, c);
			if (image->alpha_channel)
			{
				DG_SET_ALPHA(c, image->alpha_channel[i*image->width + j]);
			}
			gdImageSetPixel(gd_context->gd_surface, x + j, y + i, DG_GD_COLOR(c));
		}
		pline+=image->pitch;
	}
	image->unlock(image);
}

static void gd_gdc_draw_arc(dg_gdc_p self, int x, int y, int w, int h, int a1, int a2)
{
	if (self->pen && self->pen->size>0)
	{
		gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;

		if (self->is_antialiaze(self))
			gdImageSetAntiAliased(gd_context->gd_surface, DG_GD_COLOR(self->pen->color));
		else
			gd_context->gd_surface->AA = 0;

		gdImageArc(gd_context->gd_surface, x, y, w, h, a1, a2, gd_get_pen_color(self, self->pen));
	}
}

static void gd_gdc_draw_fillarc(dg_gdc_p self, int x, int y, int w, int h, int a1, int a2, dg_arc_style_t style)
{
	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;
	int gdStyle=(style & 1) == CHORD_STYLE?gdChord:gdArc;
	if (style & ARC_NOFILL) gdStyle|=gdNoFill;
	if (style & ARC_EDGED) gdStyle|=gdEdged;

	if (self->is_antialiaze(self))
		gdImageSetAntiAliased(gd_context->gd_surface, DG_GD_COLOR(self->brush->color));
	else
		gd_context->gd_surface->AA = 0;

	gdImageFilledArc(gd_context->gd_surface, x, y, w, h, a1, a2, 
		gd_get_brush_color(self, self->brush), gdStyle);
}

static void gd_gdc_destroy(dg_gdc_p self)
{
	gd_gdc_context_p gd_context = (gd_gdc_context_p)self->context;
	copy_gd_to_dg_image(gd_context->gd_surface, self->surface);
	self->surface->unlock(self->surface);
	gdImageDestroy(gd_context->gd_surface);
	dg_gdc_destroy(self);
}

struct dg_gdc_t* gd_gdc_create(struct dg_surface_t* surface)
{
	dg_gdc_p self = (dg_gdc_p)dg_gdc_create(surface);
	gd_gdc_context_p gd_context = (gd_gdc_context_p)malloc(sizeof(gd_gdc_context_t));
	self->context = gd_context;

	self->surface->lock(self->surface, 0);
	gd_context->gd_surface = create_gd_from_dg_image(self->surface);

	self->set_clip_rect    = gd_gdc_set_clip_rect;
	self->set_pen          = gd_gdc_set_pen;
	self->set_brush        = gd_gdc_set_brush;
	self->set_pixel        = gd_gdc_set_pixel;
	self->get_pixel        = gd_gdc_get_pixel;
	self->draw_line        = gd_gdc_draw_line;
	self->draw_rect        = gd_gdc_draw_rect;
	self->draw_fillrect    = gd_gdc_draw_fillrect;
	self->draw_fillpolygon = gd_gdc_draw_fillpolygon;
	self->draw_image       = gd_gdc_draw_image;
	self->draw_arc         = gd_gdc_draw_arc;
	self->draw_fillarc     = gd_gdc_draw_fillarc;
	self->destroy          = gd_gdc_destroy;

	return (struct dg_gdc_t*)self;
}
