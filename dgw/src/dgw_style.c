/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_style.c                                        */
/* Description:   DGW Look & Feel API                                */
/*                                                                   */
/*********************************************************************/

#include <dg/dgw_style.h>
#include <dg/dgw_skin.h>
#include "style/winxp/winxp_style.h"

dgw_style_renders_t     _dgw_renders;
dgw_skin_t              _dgw_skin_entries[SKIN_ITEM_LAST];

void dgw_set_style(dgw_style_t style)
{
	switch (style)
	{
	case DGW_STYLE_WINXP:dgw_set_style_winxp(); break;
	}
}
