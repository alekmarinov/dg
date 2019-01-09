/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_event.c                                         */
/* Description:   Event Management implementation                    */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <dg/dg_debug.h>
#include <dg/dg_event.h>
#include <dg/dg_widget.h>
#include <dg/dg_keys.h>

/* FIXME: Unix compatability */
#include <windows.h>
unsigned int GetPerformanceFrequency()
{
	return 1000;
}

unsigned int GetPerformanceCounter()
{
	return (unsigned int)timeGetTime();
}

/* focused widget */
static dg_widget_p focused_widget=0;

/* mouse capture information */
static struct
{
	int last_x;
	int last_y;
	int is_dragging;
	int is_captured;
	dg_widget_p captured_widget;
	dg_widget_p master_widget;
} mouse_event_info = {0};

static int dg_event_trigger_event(dg_event_manager_p self, dg_event_p event)
{
	dg_event_listener_list_p item = self->listeners;
	/* dispatch event to listeners */
	while (item && self->active)
	{
		if ((item->event_type_mask & event->type) != 0)
			if (item->listener(item->param, event))
				return 1;
		item=item->next;
	}
	return 0;
}

static int dg_event_send(dg_widget_p receiver, dg_event_p event)
{
	dg_widget_p widget = receiver;
	while (widget)
	{
		if (widget->event_manager->master)
		{
			dg_event_trigger_event(widget->event_manager, event);
		}
		else
		{
			if (widget->event_manager->trigger_event(widget->event_manager, event))
				break;
		}
		widget = widget->get_parent(widget);
	}
	return widget != 0;
}

static int dg_event_trigger(dg_widget_p receiver, dg_event_p event)
{
	dg_widget_p widget = receiver;
	while (widget && !(widget->event_manager->trigger_event(widget->event_manager, event)))
		widget = widget->get_parent(widget);
	return widget != 0;
}

int dg_event_focus_widget(dg_widget_p widget)
{
	if (widget != focused_widget)
	{
		dg_event_t ev;
		dg_widget_p prev_widget = focused_widget;

		/* focus next widget */
		focused_widget = widget;

		ev.type = EVT_TYPE_WIDGET;
		ev.e.widget.widget_event = EVT_WIDGET_FOCUS;
		ev.e.widget.p2 = 0;

		/* send unfocus event to the previous focused widgets */
		ev.e.widget.p1 = 0;
		if (prev_widget)
		{
			dg_event_send(prev_widget, &ev);
		}

		/* send focus event to the next focused widget */
		ev.e.widget.p1 = 1;
		if (focused_widget)
		{
			return dg_event_send(focused_widget, &ev);
		}
	}
	return 0;
}

void dg_event_capture_widget(dg_widget_p widget, int is_capture)
{
	mouse_event_info.captured_widget = widget;
	mouse_event_info.is_captured = is_capture;
	widget->surface->device->set_mouse_capture(widget->surface->device, is_capture);
}

void dg_event_notify_destroy_widget(dg_widget_p widget)
{
	if (mouse_event_info.captured_widget == widget)
	{
		DGWidgetDevice(widget)->set_mouse_capture(DGWidgetDevice(widget), 0);
		mouse_event_info.is_captured = 0;
		mouse_event_info.captured_widget = 0;
	}
}

void dg_event_set_master_widget(dg_widget_p master_widget)
{
	mouse_event_info.master_widget=master_widget;
}

/* return 1 if the given widget is focused */
/* Remark: a widget is focused if it or any of its children is focused */
int  dg_event_is_focus_widget(dg_widget_p widget)
{
	dg_widget_p p_widget=focused_widget;
	while (p_widget)
	{
		if (p_widget == widget) 
		{
			return 1;
		}
		p_widget=p_widget->get_parent(p_widget);
	}
	return 0;
}

void dg_event_kill_focus_widget(dg_widget_p widget)
{
	if (focused_widget && (widget == focused_widget))
	{
		focused_widget=focused_widget->get_parent(focused_widget);
	}
}

static void dg_event_add_listener(dg_event_manager_p self, dg_event_listener_t listener, int event_type_mask, void* param)
{
	dg_event_listener_list_p event_listener_item=(dg_event_listener_list_p)malloc(sizeof(dg_event_listener_list_t));
	event_listener_item->event_type_mask = event_type_mask;
	event_listener_item->listener        = listener;
	event_listener_item->param           = param;
	event_listener_item->next            = self->listeners;
	event_listener_item->prev            = 0;
	if (self->listeners)
		self->listeners->prev = event_listener_item;
	self->listeners=event_listener_item;
}

