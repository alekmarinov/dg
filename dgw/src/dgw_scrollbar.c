/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_scrollbar.c                                    */
/* Description:   Scrollbar widget                                   */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <dg/dg_widget.h>
#include <dg/dgw_scrollbar.h>
#include <dg/dgw_style.h>

extern dgw_style_renders_t _dgw_renders;

static int dgw_scrollbar_get_cursor_height(dgw_scrollbar_p self)
{
	int cursor_height;
	int height = DGWidgetHeight((dg_widget_p)self) - (SCROLLBAR_SIZE << 1);
	float ratio = (float)self->get_visible_item_count(self) / self->get_item_count(self);
	height = (int)(height * ratio);
	cursor_height = MAX(height, SCROLLBAR_SIZE);
	assert(cursor_height >= 0);
	return cursor_height;
}

static int dgw_scrollbar_get_scrolled_area(dgw_scrollbar_p self)
{
	int scrolled_area = DGWidgetHeight((dg_widget_p)self) - (SCROLLBAR_SIZE << 1) - 
			dgw_scrollbar_get_cursor_height(self);
	return scrolled_area;
}

static int dgw_scrollbar_scroll_to_item(dgw_scrollbar_p self, int scroll)
{
	int scrolled_area = dgw_scrollbar_get_scrolled_area(self);
	int scrolled_items = self->get_item_count(self) - self->get_visible_item_count(self);
	float scroll_part = (float)scroll / scrolled_area;
	return (int)(scrolled_items * scroll_part);
}

static int dgw_scrollbar_item_to_scroll(dgw_scrollbar_p self, int item_position)
{
	int scrolled_area = dgw_scrollbar_get_scrolled_area(self);
	int scrolled_items = self->get_item_count(self) - self->get_visible_item_count(self);
	if (scrolled_items>0)
	{
		float scroll_part = (float)item_position / scrolled_items;
		return (int)(scrolled_area * scroll_part);
	}
	return 0;
}

void dgw_scrollbar_get_cursor_rect(dgw_scrollbar_p self, dg_rect_p rect)
{
	dg_widget_p widget = (dg_widget_p)self;
	if (self->get_visible_item_count(self) < self->get_item_count(self))
	{
		rect->y = SCROLLBAR_SIZE + self->scroll;
		rect->w = DGWidgetWidth(widget);
		rect->x = 0;
		rect->h = dgw_scrollbar_get_cursor_height(self);
		rect->y = MIN(rect->y, SCROLLBAR_SIZE + DGWidgetHeight(widget) - rect->h);
	}
	else
	{
		rect->x = 0;
		rect->y = SCROLLBAR_SIZE;
		rect->w = DGWidgetWidth(widget);
		rect->h = DGWidgetHeight(widget) - (SCROLLBAR_SIZE << 1);
	}
}

static void dgw_scroll_changed(dgw_scrollbar_p self)
{
	dg_event_scrollbar_t sbe;
	dg_widget_p parent = DGWidgetParent((dg_widget_p)self);
	sbe.e.type = EVT_TYPE_SCROLLBAR;
	sbe.item_count = -1;
	sbe.visible_item_count = -1;
	sbe.scroll = dgw_scrollbar_scroll_to_item(self, self->scroll);
	/* don't send message for invalid scroll value */
	if (sbe.scroll >= 0)
		parent->event_manager->trigger_event(parent->event_manager, (dg_event_p)&sbe);
}

static int dgw_scrollbar_get_visible_item_count(dgw_scrollbar_p self)
{
	return self->visible_item_count;
}

static void dgw_scrollbar_set_visible_item_count(dgw_scrollbar_p self, int visible_item_count)
{
	self->visible_item_count=visible_item_count;
}

static int dgw_scrollbar_get_item_count(dgw_scrollbar_p self)
{
	return self->item_count;
}

static void dgw_scrollbar_set_item_count(dgw_scrollbar_p self, int item_count)
{
	self->item_count=item_count;
}

static void dgw_scrollbar_set_scroll(dgw_scrollbar_p self, int scroll)
{
	int scrolled_area = dgw_scrollbar_get_scrolled_area(self);
	if (scroll < 0) scroll = 0;
	if (scroll > scrolled_area) scroll = scrolled_area;

	if (self->scroll != scroll)
	{
		self->scroll = scroll;
		dgw_scroll_changed(self);
	}
}

static int dgw_scrollbar_get_scroll(dgw_scrollbar_p self)
{
	return self->scroll;
}

static void dgw_scrollbar_set_orientation(dgw_scrollbar_p self, int orientation)
{
	self->orientation=orientation;
}

