/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_window.c                                        */
/* Description:   Window Manager implementation                      */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <dg/dg_debug.h>
#include <dg/dg_window.h>

#if defined(DEBUG) && defined(WIN32)
#include <crtdbg.h>
#endif

dg_window_p dg_window_create(int, int, int, int, wm_update_callback_t, void*, dg_window_p);
static void dg_window_destroy(dg_window_p);
static void dg_window_remove(dg_window_p);
static void dg_window_insert_last(dg_window_p, dg_window_p);
static void dg_window_insert(dg_window_p, dg_window_p);
static void dg_window_invalidate(dg_window_p, dg_rect_p);
static void dg_window_set_rect(dg_window_p, int, int, int, int);
static int _dg_window_rectangulize(dg_rect_p, dg_rect_p, int, int, int, int, dg_rect_p[4]);
static void dg_window_get_rect(dg_window_p, dg_rect_p);
static int  dg_window_intersect(dg_window_p, dg_window_p, int*, int*, int*, int*);
static int  dg_window_intersect_rect(dg_window_p, dg_rect_p, int*, int*, int*, int*);
static void dg_window_update_window_rect(dg_window_p, dg_rect_p, int);
static void dg_window_update_tree(dg_window_p, dg_window_p, dg_rect_p, int);
static int  dg_window_is_visible(dg_window_p);
static void dg_window_set_visible(dg_window_p, int);
static dg_window_p dg_window_get_top(dg_window_p);
static void dg_window_remove_child(dg_window_p, dg_window_p);
static void dg_window_insert_child(dg_window_p, dg_window_p);
static void dg_window_set_parent(dg_window_p, dg_window_p);
static int  dg_window_is_parent(dg_window_p, dg_window_p);
static void dg_window_raise_top(dg_window_p);
static void dg_window_lower_bottom(dg_window_p);

static void dg_window_set_parent(dg_window_p self, dg_window_p parent)
{
	if (self->parent)
	{
		if (self->parent != parent)
		{
			/* remove this window from parent's children */
			self->parent->remove_child(self->parent, self);
		}
	}
	if (!self->parent && parent)
	{
		parent->insert_child(parent, self);
	}
}

static int dg_window_is_parent(dg_window_p self, dg_window_p window)
{
	dg_window_p parent=self->parent;
	while (parent)
	{
		if (parent == window)
		{
			return 1;
		}
		parent=parent->parent;
	}
	return 0;
}

static void dg_window_insert_child(dg_window_p self, dg_window_p child)
{
	/* unlink child from its previous list */
	if (child->prev)
	{
		child->remove(child);
	}
	child->next=child->prev=0;

	if (!(self->children))
	{
		self->children=child;
	}
	else
	{
		self->children->insert_last(self->children, child);
	}

	child->parent=self;
}

static void dg_window_remove_child(dg_window_p self, dg_window_p child)
{
	if (child->prev)
	{
		child->remove(child);
	}
	else if (child->next)
	{	
		/* child is the first window */
		self->children=child->next;
		self->children->prev=0;
		child->next=0;
	}
	else
	{
		/* child is the only window */
		self->children=0;
	}
	child->parent=0;
}

static dg_window_p dg_window_get_top(dg_window_p window)
{
	while (window->parent) window=window->parent;
	return window;
}

static void dg_window_set_visible(dg_window_p self, int visible)
{
	self->visible=visible;
}

static int dg_window_is_visible(dg_window_p self)
{
	if (self->parent)
	{
		return self->visible && self->parent->is_visible(self->parent);
	}
	else
	{
		return self->visible;
	}
}

static dg_rect_p dg_window_client_rect(dg_window_p window)
{
	dg_rect_p rect=dg_rect_create(0, 0, 0, 0);
	window->get_rect(window, rect);
	if (rect->w > window->client_left + window->client_right)
	{
		rect->w -= window->client_left + window->client_right;
		rect->x += window->client_left;
	}
	else
	{
		rect->w=0;
	}

	if (rect->h > window->client_top + window->client_bottom)
	{
		rect->h -= window->client_top + window->client_bottom;
		rect->y += window->client_top;
	}
	else
	{
		rect->h=0;
	}
	return rect;
}

