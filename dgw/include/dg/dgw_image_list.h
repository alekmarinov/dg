/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_image_list.h                                   */
/* Description:   Indexed list of images                             */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_IMAGE_LIST_H
#define __DGW_IMAGE_LIST_H

#include <dg/dg_surface.h>

typedef struct _dgw_image_list_t
{
	int ref;
	int count;
	int storage_count;
	dg_surface_p* images;

	void (*add)(struct _dgw_image_list_t*, dg_surface_p);
	dg_surface_p (*get)(struct _dgw_image_list_t*, int);
	int (*get_count)(struct _dgw_image_list_t*);
	void (*add_ref)(struct _dgw_image_list_t*);
	void (*destroy)(struct _dgw_image_list_t*);
} dgw_image_list_t, *dgw_image_list_p;

DG_API dgw_image_list_p dgw_image_list_create();

#endif
