/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_widget.h                                        */
/* Description:   Widget interface                                   */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_WIDGET
#define __DG_WIDGET

#include <dg/dg_surface.h>
#include <dg/dg_window.h>
#include <dg/dg_event.h>
#include <dg/dg_layout.h>
#include <dg/dg_gdc.h>

typedef enum
{
	WIDGET_ANIMATION_FADE_IN,
	WIDGET_ANIMATION_FADE_OUT
} dg_widget_animation_t;


/*!
	\addtogroup dg_widget Widgets
  *  @{
*/

typedef struct _dg_widget_t
{
	dg_surface_p surface;
	dg_window_p window;
	dg_event_manager_p event_manager;
	dg_layout_manager_p layout_manager;
	dg_layout_properties_p layout_properties;
	int can_focus;
	int tab_index;
	dg_widget_animation_t animation_effect;

	struct _dg_widget_t* (*create)(struct _dg_widget_t*, int, int, int, int, int);
	void  (*set_parent)(struct _dg_widget_t*, struct _dg_widget_t*);
	struct _dg_widget_t* (*get_parent)(struct _dg_widget_t*);
	int   (*is_master)(struct _dg_widget_t*);
	void  (*insert_child)(struct _dg_widget_t*, struct _dg_widget_t*);
	void  (*remove_child)(struct _dg_widget_t*, struct _dg_widget_t*);
	void  (*raise_top)(struct _dg_widget_t*);
	void  (*lower_bottom)(struct _dg_widget_t*);
	void  (*set_capture)(struct _dg_widget_t*, int);
	void  (*set_rect)(struct _dg_widget_t*, int, int, int, int, int);
	void  (*get_rect)(struct _dg_widget_t*, dg_rect_p);
	void  (*render)(struct _dg_widget_t*);
	void  (*invalidate)(struct _dg_widget_t*, dg_rect_p);
	void  (*redraw)(struct _dg_widget_t*);
	int   (*event)(void*, dg_event_p);
	void  (*set_layout_properties)(struct _dg_widget_t*, dg_layout_properties_p);
	void  (*set_focus)(struct _dg_widget_t*);
	int   (*is_focus)(struct _dg_widget_t*);
	void  (*kill_focus)(struct _dg_widget_t*);
	void  (*animate)(struct _dg_widget_t*, dg_widget_animation_t, unsigned int);
	dg_gdc_p (*create_gdc)(struct _dg_widget_t*);
	void  (*destroy)(struct _dg_widget_t*);
} dg_widget_t, *dg_widget_p;

dg_widget_p dg_widget_create(int, dg_video_p, int, int, int, int, dg_widget_p);
DG_API void dg_widget_destroy(dg_widget_p);

#define DGWidget(widget) ((dg_widget_p)widget)
#define DGWidgetCreate(widget, type_size, x, y, width, height) DGWidget(widget)->create(DGWidget(widget), type_size, x, y, width, height)
#define DGWidgetRaiseTop(widget) DGWidget(widget)->raise_top(DGWidget(widget))
#define DGWidgetLowerBottom(widget) DGWidget(widget)->lower_bottom(DGWidget(widget))
#define DGWidgetSetParent(widget, parent) (DGWidget(widget))->set_parent(DGWidget(widget), parent)
#define DGWidgetParent(widget) (DGWidget(widget))->get_parent(DGWidget(widget))
#define DGWidgetIsMaster(widget) (DGWidget(widget))->is_master(DGWidget(widget))
#define DGWidgetSetCapture(widget, is_capture) (DGWidget(widget))->set_capture(DGWidget(widget), is_capture)
#define DGWidgetSetRect(widget, x, y, width, height, is_update) (DGWidget(widget))->set_rect(DGWidget(widget), x, y, width, height, is_update)
#define DGWidgetRect(widget, rect) (DGWidget(widget))->get_rect(DGWidget(widget), rect)
#define DGWidgetRender(widget) (DGWidget(widget))->render(DGWidget(widget))
#define DGWidgetInvalidate(widget, rect) (DGWidget(widget))->invalidate(DGWidget(widget), rect)
#define DGWidgetRedraw(widget) (DGWidget(widget))->redraw(DGWidget(widget))
#define DGWidgetSetVisible(widget, is_visible) (DGWidget(widget))->window->set_visible((DGWidget(widget))->window, is_visible)
#define DGWidgetSetRenderer(widget, renderer) (DGWidget(widget))->render=renderer
#define DGWidgetVisible(widget) (DGWidget(widget))->window->is_visible((DGWidget(widget))->window)
#define DGWidgetDestroy(widget) (DGWidget(widget))->destroy(DGWidget(widget))
#define DGWidgetX(widget) (DGWidget(widget))->window->rect->get_x((DGWidget(widget))->window->rect)
#define DGWidgetY(widget) (DGWidget(widget))->window->rect->get_y((DGWidget(widget))->window->rect)
#define DGWidgetWidth(widget) (DGWidget(widget))->surface->width
#define DGWidgetHeight(widget) (DGWidget(widget))->surface->height
#define DGWidgetLayoutAnchor(widget) (DGWidget(widget))->layout_manager=dg_layout_anchor_manager
#define DGWidgetLayoutAnchorProps(widget, anchor) (DGWidget(widget))->set_layout_properties(DGWidget(widget), dg_layout_anchor_properties_create(anchor))
#define DGWidgetCreateGDC(widget) (DGWidget(widget))->create_gdc(DGWidget(widget))
#define DGWidgetSetFocus(widget) (DGWidget(widget))->set_focus(DGWidget(widget))
#define DGWidgetIsFocus(widget) (DGWidget(widget))->is_focus(DGWidget(widget))
#define DGWidgetDevice(widget) (DGWidget(widget))->surface->device
#define DGWidgetFlip(widget, rect) DGFlip(DGWidgetDevice(widget), rect)
#define DGWidgetWindow(widget) (DGWidget(widget))->window
#define DGWidgetTriggerEvent(widget, event) DGWidget(widget)->event_manager->trigger_event(DGWidget(widget)->event_manager, (dg_event_p)event)
#define DGWidgetAddListener(widget, listener, event_mask, param) (DGWidget(widget))->event_manager->add_listener((DGWidget(widget))->event_manager, listener, event_mask, param)
#define DGWidgetEventLoop(widget) (DGWidget(widget))->surface->device->master_widget->event_manager->loop((DGWidget(widget))->surface->device->master_widget->event_manager)
#define DGWidgetAnimate(widget, effect, duration) (DGWidget(widget))->animate(DGWidget(widget), effect, duration)


/*! @} */

#endif
