/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_gdc.c                                           */
/* Description:   Graphics Device Context                            */
/*                                                                   */
/*********************************************************************/

#include <math.h>
#include <string.h>
#include <malloc.h>
#include <dg/dg_gdc.h>
#include <dg/dg_surface.h>

static dg_font_renderer_p font_renderer=0;

/* GDC Font */

static void  dg_font_set_size(dg_font_p self, int size)
{
	self->size=size;
}

static int dg_font_get_size(dg_font_p self)
{
	return self->size;
}

static void dg_font_set_face(dg_font_p self, const char* face)
{
	if (self->face)
	{
		free(self->face);
	}
	self->face=strdup(face);
}

static const char* dg_font_get_face(dg_font_p self)
{
	return self->face;
}


static int dg_font_get_ascender(dg_font_p self)
{
	return 0;
}

static int dg_font_get_descender(dg_font_p self)
{
	return 0;
}

static int dg_font_get_height(dg_font_p self)
{
	return 0;
}

static int dg_font_get_text_width(dg_font_p self, const char* text, int length)
{
	return 0;
}

void dg_font_destroy(dg_font_p self)
{
	if (self->face)
		free(self->face);
	free(self);
}

dg_font_p dg_font_create(const char* face, int size)
{
	dg_font_p self=(dg_font_p)malloc(sizeof(dg_font_t));
	self->context=0;
	self->size=0;
	self->face=0;

	self->set_size       = dg_font_set_size;
	self->get_size       = dg_font_get_size;
	self->set_face       = dg_font_set_face;
	self->get_face       = dg_font_get_face;
	self->get_ascender   = dg_font_get_ascender;
	self->get_descender  = dg_font_get_descender;
	self->get_height     = dg_font_get_height;
	self->get_text_width = dg_font_get_text_width;
	self->destroy        = dg_font_destroy;

	self->set_size(self, size);
	self->set_face(self, face);
	return self;
}

/* GDC Pen */

static void dg_pen_set_size(dg_pen_p self, int size)
{
	self->size=size;
}

static int dg_pen_get_size(dg_pen_p self)
{
	return self->size;
}

static void dg_pen_set_color(dg_pen_p self, dg_color_t color)
{
	self->color=color;
}

static dg_color_t dg_pen_get_color(dg_pen_p self)
{
	return self->color;
}

static void dg_pen_set_style(dg_pen_p self, dg_pen_style_t style)
{
	self->style=style;
}

static dg_pen_style_t dg_pen_get_style(dg_pen_p self)
{
	return self->style;
}

static void dg_pen_destroy(dg_pen_p self)
{
	free(self);
}

dg_pen_p dg_pen_create(int size, dg_color_t color, dg_pen_style_t style)
{
	dg_pen_p self=(dg_pen_p)malloc(sizeof(dg_pen_t));
	self->size=size;
	self->color=color;
	self->style=style;

	self->set_size  = dg_pen_set_size;
	self->get_size  = dg_pen_get_size;
	self->set_color = dg_pen_set_color;
	self->get_color = dg_pen_get_color;
	self->set_style = dg_pen_set_style;
	self->get_style = dg_pen_get_style;
	self->destroy   = dg_pen_destroy;

	return self;
}

/* GDC Brush */

static void dg_brush_set_color(dg_brush_p self, dg_color_t color)
{
	self->color=color;
}

static dg_color_t dg_brush_get_color(dg_brush_p self)
{
	return self->color;
}

static void dg_brush_set_style(dg_brush_p self, dg_brush_style_t style)
{
	self->style=style;
}

static dg_brush_style_t dg_brush_get_style(dg_brush_p self)
{
	return self->style;
}

static void dg_brush_set_pattern(dg_brush_p self, dg_surface_p pattern)
{
	self->pattern=pattern;
}

static dg_surface_p dg_brush_get_pattern(dg_brush_p self)
{
	return self->pattern;
}

