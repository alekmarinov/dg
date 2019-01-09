/*
	dg_sample_widget_7.c
	Tests widget reparenting
*/

#include "dg_sample_shell.h"

#define RECT_WIDTH	200
#define RECT_HEIGHT	200

dg_widget_p widget1;
dg_widget_p widget2;
dg_widget_p child;
dg_widget_p child2;
int flip_flop=1;

int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event)
	{
		int x, y;
		/* init widget 1 */
		x = 0;//rand()%(device->xres-RECT_WIDTH);
		y = 0;//rand()%(device->yres-RECT_WIDTH);
		widget1 = device->create_widget(device, sizeof(dg_widget_t), x, y, RECT_WIDTH, RECT_HEIGHT);
		widget1->render=default_widget_render;
		widget1->window->clip_children=1;
		widget1->render(widget1);

		/* init child widget 1 */
		child = widget1->create(widget1, sizeof(dg_widget_t), RECT_WIDTH-10, RECT_HEIGHT-10, RECT_WIDTH/2, RECT_HEIGHT/2);
		child->render=default_widget_render;
		child->render(child);

		///* init child widget 2 */
		//child2=child->create(child, RECT_WIDTH/8, RECT_HEIGHT/8, RECT_WIDTH/4, RECT_HEIGHT/4);
		//child2->render=default_widget_render;
		//child2->render(child2);

		/* init widget 2 */
		x = rand()%(device->xres-RECT_WIDTH);
		y = rand()%(device->yres-RECT_WIDTH);

		//widget2 = device->create_widget(device, x, y, RECT_WIDTH, RECT_HEIGHT);
		//widget2->render=default_widget_render;
		//widget2->render(widget2);
		//widget2->invalidate(widget2, 0);

		widget1->invalidate(widget1, 0);

		swap_buffers(device);
		return 1;
	}
	else if (widget1 && widget2 && event->type == EVT_TYPE_MOUSE)
	{
		if (event->e.mouse.mouse_event & EVT_MOUSE_LBUTTON && event->e.mouse.mouse_event & EVT_PRESSED)
		{
			child->set_parent(child, flip_flop?widget2:widget1);
			flip_flop=!flip_flop;

			widget1->invalidate(widget1, 0);
			widget2->invalidate(widget2, 0);
			swap_buffers(device);
		}
		return 1;
	}
	return 0;
}

int main()
{
	int x=RECT_WIDTH-10, y=RECT_HEIGHT-10;
	dg_video_p device = dg_video_create(sizeof(dg_widget_t), 640, 480, 32, DG_OPT_DOUBLE_BUFFERING);
	event_handler(device, 0);
	device->master_widget->event_manager->add_listener(device->master_widget->event_manager, event_handler, EVT_TYPE_KEYBOARD | EVT_TYPE_MOUSE | EVT_TYPE_TIMER, device);
	do 
	{
		child->set_rect(child, (++x)%RECT_WIDTH, (++y)%RECT_HEIGHT, RECT_WIDTH/2, RECT_HEIGHT/2, 1);
		//child->set_rect(child, (55+x)%RECT_WIDTH, (45+y)%RECT_HEIGHT, RECT_WIDTH/2, RECT_HEIGHT/2, 1);
		swap_buffers(device);
		device->master_widget->event_manager->idle(device->master_widget->event_manager);
	} while (device->master_widget->event_manager->active);

	device->destroy(device);
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
