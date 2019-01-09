/*
	dg_sample_shell.c

	DG test utility implementation
*/

#include <windows.h>
#include <math.h>
#include "dg_sample_shell.h"

#define XRES 640
#define YRES 480
#define BPP 32
#define OPTIONS DG_OPT_DOUBLE_BUFFERING/*DG_OPT_FULLSCREEN | DG_OPT_DOUBLE_BUFFERING*/


LARGE_INTEGER start_counts;
DWORD start_time=0;
int is_test_performance=0;
int frames=0;
__int64 allCounts=0;

void test_performance()
{
	if (!is_test_performance)
	{
		/* performance test started */
		is_test_performance=1;
		start_time=timeGetTime();
	}
	QueryPerformanceCounter(&start_counts);
}

void swap_buffers(dg_video_p device)
{
	if (is_test_performance)
	{
		DWORD lastFrameCounts;
		DWORD allFramesTime;
		LARGE_INTEGER end_counts;
		DWORD end_time;
		QueryPerformanceCounter(&end_counts);
		end_time=timeGetTime();

		lastFrameCounts=end_counts.u.LowPart-start_counts.u.LowPart;
		allFramesTime=end_time-start_time;
		frames++;
		allCounts+=lastFrameCounts;

		if (allFramesTime > 5000) // 5 seconds elapsed
		{
			// reset average value
			printf("\rReaveraging ...                      ");
			frames=0;
			allCounts=0;
			is_test_performance=0;
			allFramesTime=0;
		}

		if (allCounts>0)
		{
			if (allFramesTime>1000)
			{
				__int64 countsPerMS=(DWORD)(allCounts/allFramesTime);
				double framesPerMS=(double)frames/((double)allCounts/(double)countsPerMS);
				int framesPerSec=(int)floor(0.5+1000*framesPerMS);
				printf("\rtime=%d, average = %d fps=%d              ", (allFramesTime/1000), (DWORD)(allCounts/frames), framesPerSec);
			}
		}
	}

	//device->vsync(device); // to avoid flickering
	if (device->options & DG_OPT_DOUBLE_BUFFERING)
	{
		device->flip(device, 0);
	}
}

void default_widget_render(dg_widget_p self)
{
	unsigned char r, g, b, a;
	long addr=(long)self;
	addr = (addr & 0xFFFF) ^ ((addr >> 16) & 0xFFFF);
	a=0;
	r=(unsigned char)(addr & 0xFF); addr>>=8;
	g=(unsigned char)(addr & 0xFF); addr>>=8;
	b=(unsigned char)(addr & 0xFF);

	self->surface->clear(self->surface, DG_RGB_COLOR(r, g, b, a));
}

static int keyboard_handler(dg_video_p device, dg_event_p event)
{
	if (event->type == EVT_TYPE_KEYBOARD)
	{
		printf("Got keyboard message: %d\n", event->e.keyboard.key);

		if (event->e.keyboard.key == KEY_Escape)
		{
			device->master_widget->event_manager->active=0;
		}
	}
	return 1;
}

void start_sample(dg_event_listener_t event_handler)
{
	dg_video_p device;

	device = dg_video_create(sizeof(dg_widget_t), XRES, YRES, BPP, OPTIONS);
	if (device)
	{
		device->master_widget->event_manager->add_listener(device->master_widget->event_manager, keyboard_handler, EVT_TYPE_KEYBOARD | EVT_TYPE_MOUSE, device);
		device->master_widget->event_manager->add_listener(device->master_widget->event_manager, event_handler, EVT_TYPE_KEYBOARD | EVT_TYPE_MOUSE | EVT_TYPE_TIMER, device);
		event_handler(device, 0);
		device->master_widget->event_manager->loop(device->master_widget->event_manager);

		device->destroy(device);
	}
	else
	{
		printf("Unable to initialize video driver (%dx%d-%d)\n", XRES, YRES, BPP);
	}
}
