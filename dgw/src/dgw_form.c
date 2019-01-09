/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_form.c                                         */
/* Description:   Form                                               */
/*                                                                   */
/*********************************************************************/

#include <string.h>
#include <malloc.h>
#include <dg/dg_debug.h>
#include <dg/dg_video.h>
#include <dg/dgw_form.h>
#include <dg/dgw_style.h>
#include <dg/dgw_keyboard.h>

extern dgw_style_renders_t _dgw_renders;

dgw_keyboard_p virtual_keyboard = 0;

static void dgw_form_set_text(dgw_form_p self, const char* text)
{
	if (self->text)
	{
		free(self->text);
	}
	self->text=strdup(text);;
}

static const char* dgw_form_get_text(dgw_form_p self)
{
	return self->text;
}

static void dgw_form_set_options(dgw_form_p self, int options)
{
	self->options=options;
}

static const int dgw_form_get_options(dgw_form_p self)
{
	return self->options;
}

static void dgw_form_destroy(dg_widget_p widget)
{
	dgw_form_p self = (dgw_form_p)widget;
	if (self->text)
	{
		free(self->text);
	}

	if (widget->is_master(widget))
	{
		DGDestroy(widget->surface->device);
	}
	else
	{
		dg_widget_destroy(widget);
	}
}

#define DRAG_TOP_LEFT     (1<<0)
#define DRAG_TOP_RIGHT    (1<<1)
#define DRAG_BOTTOM_LEFT  (1<<2)
#define DRAG_BOTTOM_RIGHT (1<<3)
#define DRAG_TOP          (1<<4)
#define DRAG_LEFT         (1<<5)
#define DRAG_RIGHT        (1<<6)
#define DRAG_BOTTOM       (1<<7)
#define DRAG_ALL          (1<<8)

/* FIXME: */
#include <windows.h>

static int get_grag_info(dg_widget_p widget, int mx, int my)
{
	dg_rect_p rect=dg_rect_create(0, 0, 0, 0);
	int wx, wy, ww, wh;

	DGWidgetRect(widget, rect);
	wx=rect->x;
	wy=rect->y;
	ww=rect->w;
	wh=rect->h;
	rect->destroy(rect);

	if (mx == wx || mx == (1+wx)) /* left */
	{
		if (my == wy || my == (1+wy)) /* top */
		{
			return DRAG_TOP_LEFT;
		}
		else
		if (my == (wy+wh) || my == (-1+wy+wh)) /* bottom */
		{
			return DRAG_BOTTOM_LEFT;
		}
		else
		{
			return DRAG_LEFT;
		}
	}
	else
	if (mx == (wx+ww) || mx == (-1+ww+wx)) /* right */
	{
		if (my == wy || my == (1+wy)) /* top */
		{
			return DRAG_TOP_RIGHT;
		}
		else
		if (my == (wy+wh) || my == (-1+wy+wh))
		{
			return DRAG_BOTTOM_RIGHT;
		}
		else
		{
			return DRAG_RIGHT;
		}
	}
	else
	if (my == wy || my == (1+wy)) /* top */
	{
		return DRAG_TOP;
	}
	if (my == (wy+wh) || my == (-1+wy+wh)) /* bottom */
	{
		return DRAG_BOTTOM;
	}
	else
		// FIXME: Here 20 must be the caption height
	if ( my-wy <= 20)
	{
		return DRAG_ALL;
	}
	return 0;
}

