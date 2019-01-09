/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      winxp_style.c                                      */
/* Description:   DGW Windows XP Look & Feel                         */
/*                                                                   */
/*********************************************************************/

#include <string.h>
#include <dg/dgw_style.h>
#include "winxp_form.h"
#include "winxp_label.h"
#include "winxp_scrollbar.h"
#include "winxp_list.h"
#include "winxp_button.h"
#include "winxp_edit.h"
#include "winxp_keyboard.h"

extern dgw_style_renders_t     _dgw_renders;
extern dgw_skin_t              _dgw_skin_entries[SKIN_ITEM_LAST];


//SKIN_ITEM_CAPTION,
//SKIN_ITEM_FRAME,
//SKIN_ITEM_WINDOW,
//SKIN_ITEM_SCROLLBAR,
//SKIN_ITEM_LAST,
//
//SKIN_MASK_ITEM_SIZE
//SKIN_MASK_ITEM_COLOR_1
//SKIN_MASK_ITEM_COLOR_2
//SKIN_MASK_FONT_FACE
//SKIN_MASK_FONT_SIZE
//SKIN_MASK_FONT_BOLD
//SKIN_MASK_FONT_ITALLIC

static dgw_skin_t _dgw_skin_entries_winxp[SKIN_ITEM_LAST] = 
{
	{ /* SKIN_ITEM_CAPTION */
		/*mask        */ SKIN_MASK_ALL,
		/*item_size   */ 20,
		/*color1      */ DG_RGB_COLOR(0, 84, 227, 0),
		/*color2      */ DG_RGB_COLOR(61, 149, 255, 255),
		/*font_face   */ "Trebuchet MS Bold",
		/*font_size   */ 14,
		/*font_color  */ DG_RGB_COLOR(255, 255, 255, 255),
		/*font_bold   */ 1,
		/*font_italic */ 0
	},
	{ /* SKIN_ITEM_FRAME */
		/*mask        */ SKIN_MASK_ALL,
		/*item_size   */ 25,
		/*color1      */ DG_RGB_COLOR(0, 84, 227, 255),
		/*color2      */ DG_RGB_COLOR(61, 149, 255, 255),
		/*font_face   */ "Trebuchet MS Bold",
		/*font_size   */ 10,
		/*font_color  */ DG_RGB_COLOR(255, 255, 255, 255),
		/*font_bold   */ 1,
		/*font_italic */ 0
	},
	{ /* SKIN_ITEM_WINDOW */
		/*mask        */ SKIN_MASK_ALL,
		/*item_size   */ 25,
		/*color1      */ DG_RGB_COLOR(0, 84, 227, 255),
		/*color2      */ DG_RGB_COLOR(236, 233, 216, 255),
		/*font_face   */ "Trebuchet MS Bold",
		/*font_size   */ 10,
		/*font_color  */ DG_RGB_COLOR(255, 255, 255, 255),
		/*font_bold   */ 1,
		/*font_italic */ 0
	},
	{ /* SKIN_ITEM_BORDER */
		/*mask        */ SKIN_MASK_ALL,
		/*item_size   */ 1,
		/*color1      */ DG_RGB_COLOR(0, 84, 227, 255),
		/*color2      */ DG_RGB_COLOR(61, 149, 255, 255),
		/*font_face   */ "Trebuchet MS Bold",
		/*font_size   */ 10,
		/*font_color  */ DG_RGB_COLOR(255, 255, 255, 255),
		/*font_bold   */ 1,
		/*font_italic */ 0
	},
	{ /* SKIN_ITEM_OBJECT */
		/*mask        */ SKIN_MASK_ALL,
		/*item_size   */ 0,
		/*color1      */ DG_RGB_COLOR(0, 84, 227, 255),
		/*color2      */ DG_RGB_COLOR(61, 149, 255, 255),
		/*font_face   */ "Arial",
		/*font_size   */ 10,
		/*font_color  */ DG_RGB_COLOR(0, 0, 0, 255),
		/*font_bold   */ 1,
		/*font_italic */ 0
	},
	{ /* SKIN_ITEM_SCROLLBAR */
		/*mask        */ SKIN_MASK_ALL,
		/*item_size   */ 25,
		/*color1      */ DG_RGB_COLOR(0, 84, 227, 255),
		/*color2      */ DG_RGB_COLOR(61, 149, 255, 255),
		/*font_face   */ "Trebuchet MS Bold",
		/*font_size   */ 10,
		/*font_color  */ DG_RGB_COLOR(255, 255, 255, 255),
		/*font_bold   */ 1,
		/*font_italic */ 0
	}
};

void dgw_set_style_winxp(void)
{
	/* export winxp skin */
	memcpy(_dgw_skin_entries, _dgw_skin_entries_winxp, SKIN_ITEM_LAST * sizeof(dgw_skin_t));

	/* export winxp widget renderers */
	_dgw_renders.render_form      = winxp_render_form;
	_dgw_renders.render_label     = winxp_render_label;
	_dgw_renders.render_scrollbar = winxp_render_scrollbar;
	_dgw_renders.render_list      = winxp_render_list;
	_dgw_renders.render_button    = winxp_render_button;
	_dgw_renders.render_edit      = winxp_render_edit;
	_dgw_renders.render_keyboard  = winxp_render_keyboard;
	_dgw_renders.render_keyboard_key  = winxp_render_keyboard_key;
	
	
}