static void dg_brush_destroy(dg_brush_p self)
{
	free(self);
}

dg_brush_p dg_brush_create(dg_color_t color, dg_brush_style_t style)
{
	dg_brush_p self=(dg_brush_p)malloc(sizeof(dg_brush_t));

	self->color=color;
	self->style=style;

	self->get_color   = dg_brush_get_color;
	self->set_color   = dg_brush_set_color;
	self->get_style   = dg_brush_get_style;
	self->set_style   = dg_brush_set_style;
	self->get_pattern = dg_brush_get_pattern;
	self->set_pattern = dg_brush_set_pattern;
	self->destroy     = dg_brush_destroy;

	return self;
}

/* GDC */

void dg_gdc_set_clip_rect(dg_gdc_p self, int x, int y, int w, int h)
{
	if (self->clip_rect)
	{
		self->clip_rect->destroy(self->clip_rect);
	}
	self->clip_rect=dg_rect_create(x, y, w, h);
}

static dg_rect_p dg_gdc_get_clip_rect(dg_gdc_p self)
{
	return self->clip_rect;
}

static void dg_gdc_set_pen(dg_gdc_p self, dg_pen_p pen)
{
	if (self->pen)
	{
		self->pen->destroy(self->pen);
	}
	self->pen=pen;
}

static dg_pen_p dg_gdc_get_pen(dg_gdc_p self)
{
	return self->pen;
}

static void dg_gdc_set_brush(dg_gdc_p self, dg_brush_p brush)
{
	if (self->brush)
	{
		self->brush->destroy(self->brush);
	}
	self->brush=brush;
}

static dg_brush_p dg_gdc_get_brush(dg_gdc_p self)
{
	return self->brush;
}


static int font_plugin_enum_callback(dg_font_renderer_p* font_renderer, dg_plugin_info_p plugin_info)
{
	*font_renderer=plugin_info->font_renderer;

	/* font renderer found, stop plugin enumeration */
	return 1;
}

static void dg_gdc_set_font(dg_gdc_p self, const char* fontface, int fontsize)
{
	static int plugin_checked=0;
	dg_font_p font;

	if (self->font)
	{
		self->font->destroy(self->font);
	}

	if (!plugin_checked)
	{
		dg_plugin_enum(PLUGIN_FONT_RENDERER, font_plugin_enum_callback, &font_renderer);
		plugin_checked=1;
	}

	font = (dg_font_p)(font_renderer ? 
		(dg_font_p)font_renderer->font_create(fontface, fontsize): /* create font by plugin */
		dg_font_create(fontface, fontsize));             /* create native DG font */

	self->font=font;
}

static dg_font_p dg_gdc_get_font(dg_gdc_p self)
{
	return self->font;
}

static void dg_gdc_set_pixel(dg_gdc_p self, int x, int y, dg_color_t color)
{

}

static dg_color_t dg_gdc_get_pixel(dg_gdc_p self, int x, int y)
{
	return 0;
}

static void dg_gdc_draw_line(dg_gdc_p self, int x1, int y1, int x2, int y2)
{
}

static void dg_gdc_draw_lines(dg_gdc_p self, int* coords, int ncoords)
{
	int i;
	ncoords<<=2;
	for (i=0; i<ncoords; i+=4)
	{
		self->draw_line(self, coords[i], coords[i+1], coords[i+2], coords[i+3]);
	}
}

static void dg_gdc_set_align(dg_gdc_p self, dg_align_t align)
{
	self->align = align;
}

static dg_align_t dg_gdc_get_align(dg_gdc_p self)
{
	return self->align;
}

static void dg_gdc_draw_text(dg_gdc_p self, int x, int y, const char* text, int length)
{
	if (font_renderer)
	{
		font_renderer->draw_text((struct dg_gdc_t*)self, x, y, text, length);
	}
}