static void dg_event_add_animation_listener(dg_event_manager_p self, dg_event_listener_t listener, 
											void* param, unsigned int animation_duration)
{
	dg_event_listener_list_p event_listener_item=(dg_event_listener_list_p)malloc(sizeof(dg_event_listener_list_t));
	event_listener_item->event_type_mask    = EVT_TYPE_ANIMATION;
	event_listener_item->listener           = listener;
	event_listener_item->param              = param;
	event_listener_item->animation_duration = animation_duration;
	event_listener_item->animation_begin    = 0;
	event_listener_item->animation_ticker   = 0.0f;
	event_listener_item->next               = self->listeners;
	event_listener_item->prev               = 0;
	if (self->listeners)
		self->listeners->prev = event_listener_item;
	self->listeners=event_listener_item;
}

/* descending search widget under mouse pointer */
/* FIXME: optimize search looking for the deepest window in direction last-first */
static void dg_event_get_widget_xy(dg_widget_p parent, int x, int y, dg_widget_p *widget)
{
	dg_window_p child = parent->window->children;
	while (child)
	{
		if (DGWidgetVisible(child->param))
		{
			dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
			child->get_rect(child, rect);
			if (rect->is_point_inside(rect, x, y))
				*widget=(dg_widget_p)child->param;
			rect->destroy(rect);

			dg_event_get_widget_xy((dg_widget_p)child->param, x, y, widget);
		}
		child=child->next;
	}
}

static int master_trigger_event(dg_event_manager_p self, dg_event_p event)
{
	int result = 0;

	dg_event_trigger_event(self, event);

	/* dispatch event keyboard and mouse events to focused widgets */
	if (event->type == EVT_TYPE_KEYBOARD)
	{
		if (focused_widget)
		{
			if ( (event->e.keyboard.key_event & EVT_PRESSED) && event->e.keyboard.key == KEY_Tab)
			{
				/* focus next widget */
				dg_widget_p parent = DGWidgetParent(focused_widget);
				if (parent)
				{
					int current_index = focused_widget->tab_index;
					int min_index = INT_MAX;
					int next_min_index = INT_MAX;
					dg_window_p child = parent->window->children;
					dg_widget_p next_widget=0;
					dg_widget_p min_widget=0;

					/* find min-max index */
					while (child) 
					{
						dg_widget_p widget = (dg_widget_p)child->param;
						if (widget->can_focus)
						{
							if (widget->tab_index > current_index)
							{
								if (widget->tab_index < next_min_index)
								{
									next_min_index = widget->tab_index;
									next_widget = widget;
								}
							}
							if (widget->tab_index < min_index)
							{
								min_index = widget->tab_index;
								min_widget = widget;
							}
						}
						child=child->next;
					}
					if (!next_widget)
						next_widget = min_widget;

					if (next_widget)
						result = dg_event_focus_widget(next_widget);
				}
			}
			else
			{
				result = dg_event_send(focused_widget, event);
			}
		}
	} 
	else if (event->type == EVT_TYPE_MOUSE)
	{
		dg_widget_p current=0;
		dg_event_t ev;
		dg_rect_p rect;
		if (mouse_event_info.is_captured )
			current=mouse_event_info.captured_widget;
		else
		{
			dg_event_get_widget_xy(mouse_event_info.master_widget, event->e.mouse.x, event->e.mouse.y, &current);
		}

		rect = dg_rect_create(0, 0, 0, 0);
		ev.type = EVT_TYPE_WIDGET;
		ev.e.widget.p1=DG_MAKE_LONG(event->e.mouse.x, event->e.mouse.y);
		ev.e.widget.p2=0;

		if (current)
		{
			if (mouse_event_info.captured_widget != current)
			{
				if (mouse_event_info.captured_widget)
				{
					ev.e.widget.widget_event=EVT_WIDGET_MOUSE_LEAVE;
					result = mouse_event_info.captured_widget->event_manager->trigger_event(mouse_event_info.captured_widget->event_manager, &ev);
					if (result) return 1;
				}
				/* FIXME: what happens with the captured_widget if self is destroyed? */
				mouse_event_info.captured_widget=current;
				ev.e.widget.widget_event = EVT_WIDGET_MOUSE_ENTER;
				result = mouse_event_info.captured_widget->event_manager->trigger_event(mouse_event_info.captured_widget->event_manager, &ev);
			}

			ev.e.widget.widget_event = EVT_WIDGET_MOUSE_MOVE;
			result = dg_event_trigger(mouse_event_info.captured_widget, &ev);

			if (event->e.mouse.mouse_event & EVT_MOUSE_WHEEL)
			{
				ev.e.widget.p2=event->e.mouse.mouse_event & EVT_PRESSED;

				ev.e.widget.widget_event = EVT_WIDGET_MOUSE_WHEEL;
				result = dg_event_trigger(mouse_event_info.captured_widget, &ev);
			}
			else if ( (event->e.mouse.mouse_event & EVT_MOUSE_LBUTTON) && (event->e.mouse.mouse_event & EVT_PRESSED))
			{
				if (!mouse_event_info.is_dragging)
				{
					mouse_event_info.is_dragging=1;
					ev.e.widget.widget_event = EVT_WIDGET_MOUSE_DRAG_START;
					result = dg_event_trigger(mouse_event_info.captured_widget, &ev);
				}
			} 
			else if ( (event->e.mouse.mouse_event & EVT_MOUSE_LBUTTON) && (event->e.mouse.mouse_event & EVT_RELEASED))
			{
				if (mouse_event_info.is_dragging)
				{
					mouse_event_info.is_dragging=0;
					ev.e.widget.widget_event = EVT_WIDGET_MOUSE_DRAG_STOP;
					result = dg_event_trigger(mouse_event_info.captured_widget, &ev);
				}
			} 
			else if (mouse_event_info.is_dragging)
			{
				ev.e.widget.widget_event = EVT_WIDGET_MOUSE_DRAGGING;
				ev.e.widget.p2=DG_MAKE_LONG(event->e.mouse.x-mouse_event_info.last_x, event->e.mouse.y-mouse_event_info.last_y);
				result = dg_event_trigger(mouse_event_info.captured_widget, &ev);
			}

			mouse_event_info.last_x = event->e.mouse.x;
			mouse_event_info.last_y = event->e.mouse.y;
		}

		rect->destroy(rect);
	}
	return result;
}

