/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_skin.h                                         */
/* Description:   Skin                                               */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_SKIN_H
#define __DGW_SKIN_H

#include <dg/dg_pixel.h>

typedef enum
{
	SKIN_ITEM_CAPTION,
	SKIN_ITEM_FRAME,
	SKIN_ITEM_WINDOW,
	SKIN_ITEM_BORDER,
	SKIN_ITEM_OBJECT,
	SKIN_ITEM_SCROLLBAR,
	SKIN_ITEM_LAST,
} dgw_skin_item_t;

#define SKIN_MASK_ITEM_SIZE         (1<<0)
#define SKIN_MASK_ITEM_COLOR_1      (1<<1)
#define SKIN_MASK_ITEM_COLOR_2      (1<<2)
#define SKIN_MASK_FONT_FACE         (1<<3)
#define SKIN_MASK_FONT_SIZE         (1<<4)
#define SKIN_MASK_FONT_color        (1<<5)
#define SKIN_MASK_FONT_BOLD         (1<<6)
#define SKIN_MASK_FONT_ITALLIC      (1<<7)

#define SKIN_MASK_ALL        \
	SKIN_MASK_ITEM_SIZE |    \
	SKIN_MASK_ITEM_COLOR_1 | \
	SKIN_MASK_ITEM_COLOR_2 | \
	SKIN_MASK_FONT_FACE |    \
	SKIN_MASK_FONT_SIZE |    \
	SKIN_MASK_FONT_BOLD |    \
	SKIN_MASK_FONT_ITALLIC

typedef struct _dgw_skin_t
{
	unsigned int mask;
	int item_size;
	dg_color_t color1;
	dg_color_t color2;
	char* font_face;
	int font_size;
	dg_color_t font_color;
	int font_bold;
	int font_italic;
} dgw_skin_t, *dgw_skin_p;

#endif
