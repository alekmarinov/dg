/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_edit.c                                         */
/* Description:   Edit widget                                        */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <dg/dg_keys.h>
#include <dg/dg_utf8.h>
#include <dg/dg_widget.h>
#include <dg/dg_event.h>
#include <dg/dgw_edit.h>
#include <dg/dgw_keyboard.h>
#include <dg/dgw_style.h>

extern dgw_style_renders_t _dgw_renders;

extern dgw_keyboard_p virtual_keyboard;

static void dgw_edit_set_position(dgw_edit_p self, int position)
{
	self->position=position;
}

static int dgw_edit_get_position(dgw_edit_p self)
{
	return self->position;
}

static void dgw_edit_set_scroll(dgw_edit_p self, int scroll)
{
	self->scroll=scroll;
}

static int dgw_edit_get_scroll(dgw_edit_p self)
{
	return self->scroll;
}

static void dgw_edit_set_text(dgw_edit_p self, const char* text)
{
	if (self->text)
	{
		free(self->text);
	}
	self->text=strdup(text);
}

static const char* dgw_edit_get_text(dgw_edit_p self)
{
	return self->text;
}

static void dgw_edit_set_options(dgw_edit_p self, int options)
{
	self->options=options;
}

static int dgw_edit_get_options(dgw_edit_p self)
{
	return self->options;
}

static int dgw_edit_left(dgw_edit_p self)
{
	if (self->position > 0)
	{
		self->position--;
		return 1;
	}
	return 0;
}

static int dgw_edit_right(dgw_edit_p self)
{
	if (self->position < (int)utf8_strlen(self->get_text(self)))
	{
		self->position++;
		return 1;
	}
	return 0;
}

static int dgw_edit_home(dgw_edit_p self)
{
	if (self->position != 0)
	{
		self->position = 0;
		return 1;
	}
	return 0;
}

static int dgw_edit_end(dgw_edit_p self)
{
	int tlen = (int)utf8_strlen(self->get_text(self));
	if (self->position != tlen)
	{
		self->position = tlen;
		return 1;
	}
	return 0;
}

static int dgw_edit_backspace(dgw_edit_p self)
{
	if (self->position > 0)
	{
		char* text = strdup(self->get_text(self));
		strcpy(utf8_strpos(text, self->position - 1), utf8_strpos(text, self->position));
		self->set_text(self, text);
		free(text);
		self->position--;
		return 1;
	}
	return 0;
}

static int dgw_edit_del(dgw_edit_p self)
{
	if (self->position < (int)utf8_strlen(self->get_text(self)))
	{
		char* text = strdup(self->get_text(self));
		strcpy(utf8_strpos(text, self->position), utf8_strpos(text, self->position + 1));
		self->set_text(self, text);
		free(text);
		return 1;
	}
	return 0;
}

static int dgw_edit_insert_text(dgw_edit_p self, const char* text, int count)
{
	if (*text)
	{
		char* p = utf8_strpos(text, count); 
		int text_size = (int)(p - text);
		char* temp_text = malloc(strlen(self->text) + text_size + 1);
		char* text_dst = temp_text + utf8_strcpy(temp_text, self->text, self->position);
		text_dst += utf8_strcpy(text_dst, text, count);
		strcat(text_dst, utf8_strpos(self->text, self->position));
		self->set_text(self, temp_text);
		free(temp_text);
		self->position++;
		return 1;
	}
	return 0;
}

static void dgw_edit_destroy(dg_widget_p widget)
{
	dgw_edit_p self = (dgw_edit_p)widget;
	if (self->text)
		free(self->text);
	dg_widget_destroy(widget);
}

/* FIXME: */
#include <windows.h>

static int x_to_position(dgw_edit_p self, int x)
{
/* FIXME!!!: */
	static const char* edit_font_face         = "Arial";
	static const int edit_font_size           = 20;
	int position = self->scroll;
	dg_gdc_p gdc = DGWidgetCreateGDC(self);
	const char* text = utf8_strpos(self->get_text(self), self->scroll);
	dg_font_p font;
	gdc->set_font(gdc, edit_font_face, edit_font_size);
	font = gdc->get_font(gdc);
	
	x -= 2;

	while (x > font->get_text_width(font, text, position - self->scroll) + 
		  (font->get_text_width(font, utf8_strpos(text, position - self->scroll + 1), 1) / 2))
	{
		position++;
	}

	gdc->destroy(gdc);
	return position;
}

