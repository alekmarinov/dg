/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_rect.h                                          */
/* Description:   Rectangle structure interface                      */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_RECT_H
#define __DG_RECT_H

#include <dg/dg.h>
#include <dg/dg_debug.h>

/*!
	\addtogroup dg_rect Rects
	<b>This page is under construction</b>
  *  @{
*/

/*! \brief dg_rect_t defines rectanglular area with ability to move, resize and intersect with another rectangular area
*/
typedef struct _dg_rect_t
{
	int x, y, w, h;

	void  (*set_x)(struct _dg_rect_t*, int x);
	int   (*get_x)(struct _dg_rect_t*);
	void  (*set_y)(struct _dg_rect_t*, int y);
	int   (*get_y)(struct _dg_rect_t*);
	void  (*set_w)(struct _dg_rect_t*, int w);
	int   (*get_w)(struct _dg_rect_t*);
	void  (*set_h)(struct _dg_rect_t*, int h);
	int   (*get_h)(struct _dg_rect_t*);
	int   (*get_left)(struct _dg_rect_t*);
	int   (*get_top)(struct _dg_rect_t*);
	int   (*get_right)(struct _dg_rect_t*);
	int   (*get_bottom)(struct _dg_rect_t*);

	int   (*is_point_inside)(struct _dg_rect_t*, int, int);
	int   (*intersect)(struct _dg_rect_t*, struct _dg_rect_t*, int*, int*, int*, int*);
	void  (*move)(struct _dg_rect_t*, int x, int y);
	void  (*resize)(struct _dg_rect_t*, int w, int h);
	void  (*copy)(struct _dg_rect_t*, int x, int y, int w, int h);
	void  (*copy_rect)(struct _dg_rect_t*, struct _dg_rect_t*);
	struct _dg_rect_t* (*clone)(struct _dg_rect_t*);
	void  (*destroy)(struct _dg_rect_t*);

} dg_rect_t, *dg_rect_p;

/*! Creates dg_rect_t object */
DG_API dg_rect_p dg_rect_create(int x, int y, int w, int h);

/*! @} */

#endif
