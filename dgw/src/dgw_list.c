/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_list.c                                         */
/* Description:   List                                               */
/*                                                                   */
/*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <string.h>
#include <dg/dg_widget.h>
#include <dg/dgw_list.h>
#include <dg/dgw_style.h>
#include <dg/dg_keys.h>

extern dgw_style_renders_t _dgw_renders;

static void dgw_list_scroll_changed(dgw_list_p self)
{
	dg_event_scrollbar_t sbe;
	sbe.e.type = EVT_TYPE_SCROLLBAR;
	sbe.item_count = self->nrows;
	sbe.visible_item_count = self->get_visible_row_count(self);
	sbe.scroll = self->scroll;
	DGWidgetTriggerEvent(self->vscroll, &sbe);
}

static int dgw_list_get_visible_row_count(dgw_list_p self)
{
	return (int)floor(DGWidgetHeight(self) / (self->row_height + self->padding + self->spacing) );
}

static void dgw_list_set_image_list(dgw_list_p self, dgw_image_list_p image_list)
{
	if (self->image_list)
	{
		self->image_list->destroy(self->image_list);
	}
	image_list->add_ref(image_list);
	self->image_list = image_list;
}

static dgw_image_list_p dgw_list_get_image_list(dgw_list_p self)
{
	return self->image_list;
}

static void dgw_list_add_column(dgw_list_p self, dgw_list_column_t column)
{
	self->cols = (dgw_list_column_p)realloc(self->cols, (self->ncols+1)*sizeof(dgw_list_column_t));
	self->cols[self->ncols++] = column;
}

static const dgw_list_column_p dgw_list_get_column(dgw_list_p self, int col)
{
	return &self->cols[col];
}

static int dgw_list_get_column_count(dgw_list_p self)
{
	return self->ncols;
}

static void dgw_list_add_row(dgw_list_p self, dgw_list_row_t row)
{
	int col;
	if (!self->storage_count)
	{
		self->rows = (dgw_list_row_p)malloc(sizeof(dgw_list_row_t));
		self->storage_count=1;
	}
	else if (self->nrows == self->storage_count)
	{
		self->storage_count <<= 1;
		self->rows = (dgw_list_row_p)realloc(self->rows, 
											 self->storage_count * sizeof(dgw_list_row_t));
	}
	if (self->ncols)
	{
		self->rows[self->nrows].cells = (dgw_list_cell_p)malloc(self->ncols * sizeof(dgw_list_cell_t));
		for (col = 0; col < self->ncols; col++)
		{
			if (self->cols[col].column_type == LIST_COLUMN_IMAGE)
			{
				self->rows[self->nrows].cells[col].image = row.cells[col].image;
			}
			else if (self->cols[col].column_type == LIST_COLUMN_TEXT)
			{
				self->rows[self->nrows].cells[col].text = 
					(row.cells[col].text) ? strdup(row.cells[col].text) : 0;
			}
		}
	}
	else
	{
		self->rows[self->nrows].cells = 0;
	}
	self->rows[self->nrows].is_selected = 0;
	self->rows[self->nrows].is_separator = 0;
	self->nrows++;
	DGWidgetSetVisible(self->vscroll, self->nrows > self->get_visible_row_count(self));
	dgw_list_scroll_changed(self);
}

static const dgw_list_row_p dgw_list_get_row(dgw_list_p self, int row)
{
	if (row >= 0 && row < self->nrows)
	{
		return &self->rows[row];
	}
	return 0;
}

static void dgw_list_remove_row(dgw_list_p self, int row)
{
	if (row >= 0 && row < self->nrows)
	{
		int col;
		for (col = 0; col < self->ncols; col++)
		{
			if (self->cols[col].column_type == LIST_COLUMN_TEXT)
			{
				if (self->rows[row].cells[col].text)
					free(self->rows[row].cells[col].text);
			}
		}
		free(self->rows[row].cells);

		if (row < self->nrows - 1)
			memmove(self->rows + row, self->rows + row + 1, 
				   (self->nrows - row - 1) * sizeof(dgw_list_row_t));
		self->nrows--;
		dgw_list_scroll_changed(self);
	}
}

static void dgw_list_remove_all(dgw_list_p self)
{
	int row;
	for (row = 0; row < self->nrows; row++)
	{
		int col;
		for (col = 0; col < self->ncols; col++)
		{
			if (self->cols[col].column_type == LIST_COLUMN_TEXT)
			{
				if (self->rows[row].cells[col].text)
					free(self->rows[row].cells[col].text);
			}
		}
		free(self->rows[row].cells);
	}

	if (self->rows)
		free(self->rows);
	self->nrows = self->storage_count = 0;
	dgw_list_scroll_changed(self);
}

static int dgw_list_get_row_count(dgw_list_p self)
{
	return self->nrows;
}

static void dgw_list_set_options(dgw_list_p self, int options)
{
	self->options = options;
}

static int dgw_list_get_options(dgw_list_p self)
{
	return self->options;
}

static void dgw_list_validate_scroll(dgw_list_p self)
{
	int visible_items = self->get_visible_row_count(self);

	/* validate scroll */
	if (self->cursor < self->scroll)
	{
		self->scroll=self->cursor;
	}
	else if (self->cursor > (self->scroll + visible_items - 1))
	{
		self->scroll=self->cursor - visible_items + 1;
	}

	if (self->nrows - self->scroll < visible_items)
	{
		self->scroll=self->nrows - visible_items;
		if (self->scroll < 0) self->scroll = 0;
	}
	dgw_list_scroll_changed(self);
}

