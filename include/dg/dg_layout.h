/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_layout.h                                        */
/* Description:   Layout Management interface                        */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_LAYOUT_H
#define __DG_LAYOUT_H

#include <dg/dg_window.h>

/*!
	\addtogroup dg_layout Layout Management
	<b>This page is under construction</b>
  *  @{
*/

/*! \brief \c the common body of dg_layout_properties_p derivates
*/
#define DG_LAYOUT_PROPERTIES_STUB                     \
	void* properties;                                 \
	void (*destroy)(struct _dg_layout_properties_t*);

/*! \brief \c the base structure for dg_layout_properties_p derivates
*/
typedef struct _dg_layout_properties_t
{
	DG_LAYOUT_PROPERTIES_STUB
} dg_layout_properties_t, *dg_layout_properties_p;

/*! \brief \c dg_layout_t defines a layout manager which can be attached to dg_widget_p
* The layout manager role is to rearrange children widgets according 
* their currently set widget's dg_layout_properties_p
*/
typedef void (*dg_layout_manager_p)(dg_window_p, int, int, int, int);

/*! \brief \c anchor layout manager
*/
DG_API void dg_layout_anchor_manager(dg_window_p, int, int, int, int);

/*! \brief \c dg_layout_anchor_properties_p defines a layout properties containing anchor
*/
typedef struct _dg_layout_anchor_properties_t
{
	DG_LAYOUT_PROPERTIES_STUB
	void (*set)(struct _dg_layout_anchor_properties_t*, int);
	void (*get)(struct _dg_layout_anchor_properties_t*, int*);
} dg_layout_anchor_properties_t, *dg_layout_anchor_properties_p;

/*! \brief \c dg_layout_anchor_properties_p constructor
* anchor is a mask with any DG_LAYOUT_ANCHOR_* bits 
*/
DG_API dg_layout_properties_p dg_layout_anchor_properties_create(int anchor);

#define DG_LAYOUT_ANCHOR_NONE              0
#define DG_LAYOUT_ANCHOR_LEFT              (1<<0)
#define DG_LAYOUT_ANCHOR_RIGHT             (1<<1)
#define DG_LAYOUT_ANCHOR_TOP               (1<<2)
#define DG_LAYOUT_ANCHOR_BOTTOM            (1<<3)
#define DG_LAYOUT_ANCHOR_LEFT_TOP          (DG_LAYOUT_ANCHOR_LEFT  | DG_LAYOUT_ANCHOR_TOP)
#define DG_LAYOUT_ANCHOR_LEFT_BOTTOM       (DG_LAYOUT_ANCHOR_LEFT  | DG_LAYOUT_ANCHOR_BOTTOM)
#define DG_LAYOUT_ANCHOR_RIGHT_TOP         (DG_LAYOUT_ANCHOR_RIGHT | DG_LAYOUT_ANCHOR_TOP)
#define DG_LAYOUT_ANCHOR_RIGHT_BOTTOM      (DG_LAYOUT_ANCHOR_RIGHT | DG_LAYOUT_ANCHOR_BOTTOM)
#define DG_LAYOUT_ANCHOR_LEFT_RIGHT        (DG_LAYOUT_ANCHOR_LEFT  | DG_LAYOUT_ANCHOR_RIGHT)
#define DG_LAYOUT_ANCHOR_TOP_BOTTOM        (DG_LAYOUT_ANCHOR_TOP   | DG_LAYOUT_ANCHOR_BOTTOM)
#define DG_LAYOUT_ANCHOR_LEFT_RIGHT_TOP    (DG_LAYOUT_ANCHOR_LEFT_RIGHT | DG_LAYOUT_ANCHOR_TOP)
#define DG_LAYOUT_ANCHOR_LEFT_RIGHT_BOTTOM (DG_LAYOUT_ANCHOR_LEFT_RIGHT | DG_LAYOUT_ANCHOR_BOTTOM)
#define DG_LAYOUT_ANCHOR_TOP_BOTTOM_LEFT   (DG_LAYOUT_ANCHOR_TOP_BOTTOM | DG_LAYOUT_ANCHOR_LEFT)
#define DG_LAYOUT_ANCHOR_TOP_BOTTOM_RIGHT  (DG_LAYOUT_ANCHOR_TOP_BOTTOM | DG_LAYOUT_ANCHOR_RIGHT)
#define DG_LAYOUT_ANCHOR_ALL               (DG_LAYOUT_ANCHOR_LEFT_RIGHT   | DG_LAYOUT_ANCHOR_TOP_BOTTOM)

/*! @} */

#endif
