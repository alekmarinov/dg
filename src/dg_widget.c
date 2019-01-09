/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_widget.c                                        */
/* Description:   Widget implementation                              */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <math.h>
#include <dg/dg_debug.h>
#include <dg/dg_event.h>
#include <dg/dg_widget.h>
#include <dg/dg_blit.h>
#include <dg/dg_plugin.h>

static dg_widget_p dg_widget_create_child(dg_widget_p, int, int, int, int, int);
static void dg_widget_set_parent(dg_widget_p, dg_widget_p);
static dg_widget_p dg_widget_get_parent(dg_widget_p);
static int dg_widget_is_master(dg_widget_p);
static void dg_widget_insert_child(dg_widget_p, dg_widget_p);
static void dg_widget_remove_child(dg_widget_p, dg_widget_p);
static void dg_widget_raise_top(dg_widget_p);
static void dg_widget_lower_bottom(dg_widget_p);
static void dg_widget_set_capture(dg_widget_p, int);
static void dg_widget_get_rect(dg_widget_p, dg_rect_p);
static void dg_widget_set_rect(dg_widget_p, int, int, int, int, int);
static void dg_widget_render(dg_widget_p);
static void dg_widget_invalidate(dg_widget_p, dg_rect_p);
static void dg_widget_redraw(dg_widget_p);
static void dg_widget_update(dg_widget_p, int, int, int, int);
static int  dg_widget_event(void*, dg_event_p);
static void dg_widget_set_layout_properties(dg_widget_p, dg_layout_properties_p);
static void dg_widget_set_focus(dg_widget_p);
static int  dg_widget_is_focus(dg_widget_p);
static void dg_widget_kill_focus(dg_widget_p);
static void dg_widget_animate(dg_widget_p, dg_widget_animation_t, unsigned int);
static void dg_widget_destroy(dg_widget_p);

static dg_widget_p dg_widget_create_child(dg_widget_p self, int type_size, int x, int y, int width, int height)
{
	return dg_widget_create(type_size, self->surface->device, x, y, width, height, self);
}

static void dg_widget_set_parent(dg_widget_p self, dg_widget_p parent)
{
	self->window->set_parent(self->window, parent->window);
}

static dg_widget_p dg_widget_get_parent(dg_widget_p self)
{
	if (self->window->parent)
	{
		return (dg_widget_p)self->window->parent->param;
	}
	return 0;
}

static int dg_widget_is_master(dg_widget_p self)
{
	return self == self->surface->device->master_widget;
}

static void dg_widget_insert_child(dg_widget_p self, dg_widget_p child)
{
	self->window->insert_child(self->window, child->window);
}

static void dg_widget_remove_child(dg_widget_p self, dg_widget_p child)
{
	self->window->remove_child(self->window, child->window);
}

static void dg_widget_raise_top(dg_widget_p self)
{
	dg_widget_p parent = DGWidgetParent(self);
	if (parent)
	{
		parent->raise_top(parent);
	}

	self->window->raise_top(self->window);
}

static void dg_widget_lower_bottom(dg_widget_p self)
{
	dg_widget_p parent = DGWidgetParent(self);
	if (parent)
	{
		parent->lower_bottom(parent);
	}

	self->window->lower_bottom(self->window);
}

static void dg_widget_set_capture(dg_widget_p self, int is_capture)
{
	dg_event_capture_widget(self, is_capture);
}

static void dg_widget_get_rect(dg_widget_p self, dg_rect_p rect)
{
	self->window->get_rect(self->window, rect);
}

static void dg_widget_set_rect(dg_widget_p self, int x, int y, int width, int height, int isUpdate)
{
	if (width>0 && height>0)
	{
		dg_event_t ev;
		int size_changed=0;
		int pos_changed=0;
		if (self->surface->width != width || self->surface->height != height)
		{
			int oldWidth=self->surface->width;
			int oldHeight=self->surface->height;
			if (self->layout_manager)
			{
				self->layout_manager(self->window->children, oldWidth, oldHeight, width, height);
			}
			self->surface->resize(self->surface, width, height);
			self->render(self);
			size_changed=1;
		}
		if (isUpdate)
		{
			pos_changed=x != self->window->rect->x || y != self->window->rect->y;
			self->window->set_rect(self->window, x, y, width, height);
		}
		else
		{
			self->window->rect->copy(self->window->rect, x, y, width, height);
		}

		ev.type = EVT_TYPE_WIDGET;
		ev.e.widget.p1 = DG_MAKE_LONG(x, y);
		ev.e.widget.p2 = DG_MAKE_LONG(width, height);
		if (size_changed)
		{
			ev.e.widget.widget_event = EVT_WIDGET_SIZE;
			self->event_manager->trigger_event(self->event_manager, &ev);
		}
		if (pos_changed)
		{
			ev.e.widget.widget_event = EVT_WIDGET_MOVE;
			self->event_manager->trigger_event(self->event_manager, &ev);
		}
	}
}

