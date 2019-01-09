/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_style.h                                        */
/* Description:   DGW Look & Feel API                                */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_STYLE_H
#define __DGW_STYLE_H

typedef enum
{
	DGW_STYLE_WINXP
} dgw_style_t;

#define DGW_STYLE_DEFAULT DGW_STYLE_WINXP

typedef struct
{
	void (*render_form)(dg_widget_p);
	void (*render_label)(dg_widget_p);
	void (*render_scrollbar)(dg_widget_p);
	void (*render_list)(dg_widget_p);
	void (*render_button)(dg_widget_p);
	void (*render_edit)(dg_widget_p);
	void (*render_keyboard)(dg_widget_p);
	void (*render_keyboard_key)(dg_widget_p);
} dgw_style_renders_t;

DG_API void dgw_set_style(dgw_style_t);

#endif
