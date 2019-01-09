/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_keyboard.c                                     */
/* Description:   Keyboard widget                                    */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <dg/dg_keys.h>
#include <dg/dg_utf8.h>
#include <dg/dg_widget.h>
#include <dg/dg_event.h>
#include <dg/dgw_keyboard.h>
#include <dg/dgw_style.h>

extern dgw_style_renders_t _dgw_renders;

#define H_KEYS 12
#define V_KEYS 4

enum
{
	KB_TRANSLATE_KEYS,
	KB_TRANSLATE_LANG,
	KB_TRANSLATE_SPACE,
	KB_TRANSLATE_ESC,
	KB_TRANSLATE_COUNT
};

static const char* keyboard_translations[KB_TRANSLATE_COUNT][KEYBOARD_LANGUAGE_COUNT] = 
{
	{
		"1234567890-=qwertyuiop[]asdfghjkl;'\\\\zxcvbnm,./`!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:\"||ZXCVBNM<>?~",
		"\61\62\63\64\65\66\67\70\71\60\55\56\54\321\203\320\265\320\270\321\210\321\211\320\272\321\201\320\264\320\267\321\206\73\321\214\321\217\320\260\320\276\320\266\320\263\321\202\320\275\320\262\320\274\321\207\50\51\321\216\320\271\321\212\321\215\321\204\321\205\320\277\321\200\320\273\320\261\140\41\77\53\42\45\75\72\57\137\342\204\226\320\206\126\321\213\320\243\320\225\320\230\320\250\320\251\320\232\320\241\320\224\320\227\320\246\302\247\320\254\320\257\320\220\320\236\320\226\320\223\320\242\320\235\320\222\320\234\320\247\50\51\320\256\320\231\320\252\320\255\320\244\320\245\320\237\320\240\320\233\320\221\176"
	},
	{
		"US", 
		"\320\232\320\230\320\240"
	},
	{
		"SPACE", 
		"\320\230\320\235\320\242\320\225\320\240\320\222\320\220\320\233"
	},
	{
		"ESC", 
		"\320\236\320\242\320\232"
	}
};


static void dgw_keyboard_set_text(dgw_keyboard_p self, const char* text)
{
	if (self->text)
	{
		free(self->text);
	}
	self->text = strdup(text);
}

static const char* dgw_keyboard_get_text(dgw_keyboard_p self)
{
	return self->text;
}

static void dgw_keyboard_set_caps(dgw_keyboard_p self, int caps)
{
	if (self->caps != caps)
	{
		int i;
		int nkeys = KEYB_COUNT + V_KEYS * H_KEYS;
		self->caps = caps;
		for (i = KEYB_COUNT; i < nkeys; i++)
		{
			DGWidgetRender(self->keys[i]);
			DGWidgetInvalidate(self->keys[i], 0);
		}
	}
}

static int dgw_keyboard_is_caps(dgw_keyboard_p self)
{
	return self->caps;
}

static void dgw_keyboard_set_language(dgw_keyboard_p self, dg_keyboard_language_t language)
{
	if (language < KEYBOARD_LANGUAGE_COUNT)
	{
		int key;
		unsigned int code_size;
		int nkeys = KEYB_COUNT + V_KEYS * H_KEYS;
		self->language = language;
		self->set_text(self, keyboard_translations[KB_TRANSLATE_KEYS][language]);

		for (key = 0; key < nkeys; key++)
		{
			code_size = utf8_strchar(self->text, key - KEYB_COUNT, self->keys[key]->text);
			code_size += utf8_strchar(self->text, utf8_strlen(self->text) / 2 + key - KEYB_COUNT, self->keys[key]->text + code_size);
			self->keys[key]->text[code_size] = 0;
	
			DGWidgetRender(self->keys[key]);
			DGWidgetInvalidate(self->keys[key], 0);
		}
	}
}

static dg_keyboard_language_t dgw_keyboard_get_language(dgw_keyboard_p self)
{
	return self->language;
}

static const char* dgw_keyboard_get_language_id(dgw_keyboard_p self)
{
	return keyboard_translations[KB_TRANSLATE_LANG][self->language];
}

static const char* dgw_keyboard_get_space_id(dgw_keyboard_p self)
{
	return keyboard_translations[KB_TRANSLATE_SPACE][self->language];
}

