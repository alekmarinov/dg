/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_draw.h                                         */
/* Description:   Helper drawing functions                           */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_DRAW_H
#define __DGW_DRAW_H

#include <dg/dg_gdc.h>

/* Various rectangle drawers */

void dgw_draw_border_rectangle(
	dg_gdc_p,    /* GDC context */
	int,         /* x */
	int,         /* y */
	int,         /* width */
	int,         /* height */
	dg_color_t   /* border color */
);

void dgw_draw_sunken_rectangle(
	dg_gdc_p,    /* GDC context */
	int,         /* x */
	int,         /* y */
	int,         /* width */
	int,         /* height */
	dg_color_t,  /* shadow color */
	dg_color_t   /* hilite color */
);

void dgw_draw_raised_rectangle(
	dg_gdc_p,    /* GDC context */
	int,         /* x */
	int,         /* y */
	int,         /* width */
	int,         /* height */
	dg_color_t,  /* shadow color */
	dg_color_t   /* hilite color */
);

void dgw_draw_groove_rectangle(
	dg_gdc_p,    /* GDC context */
	int,         /* x */
	int,         /* y */
	int,         /* width */
	int,         /* height */
	dg_color_t,  /* shadow color */
	dg_color_t   /* hilite color */
);

void dgw_draw_ridge_rectangle(
	dg_gdc_p,    /* GDC context */
	int,         /* x */
	int,         /* y */
	int,         /* width */
	int,         /* height */
	dg_color_t,  /* shadow color */
	dg_color_t   /* hilite color */
);

void dgw_draw_double_sunken_rectangle(
	dg_gdc_p,    /* GDC context */
	int,         /* x */
	int,         /* y */
	int,         /* width */
	int,         /* height */
	dg_color_t,  /* shadow color */
	dg_color_t,  /* hilite color */
	dg_color_t,  /* border color */
	dg_color_t   /* base color */
);

void dgw_draw_double_raised_rectangle(
	dg_gdc_p,    /* GDC context */
	int,         /* x */
	int,         /* y */
	int,         /* width */
	int,         /* height */
	dg_color_t,  /* shadow color */
	dg_color_t,  /* hilite color */
	dg_color_t,  /* border color */
	dg_color_t   /* base color */
);

#endif
