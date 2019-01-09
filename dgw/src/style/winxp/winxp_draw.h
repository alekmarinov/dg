/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      winxp_draw.h                                       */
/* Description:   Common render functions for Windows XP look & feel */
/*                                                                   */
/*********************************************************************/

#ifndef __WINXP_DRAW_H
#define __WINXP_DRAW_H

#include <dg/dg_gdc.h>
#include <dg/dgw_skin.h>
#include <dg/dgw_form.h>

void winxp_draw_caption(
	dg_gdc_p,                  /* GDC context */
	int,                       /* x */
	int,                       /* y */
	int,                       /* width */
	int,                       /* height */
	int,                       /* options describing caption */
	const char*,               /* caption text */
	dg_color_t,                /* color #1 */
	dg_color_t,                /* color #2 (e.g. for gradient style) */
	const char*,               /* font face of caption text */
	int,                       /* font size of caption text */
	dg_color_t                 /* font color of caption text */
);

void winxp_draw_frame(
	dg_gdc_p,                  /* GDC context */
	int,                       /* x */
	int,                       /* y */
	int,                       /* width */
	int,                       /* height */
	int,                       /* frame options */
	int,                       /* border size */
	dg_color_t,                /* border color */
	dg_color_t,                /* background color */
	dg_pen_p,                  /* pen for drawing the border (optional) */
	dg_brush_p                 /* brush for drawing the background (optional) */
);

void winxp_draw_label(
	dg_gdc_p,                  /* GDC context */
	int,                       /* x */
	int,                       /* y */
	int,                       /* width */
	int,                       /* height */
	int,                       /* label options */
	const char*,               /* label text */
	const char*,               /* font face */
	int,                       /* font size */
	dg_color_t,                /* font color */
	int,                       /* border size */
	dg_color_t,                /* border color */
	dg_color_t                 /* background color */
);

void winxp_draw_button(
	dg_gdc_p,                  /* GDC context */
	int,                       /* x */
	int,                       /* y */
	int,                       /* width */
	int,                       /* height */
	int,                       /* button options */
	int,                       /* pushed button? */
	const char*,               /* button text */
	const char*,               /* font face */
	int,                       /* font size */
	dg_color_t,                /* font color */
	int,                       /* border size */
	dg_color_t,                /* border color */
	dg_color_t                 /* background color */
);

void winxp_draw_edit(	
	dg_gdc_p,                  /* GDC context */
	int,                       /* x */
	int,                       /* y */
	int,                       /* width */
	int,                       /* height */
	int,                       /* edit options */
	int,                       /* scroll position */
	const char*,               /* edit text */
	const char*,               /* font face */
	int,                       /* font size */
	dg_color_t,                /* font color */
	int,                       /* border size */
	dg_color_t,                /* border color */
	dg_color_t                 /* background color */
);

#endif