static dg_rect_p dg_window_get_clip_rect(dg_window_p window)
{
	dg_rect_p clip_rect=dg_rect_create(0, 0, 0, 0);
	dg_window_p parent=window->parent;
	window->get_rect(window, clip_rect);

	if (parent && window->owner_window)
	{
		if (!parent->intersect_rect(parent, clip_rect, 
				&clip_rect->x, &clip_rect->y, &clip_rect->w, &clip_rect->h))
		{
			clip_rect->copy(clip_rect, 0, 0, 0, 0);
			return clip_rect;
		}
	}

	while (parent && !parent->clip_children)
		parent=parent->parent;

	if (parent)
	{
		dg_rect_p parent_clip_rect=dg_window_get_clip_rect(parent);
		if (parent_clip_rect && 
			!clip_rect->intersect(clip_rect, parent_clip_rect, 
				&clip_rect->x, &clip_rect->y, &clip_rect->w, &clip_rect->h) || 
			!parent->intersect_rect(parent, clip_rect, 
				&clip_rect->x, &clip_rect->y, &clip_rect->w, &clip_rect->h))
		{
				clip_rect->copy(clip_rect, 0, 0, 0, 0);
		}
		if (parent_clip_rect) parent_clip_rect->destroy(parent_clip_rect);
	}
	return clip_rect;
}

static void dg_window_update_tree(dg_window_p tree, dg_window_p window, dg_rect_p window_rect, int is_show)
{
	if (tree)
	{
		dg_window_p iter_win;
		dg_window_p first=tree;
		while (first->prev) first=first->prev;
		for (iter_win=first; iter_win; iter_win=iter_win->next)
		{
			/* update a subtree if its root is visible */
			if (iter_win->is_visible(iter_win))
			{
				int is_update=iter_win != window || is_show;
				if (!is_show && iter_win->owner_window && iter_win->parent == window) /*iter_win->is_parent(iter_win, window)*/
				{
					continue;
				}
				if (is_update)
				{
					dg_rect_p update_rect=dg_rect_create(0, 0, 0, 0);
					if (window_rect)
						update_rect->copy_rect(update_rect, window_rect);
					else
						window->get_rect(window, update_rect);

					if (iter_win->intersect_rect(iter_win, update_rect,
						&update_rect->x, &update_rect->y, &update_rect->w, &update_rect->h))
					{
						dg_rect_p clip_rect=dg_window_get_clip_rect(iter_win);
						if ( (clip_rect->w && clip_rect->h) && update_rect->intersect(update_rect, clip_rect,
							&update_rect->x, &update_rect->y, &update_rect->w, &update_rect->h))
						{
							if (iter_win->owner_window)
							{
								dg_rect_p client_rect=dg_window_client_rect(iter_win->parent);
								if (client_rect->w && client_rect->h)
									client_rect->intersect(client_rect, update_rect, 
										&update_rect->x, &update_rect->y, &update_rect->w, &update_rect->h);
								client_rect->destroy(client_rect);
							}

							iter_win->update_callback(iter_win->param, 
								update_rect->x, update_rect->y, update_rect->w, update_rect->h);
						}
						clip_rect->destroy(clip_rect);
					}

					update_rect->destroy(update_rect);
				}
				/* update children */
				if ( is_update || !iter_win->clip_children)
					dg_window_update_tree(iter_win->children, window, window_rect, is_show);
			}
		}
	}
}

static void dg_window_update_window_rect(dg_window_p window, dg_rect_p rect, int is_show)
{
	dg_window_update_tree(window->get_top(window), window, rect, is_show);
}

static int dg_window_intersect(dg_window_p self, dg_window_p window, int* px, int* py, int* pw, int* ph)
{
	int result;
	dg_rect_p rect1 = dg_rect_create(0, 0, 0, 0);
	dg_rect_p rect2 = rect1->clone(rect1);
	self->get_rect(self, rect1);
	window->get_rect(window, rect2);
	result=rect1->intersect(rect1, rect2, px, py, pw, ph);
	rect1->destroy(rect1);
	rect2->destroy(rect2);
	return result;
}

static int dg_window_intersect_rect(dg_window_p self, dg_rect_p rect, int* px, int* py, int* pw, int* ph)
{
	int result;
	dg_rect_p rect1 = dg_rect_create(0, 0, 0, 0);
	self->get_rect(self, rect1);
	result=rect1->intersect(rect1, rect, px, py, pw, ph);
	rect1->destroy(rect1);
	return result;
}