static const char* dgw_keyboard_get_esc_id(dgw_keyboard_p self)
{
	return keyboard_translations[KB_TRANSLATE_ESC][self->language];
}

static void dgw_keyboard_set_options(dgw_keyboard_p self, int options)
{
	self->options=options;
}

static int dgw_keyboard_get_options(dgw_keyboard_p self)
{
	return self->options;
}

static void dgw_keyboard_capture(dgw_keyboard_p self, dgw_edit_p captured_edit)
{
	self->captured_edit = captured_edit;
}

static void dgw_keyboard_destroy(dg_widget_p widget)
{
	dgw_keyboard_p self = (dgw_keyboard_p)widget;
	if (self->text)
	{
		free(self->text);
	}
	if (self->keys)
		free(self->keys);
	dg_widget_destroy(widget);
}

/* FIXME: */
#include <windows.h>

static int dgw_key_event(dgw_key_p self, dg_event_p event)
{
	int redraw = 0;
	int redraw_edit = 0;
	int previous_status = self->is_push;
	dgw_keyboard_p keyboard = (dgw_keyboard_p)DGWidgetParent(self);
	if (event->type == EVT_TYPE_WIDGET)
	{
		if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAGGING)
		{
			return 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_START)
		{
			if (self->key == KEYB_SHIFT)
			{
				dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
				self->is_push = !self->is_push;
				keyboard->set_caps(keyboard, self->is_push);
				DGWidgetRect(keyboard, rect);
				DGWidgetFlip(keyboard, rect);
				rect->destroy(rect);
			}
			else if (self->key == KEYB_LANGUAGE)
			{
				dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
				DGWidgetSetCapture(self, 1);
				self->is_push = 1;
				keyboard->set_language(keyboard, (keyboard->language + 1) % KEYBOARD_LANGUAGE_COUNT);
				DGWidgetRect(keyboard, rect);
				DGWidgetFlip(keyboard, rect);
				rect->destroy(rect);
			}
			else
			{
				DGWidgetSetCapture(self, 1);
				self->is_push = 1;
			}
			redraw = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_STOP)
		{
			if (self->key != KEYB_SHIFT)
			{
				DGWidgetSetCapture(self, 0);
				self->is_push = 0;

				if (self->key >= KEYB_COUNT)
				{
					if (keyboard->captured_edit)
					{
						char *text = self->text;
						if (keyboard->caps)
						{
							text = utf8_strpos(self->text, 1);
						}
						keyboard->captured_edit->insert_text(keyboard->captured_edit, text, 1);
						redraw_edit = 1;
					}
				}
				else
				{
					switch (self->key)
					{
					case KEYB_LANGUAGE:
						DGWidgetSetCapture(self, 0);
						self->is_push = 0;
					break;
					case KEYB_BACKSPACE:
						if (keyboard->captured_edit)
							redraw_edit = keyboard->captured_edit->backspace(keyboard->captured_edit);
					break;
					case KEYB_SPACE:
						if (keyboard->captured_edit)
							redraw_edit = keyboard->captured_edit->insert_text(keyboard->captured_edit, " ", 1);
					break;
					case KEYB_ENTER:
					case KEYB_ESC:
					break;
					}
				}
				redraw = 1;
			}
		}
	}

	if (redraw)
	{
		dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
		DGWidgetRect(self, rect);
		DGWidgetRender(self);
		DGWidgetInvalidate(self, 0);
		DGWidgetFlip(self, rect);
		rect->destroy(rect);
	}
	
	if (redraw_edit && keyboard->captured_edit)
	{
		dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
		DGWidgetRender(keyboard->captured_edit);
		DGWidgetInvalidate(keyboard->captured_edit, 0);
		DGWidgetRect(keyboard->captured_edit, rect);
		DGWidgetFlip(keyboard->captured_edit, rect);
		rect->destroy(rect);
	}
	return redraw || redraw_edit;
}

