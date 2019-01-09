/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_form.h                                         */
/* Description:   Form widget                                        */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_FORM_H
#define __DGW_FORM_H

#include <dg/dg_widget.h>

#define DGW_FORM_OPTION_RESIZE 1

typedef struct _dgw_form_t
{
	struct _dg_widget_t widget;
	char*               text;
	int                 options;
	int                 drag_info;

	void (*set_text)(struct _dgw_form_t*, const char* );
	const char* (*get_text)(struct _dgw_form_t*);
	void (*set_options)(struct _dgw_form_t*, int );
	const int (*get_options)(struct _dgw_form_t*);

} dgw_form_t, *dgw_form_p;

DG_API dgw_form_p dgw_form_create(dg_widget_p, int, int, int, int, const char*);

#endif
