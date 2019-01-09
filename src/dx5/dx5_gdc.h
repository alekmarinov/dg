/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_gdc.h                                          */
/* Description:   Win32 Graphics Device Context                      */
/*                                                                   */
/*********************************************************************/

#ifndef __DX5_GDC
#define __DX5_GDC

#include <dg/dg_gdc.h>

typedef struct
{
	HDC hDC;
	HPEN hPen;
	HBRUSH hBrush;
	HFONT hFont;
} dx5_gdc_t, *dx5_gdc_p; 

DG_API dg_gdc_p dx5_gdc_create(dg_widget_p);

#endif
