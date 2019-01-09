/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_image_list.c                                   */
/* Description:   Indexed list of images                             */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <dg/dgw_image_list.h>

static void dgw_image_list_add(dgw_image_list_p self, dg_surface_p image)
{
	if (!self->storage_count)
	{
		self->images = (dg_surface_p*)malloc(sizeof(dg_surface_p));
		self->storage_count=1;
	}
	else if (self->count == self->storage_count)
	{
		self->storage_count <<= 1;
		self->images = (dg_surface_p*)realloc(self->images, 
											  self->storage_count * sizeof(dg_surface_p));
	}
	self->images[self->count++] = image;
}

static dg_surface_p dgw_image_list_get(dgw_image_list_p self, int n)
{
	return (n>=0 && n<self->count)?self->images[n]:0;
}

static int dgw_image_list_get_count(dgw_image_list_p self)
{
	return self->count;
}

static void dgw_image_list_add_ref(dgw_image_list_p self)
{
	self->ref++;
}

static void dgw_image_list_destroy(dgw_image_list_p self)
{
	self->ref--;
	if (self->ref == 0)
	{
		int i;
		for (i=0; i<self->count; i++)
		{
			self->images[i]->destroy(self->images[i]);
		}
		free(self->images);
		free(self);
	}
}

dgw_image_list_p dgw_image_list_create()
{
	dgw_image_list_p self  = (dgw_image_list_p)malloc(sizeof(dgw_image_list_t));

	self->ref = 1;
	self->count = 0;
	self->storage_count = 0;
	self->images = 0;

	self->add       = dgw_image_list_add;
	self->get       = dgw_image_list_get;
	self->get_count = dgw_image_list_get_count;
	self->add_ref   = dgw_image_list_add_ref;
	self->destroy   = dgw_image_list_destroy;

	return self;
}
