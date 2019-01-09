/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_gdc.h                                           */
/* Description:   Graphics Device Context                            */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_GDC_H
#define __DG_GDC_H

#include <dg/dg_rect.h>
#include <dg/dg_pixel.h>
#include <dg/dg_surface.h>

/*!
	\addtogroup dg_gdc Graphics Device Context
	<b>This page is under construction</b>
  *  @{
*/

/*! \brief \c dg_font_t defines a font used to write text
*/
typedef struct _dg_font_t
{
	void*          context;
	int            size;
	char*          face;

	void           (*set_size)(struct _dg_font_t*, int);
	int            (*get_size)(struct _dg_font_t*);
	void           (*set_face)(struct _dg_font_t*, const char*);
	const char*    (*get_face)(struct _dg_font_t*);
	int            (*get_ascender)(struct _dg_font_t*);
	int            (*get_descender)(struct _dg_font_t*);
	int            (*get_height)(struct _dg_font_t*);
	int            (*get_text_width)(struct _dg_font_t*, const char*, int);
	void           (*destroy) (struct _dg_font_t*);
} dg_font_t, *dg_font_p;

DG_API dg_font_p dg_font_create(const char*, int);

/*! \brief \c dg_pen_style_t defines a pen style
*/
typedef enum
{
	PEN_SOLID,
	PEN_DOTTED,
	PEN_DASHED,
} dg_pen_style_t;

/*! \brief \c dg_pen_t defines a pen used to draw lines
*/
typedef struct _dg_pen_t
{
	int            size;
	dg_color_t     color;
	dg_pen_style_t style;

	void           (*set_size) (struct _dg_pen_t*, int);
	int            (*get_size) (struct _dg_pen_t*);
	void           (*set_color)(struct _dg_pen_t*, dg_color_t);
	dg_color_t     (*get_color)(struct _dg_pen_t*);
	void           (*set_style)(struct _dg_pen_t*, dg_pen_style_t);
	dg_pen_style_t (*get_style)(struct _dg_pen_t*);
	void           (*destroy)  (struct _dg_pen_t*);
} dg_pen_t, *dg_pen_p;

DG_API dg_pen_p dg_pen_create(int, dg_color_t, dg_pen_style_t);


/*! \brief \c dg_brush_style_t defines a brush style
*/
typedef enum
{
	BRUSH_SOLID=0,
	BRUSH_PATTERN,
} dg_brush_style_t;

/*! \brief \c dg_brush_t defines a brush used to fill areas
*/
typedef struct _dg_brush_t
{
	dg_color_t       color;
	dg_brush_style_t style;
	dg_surface_p     pattern;

	void             (*set_color)(struct _dg_brush_t*, dg_color_t);
	dg_color_t       (*get_color)(struct _dg_brush_t*);
	void             (*set_style)(struct _dg_brush_t*, dg_brush_style_t);
	dg_brush_style_t (*get_style)(struct _dg_brush_t*);
	void             (*set_pattern)(struct _dg_brush_t*, dg_surface_p);
	dg_surface_p     (*get_pattern)(struct _dg_brush_t*);
	void             (*destroy)  (struct _dg_brush_t*);
} dg_brush_t, *dg_brush_p;

DG_API dg_brush_p dg_brush_create(dg_color_t, dg_brush_style_t);

/*! \brief \c dg_arc_style_t defines an arc style
*/
typedef enum
{
	/*! produces arc with rounded edges */
	ARC_STYLE=0,
	/*! connects the starting and ending angles with a streight line */
	CHORD_STYLE=1,
	/*! outlined arc */
	ARC_NOFILL=2,
	/*! connect the starting and ending angles with the center */
	ARC_EDGED=4,
} dg_arc_style_t;

/*! \brief \c dg_align_t defines all alignment masks
*/
typedef enum
{
	ALIGN_LEFT          = 0x1, /* 0001 */
	ALIGN_RIGHT         = 0x2, /* 0010 */
	ALIGN_CENTER_X      = 0x3, /* 0011 */
	ALIGN_TOP           = 0x4, /* 0100 */
	ALIGN_LEFT_TOP      = 0x5, /* 0101 */
	ALIGN_RIGHT_TOP     = 0x6, /* 0110 */
	ALIGN_TOP_CENTER    = 0x7, /* 0111 */
	ALIGN_BOTTOM        = 0x8, /* 1000 */
	ALIGN_LEFT_BOTTOM   = 0x9, /* 1001 */
	ALIGN_RIGHT_BOTTOM  = 0xA, /* 1010 */
	ALIGN_BOTTOM_CENTER = 0xB, /* 1011 */
	ALIGN_CENTER_Y      = 0xC, /* 1100 */
	ALIGN_LEFT_CENTER   = 0xD, /* 1101 */
	ALIGN_RIGHT_CENTER  = 0xE, /* 1110 */
	ALIGN_CENTER        = 0xF, /* 1111 */
} dg_align_t;

