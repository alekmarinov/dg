/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_blit.h                                         */
/* Description:   DG/DirectX blittings                               */
/*                                                                   */
/*********************************************************************/

#ifndef __DX5_BLIT_H
#define __DX5_BLIT_H

#include <ddraw.h>
#include <dg/dg_blit.h>
#include <dg/dg_surface.h>

/* 
* dx5_surface_blit copies a bitmap from a source rectangle into destination rectangle
* dSurface - the destination surface
* dstRect - the destination rectangle
* sSurface - the source surface
* srcRect - the source rectangle
* alpha - transparency level changing from 0 to 255 changes from invisible to visible
* alphaChannel - transparency level changing from 0 to 255 changes from invisible to visible
* colorKey - transparent color
* options - enable or disable any of alpha, alphaChannel, colorKey parameters
*/
int dx5_surface_blit(dg_surface_p, dg_rect_p, dg_surface_p, dg_rect_p, unsigned char, unsigned char*, dg_color_t, int);

#endif
