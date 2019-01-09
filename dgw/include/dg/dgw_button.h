/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_button.h                                       */
/* Description:   Button widget                                      */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_BUTTON_H
#define __DGW_BUTTON_H

#include <dg/dg_widget.h>

typedef struct _dgw_button_t
{
	struct _dg_widget_t widget;
	char*               text;
	int                 options;
	int                 is_push;

	void (*set_pushed)(struct _dgw_button_t*, int);
	int  (*is_pushed)(struct _dgw_button_t*);
	void (*set_text)(struct _dgw_button_t*, const char* );
	const char* (*get_text)(struct _dgw_button_t*);
	void (*set_options)(struct _dgw_button_t*, int );
	int  (*get_options)(struct _dgw_button_t*);
} dgw_button_t, *dgw_button_p;

DG_API dgw_button_p dgw_button_create(dg_widget_p, int, int, int, int, const char*);

#endif
