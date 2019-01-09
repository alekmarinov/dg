/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_label.h                                        */
/* Description:   Label widget                                       */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_LABEL_H
#define __DGW_LABEL_H

#include <dg/dg_widget.h>

typedef struct _dgw_label_t
{
	struct _dg_widget_t widget;
	char*               text;
	int                 options;

	void (*set_text)(struct _dgw_label_t*, const char* );
	const char* (*get_text)(struct _dgw_label_t*);
	void (*set_options)(struct _dgw_label_t*, int );
	int (*get_options)(struct _dgw_label_t*);
} dgw_label_t, *dgw_label_p;

DG_API dgw_label_p dgw_label_create(dg_widget_p, int, int, int, int, const char*);

#endif
