/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_button.c                                       */
/* Description:   Button widget                                      */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <dg/dg_keys.h>
#include <dg/dg_widget.h>
#include <dg/dg_event.h>
#include <dg/dgw_button.h>
#include <dg/dgw_style.h>

extern dgw_style_renders_t _dgw_renders;

static void dgw_button_set_pushed(dgw_button_p self, int pushed)
{
	self->is_push = pushed;
}

static int dgw_button_is_pushed(dgw_button_p self)
{
	return self->is_push;
}

static void dgw_button_set_text(dgw_button_p self, const char* text)
{
	if (self->text)
	{
		free(self->text);
	}
	self->text=strdup(text);
}

static const char* dgw_button_get_text(dgw_button_p self)
{
	return self->text;
}

static void dgw_button_set_options(dgw_button_p self, int options)
{
	self->options=options;
}

static int dgw_button_get_options(dgw_button_p self)
{
	return self->options;
}

static void dgw_button_destroy(dg_widget_p widget)
{
	dgw_button_p self = (dgw_button_p)widget;
	if (self->text)
	{
		free(self->text);
	}
	dg_widget_destroy(widget);
}

static int dgw_button_event(dgw_button_p self, dg_event_p event)
{
	int redraw = 0;
	int previous_status = self->is_pushed(self);
	if (event->type == EVT_TYPE_WIDGET)
	{
		if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_START)
		{
			DGWidgetSetFocus(self);
			DGWidgetSetCapture(self, 1);
			self->set_pushed(self, 1);
			redraw = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_STOP)
		{
			DGWidgetSetCapture(self, 0);
			self->set_pushed(self, 0);
			redraw = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_FOCUS)
		{
			redraw = 1;
		}
	}
	else if (event->type == EVT_TYPE_KEYBOARD)
	{
		switch (event->e.keyboard.key)
		{
		case KEY_Space: case KEY_Return: /* button pressed or released */ 
			if (event->e.keyboard.key_event & EVT_PRESSED)
			{
				self->set_pushed(self, 1);
				redraw = 1;
			}
			else
			{
				self->set_pushed(self, 0);
				redraw = 1;
			}
		break;
		}
	}

	if (redraw)
	{
		DGWidgetRedraw(self);

		if (previous_status != self->is_pushed(self))
		{
			dg_event_button_t button_event;
			button_event.type = EVT_TYPE_WIDGET;
			button_event.e.widget.widget_event = EVT_WIDGET_STATUS_CHANGED;
			button_event.e.widget.p1 = self->is_pushed(self)?BUTTON_STATUS_PRESSED:BUTTON_STATUS_RELEASED;
			button_event.e.widget.p2 = 0;
			DGWidgetTriggerEvent(self, &button_event);
		}
	}
	return redraw;
}

dgw_button_p dgw_button_create(dg_widget_p parent, int x, int y, int width, int height, const char* text)
{
	dg_widget_p widget = parent->create(parent, sizeof(dgw_button_t), x, y, width, height);
	dgw_button_p self = (dgw_button_p)widget;

	widget->can_focus = 1;
	widget->window->owner_window = 1;
	widget->render = _dgw_renders.render_button;
	//widget->event = dgw_button_event;
	widget->destroy = dgw_button_destroy;
	DGWidgetAddListener(widget, dgw_button_event, EVT_TYPE_WIDGET | EVT_TYPE_KEYBOARD, self);

	self->text = 0;
	self->options = 0;
	self->is_push = 0;

	self->set_pushed     = dgw_button_set_pushed;
	self->is_pushed      = dgw_button_is_pushed;
	self->set_text       = dgw_button_set_text;
	self->get_text       = dgw_button_get_text;
	self->set_options    = dgw_button_set_options;
	self->get_options    = dgw_button_get_options;

	self->set_text(self, text);

	return self;
}
