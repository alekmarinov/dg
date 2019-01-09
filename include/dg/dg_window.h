/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_window.h                                        */
/* Description:   Window Manager interface                           */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_WINDOW_H
#define __DG_WINDOW_H

#include <dg/dg.h>
#include <dg/dg_debug.h>
#include <dg/dg_rect.h>

/*!
	\addtogroup dg_winman Windows Management
	<b>This page is under construction</b>
  *  @{
*/

/*!
	Definition of callback function used to redraw an invalidated window area. 
*/
typedef void (*wm_update_callback_t)(void* param, int x, int y, int w, int h);

/*! \brief \c dg_window_t defines a window object which is a part of \ref dg_winman
*/
typedef struct _dg_window_t
{
/*! Window position and size */
	dg_rect_p rect;

/*! Callback function called when a given area of this window must be updated */
	wm_update_callback_t update_callback;

/*! User parameter provided to the callback function */
	void *param;

/*! Child windows */
	struct _dg_window_t* children;

/*! Owner of this window */
	struct _dg_window_t* parent;

/*! Next window */
	struct _dg_window_t* next;

/*! Previous window */
	struct _dg_window_t* prev;

/*! Is the window visible */
	int visible;

/*! Is the window clips its children */
	int clip_children;

/*! Is the window clips its children */
	int owner_window;

/*! Client area left offset */
	int client_left;

/*! Client area top offset */
	int client_top;

/*! Client area right offset */
	int client_right;

/*! Client area right bottom */
	int client_bottom;

	void (*set_parent)(struct _dg_window_t*, struct _dg_window_t*);
	int  (*is_parent)(struct _dg_window_t*, struct _dg_window_t*);
	void (*insert_child)(struct _dg_window_t*, struct _dg_window_t*);
	void (*remove_child)(struct _dg_window_t*, struct _dg_window_t*);
	struct _dg_window_t* (*get_top)(struct _dg_window_t*);
	void (*set_visible)(struct _dg_window_t*, int);
	int  (*is_visible)(struct _dg_window_t*);
	int  (*intersect)(struct _dg_window_t*, struct _dg_window_t*, int*, int*, int*, int*);
	int  (*intersect_rect)(struct _dg_window_t*, dg_rect_p, int*, int*, int*, int*);
	void (*get_rect)(struct _dg_window_t*, dg_rect_p);
	void (*set_rect)(struct _dg_window_t*, int, int, int, int);
	void (*invalidate)(struct _dg_window_t*, dg_rect_p);
	void (*insert)(struct _dg_window_t*, struct _dg_window_t*);
	void (*insert_last)(struct _dg_window_t*, struct _dg_window_t*);
	void (*raise_top)(struct _dg_window_t*);
	void (*lower_bottom)(struct _dg_window_t*);
	void (*remove)(struct _dg_window_t*);
	void (*destroy)(struct _dg_window_t*);

} dg_window_t, *dg_window_p;

/*! Creates dg_window_t object */
dg_window_p dg_window_create(int, int, int, int, wm_update_callback_t, void*, dg_window_p);

/*! @} */

#endif
