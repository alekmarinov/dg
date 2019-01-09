/*
	dgw_sample_animation_1.c
	Test widget library - animation
*/


#include <malloc.h>
#include <dg/dgw_form.h>
#include <dg/dgw_style.h>

#define BACKGROUND "c:\\temp\\demo-dg-1\\background.jpg"

dgw_form_p main_form;
dgw_form_p dialog;

int animation_event(dgw_form_p dialog, dg_event_p event)
{
	dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
	float aleph = (float)event->e.custom.n0 / 1000;
	DGWidget(dialog)->surface->set_alpha(DGWidget(dialog)->surface, (char)((int)((float)255*aleph) & 255));
	DGWidgetInvalidate(dialog, 0);
	DGWidgetRect(dialog, rect);
	DGWidgetFlip(dialog, rect);
	rect->destroy(rect);
	if (aleph == 1.0f) 
	{
		return 0;
	}
	return 1;
}

int main()
{
	dg_surface_p background_image;

	main_form = dgw_form_create(0, 0, 0, 800, 600, "Simple Label");
	DGWidgetDevice(main_form)->set_background_color(DGWidgetDevice(main_form), DG_RGB_COLOR(255, 139, 0, 255));
	background_image = DGWidgetDevice(main_form)->load_surface_file(DGWidgetDevice(main_form), BACKGROUND);
	DGWidgetDevice(main_form)->set_background_image(DGWidgetDevice(main_form), background_image);
	dialog = dgw_form_create((dg_widget_p)main_form, 20, 50, 540, 180, "Dialog");
	DGWidgetDevice(main_form)->master_widget->event_manager->add_animation_listener(DGWidgetDevice(main_form)->master_widget->event_manager, animation_event, dialog, 1000);
	DGWidgetRender(dialog);
	DGWidgetInvalidate(dialog, 0);
	DGWidgetFlip(main_form, 0);
	DGWidgetEventLoop(main_form);
	DGWidgetDestroy(main_form);
	return 0;
}
