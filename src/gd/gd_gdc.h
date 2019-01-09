/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      gd_gdc.h                                           */
/* Description:   GDC implementation over GD library                 */
/*                                                                   */
/*********************************************************************/

#ifndef GD_GDC_H
#define GD_GDC_H

#include <gd.h>
#include <dg/dg_pixel.h>
#include <dg/dg_gdc.h>

struct dg_gdc_t* gd_gdc_create(struct dg_surface_t*);

#endif