static int dg_event_peek(dg_event_manager_p self, dg_event_p event)
{
	return 0;
}

static void dg_event_idle(dg_event_manager_p self)
{
	dg_event_t event;
	if (self->peek(self, &event))
	{
		if (event.type == EVT_TYPE_QUIT)
			self->active=0;
		else
			master_trigger_event(self, &event);
	}
	else
	{
		dg_event_listener_list_p item = self->listeners;
		while (item)
		{
			if ( (item->event_type_mask & EVT_TYPE_ANIMATION) != 0)
			{
				unsigned int begin = item->animation_begin;
				unsigned int now = GetPerformanceCounter();
				float secs;

				if (!begin)
				{
					item->animation_begin = begin = now;
				}

				secs = (float)(now - begin) / (float)GetPerformanceFrequency();
				if (secs > 0)
				{
					float current = (secs * 1000) / item->animation_duration;
					if (current > 1.0f) current = 1.0f;

					event.type = EVT_TYPE_ANIMATION;
					event.e.custom.n0 = (int)(current * 1000);
					if (item->listener(item->param, &event))
					{
						/* keep sending animation events */
					}
					else
					{
						/* remove animation listener */
						dg_event_listener_list_p free_item;
						if (item->prev)
							item->prev->next = item->next;
						if (item->next)
							item->next->prev = item->prev;
						if (!(item->next || item->prev))
							self->listeners = 0;

						free_item = item;
						item = item->next;
						free(free_item);
						if (item && !item->prev)
							self->listeners = item;
						continue;
					}
				}
			}
			item=item->next;
		}
	}
}

static void dg_event_loop(dg_event_manager_p self)
{
	while (self->active)
	{
		self->idle(self);
	}
}

static void dg_event_destroy(dg_event_manager_p self)
{
	/* free listeners */
	while (self->listeners)
	{
		dg_event_listener_list_p item=self->listeners;
		self->listeners=self->listeners->next;
		free(item);
	}
	self->listeners=0;
	self->active=0;
	free(self);
}

dg_event_manager_p dg_event_manager_create()
{
	dg_event_manager_p self = (dg_event_manager_p)malloc(sizeof(dg_event_manager_t));
	self->master = 0;
	self->active = 1;
	self->listeners = 0;

	self->peek                    = dg_event_peek;
	self->idle                    = dg_event_idle;
	self->loop                    = dg_event_loop;
	self->destroy                 = dg_event_destroy;
	self->add_listener            = dg_event_add_listener;
	self->add_animation_listener  = dg_event_add_animation_listener;
	self->trigger_event           = dg_event_trigger_event; 

	return self;
}

dg_event_manager_p dg_event_manager_master_create()
{
	dg_event_manager_p self = dg_event_manager_create();
	self->master = 1;
	self->trigger_event = master_trigger_event;
	return self;
}