static void dgw_list_move_up(dgw_list_p self)
{
	if (self->nrows > 0)
	{
		if (self->cursor > 0)
		{
			self->set_cursor(self, self->cursor - 1);
			if (self->rows[self->cursor].is_separator) 
			{
				self->move_up(self);
			}
		}
		else if (self->is_wrap)
		{
			self->set_cursor(self, self->nrows - 1);
			if (self->rows[self->cursor].is_separator) 
			{
				self->move_up(self);
			}
		}
	}
}

static void dgw_list_move_down(dgw_list_p self)
{
	if (self->nrows > 0)
	{
		if ((self->cursor + 1) < self->nrows)
		{
			self->set_cursor(self, self->cursor + 1);
			if (self->rows[self->cursor].is_separator) 
			{
				self->move_down(self);
			}
		}
		else if (self->is_wrap)
		{
			self->set_cursor(self, 0);
			if (self->rows[self->cursor].is_separator) 
			{
				self->move_down(self);
			}
		}
	}
}

static void dgw_list_page_up(dgw_list_p self)
{
	if (self->nrows > 0)
	{
		if (self->cursor > 0)
		{
			self->set_cursor(self, self->cursor - self->get_visible_row_count(self));
			if (self->rows[self->cursor].is_separator) 
			{
				self->move_up(self);
			}
		}
		else if (self->is_wrap)
		{
			self->set_cursor(self, self->nrows - 1);
			if (self->rows[self->cursor].is_separator) 
			{
				self->move_up(self);
			}
		}
	}
}

static void dgw_list_page_down(dgw_list_p self)
{
	if (self->nrows > 0)
	{
		if ((self->cursor + 1) < self->nrows)
		{
			self->set_cursor(self, self->cursor + self->get_visible_row_count(self));
			if (self->rows[self->cursor].is_separator) 
			{
				self->move_down(self);
			}
		}
		else if (self->is_wrap)
		{
			self->set_cursor(self, 0);
			if (self->rows[self->cursor].is_separator) 
			{
				self->move_down(self);
			}
		}
	}
}