static void dg_widget_render(dg_widget_p self)
{
	dg_event_t event;
	event.type = EVT_TYPE_WIDGET;
	event.e.widget.widget_event = EVT_WIDGET_RENDER;
	event.e.widget.p1=event.e.widget.p2=0;
	self->surface->clear(self->surface, DG_COLOR_BLACK);
	self->event_manager->trigger_event(self->event_manager, &event);
}

static void dg_widget_invalidate(dg_widget_p self, dg_rect_p rect)
{
	self->window->invalidate(self->window, rect);
}

static void dg_widget_redraw(dg_widget_p self)
{
	dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
	DGWidgetRender(self);
	DGWidgetInvalidate(self, 0);
	DGWidgetRect(self, rect);
	DGWidgetFlip(self, rect);
	rect->destroy(rect);
}

static void dg_widget_update(dg_widget_p self, int x, int y, int width, int height)
{
	if (self->is_master(self))
	{
		/* update master surface */
		self->surface->fill_rect(self->surface, x, y, width, height, self->surface->device->background_color);
		if (DGWidgetDevice(self)->background_image)
		{
			dg_rect_p dst_rect = dg_rect_create(x, y, width, height);
			dg_rect_p image_rect = dg_rect_create(0, 0, 
												  DGWidgetDevice(self)->background_image->width, 
												  DGWidgetDevice(self)->background_image->height);
			dg_rect_p intersect_rect = dg_rect_create(0, 0, 0, 0);

			if (image_rect->w < DGWidgetWidth(self))
				image_rect->x = (DGWidgetWidth(self) - image_rect->w) >> 1;

			if (image_rect->h < DGWidgetHeight(self))
				image_rect->y = (DGWidgetHeight(self) - image_rect->h) >> 1;

			image_rect->intersect(image_rect, dst_rect, 
								  &intersect_rect->x, &intersect_rect->y, 
								  &intersect_rect->w, &intersect_rect->h);

			dst_rect->copy_rect(dst_rect, intersect_rect);
			intersect_rect->x -= image_rect->x;
			intersect_rect->y -= image_rect->y;

			self->surface->blit(self->surface, dst_rect, DGWidgetDevice(self)->background_image, intersect_rect, 0, 0, 0, DG_OPT_BLIT_DEFAULT);
			dst_rect->destroy(dst_rect);
			image_rect->destroy(image_rect);
			intersect_rect->destroy(intersect_rect);
		}
	}
	else
	{
		dg_rect_p widget_rect = dg_rect_create(0, 0, width, height);
		dg_rect_p src_rect = widget_rect->clone(widget_rect);
		dg_rect_p dst_rect = widget_rect->clone(widget_rect);
		dg_color_t dummy={0};

		self->get_rect(self, widget_rect);

		src_rect->move(src_rect, x-widget_rect->x, y-widget_rect->y);
		dst_rect->move(dst_rect, x, y);

		if (!self->surface->device->master_widget->surface->blit(self->surface->device->master_widget->surface, dst_rect, self->surface, src_rect, 0, 0, dummy, DG_OPT_BLIT_DEFAULT))
		{
			LOG_IMPORTANT("Unable to blit widget to master surface\n");
		}

		src_rect->destroy(src_rect);
		dst_rect->destroy(dst_rect);
		widget_rect->destroy(widget_rect);
	}
}

static int dg_widget_event(void* param, dg_event_p event)
{
	return 0;
}

static int widget_animation_event(dg_widget_p self, dg_event_p event)
{
	dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
	float aleph = (float)event->e.custom.n0 / 1000;

	switch (self->animation_effect)
	{
	case WIDGET_ANIMATION_FADE_IN:
		DGWidget(self)->surface->set_alpha(DGWidget(self)->surface, (char)((int)((float)255*aleph) & 255));
	break;
	case WIDGET_ANIMATION_FADE_OUT:
		DGWidget(self)->surface->set_alpha(DGWidget(self)->surface, (char)((int)((float)255*(1-aleph)) & 255));
	break;
	}

	DGWidgetInvalidate(self, 0);
	DGWidgetRect(self, rect);
	DGWidgetFlip(self, rect);
	rect->destroy(rect);
	return !(aleph == 1.0f);
}

