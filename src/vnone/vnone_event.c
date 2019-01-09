/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      vnone_event.c                                      */
/* Description:   DG/None Event Management                           */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_keys.h>
#include <dg/dg_video.h>
#include "vnone_event.h"

static int vnone_event_peek(dg_event_manager_p self, dg_event_p event)
{
	return 0;
}

dg_event_manager_p vnone_create_event_manager(dg_video_p device)
{
	dg_event_manager_p self = dg_event_manager_master_create();
	self->peek = vnone_event_peek;
	return self;
}