static void dgw_list_order_up(dgw_list_p self)
{
	if (self->cursor > 0)
	{
		dgw_list_row_t prev_row = self->rows[self->cursor - 1];
		self->rows[self->cursor - 1] = self->rows[self->cursor];
		self->rows[self->cursor] = prev_row;
		self->move_up(self);
	}
}

static void dgw_list_order_down(dgw_list_p self)
{
	if ((self->cursor + 1) < self->nrows)
	{
		dgw_list_row_t next_row = self->rows[self->cursor + 1];
		self->rows[self->cursor + 1] = self->rows[self->cursor];
		self->rows[self->cursor] = next_row;
		self->move_down(self);
	}
}

static void dgw_list_set_cursor(dgw_list_p self, int cursor)
{
	if (self->cursor != cursor)
	{
		if (cursor < 0) cursor = 0;
		if (cursor >= self->nrows) cursor = self->nrows-1;

		self->cursor = cursor;
	}
	self->validate_scroll(self);
}

static int dgw_list_get_cursor(dgw_list_p self)
{
	return self->cursor;
}

static void dgw_list_set_scroll(dgw_list_p self, int scroll)
{
	int visible_count = self->get_visible_row_count(self);
	if (scroll > (self->nrows - visible_count)) 
		scroll = self->nrows - visible_count;
	if (scroll < 0) scroll = 0;

	if (self->scroll != scroll)
	{
		self->scroll = scroll;
		dgw_list_scroll_changed(self);
	}
}

static int dgw_list_get_scroll(dgw_list_p self)
{
	return self->scroll;
}

static void dgw_list_destroy(dg_widget_p widget)
{
	dgw_list_p self = (dgw_list_p)widget;

	/* remove rows */
	self->remove_all(self);

	/* remove columns */
	if (self->cols)
		free(self->cols);

	if (self->image_list)
	{
		self->image_list->destroy(self->image_list);
		self->image_list=0;
	}

	dg_widget_destroy(widget);
}

int dgw_list_event(dgw_list_p self, dg_event_p event)
{
	int redraw = 0;
	int result = 0;
	if (event->type == EVT_TYPE_KEYBOARD && event->e.keyboard.key_event & EVT_PRESSED)
	{
		switch (event->e.keyboard.key)
		{
		case KEY_Up:
			self->move_up(self);
			redraw=1;
		break;
		case KEY_Down:
			self->move_down(self);
			redraw=1;
		break;
		case KEY_Page_Up:
			self->page_up(self);
			redraw=1;
		break;
		case KEY_Page_Down:
			self->page_down(self);
			redraw=1;
		break;
		}
	}
	else if (event->type == EVT_TYPE_WIDGET)
	{
		if (event->e.widget.widget_event & EVT_WIDGET_FOCUS)
		{
			redraw = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_WHEEL)
		{
			if (event->e.widget.p2)
			{
				/* Wheel Down */
				self->set_scroll(self, self->scroll + 1);
			}
			else
			{
				/* Wheel Up */
				self->set_scroll(self, self->scroll - 1);
			}
			redraw = 1;
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_MOUSE_DRAG_START)
		{
			if (!DGWidgetIsFocus(self))
			{
				DGWidgetSetFocus(self);
			}
			else
			{
				dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
				int x = DG_WORD_LO(event->e.widget.p1);
				int y = DG_WORD_HI(event->e.widget.p1);
				DGWidgetRect(self, rect);
				y -= rect->y;
				y /= self->row_height + self->spacing + self->padding;
				y += self->scroll;
				if (y < self->get_row_count(self))
				{
					redraw = self->get_cursor(self) != y;
					self->set_cursor(self, y);
				}
				rect->destroy(rect);
			}
		}
		else if (event->e.widget.widget_event & EVT_WIDGET_SIZE)
		{
			DGWidgetSetVisible(self->vscroll, self->nrows > self->get_visible_row_count(self));
			if (self->nrows - self->scroll < self->get_visible_row_count(self))
			{
				self->set_scroll(self, self->scroll-1);
			}
			else
			{
				dgw_list_scroll_changed(self);
			}
			result = 1;
		}
	}
	else if (event->type == EVT_TYPE_SCROLLBAR)
	{
		dg_event_scrollbar_p sbe = (dg_event_scrollbar_p)event;
		self->scroll = sbe->scroll;
		redraw = 1;
	}
	if (redraw)
	{
		dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
		DGWidgetRect(self, rect);
		DGWidgetRender(self);
		DGWidgetRender(self->vscroll);
		DGWidgetInvalidate(self, 0);
		DGWidgetFlip(self, rect);
		rect->destroy(rect);
		result = 1;
	}
	return result;
}