static void dg_window_get_rect(dg_window_p self, dg_rect_p rect)
{
	dg_window_p parent=self->parent;
	dg_window_p prev_parent=self;
	rect->copy_rect(rect, self->rect);
	while (parent)
	{
		if (parent->clip_children || (prev_parent->owner_window))
		{
			rect->x+=parent->rect->x + parent->client_left;
			rect->y+=parent->rect->y + parent->client_top;
		}
		prev_parent=parent;
		parent=parent->parent;
	}
}

static int _dg_window_rectangulize(dg_rect_p rect1, dg_rect_p rect2, int ix, int iy, int iw, int ih, dg_rect_p rects[4])
{
	if ( ix >= rect1->x && (ix+iw) <= (rect1->x+rect1->w) && iy >= rect1->y && (iy+ih) <= (rect1->y+rect1->h) )
	{
		rects[0] = dg_rect_create(rect1->x, iy, ix-rect1->x, ih); // left
		rects[1] = dg_rect_create(ix+iw, iy, rect1->x+rect1->w-ix-iw, ih); // rigth
		rects[2] = dg_rect_create(rect1->x, rect1->y, rect1->w, iy-rect1->y); // top
		rects[3] = dg_rect_create(rect1->x, iy+ih, rect1->w, rect1->y+rect1->h-ih-iy); // bottom
		return 4;
	}
	return 0;
}

static void dg_window_set_rect(dg_window_p self, int x, int y, int w, int h)
{
	int ix, iy, iw, ih;
	int old_x, old_y;
	dg_rect_p rect_old;
	dg_rect_p rect_new;
	dg_rect_p rects[4];

	rect_old=dg_rect_create(0, 0, 0, 0);
	self->get_rect(self, rect_old);
	old_x=self->rect->x;
	old_y=self->rect->y;

	rect_new=dg_rect_create(0, 0, 0, 0);
	self->rect->x=x;
	self->rect->y=y;
	self->get_rect(self, rect_new);
	self->rect->x=old_x;
	self->rect->y=old_y;
	rect_new->w=w;
	rect_new->h=h;

	if (rect_old->intersect(rect_old, rect_new, &ix, &iy, &iw, &ih))
	{
		/* update only not intersected zone */
		int nrects;
		if ( (nrects=_dg_window_rectangulize(rect_old, rect_new, ix, iy, iw, ih, rects)) )
		{
			int i;
			dg_rect_p old_rect=self->rect;
			for (i=0; i<nrects; i++)
			{
				dg_rect_p rect=rects[i];
				rect->move(rect, rect->x-rect_new->x+x, rect->y-rect_new->y+y);
				if (rect->w && rect->h)
				{
					self->rect=rect;
					dg_window_update_window_rect(self, 0, 0);
				}
				rect->destroy(rect);
			}
			self->rect=old_rect;
		}
	}
	else
	{
		/* update the whole zone under window */
		dg_window_update_window_rect(self, 0, 0);
	}

	self->rect->copy(self->rect, x, y, w, h);

	/* update all windows under the new area including this window */
	dg_window_update_window_rect(self, 0, 1);

	rect_old->destroy(rect_old);
	rect_new->destroy(rect_new);
}

static void dg_window_invalidate(dg_window_p self, dg_rect_p rect)
{
	if (rect)
	{
		int ix, iy, iw, ih;
		dg_rect_p absolute_rect=dg_rect_create(0, 0, 0, 0);
		dg_rect_p offset_rect=rect->clone(rect);
		self->get_rect(self, absolute_rect);
		offset_rect->move(offset_rect, offset_rect->x+absolute_rect->x, offset_rect->y+absolute_rect->y);

		if (absolute_rect->intersect(absolute_rect, offset_rect, &ix, &iy, &iw, &ih))
		{
			absolute_rect->copy(absolute_rect, ix, iy, iw, ih);
			dg_window_update_window_rect(self, absolute_rect, 1);
		}
		
		offset_rect->destroy(offset_rect);
		absolute_rect->destroy(absolute_rect);
	}
	else
	{
		dg_window_update_window_rect(self, 0, 1);
	}
}