static void dg_gdc_draw_text_rect(dg_gdc_p self, int x, int y, int w, int h, const char* text, int length)
{
	dg_align_t align = self->get_align(self);

	/* align horizontaly */
	if (align & ALIGN_CENTER_X)
	{
		switch (align & ALIGN_CENTER_X)
		{
		case ALIGN_CENTER_X:
			x += (w >> 1);
		break;
		case ALIGN_LEFT:
		break;
		case ALIGN_RIGHT:
			x += w;
		break;
		}
	}

	/* align verticaly */
	if (align & ALIGN_CENTER_Y)
	{
		switch (align & ALIGN_CENTER_Y)
		{
		case ALIGN_CENTER_Y:
			y += (h >> 1);
		break;
		case ALIGN_TOP:
		break;
		case ALIGN_BOTTOM:
			y += h;
		break;
		}
	}

	self->draw_text(self, x, y, text, length);
}

static void dg_gdc_draw_rect(dg_gdc_p self, int x, int y, int w, int h)
{
}

static void dg_gdc_draw_fillrect(dg_gdc_p self, int x, int y, int w, int h)
{
}

static void dg_gdc_draw_fillrects(dg_gdc_p self, dg_rect_p* rects, int nrects)
{
	int i;
	for (i=0; i<nrects; i++)
	{
		self->draw_fillrect(self, rects[i]->x, rects[i]->y, rects[i]->w, rects[i]->h);
	}
}

static void dg_gdc_draw_polygon(dg_gdc_p self, int* coords, int ncoords)
{
	switch (ncoords)
	{
	case 0: return;
	case 1: self->set_pixel(self, coords[0], coords[1], self->pen->color); break;
	case 2: self->draw_line(self, coords[0], coords[1], coords[2], coords[3]); break;
	default: 
		{
			int i, x=coords[0], y=coords[1];
			ncoords<<=1;
			for (i=2; i<ncoords; i+=2)
			{
				self->draw_line(self, x, y, coords[i], coords[i+1]);
				x=coords[i], y=coords[i+1];
			}
			self->draw_line(self, x, y, coords[0], coords[1]);
		}
		break;
	}
}

static void dg_gdc_draw_fillpolygon(dg_gdc_p self, int* coords, int ncoords)
{
}

static void dg_gdc_draw_image(dg_gdc_p self, int x, int y, dg_surface_p image)
{
}

static void dg_gdc_draw_image_rect(dg_gdc_p self, int x, int y, int w, int h, dg_surface_p image)
{
	dg_align_t align = self->get_align(self);

	/* align horizontaly */
	if (align & ALIGN_CENTER_X)
	{
		switch (align & ALIGN_CENTER_X)
		{
		case ALIGN_CENTER_X:
			x += ((w - image->width) >> 1);
		break;
		case ALIGN_LEFT:
		break;
		case ALIGN_RIGHT:
			x += (w - image->width);
		break;
		}
	}

	/* align verticaly */
	if (align & ALIGN_CENTER_Y)
	{
		switch (align & ALIGN_CENTER_Y)
		{
		case ALIGN_CENTER_Y:
			y += ((h - image->height) >> 1);
		break;
		case ALIGN_TOP:
		break;
		case ALIGN_BOTTOM:
			y += (h - image->height);
		break;
		}
	}
	self->draw_image(self, x, y, image);
}

static void dg_gdc_draw_arc(dg_gdc_p self, int x, int y, int w, int h, int a1, int a2)
{
}

static void dg_gdc_draw_fillarc(dg_gdc_p self, int x, int y, int w, int h, int a1, int a2, dg_arc_style_t style)
{
}

void dg_gdc_set_antialiaze(dg_gdc_p self, int is_antialiaze)
{
	if (is_antialiaze)
		self->antialiazing++;
	else
		self->antialiazing--;
}

static int dg_gdc_is_antialiaze(dg_gdc_p self)
{
	return (self->antialiazing>0);
}

