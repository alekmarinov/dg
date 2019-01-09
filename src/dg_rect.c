/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_rect.c                                          */
/* Description:   Rectangle structure implementation                 */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <dg/dg_debug.h>
#include <dg/dg_rect.h>

static void dg_rect_set_x(dg_rect_p self, int x)
{
	self->x=x;
}

static int dg_rect_get_x(dg_rect_p self)
{
	return self->x;
}

static void dg_rect_set_y(dg_rect_p self, int y)
{
	self->y=y;
}

static int dg_rect_get_y(dg_rect_p self)
{
	return self->y;
}

static void dg_rect_set_w(dg_rect_p self, int w)
{
	self->w=w;
}

static int dg_rect_get_w(dg_rect_p self)
{
	return self->w;
}

static void dg_rect_set_h(dg_rect_p self, int h)
{
	self->h=h;
}

static int dg_rect_get_h(dg_rect_p self)
{
	return self->h;
}

static int dg_rect_get_right(dg_rect_p self)
{
	return self->x + self->w - 1;
}

static int dg_rect_get_bottom(dg_rect_p self)
{
	return self->y + self->h - 1;
}

static int dg_rect_is_point_inside(dg_rect_p self, int x, int y)
{
	return ( (x>=self->x) && x<=(self->x + self->w) && (y>=self->y) && y<=(self->y + self->h));
}

static int dg_rect_intersect(dg_rect_p self, dg_rect_p rect, int* px, int* py, int* pw, int* ph)
{
	if (
	(rect->get_right(rect) >= self->get_left(self)) &&
	(rect->get_left(rect) <= self->get_right(self)) &&
	(rect->get_bottom(rect) >= self->get_top(self)) &&
	(rect->get_top(rect) <= self->get_bottom(self))) 
	{
		int x[4],y[4];
		int i,j,temp;

		x[0]=self->get_left(self);
		x[1]=self->get_right(self);
		x[2]=rect->get_left(rect);
		x[3]=rect->get_right(rect);

		y[0]=self->get_top(self);
		y[1]=self->get_bottom(self);
		y[2]=rect->get_top(rect);
		y[3]=rect->get_bottom(rect);

		for(i=0; i<3; i++)
			for(j=0; j<3-i; j++)
			{
				if(x[j]>x[j+1])
				{
					temp=x[j];
					x[j]=x[j+1];
					x[j+1]=temp;
				}
				if(y[j]>y[j+1])
				{
					temp=y[j];
					y[j]=y[j+1];
					y[j+1]=temp;
				}
			}
		*px=x[1];
		*py=y[1];
		*pw=x[2]-x[1]+1;
		*ph=y[2]-y[1]+1;
		return 1;
	}
	return 0;
}

static void dg_rect_move(dg_rect_p self, int x, int y)
{
	self->set_x(self, x);
	self->set_y(self, y);
}

static void dg_rect_resize(dg_rect_p self, int w, int h)
{
	self->set_w(self, w);
	self->set_h(self, h);
}

static void dg_rect_copy(dg_rect_p self, int x, int y, int w, int h)
{
	self->set_x(self, x);
	self->set_y(self, y);
	self->set_w(self, w);
	self->set_h(self, h);
}
static void dg_rect_copy_rect(dg_rect_p self, dg_rect_p source)
{
	self->copy(self, source->x, source->y, source->w, source->h);
}

static struct _dg_rect_t* dg_rect_clone(struct _dg_rect_t* self)
{
	return dg_rect_create(self->x, self->y, self->w, self->h);
}

static void dg_rect_destroy(dg_rect_p self)
{
	assert(self);
	free(self);
}

dg_rect_p dg_rect_create(int x, int y, int w, int h)
{
	dg_rect_p self=(dg_rect_p)malloc(sizeof(dg_rect_t));
	self->x=x;
	self->y=y;
	self->w=w;
	self->h=h;
	
	self->set_x=dg_rect_set_x;
	self->get_x=dg_rect_get_x;
	self->set_y=dg_rect_set_y;
	self->get_y=dg_rect_get_y;
	self->set_w=dg_rect_set_w;
	self->get_w=dg_rect_get_w;
	self->set_h=dg_rect_set_h;
	self->get_h=dg_rect_get_h;
	self->get_left=dg_rect_get_x;
	self->get_top=dg_rect_get_y;
	self->get_right=dg_rect_get_right;
	self->get_bottom=dg_rect_get_bottom;
	self->is_point_inside=dg_rect_is_point_inside;
	self->intersect=dg_rect_intersect;
	self->move=dg_rect_move;
	self->resize=dg_rect_resize;
	self->copy=dg_rect_copy;
	self->copy_rect=dg_rect_copy_rect;
	self->clone=dg_rect_clone;
	self->destroy=dg_rect_destroy;

	return self;
}