/*! \brief \c dg_gdc_t defines a graphics device context attached to a certain surface
*/
typedef struct _dg_gdc_t
{
/*! Platform specific GDC object */
	void* context;

/*! The target surface this GDC is attached to */
	dg_surface_p surface;

/*! Clipped rectangle */
	dg_rect_p clip_rect;

/*! Current pen */
	dg_pen_p pen;

/*! Current brush */
	dg_brush_p brush;

/*! Current font */	
	dg_font_p font;

/*! Antialiazing option set to on if antialiazing > 0 or off antialiazing <= 0
 * Use method set_antialiaze with boolean value to set on or off antialiazing
*/
	int antialiazing;

/*! Text alignment */	
	dg_align_t align;

	void       (*set_clip_rect)(struct _dg_gdc_t*, int, int, int, int);
	dg_rect_p  (*get_clip_rect)(struct _dg_gdc_t*);
	void       (*set_pen)      (struct _dg_gdc_t*, dg_pen_p);
	dg_pen_p   (*get_pen)      (struct _dg_gdc_t*);
	void       (*set_brush)    (struct _dg_gdc_t*, dg_brush_p);
	dg_brush_p (*get_brush)    (struct _dg_gdc_t*);
	void       (*set_font)     (struct _dg_gdc_t*, const char*, int);
	dg_font_p  (*get_font)     (struct _dg_gdc_t*);
	void       (*set_pixel)    (struct _dg_gdc_t*, int, int, dg_color_t);
	dg_color_t (*get_pixel)    (struct _dg_gdc_t*, int, int);
	void       (*draw_line)    (struct _dg_gdc_t*, int, int, int, int);
	void       (*draw_lines)   (struct _dg_gdc_t*, int*, int);
	void       (*set_align)    (struct _dg_gdc_t*, dg_align_t);
	dg_align_t (*get_align)    (struct _dg_gdc_t*);
	void       (*draw_text)    (struct _dg_gdc_t*, int, int, const char*, int);
	void       (*draw_text_rect)(struct _dg_gdc_t*, int, int, int, int, const char*, int);
	void       (*draw_rect)    (struct _dg_gdc_t*, int, int, int, int);
	void       (*draw_fillrect)(struct _dg_gdc_t*, int, int, int, int);
	void       (*draw_fillrects)(struct _dg_gdc_t*, dg_rect_p*, int);
	void       (*draw_polygon) (struct _dg_gdc_t*, int*, int);
	void       (*draw_fillpolygon)(struct _dg_gdc_t*, int*, int);
	void       (*draw_image)   (struct _dg_gdc_t*, int, int, dg_surface_p);
	void       (*draw_image_rect)(struct _dg_gdc_t*, int, int, int, int, dg_surface_p);
	void       (*draw_arc)     (struct _dg_gdc_t*, int, int, int, int, int, int);
	void       (*draw_fillarc) (struct _dg_gdc_t*, int, int, int, int, int, int, dg_arc_style_t);
	void       (*set_antialiaze)(struct _dg_gdc_t*, int);
	int        (*is_antialiaze)(struct _dg_gdc_t*);
	void       (*gradient_colors)(struct _dg_gdc_t*, dg_color_t, dg_color_t, dg_color_p, int);
	void       (*destroy)      (struct _dg_gdc_t*);
} dg_gdc_t, *dg_gdc_p;

DG_API struct dg_gdc_t* dg_gdc_create(struct dg_surface_t*);
DG_API void dg_gdc_destroy(dg_gdc_p);
DG_API void dg_font_destroy(dg_font_p);
DG_API void dg_gdc_set_clip_rect(dg_gdc_p, int, int, int, int);
DG_API void dg_gdc_set_pen(dg_gdc_p, dg_pen_p);
DG_API void dg_gdc_set_font(dg_gdc_p, const char*, int);
DG_API void dg_gdc_set_brush(dg_gdc_p, dg_brush_p);

/*! @} */

#endif