static int dgw_edit_event(dgw_edit_p self, dg_event_p event)
{
	int redraw = 0;
	if (event->type == EVT_TYPE_WIDGET)
	{
		if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_START)
		{
			int mouse_x = DG_WORD_LO(event->e.widget.p1);
			int mouse_y = DG_WORD_HI(event->e.widget.p1);
			dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
			DGWidgetRect(self, rect);
			mouse_x -= rect->x;
			mouse_y -= rect->y;
			rect->destroy(rect);

			DGWidgetSetFocus(self);
			DGWidgetSetCapture(self, 1);
			self->position = x_to_position(self, mouse_x);
			if (self->position > (int)utf8_strlen(self->get_text(self)))
			{
				self->position = (int)utf8_strlen(self->get_text(self));
			}
			self->selection = self->position;
			redraw = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_STOP)
		{
			DGWidgetSetCapture(self, 0);
			redraw = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_FOCUS)
		{
			if (event->e.widget.p1)
			{
				dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
				virtual_keyboard->capture(virtual_keyboard, self);
				DGWidgetSetVisible(virtual_keyboard, 0);
				DGWidgetInvalidate(virtual_keyboard, 0);
				DGWidgetRect(virtual_keyboard, rect);
				DGWidgetFlip(virtual_keyboard, rect);
				rect->destroy(rect);
			}
			redraw = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_MOVE)
		{
			SetCursor(LoadCursor(0, IDC_IBEAM));
			return 1;
		}
	}
	else if (event->type == EVT_TYPE_KEYBOARD)
	{
		if (event->e.keyboard.key_event & EVT_PRESSED)
			switch (event->e.keyboard.key)
			{
			case KEY_Up: 
			case KEY_Down: 
				{
					dg_rect_p edit_rect = dg_rect_create(0, 0, 0, 0);
					dg_rect_p kb_rect = dg_rect_create(0, 0, 0, 0);
					int screen_width = DGWidgetDevice(self)->xres;
					int screen_height = DGWidgetDevice(self)->yres;
					
					DGWidgetRect(self, edit_rect);
					DGWidgetRect(virtual_keyboard, kb_rect);

					if (edit_rect->w < kb_rect->w)
						kb_rect->x = edit_rect->x - ((kb_rect->w - edit_rect->w) >> 1);
					else
						kb_rect->x = edit_rect->x + ((edit_rect->w - kb_rect->w) >> 1);

					if (kb_rect->x + kb_rect->w > screen_width)
						kb_rect->x = screen_width - kb_rect->w;

					if (kb_rect->x < 0) kb_rect->x = 0;

					if (edit_rect->y + edit_rect->h + kb_rect->h < screen_height )
						kb_rect->y = edit_rect->y + edit_rect->h;
					else
						kb_rect->y = edit_rect->y - kb_rect->h;

					
					if (kb_rect->y < 0) kb_rect->y = 0;

					DGWidgetSetRect(virtual_keyboard, kb_rect->x, kb_rect->y, kb_rect->w, kb_rect->h, 0);
					DGWidgetRaiseTop(virtual_keyboard);
					DGWidgetSetFocus(virtual_keyboard);
					DGWidgetSetVisible(virtual_keyboard, 1);
					//DGWidgetInvalidate(virtual_keyboard, 0);
					DGWidgetRect(virtual_keyboard, kb_rect);
					//DGWidgetFlip(virtual_keyboard, 0);
					kb_rect->destroy(kb_rect);
					edit_rect->destroy(edit_rect);

					DGWidgetAnimate(virtual_keyboard, WIDGET_ANIMATION_FADE_IN, 1000);
				}
			break;
			case KEY_Left: redraw = self->left(self); break;
			case KEY_Right: redraw = self->right(self); break;
			case KEY_Home:  redraw = self->home(self); break;
			case KEY_End:  redraw = self->end(self); break;
			case KEY_Backspace: redraw = self->backspace(self); break;
			case KEY_Delete: redraw = self->del(self); break;

			default:
			{
				char utf8char_bytes[4];
				utf8_encode(event->e.keyboard.key, utf8char_bytes);
				self->insert_text(self, utf8char_bytes, 1);
				redraw = 1;
			}
			break;
			}
	}

	if (redraw)
	{
		DGWidgetRedraw(self);
	}
	return redraw;
}

void render_cursor(dg_widget_p cursor)
{
	cursor->surface->clear(cursor->surface, DG_COLOR_BLACK);
	cursor->surface->set_alpha(cursor->surface, 128);
}

dgw_edit_p dgw_edit_create(dg_widget_p parent, int x, int y, int width, int height, const char* text)
{
	dg_widget_p widget = parent->create(parent, sizeof(dgw_edit_t), x, y, width, height);
	dgw_edit_p self = (dgw_edit_p)widget;

	widget->render = _dgw_renders.render_edit;
	widget->destroy = dgw_edit_destroy;
	widget->event_manager->add_listener(widget->event_manager, dgw_edit_event, EVT_TYPE_MOUSE, self);
	widget->event_manager->add_listener(widget->event_manager, dgw_edit_event, EVT_TYPE_WIDGET, self);
	//widget->event = dgw_edit_event;
	widget->can_focus = 1;
	widget->window->owner_window = 1;

	DGWidgetAddListener(widget, dgw_edit_event, EVT_TYPE_WIDGET | EVT_TYPE_KEYBOARD, self);

	self->cursor = widget->create(widget, sizeof(dg_widget_t), 0, 0, 2, height - 4);
	self->cursor->window->owner_window = 1;
	self->cursor->render = render_cursor;
	render_cursor(self->cursor);

	self->text = 0;
	self->options = 0;
	self->position = 0;
	self->selection = 0;
	self->scroll = 0;

	self->set_position   = dgw_edit_set_position;
	self->get_position   = dgw_edit_get_position;
	self->set_scroll     = dgw_edit_set_scroll;
	self->get_scroll     = dgw_edit_get_scroll;
	self->set_text       = dgw_edit_set_text;
	self->get_text       = dgw_edit_get_text;
	self->set_options    = dgw_edit_set_options;
	self->get_options    = dgw_edit_get_options;
	self->left           = dgw_edit_left;
	self->right          = dgw_edit_right;
	self->home           = dgw_edit_home;
	self->end            = dgw_edit_end;
	self->backspace      = dgw_edit_backspace;
	self->del            = dgw_edit_del;
	self->insert_text    = dgw_edit_insert_text;

	self->set_text(self, text);

	return self;
}
