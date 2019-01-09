/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_edit.h                                       */
/* Description:   Edit widget                                      */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_EDIT_H
#define __DGW_EDIT_H

#include <dg/dg_widget.h>

typedef struct _dgw_edit_t
{
	struct _dg_widget_t widget;
	dg_widget_p         cursor;
	char*               text;
	int                 options;
	int                 position;
	int                 selection;
	int                 scroll;

	void (*set_position)(struct _dgw_edit_t*, int);
	int (*get_position)(struct _dgw_edit_t*);
	void (*set_scroll)(struct _dgw_edit_t*, int);
	int (*get_scroll)(struct _dgw_edit_t*);
	void (*set_text)(struct _dgw_edit_t*, const char*);
	const char* (*get_text)(struct _dgw_edit_t*);
	void (*set_options)(struct _dgw_edit_t*, int);
	int  (*get_options)(struct _dgw_edit_t*);
	int (*left)(struct _dgw_edit_t*);
	int (*right)(struct _dgw_edit_t*);
	int (*home)(struct _dgw_edit_t*);
	int (*end)(struct _dgw_edit_t*);
	int (*backspace)(struct _dgw_edit_t*);
	int (*del)(struct _dgw_edit_t*);
	int (*insert_text)(struct _dgw_edit_t*, const char*, int count);
} dgw_edit_t, *dgw_edit_p;

DG_API dgw_edit_p dgw_edit_create(dg_widget_p, int, int, int, int, const char*);

#endif