static void dg_gdc_destroy(dg_gdc_p self)
{
	if (self->clip_rect)
	{
		self->clip_rect->destroy(self->clip_rect);
	}
	if (self->pen)
	{
		self->pen->destroy(self->pen);
	}
	if (self->brush)
	{
		self->brush->destroy(self->brush);
	}
	if (self->font)
	{
		self->font->destroy(self->font);
	}
	free(self);
}

void dg_gdc_gradient_colors(dg_gdc_p self, dg_color_t color1, dg_color_t color2, dg_color_p colors, int count)
{
	int i, j;
	double A[4], B[4], AB[4], ab, n[4];

	A[0] = DG_GET_RED(color1);
	A[1] = DG_GET_GREEN(color1);
	A[2] = DG_GET_BLUE(color1);
	A[3] = DG_GET_ALPHA(color1);

	B[0] = DG_GET_RED(color2);
	B[1] = DG_GET_GREEN(color2);
	B[2] = DG_GET_BLUE(color2);
	B[3] = DG_GET_ALPHA(color2);

	ab = 0;
	for (i = 0; i < 4; i++)
	{
		AB[i] = B[i] - A[i];
		ab += AB[i] * AB[i];
	}
	ab = sqrt(ab);

	for (i = 0; i < 4; i++)
	{
		n[i] = AB[i] / ab;
	}
	
	ab /= (double)count;

	for (i = 0; i < count; i++)
	{
		colors[i] = DG_RGB_COLOR((int)floor(A[0]), (int)floor(A[1]), (int)floor(A[2]), (int)floor(A[3]));

		for (j = 0; j < 4; j++)
		{
			A[j] += n[j] * ab;
		}
	}
}

struct dg_gdc_t* dg_gdc_create(struct dg_surface_t* surface)
{
	dg_gdc_p self = (dg_gdc_p)malloc(sizeof(dg_gdc_t));
	self->surface = (dg_surface_p)surface;

	self->pen              = 0;
	self->font             = 0;
	self->brush            = 0;
	self->antialiazing     = 0;
	self->clip_rect        = 0;
	self->align            = ALIGN_LEFT_TOP;

	self->set_clip_rect    = dg_gdc_set_clip_rect;
	self->get_clip_rect    = dg_gdc_get_clip_rect;
	self->set_pen          = dg_gdc_set_pen;
	self->get_pen          = dg_gdc_get_pen;
	self->set_brush        = dg_gdc_set_brush;
	self->get_brush        = dg_gdc_get_brush;
	self->set_font         = dg_gdc_set_font;
	self->get_font         = dg_gdc_get_font;
	self->set_pixel        = dg_gdc_set_pixel;
	self->get_pixel        = dg_gdc_get_pixel;
	self->draw_line        = dg_gdc_draw_line;
	self->draw_lines       = dg_gdc_draw_lines;
	self->set_align        = dg_gdc_set_align;
	self->get_align        = dg_gdc_get_align;
	self->draw_text        = dg_gdc_draw_text;
	self->draw_text_rect   = dg_gdc_draw_text_rect;
	self->draw_rect        = dg_gdc_draw_rect;
	self->draw_fillrect    = dg_gdc_draw_fillrect;
	self->draw_fillrects   = dg_gdc_draw_fillrects;
	self->draw_polygon     = dg_gdc_draw_polygon;
	self->draw_fillpolygon = dg_gdc_draw_fillpolygon;
	self->draw_image       = dg_gdc_draw_image;
	self->draw_image_rect  = dg_gdc_draw_image_rect;
	self->draw_arc         = dg_gdc_draw_arc;
	self->draw_fillarc     = dg_gdc_draw_fillarc;
	self->set_antialiaze   = dg_gdc_set_antialiaze;
	self->is_antialiaze    = dg_gdc_is_antialiaze;
	self->gradient_colors  = dg_gdc_gradient_colors;
	self->destroy          = dg_gdc_destroy;

	return (struct dg_gdc_t*)self;
}