static void dg_window_insert(dg_window_p self, dg_window_p window)
{
	assert(window);
	if (window->prev)
	{
		window->prev->next=window->next;
	}
	if (window->next)
	{
		window->next->prev=window->prev;
	}
	window->next=self->next;
	self->next=window;
	window->prev=self;
}

static void dg_window_insert_last(dg_window_p self, dg_window_p window)
{
	dg_window_p iter_win=self;
	assert(iter_win);
	while (iter_win->next) iter_win=iter_win->next;
	iter_win->insert(iter_win, window);
}

static void dg_window_remove(dg_window_p self)
{
	if (self->prev) /* never remove the first (background) window */
	{
		self->prev->next=self->next;
		if (self->next)
			self->next->prev=self->prev;
		self->next=self->prev=0;
	}
}

/* move the window to the last position in the siblings list */
static void dg_window_raise_top(dg_window_p self)
{
	if (self->next)
	{
		/* find last */
		dg_window_p last=self->next;
		while (last->next) last=last->next;

		/* unlink from current position */
		if (self->prev)
		{
			self->prev->next=self->next;
		}
		else
		{
			if (self->parent)
				self->parent->children=self->next;
		}
		self->next->prev=self->prev;

		/* link to the last position */
		last->next=self;
		self->prev=last;
		self->next=0;

		/* invalidate children  */
		if (!self->clip_children)
		{
			/* invalidate children */
			dg_window_p child=self->children;
			while (child && child->next)
			{
				child=child->next;
			}
			while (child)
			{
				if (!child->owner_window)
					child->invalidate(child, 0);
				child=child->prev;
			}
		}

		/* invalidate window area */
		self->invalidate(self, 0);
	}
}

/* move the window to the first position in the siblings list */
static void dg_window_lower_bottom(dg_window_p self)
{
	if (self->prev)
	{
		/* find first */
		dg_window_p first=self->prev;
		while (first->prev) first=first->prev;

		/* unlink from current position */
		if (self->next)
		{
			self->next->prev=self->prev;
		}
		self->prev->next=self->next;

		/* link to the first position */
		first->prev=self;
		self->next=first;
		self->prev=0;
		if (self->parent)
		{
			self->parent->children=self;
		}

		/* invalidate children  */
		if (!self->clip_children)
		{
			/* invalidate children */
			dg_window_p child=self->children;
			while (child && child->next)
			{
				child=child->next;
			}
			while (child)
			{
				if (!child->owner_window)
					child->invalidate(child, 0);
				child=child->prev;
			}
		}

		/* invalidate window area */
		self->invalidate(self, 0);
	}
}

static void dg_window_destroy(dg_window_p self)
{
	assert(self);
	assert(!self->param);

	/* unlink this window from its parent */
	self->set_parent(self, 0);
	self->rect->destroy(self->rect);
	free(self);
}

dg_window_p dg_window_create(int x, int y, int width, int height, wm_update_callback_t update_callback, void* param, dg_window_p parent)
{
	dg_window_p self=(dg_window_p)malloc(sizeof(dg_window_t));
	self->rect=dg_rect_create(x, y, width, height);
	self->update_callback=update_callback;
	self->param=param;
	self->parent=0;
	self->children=0;
	self->visible=1;
	self->clip_children=0;
	self->owner_window=0;
	self->next=self->prev=0;
	self->client_left=self->client_top=self->client_right=self->client_bottom=0;

	self->set_parent     = dg_window_set_parent;
	self->is_parent      = dg_window_is_parent;
	self->insert_child   = dg_window_insert_child;
	self->remove_child   = dg_window_remove_child;
	self->get_top        = dg_window_get_top;
	self->set_visible    = dg_window_set_visible;
	self->is_visible    = dg_window_is_visible;
	self->intersect      = dg_window_intersect;
	self->intersect_rect = dg_window_intersect_rect;
	self->get_rect       = dg_window_get_rect;
	self->set_rect       = dg_window_set_rect;
	self->invalidate     = dg_window_invalidate;
	self->insert         = dg_window_insert;
	self->insert_last    = dg_window_insert_last;
	self->raise_top      = dg_window_raise_top;
	self->lower_bottom   = dg_window_lower_bottom;
	self->remove         = dg_window_remove;
	self->destroy        = dg_window_destroy;

	if (parent)
	{
		self->set_parent(self, parent);
	}

	return self;
}