dgw_list_p dgw_list_create(dg_widget_p parent, int x, int y, int width, int height)
{
	dg_widget_p widget = parent->create(parent, sizeof(dgw_list_t), x, y, width, height);
	dgw_list_p self = (dgw_list_p)widget;

	DGWidgetLayoutAnchor(widget);
	widget->render = _dgw_renders.render_list;
	DGWidgetAddListener(widget, dgw_list_event, EVT_TYPE_WIDGET | EVT_TYPE_MOUSE | EVT_TYPE_SCROLLBAR, self);
	//widget->event = dgw_list_event;
	widget->can_focus = 1;
	widget->destroy = dgw_list_destroy;
	widget->window->owner_window = 1;

	/* create horizontal scrollbar */
	//self->hscroll=dgw_scrollbar_create(widget, SCROLLBAR_HORIZONTAL);
	//DGWidgetLayoutAnchorProps(self->hscroll->widget, DG_LAYOUT_ANCHOR_LEFT_RIGHT_BOTTOM);

	/* create vertical scrollbar */
	self->vscroll = dgw_scrollbar_create(widget, SCROLLBAR_VERTICAL);
	DGWidgetLayoutAnchorProps(self->vscroll, DG_LAYOUT_ANCHOR_TOP_BOTTOM_RIGHT);
	DGWidgetSetVisible(self->vscroll, 0);

	self->rows                   = 0;
	self->cols                   = 0;
	self->nrows                  = 0;
	self->ncols                  = 0;
	self->storage_count          = 0;
	self->options                = 0;
	self->spacing                = 0;
	self->padding                = 0;
	self->row_height             = 20; /* FIXME: hardcoded row height */
	self->cursor                 = 0;
	self->scroll                 = 0;
	self->is_wrap                = 0;
	self->is_cursor_visible      = 1;
	self->image_list             = 0;

	self->move_up                = dgw_list_move_up;
	self->move_down              = dgw_list_move_down;
	self->page_up                = dgw_list_page_up;
	self->page_down              = dgw_list_page_down;
	self->order_up               = dgw_list_order_up;
	self->order_down             = dgw_list_order_down;
	self->set_cursor             = dgw_list_set_cursor;
	self->get_cursor             = dgw_list_get_cursor;
	self->set_scroll             = dgw_list_set_scroll;
	self->get_scroll             = dgw_list_get_scroll;
	self->set_image_list         = dgw_list_set_image_list;
	self->get_image_list         = dgw_list_get_image_list;
	self->add_column             = dgw_list_add_column;
	self->get_column             = dgw_list_get_column;
	self->get_column_count       = dgw_list_get_column_count;
	self->add_row                = dgw_list_add_row;
	self->get_row                = dgw_list_get_row;
	self->get_row_count          = dgw_list_get_row_count;
	self->get_visible_row_count  = dgw_list_get_visible_row_count;
	self->remove_row             = dgw_list_remove_row;
	self->remove_all             = dgw_list_remove_all;
	self->set_options            = dgw_list_set_options;
	self->get_options            = dgw_list_get_options;
	self->validate_scroll        = dgw_list_validate_scroll;

	dgw_list_scroll_changed(self);

	return self;
}
