/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_label.c                                        */
/* Description:   Label widget                                       */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <dg/dg_widget.h>
#include <dg/dgw_label.h>
#include <dg/dgw_style.h>

extern dgw_style_renders_t _dgw_renders;

static void dgw_label_set_text(dgw_label_p self, const char* text)
{
	if (self->text)
	{
		free(self->text);
	}
	self->text=strdup(text);
}

static const char* dgw_label_get_text(dgw_label_p self)
{
	return self->text;
}

static void dgw_label_set_options(dgw_label_p self, int options)
{
	self->options=options;
}

static int dgw_label_get_options(dgw_label_p self)
{
	return self->options;
}

static void dgw_label_destroy(dg_widget_p widget)
{
	dgw_label_p self = (dgw_label_p)widget;
	if (self->text)
	{
		free(self->text);
	}
	dg_widget_destroy(widget);
}

static int dgw_label_event(dgw_label_p self, dg_event_p event)
{
	/* TODO: do something */
	return 0;
}

dgw_label_p dgw_label_create(dg_widget_p parent, int x, int y, int width, int height, const char* text)
{
	dg_widget_p widget = parent->create(parent, sizeof(dgw_label_t), x, y, width, height);
	dgw_label_p self = (dgw_label_p)widget;

	widget->can_focus = 1;
	widget->window->owner_window = 1;
	widget->render = _dgw_renders.render_label;
	//widget->event = dgw_label_event;
	widget->destroy = dgw_label_destroy;
	DGWidgetAddListener(widget, dgw_label_event, EVT_TYPE_WIDGET, self);
	//widget->event_manager->add_listener(widget->event_manager, dgw_label_event, EVT_TYPE_WIDGET, self);

	self->text = 0;
	self->options = 0;

	self->set_text       = dgw_label_set_text;
	self->get_text       = dgw_label_get_text;
	self->set_options    = dgw_label_set_options;
	self->get_options    = dgw_label_get_options;

	self->set_text(self, text);

	return self;
}
