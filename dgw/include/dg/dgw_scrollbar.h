/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_scrollbar.h                                    */
/* Description:   Scrollbar widget                                   */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_SCROLLBAR_H
#define __DGW_SCROLLBAR_H

#include <dg/dg_widget.h>
#include <dg/dgw_skin.h>

#define SCROLLBAR_SIZE 13

typedef enum
{
	SCROLLBAR_VERTICAL,
	SCROLLBAR_HORIZONTAL
} dgw_scrollbar_orientation_t;

typedef struct _dgw_scrollbar_t
{
	struct _dg_widget_t         widget;
	int                         scroll;
	int                         item_count;
	int                         visible_item_count;
	dgw_scrollbar_orientation_t orientation;

	int  (*get_visible_item_count)(struct _dgw_scrollbar_t*);
	void (*set_visible_item_count)(struct _dgw_scrollbar_t*, int);
	int  (*get_item_count)(struct _dgw_scrollbar_t*);
	void (*set_item_count)(struct _dgw_scrollbar_t*, int);
	void (*set_scroll)(struct _dgw_scrollbar_t*, int);
	int (*get_scroll)(struct _dgw_scrollbar_t*);
	void (*set_orientation)(struct _dgw_scrollbar_t*, int);
	int (*get_orientation)(struct _dgw_scrollbar_t*);
} dgw_scrollbar_t, *dgw_scrollbar_p;

DG_API dgw_scrollbar_p dgw_scrollbar_create(dg_widget_p, dgw_scrollbar_orientation_t);
void dgw_scrollbar_get_cursor_rect(dgw_scrollbar_p, dg_rect_p);

#endif
