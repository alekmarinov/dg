/*
	dgw_sample_movie_1.c
	Test movie reader
*/

#include "dg_sample_shell.h"
#include <dg/dg_movie.h>
#include <dg/dg_plugin.h>

#define XRES 1280
#define YRES 1024
#define RECT_WIDTH	200
#define RECT_HEIGHT	200

const char* movie_file_name = "C:\\svn\\aviq\\trunk\\tipc-du\\www\\featuredcontent\\Future.Air.Power.HDTV.avi";
//const char* movie_file_name = "C:\\cvs\\lrun-doc\\moew\\vladi\\girl.avi";
//const char* movie_file_name = "C:\\alek.avi";

dg_widget_p widget1;
dg_movie_reader_p movie_reader=0;
dg_movie_p movie;

static int plugin_enum_callback(dg_movie_reader_p* movie_reader, dg_plugin_info_p plugin_info)
{
	*movie_reader=plugin_info->movie_reader;

	/* movie reader found, stop plugin enumeration */
	return 1;
}

int main()
{
	int x=RECT_WIDTH-10, y=RECT_HEIGHT-10;
	dg_video_p device = dg_video_create(sizeof(dg_widget_t), XRES, YRES, 32, DG_OPT_FULLSCREEN | DG_OPT_DOUBLE_BUFFERING);

	device->master_widget->layout_manager=dg_layout_anchor_manager;

	dg_plugin_enum(PLUGIN_MOVIE_READER, plugin_enum_callback, &movie_reader);
	if (movie_reader)
	{
		movie = (dg_movie_p)movie_reader->open_movie_file((struct dg_video_t*)device, movie_file_name);
	}

//	widget1 = device->create_widget(device, sizeof(dg_widget_t), 0, 0,movie?movie->width:RECT_WIDTH, movie?movie->height:RECT_HEIGHT);
	widget1 = device->create_widget(device, sizeof(dg_widget_t), 0, 0, XRES, YRES);
	widget1->layout_properties=dg_layout_anchor_properties_create(DG_LAYOUT_ANCHOR_ALL);
	widget1->render=default_widget_render;
	widget1->render(widget1);
	widget1->invalidate(widget1, 0);

	do 
	{
		if (movie)
		{
			movie_reader->read_next_frame((struct dg_movie_t*)movie);
			widget1->surface->blit(widget1->surface, 0, movie->surface, 0, 0, 0, 0, 0);
		}
		widget1->invalidate(widget1, 0);
		swap_buffers(device);
		device->master_widget->event_manager->idle(device->master_widget->event_manager);
	} while (device->master_widget->event_manager->active);

	movie_reader->close_movie((struct dg_movie_t*)movie);

	device->destroy(device);
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