static int dgw_scrollbar_get_orientation(dgw_scrollbar_p self)
{
	return self->orientation;
}

static int dg_scroll_event(dgw_scrollbar_p self, dg_event_p event)
{
	dg_widget_p widget = (dg_widget_p)self;
	static int scrollbar_y_offset=0;
	int result = 0;
	if (event->type == EVT_TYPE_WIDGET)
	{
		if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAGGING)
		{
			dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
			int mouse_y = DG_WORD_HI(event->e.widget.p1);
			widget->get_rect(widget, rect);
			mouse_y -= rect->y;
			self->set_scroll(self, mouse_y - SCROLLBAR_SIZE - scrollbar_y_offset);
			rect->destroy(rect);
			result = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_START)
		{
			dg_widget_p parent = widget->get_parent(widget);
			dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
			dg_rect_p cursor_rect = dg_rect_create(0, 0, 0, 0);
			int mouse_x = DG_WORD_LO(event->e.widget.p1);
			int mouse_y = DG_WORD_HI(event->e.widget.p1);
			dgw_scrollbar_get_cursor_rect(self, cursor_rect);
			widget->get_rect(widget, rect);
			mouse_x -= rect->x;
			mouse_y -= rect->y;

			if (cursor_rect->is_point_inside(cursor_rect, mouse_x, mouse_y))
			{
				scrollbar_y_offset = mouse_y - cursor_rect->y;
				DGWidgetSetCapture(widget, 1);
			}
			else
			{
				self->set_scroll(self, mouse_y - SCROLLBAR_SIZE);
			}

			rect->destroy(rect);
			cursor_rect->destroy(cursor_rect);

			if (!parent->is_focus(parent))
			{
				parent->set_focus(parent);
			}
			result = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_STOP)
		{
			DGWidgetSetCapture(widget, 0);
			result = 1;
		}
	}
	else if (event->type == EVT_TYPE_SCROLLBAR)
	{
		dg_event_scrollbar_p sbe     = (dg_event_scrollbar_p)event;
		self->scroll                 = dgw_scrollbar_item_to_scroll(self, sbe->scroll);
		self->item_count             = sbe->item_count;
		self->visible_item_count     = sbe->visible_item_count;
		DGWidgetRender(widget);
		result = 1;
	}
	return result;
}

dgw_scrollbar_p dgw_scrollbar_create(dg_widget_p parent, dgw_scrollbar_orientation_t orientation)
{
	dg_widget_p widget;
	dgw_scrollbar_p self;

	if (orientation == SCROLLBAR_VERTICAL)
	{
		/* vertical scroll bar */
		widget = parent->create(parent, sizeof(dgw_scrollbar_t), 
			DGWidgetWidth(parent) - parent->window->client_right - SCROLLBAR_SIZE,
			parent->window->client_top, 
			SCROLLBAR_SIZE, 
			DGWidgetHeight(parent) - parent->window->client_top - parent->window->client_bottom);
	}
	else
	{
		/* horizontal scroll bar */
		widget = parent->create(parent, sizeof(dgw_scrollbar_t), 
			parent->window->client_left, 
			DGWidgetHeight(parent) - parent->window->client_bottom - SCROLLBAR_SIZE, 
			DGWidgetWidth(parent) - parent->window->client_left - parent->window->client_right, 
			SCROLLBAR_SIZE);
	}

	self = (dgw_scrollbar_p)widget;

	widget->render = _dgw_renders.render_scrollbar;
	widget->window->owner_window = 1;
	
	self->scroll                 = 0;
	self->item_count             = 0;
	self->visible_item_count     = 0;
	self->orientation            = orientation;

	self->get_visible_item_count = dgw_scrollbar_get_visible_item_count;
	self->set_visible_item_count = dgw_scrollbar_set_visible_item_count;
	self->get_item_count         = dgw_scrollbar_get_item_count;
	self->set_item_count         = dgw_scrollbar_set_item_count;
	self->get_scroll             = dgw_scrollbar_get_scroll;
	self->set_scroll             = dgw_scrollbar_set_scroll;
	self->set_orientation        = dgw_scrollbar_set_orientation;
	self->get_orientation        = dgw_scrollbar_get_orientation;

	DGWidgetAddListener(widget, dg_scroll_event, EVT_TYPE_WIDGET | EVT_TYPE_MOUSE | EVT_TYPE_SCROLLBAR, self);

	//widget->surface->device->master_widget->event_manager->add_listener(widget->surface->device->master_widget->event_manager, dg_scroll_event, EVT_TYPE_MOUSE, self);

	return self;
}
