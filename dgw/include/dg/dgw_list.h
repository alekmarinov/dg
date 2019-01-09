/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_list.h                                         */
/* Description:   List widget                                        */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_LIST_H
#define __DGW_LIST_H

#include <dg/dg_widget.h>
#include <dg/dgw_scrollbar.h>
#include <dg/dgw_image_list.h>

typedef enum 
{
	LIST_COLUMN_IMAGE,
	LIST_COLUMN_TEXT
} dgw_list_column_type;

typedef struct _dgw_list_column_t
{
	dgw_list_column_type    column_type;
	int                     width;
	int                     visible;
} dgw_list_column_t, *dgw_list_column_p;

typedef union _dgw_list_cell_t
{
	char*                   text;
	int                     image;
} dgw_list_cell_t, *dgw_list_cell_p;

typedef struct _dgw_list_row_t
{
	int                     is_selected;
	int                     is_separator;
	dgw_list_cell_p         cells;
} dgw_list_row_t, *dgw_list_row_p;

typedef struct _dgw_list_t
{
	struct _dg_widget_t     widget;
	dgw_scrollbar_p         hscroll;
	dgw_scrollbar_p         vscroll;
	dgw_list_row_p          rows;
	dgw_list_column_p       cols;
	int                     nrows;
	int                     ncols;
	int                     storage_count;
	int                     options;
	int                     spacing;
	int                     padding;
	int                     row_height;
	int                     cursor;
	int                     scroll;
	int                     is_wrap;
	int                     is_cursor_visible;
	dgw_image_list_p        image_list;

	void                    (*move_up)(struct _dgw_list_t*);
	void                    (*move_down)(struct _dgw_list_t*);
	void                    (*page_up)(struct _dgw_list_t*);
	void                    (*page_down)(struct _dgw_list_t*);
	void                    (*order_up)(struct _dgw_list_t*);
	void                    (*order_down)(struct _dgw_list_t*);
	void                    (*set_cursor)(struct _dgw_list_t*, int);
	int                     (*get_cursor)(struct _dgw_list_t*);
	void                    (*set_scroll)(struct _dgw_list_t*, int);
	int                     (*get_scroll)(struct _dgw_list_t*);
	void                    (*set_image_list)(struct _dgw_list_t*, dgw_image_list_p);
	dgw_image_list_p        (*get_image_list)(struct _dgw_list_t*);
	void                    (*add_column)(struct _dgw_list_t*, dgw_list_column_t);
	const dgw_list_column_p (*get_column)(struct _dgw_list_t*, int);
	int                     (*get_column_count)(struct _dgw_list_t*);
	void                    (*add_row)(struct _dgw_list_t*, dgw_list_row_t);
	const dgw_list_row_p    (*get_row)(struct _dgw_list_t*, int);
	int                     (*get_row_count)(struct _dgw_list_t*);
	int                     (*get_visible_row_count)(struct _dgw_list_t*);
	void                    (*remove_row)(struct _dgw_list_t*, int);
	void                    (*remove_all)(struct _dgw_list_t*);
	void                    (*set_options)(struct _dgw_list_t*, int );
	int                     (*get_options)(struct _dgw_list_t*);
	void                    (*validate_scroll)(struct _dgw_list_t*);
} dgw_list_t, *dgw_list_p;

DG_API dgw_list_p dgw_list_create(dg_widget_p, int, int, int, int);

#endif