static void dg_widget_set_focus(dg_widget_p self)
{
	dg_event_focus_widget(self);
}

static int dg_widget_is_focus(dg_widget_p self)
{
	return dg_event_is_focus_widget(self);
}

static void dg_widget_kill_focus(dg_widget_p self)
{
	dg_event_kill_focus_widget(self);
}

static void dg_widget_animate(dg_widget_p self, dg_widget_animation_t animation_effect, 
							  unsigned int animation_duration)
{
	self->animation_effect = animation_effect;
	DGWidgetDevice(self)->master_widget->event_manager->add_animation_listener(
		DGWidgetDevice(self)->master_widget->event_manager, widget_animation_event, 
		self, animation_duration);
}

void dg_widget_destroy(dg_widget_p self)
{
	dg_event_notify_destroy_widget(self);
	self->kill_focus(self);
	
	self->window->set_visible(self->window, 0);
	self->window->invalidate(self->window, 0);

	/* destroy child widgets */
	while (self->window->children)
	{
		dg_widget_p widget=(dg_widget_p)self->window->children->param;
		widget->destroy(widget);
	}

	self->window->param=0;
	self->window->destroy(self->window);
	self->surface->destroy(self->surface);
	self->event_manager->destroy(self->event_manager);
	if (self->layout_properties)
	{
		self->layout_properties->destroy(self->layout_properties);
	}

	free(self);
}

static void dg_widget_set_layout_properties(dg_widget_p self, dg_layout_properties_p layout_properties)
{
	if (self->layout_properties)
	{
		self->layout_properties->destroy(self->layout_properties);
	}
	self->layout_properties=layout_properties;
}

static int gdc_plugin_enum_callback(dg_gdc_driver_p* gdc_driver, dg_plugin_info_p plugin_info)
{
	*gdc_driver=plugin_info->gdc_driver;

	/* gdc driver found, stop plugin enumeration */
	return 1;
}

static dg_gdc_p dg_widget_create_gdc(dg_widget_p self)
{
	static int plugin_checked=0;
	static dg_gdc_driver_p gdc_driver=0;

	if (!plugin_checked)
	{
		dg_plugin_enum(PLUGIN_GDC_DRIVER, gdc_plugin_enum_callback, &gdc_driver);
		plugin_checked=1;
	}

	return (dg_gdc_p)(gdc_driver ? 
		gdc_driver->gdc_create((struct dg_surface_t*)self->surface) : /* create gdc from plugin */
		dg_gdc_create((struct dg_surface_t*)self->surface));          /* create native DG gdc   */
}

dg_widget_p dg_widget_create(int type_size, dg_video_p device, int x, int y, int width, int height, dg_widget_p parent)
{
	dg_widget_p self = (dg_widget_p)malloc(type_size);
	self->window = dg_window_create(x, y, width, height, dg_widget_update, self, (parent?parent->window:0));

	/* If not parent the surface is master surface created by the video driver */
	if (parent)
	{
		self->surface = device->create_surface(device, width, height, 0);
	}
	else
	{
		dg_event_set_master_widget(self);
		self->window->clip_children=1;
	}

	self->event_manager      = dg_event_manager_create();
	self->layout_manager     = 0;
	self->layout_properties  = 0;
	self->can_focus          = 0;
	self->tab_index          = 0;

	self->create             = dg_widget_create_child;
	self->set_parent         = dg_widget_set_parent;
	self->get_parent         = dg_widget_get_parent;
	self->is_master          = dg_widget_is_master;
	self->insert_child       = dg_widget_insert_child;
	self->remove_child       = dg_widget_remove_child;
	self->raise_top          = dg_widget_raise_top;
	self->lower_bottom       = dg_widget_lower_bottom;
	self->set_capture        = dg_widget_set_capture;
	self->get_rect           = dg_widget_get_rect;
	self->set_rect           = dg_widget_set_rect;
	self->render             = dg_widget_render;
	self->invalidate         = dg_widget_invalidate;
	self->redraw             = dg_widget_redraw;
	self->event              = dg_widget_event;
	self->set_layout_properties = dg_widget_set_layout_properties;
	self->set_focus          = dg_widget_set_focus;
	self->is_focus           = dg_widget_is_focus;
	self->kill_focus         = dg_widget_kill_focus;
	self->animate            = dg_widget_animate;
	self->create_gdc         = dg_widget_create_gdc;
	self->destroy            = dg_widget_destroy;

	return self;
}
