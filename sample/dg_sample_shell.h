/*
	dg_sample_shell.h

	DG test utility interface
*/
#ifndef __DG_SAMPLE_SHELL
#define __DG_SAMPLE_SHELL

#include <stdio.h>
#include <windows.h>
#include <dg/dg_video.h>
#include <dg/dg_blit.h>
#include <dg/dg_event.h>
#include <dg/dg_keys.h>
#include <dg/dg_surface.h>
#include <dg/dg_widget.h>
#include <dg/dg_rect.h>
#include <dg/dg_gdc.h>

void test_performance( void );
void swap_buffers(dg_video_p device);
void start_sample(dg_event_listener_t event_handler);
void insert_widget(dg_widget_p widget);
void default_widget_render(dg_widget_p widget);

#endif