static int dgw_keyboard_event(dgw_keyboard_p self, dg_event_p event)
{
	int result = 0;
	int redraw_edit = 0;
	if (event->type == EVT_TYPE_WIDGET)
	{
		if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAGGING)
		{
			DGWidgetSetRect(self, 
							DGWidgetX(self) + DG_WORD_LO(event->e.widget.p2), 
							DGWidgetY(self) + DG_WORD_HI(event->e.widget.p2), 
							DGWidgetWidth(self), DGWidgetHeight(self), 1);
			DGWidgetFlip(self, 0);
			result = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_START)
		{
			DGWidgetSetCapture(self, 1);

			/* raise the widget to top */
			DGWidgetRaiseTop(self);

			DGWidgetFlip(self, 0);
			result = 1;
		} 
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_STOP)
		{
			DGWidgetSetCapture(self, 0);
			result = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_FOCUS)
		{
			DGWidgetSetVisible(self, event->e.widget.p1);
			self->keys[self->key_cursor]->is_selected = event->e.widget.p1;
			DGWidgetRedraw(self->keys[self->key_cursor]);
			if (!event->e.widget.p1)
			{
				DGWidgetInvalidate(self, 0);
				//DGWidgetRedraw(self->keys[self->key_cursor]);
				DGWidgetFlip(self, 0);
			}
			result = 1;
		}
	}
	else if (event->type == EVT_TYPE_KEYBOARD)
	{
		int prev_key = self->key_cursor;
		if (event->e.keyboard.key_event & EVT_PRESSED)
		{
			switch (event->e.keyboard.key)
			{
			case KEY_Up: 
				if (self->key_cursor >= KEYB_COUNT)
				{
					if ((self->key_cursor - self->hkeys) < KEYB_COUNT)
					{
						if (self->captured_edit)
						{
							DGWidgetSetFocus(self->captured_edit);
							self->keys[self->key_cursor]->is_selected = 0;
							DGWidgetRedraw(self->keys[self->key_cursor]);
						}
					}
					else					
						self->key_cursor -= self->hkeys;
				}
				else
				{
					switch (self->key_cursor)
					{
					case KEYB_SPACE: self->key_cursor = KEYB_COUNT + (self->vkeys - 1) * self->hkeys + self->hkeys / 2 - 1; break;
					case KEYB_ENTER: self->key_cursor = KEYB_COUNT + (self->vkeys - 1) * self->hkeys + self->hkeys - 1; break;
					case KEYB_ESC: self->key_cursor = KEYB_LANGUAGE; break;
					case KEYB_SHIFT: self->key_cursor = KEYB_BACKSPACE; break;
					case KEYB_LANGUAGE: self->key_cursor = KEYB_SHIFT; break;
					case KEYB_BACKSPACE: 
						if (self->captured_edit)
						{
							DGWidgetSetFocus(self->captured_edit);
							self->keys[self->key_cursor]->is_selected = 0;
							DGWidgetRedraw(self->keys[self->key_cursor]);
						}
					break;
					}
				}
			break;
			case KEY_Down: 
				if (self->key_cursor >= KEYB_COUNT)
				{
					self->key_cursor += self->hkeys;
					if (self->key_cursor >= KEYB_COUNT + self->vkeys  * self->hkeys)
					{
						if (self->key_cursor < KEYB_COUNT + (self->vkeys + 1) * self->hkeys - 2)
							self->key_cursor = KEYB_SPACE;
						else
							self->key_cursor = KEYB_ENTER;
					}
				}
				else
				{
					switch (self->key_cursor)
					{
					case KEYB_SPACE: 
					case KEYB_ENTER: 
					case KEYB_ESC:
						if (self->captured_edit)
						{
							DGWidgetSetFocus(self->captured_edit);
							self->keys[self->key_cursor]->is_selected = 0;
							DGWidgetRedraw(self->keys[self->key_cursor]);
						}
					break;
					case KEYB_SHIFT: self->key_cursor = KEYB_LANGUAGE; break;
					case KEYB_LANGUAGE: self->key_cursor = KEYB_ESC; break;
					case KEYB_BACKSPACE: self->key_cursor = KEYB_SHIFT; break;
					}
				}
			break;
			case KEY_Left: 
				if (self->key_cursor >= KEYB_COUNT)
				{
					self->key_cursor--;
					if (self->key_cursor < KEYB_COUNT)
					{
						self->key_cursor = KEYB_COUNT + self->hkeys - 1;
					}
				}
				else
				{
					switch (self->key_cursor)
					{
					case KEYB_SPACE: self->key_cursor = KEYB_ESC; break;
					case KEYB_ENTER: self->key_cursor = KEYB_SPACE; break;
					case KEYB_ESC: self->key_cursor = KEYB_ENTER; break;
					case KEYB_SHIFT:
					case KEYB_LANGUAGE:
					case KEYB_BACKSPACE: self->key_cursor = KEYB_COUNT + self->hkeys - 1; break;
					}
				}
			break;
			case KEY_Right: 
				if (self->key_cursor >= KEYB_COUNT)
				{
					if (((self->key_cursor - KEYB_COUNT + 1) % self->hkeys) == 0)
					{
						if ((self->key_cursor - KEYB_COUNT + 1) / self->hkeys < self->vkeys / 2)
							self->key_cursor = KEYB_BACKSPACE;
						else
							self->key_cursor = KEYB_SHIFT;
					}
					else
					{
						self->key_cursor++;
					}
				}
				else
				{
					switch (self->key_cursor)
					{
					case KEYB_SPACE: self->key_cursor = KEYB_ENTER; break;
					case KEYB_ENTER: self->key_cursor = KEYB_ESC; break;
					case KEYB_ESC: self->key_cursor = KEYB_SPACE; break;
					case KEYB_LANGUAGE: self->key_cursor = KEYB_COUNT + 2*self->hkeys; break;
					case KEYB_SHIFT: self->key_cursor = KEYB_COUNT + self->hkeys; break;
					case KEYB_BACKSPACE: self->key_cursor = KEYB_COUNT; break;
					}
				}
			break;
			case KEY_Home:  
				self->key_cursor = KEYB_COUNT;
			break;
			case KEY_End:  
				self->key_cursor = KEYB_COUNT + self->vkeys * self->hkeys - 1;
			break;
			case KEY_Backspace: 
				if (self->captured_edit)
					redraw_edit = self->captured_edit->backspace(self->captured_edit);
			break;
			case KEY_Delete:
				if (self->captured_edit)
					redraw_edit = self->captured_edit->del(self->captured_edit);
			break;
			case KEY_Return: 
				if (self->key_cursor >= KEYB_COUNT)
				{
					if (self->captured_edit)
					{
						char *text = self->keys[self->key_cursor]->text;
						if (self->caps)
						{
							text = utf8_strpos(text, 1);
						}
						self->captured_edit->insert_text(self->captured_edit, text, 1);
						redraw_edit = 1;
					}
				}
				else
				{
					switch (self->key_cursor)
					{
					case KEYB_BACKSPACE:
						if (self->captured_edit)
							redraw_edit = self->captured_edit->backspace(self->captured_edit);
					break;
					case KEYB_SPACE:
						if (self->captured_edit)
							redraw_edit = self->captured_edit->insert_text(self->captured_edit, " ", 1);
					break;
					case KEYB_SHIFT:
						self->set_caps(self, !self->is_caps(self));
						DGWidgetRedraw(self);
					break;
					case KEYB_LANGUAGE:
						self->set_language(self, (self->language + 1) % KEYBOARD_LANGUAGE_COUNT);
						DGWidgetRedraw(self);
					break;
					case KEYB_ENTER:
					case KEYB_ESC:
					break;
					}
				}
				result = 1;
			break;
			default:
			{
				if (self->captured_edit)
				{
					char utf8char_bytes[4];
					utf8_encode(event->e.keyboard.key, utf8char_bytes);
					self->captured_edit->insert_text(self->captured_edit, utf8char_bytes, 1);
					redraw_edit = 1;
				}
			}
			break;
			}
		}
		if (prev_key != self->key_cursor)
		{
			/* cursor position changed */
			self->keys[prev_key]->is_selected = 0;
			DGWidgetRedraw(self->keys[prev_key]);
			self->keys[self->key_cursor]->is_selected = 1;
			DGWidgetRedraw(self->keys[self->key_cursor]);
		}
	}
	if (redraw_edit)
	{
		DGWidgetRedraw(self->captured_edit);
	}
	return result;
}