static int dgw_form_event(dgw_form_p self, dg_event_p event)
{
	int result = 0;
	if ((event->e.widget.widget_event & EVT_WIDGET_SIZE) && DGWidgetIsMaster(self))
	{
		/* main window size changed */
		DGWidgetFlip(self, 0);
		result = 1;
	}
	else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAGGING)
	{
		int mx=DG_WORD_LO(event->e.widget.p1);
		int my=DG_WORD_HI(event->e.widget.p1);
		int wx=DGWidgetX(self);
		int wy=DGWidgetY(self);
		int ww=DGWidgetWidth(self);
		int wh=DGWidgetHeight(self);
		int dx=DG_WORD_LO(event->e.widget.p2);
		int dy=DG_WORD_HI(event->e.widget.p2);

		switch (self->drag_info)
		{
			case DRAG_TOP_LEFT:
					DGWidgetSetRect(self, wx+dx, wy+dy, ww-dx, wh-dy, 1);
				break;
			case DRAG_BOTTOM_RIGHT: 
					DGWidgetSetRect(self, wx, wy, ww+dx, wh+dy, 1);
				break;
			case DRAG_BOTTOM_LEFT: 
					DGWidgetSetRect(self, wx+dx, wy, ww-dx, wh+dy, 1);
				break;
			case DRAG_TOP_RIGHT:  
					DGWidgetSetRect(self, wx, wy+dy, ww+dx, wh-dy, 1);
				break;
			case DRAG_LEFT: 
					DGWidgetSetRect(self, wx+dx, wy, ww-dx, wh, 1);
				break;
			case DRAG_RIGHT: 
					DGWidgetSetRect(self, wx, wy, ww+dx, wh, 1);
				break;
			case DRAG_BOTTOM: 
					DGWidgetSetRect(self, wx, wy, ww, wh+dy, 1);
				break;
			case DRAG_TOP: 
					DGWidgetSetRect(self, wx, wy+dy, ww, wh-dy, 1);
				break;
			case DRAG_ALL: 
					DGWidgetSetRect(self,
									 DGWidgetX(self) + DG_WORD_LO(event->e.widget.p2), 
									 DGWidgetY(self) + DG_WORD_HI(event->e.widget.p2), 
									 DGWidgetWidth(self), DGWidgetHeight(self), 1);
				break;
		}

		DGWidgetFlip(self, 0);
		result = 1;
	}
	else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_START)
	{
		/* set what to drag */
		self->drag_info = get_grag_info(DGWidget(self), DG_WORD_LO(event->e.widget.p1), 
										DG_WORD_HI(event->e.widget.p1));

		DGWidgetSetCapture(self, 1);

		/* raise the widget to top */
		DGWidgetRaiseTop(self);

		DGWidgetFlip(self, 0);
		result = 1;
	} 
	else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_STOP)
	{
		DGWidgetSetCapture(self, 0);
		SetCursor(LoadCursor(0, IDC_ARROW));
		self->drag_info=0;
		result = 1;
	}
	else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_MOVE)
	{
		/* move, resize or nothing */
		if (!self->drag_info)
		{
			LPSTR idc = IDC_ARROW;
			switch (get_grag_info(DGWidget(self), DG_WORD_LO(event->e.widget.p1), DG_WORD_HI(event->e.widget.p1)))
			{
			case DRAG_TOP_LEFT: 
			case DRAG_BOTTOM_RIGHT:
				idc = IDC_SIZENWSE; break;
			case DRAG_BOTTOM_LEFT:
			case DRAG_TOP_RIGHT: 
				idc = IDC_SIZENESW; break;
			case DRAG_LEFT:
			case DRAG_RIGHT:
				idc = IDC_SIZEWE; break;
			case DRAG_BOTTOM:
			case DRAG_TOP:
				idc = IDC_SIZENS; break;
			case DRAG_ALL:
				idc = IDC_SIZEALL; break;
			}
			SetCursor(LoadCursor(0, idc));
		}
		result = 1;
	}
	return result;
}

dgw_form_p dgw_form_create(dg_widget_p parent, int x, int y, int width, int height, 
							const char* text)
{
	dgw_form_p self;
	dg_widget_p widget;

#if defined(DEBUG) && defined(_WIN32)
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	if (parent)
	{
		widget = parent->create(parent, sizeof(dgw_form_t), x, y, width, height);
	}
	else /* Only the main window doesn't have a parent */
	{

		/* Set default widgets style */
		dgw_set_style(DGW_STYLE_DEFAULT);

		/* Initialize video device */

		/* FIXME: guess video parameters */
		widget = dg_video_create(sizeof(dgw_form_t), width, height, 32, /*DG_OPT_FULLSCREEN |*/ DG_OPT_DOUBLE_BUFFERING)->master_widget;

		virtual_keyboard = dgw_keyboard_create(widget, 0, 0, 500, 160);
		DGWidgetSetVisible(virtual_keyboard, 0);
		DGWidgetRender(virtual_keyboard);
	}

	self = (dgw_form_p)widget;

	if (parent)
	{
		widget->render = _dgw_renders.render_form;
		DGWidgetAddListener(widget, dgw_form_event, EVT_TYPE_WIDGET | EVT_TYPE_MOUSE, self);
	}
	widget->destroy      = dgw_form_destroy;

	self->text = 0;
	self->options = DGW_FORM_OPTION_RESIZE;
	self->drag_info = 0;

	self->set_text       = dgw_form_set_text;
	self->get_text       = dgw_form_get_text;
	self->set_options    = dgw_form_set_options;
	self->get_options    = dgw_form_get_options;

	self->set_text(self, text);

	return self;
}
