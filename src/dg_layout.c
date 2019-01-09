/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_layout.c                                        */
/* Description:   Layout Management implementation                   */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <dg/dg_layout.h>
#include <dg/dg_widget.h>

/* Anchor layout API implementation */
static void dg_layout_anchor_properties_destroy(dg_layout_properties_p self)
{
	if (self)
	{
		free(self);
	}
}

static void dg_layout_anchor_properties_set(dg_layout_anchor_properties_p self, int anchor)
{
	int* nProps=(int*)&self->properties;
	self->properties=0;
	*nProps=anchor;
}

static void dg_layout_anchor_properties_get(dg_layout_anchor_properties_p self, int *anchor)
{
	void** pProps=(void*)anchor;
	*pProps=self->properties;
}

/* anchor layout manager */
void dg_layout_anchor_manager(dg_window_p child_windows, int oldWidth, int oldHeight, int newWidth, int newHeight)
{
	/* rearrange widget's children  */
	while (child_windows)
	{
		/* set child widget position according its anchor layout properties */
		dg_widget_p widget = (dg_widget_p)child_windows->param;
		dg_rect_p rect     = widget->window->rect; //dg_rect_create(0, 0, 0, 0);
		int anchor, changedX, changedY, changedWidth, changedHeight;

		//widget->get_rect(widget, widget);
		changedX       = rect->x;
		changedY       = rect->y;
		changedWidth   = rect->w;
		changedHeight  = rect->h;

		if (!widget->layout_properties)
		{
			child_windows=child_windows->next;
			continue;
		}

		((dg_layout_anchor_properties_p)widget->layout_properties)->get((dg_layout_anchor_properties_p)(widget->layout_properties), &anchor);

		if ( (anchor & DG_LAYOUT_ANCHOR_LEFT_RIGHT) == DG_LAYOUT_ANCHOR_LEFT_RIGHT)
		{
			/* change width to satisfy left and right anchors */
			changedWidth = rect->w + newWidth - oldWidth;
		} else
		if (anchor & DG_LAYOUT_ANCHOR_RIGHT)
		{
			/* change x to satisfy right anchor */
			changedX = rect->x + newWidth - oldWidth;
		}

		if ( (anchor & DG_LAYOUT_ANCHOR_TOP_BOTTOM) == DG_LAYOUT_ANCHOR_TOP_BOTTOM)
		{
			/* change height to satisfy top and bottom anchors */
			changedHeight = rect->h + newHeight - oldHeight;
		} else
		if (anchor & DG_LAYOUT_ANCHOR_BOTTOM)
		{
			/* change y to satisfy bottom anchor */
			changedY = rect->y + newHeight - oldHeight;
		}

		widget->set_rect(widget, changedX, changedY, changedWidth, changedHeight, 0);
		child_windows = child_windows->next;
	}
}

/* Anchor layout manager constructor */
dg_layout_properties_p dg_layout_anchor_properties_create(int anchor)
{
	dg_layout_anchor_properties_p self=(dg_layout_anchor_properties_p)malloc(sizeof(dg_layout_anchor_properties_t));
	self->destroy = dg_layout_anchor_properties_destroy;
	self->get     = dg_layout_anchor_properties_get;
	self->set     = dg_layout_anchor_properties_set;

	self->set(self, anchor);

	return (dg_layout_properties_p)self;
}