dgw_keyboard_p dgw_keyboard_create(dg_widget_p parent, int x, int y, int width, int height)
{
	int key;
	int nkeys = KEYB_COUNT + V_KEYS * H_KEYS;
	dg_widget_p widget = parent->create(parent, sizeof(dgw_keyboard_t), x, y, width, height);
	dgw_keyboard_p self = (dgw_keyboard_p)widget;

	widget->render = _dgw_renders.render_keyboard;
	widget->destroy = dgw_keyboard_destroy;
	DGWidgetAddListener(widget, dgw_keyboard_event, EVT_TYPE_WIDGET | EVT_TYPE_KEYBOARD, self);
	//widget->event = dgw_keyboard_event;
	widget->window->owner_window = 0;


	self->keys = 0;
	self->hkeys = H_KEYS;
	self->vkeys = V_KEYS;
	self->text = 0;
	self->options = 0;
	self->caps = 0;
	self->key_cursor = KEYB_ESC;
	self->captured_edit = 0;
	self->language = KEYBOARD_ENGLISH;

	self->set_text        = dgw_keyboard_set_text;
	self->get_text        = dgw_keyboard_get_text;
	self->set_caps        = dgw_keyboard_set_caps;
	self->is_caps         = dgw_keyboard_is_caps;
	self->set_language    = dgw_keyboard_set_language;
	self->get_language    = dgw_keyboard_get_language;
	self->get_language_id = dgw_keyboard_get_language_id;
	self->get_space_id    = dgw_keyboard_get_space_id;
	self->get_esc_id      = dgw_keyboard_get_esc_id;
	self->set_options     = dgw_keyboard_set_options;
	self->get_options     = dgw_keyboard_get_options;
	self->capture         = dgw_keyboard_capture;

	/* initialize keys */
	self->keys = (dgw_key_p*)malloc(nkeys * sizeof(dgw_key_p));
	for (key = 0; key < nkeys; key++)
	{
		int hspace = 5;
		int vspace = 5;
		int key_width = width / (H_KEYS + 4) - 1;
		int key_height = height / (V_KEYS + 2) - 1;
		dg_widget_p pkey;

		if (key < KEYB_COUNT)
		{
			switch (key)
			{
			case KEYB_BACKSPACE:
				self->keys[KEYB_BACKSPACE] = (dgw_key_p)widget->create(widget, sizeof(dgw_key_t), width - 2*hspace - 2 * key_width, vspace, 2 * key_width, key_height);
			break;
			case KEYB_SHIFT:
				self->keys[KEYB_SHIFT] = (dgw_key_p)widget->create(widget, sizeof(dgw_key_t), width - 2*hspace - 2 * key_width, 2 * vspace + key_height, 2 * key_width, key_height);
			break;
			case KEYB_LANGUAGE:
				self->keys[KEYB_LANGUAGE] = (dgw_key_p)widget->create(widget, sizeof(dgw_key_t), width - 2*hspace - 2 * key_width, 2 * vspace + 2*key_height, 2 * key_width, key_height);
			break;
			case KEYB_SPACE:
				self->keys[KEYB_SPACE] = (dgw_key_p)widget->create(widget, sizeof(dgw_key_t), hspace, height - key_height - 2*vspace, H_KEYS * key_width - hspace, key_height);
			break;
			case KEYB_ENTER:
				self->keys[KEYB_ENTER] = (dgw_key_p)widget->create(widget, sizeof(dgw_key_t), width - 3*hspace - 4 * key_width, height - key_height - 2*vspace, 2 * key_width, key_height);
			break;
			case KEYB_ESC:
				self->keys[KEYB_ESC] = (dgw_key_p)widget->create(widget, sizeof(dgw_key_t), width - 2*hspace - 2 * key_width, height - key_height - 2*vspace, 2 * key_width, key_height);
			break;			
			}
		}
		else
		{
			int x = hspace + (key_width + hspace) * ((key - KEYB_COUNT) % H_KEYS);
			int y = vspace + (key_height + vspace) * ((key - KEYB_COUNT) / H_KEYS);
			self->keys[key] = (dgw_key_p)widget->create(widget, sizeof(dgw_key_t), x, y, key_width, key_height);
		}

		self->keys[key]->key = key;	
		self->keys[key]->is_push = 0;
		self->keys[key]->is_selected = 0;

		pkey = (dg_widget_p)self->keys[key];
		pkey->window->owner_window = 1;
		pkey->render = _dgw_renders.render_keyboard_key;
		//pkey->event = dgw_key_event;
		DGWidgetAddListener(pkey, dgw_key_event, EVT_TYPE_WIDGET, pkey);
		DGWidgetRender(pkey);
	}

	self->set_language(self, KEYBOARD_ENGLISH);

	return self;
}
