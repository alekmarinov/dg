	/*
	dg_sample_keyboard_1.c
	Test widget library - Keyboard
*/


#include <dg/dgw_form.h>
#include <dg/dgw_keyboard.h>
#include <dg/dgw_style.h>

dgw_form_p main_form;
dgw_keyboard_p keyboard;

//#define BACKGROUND "c:\\cvs\\lrun-art\\projects\\aviq\\a1\\image\\backgroundRudy.jpg"
#define BACKGROUND "c:\\cvs\\lrun-art\\projects\\aviq\\a1\\image\\flower_background.png"

int main()
{
	dg_surface_p background_image;
	main_form=dgw_form_create(0, 0, 0, 640, 480, "Simple keyboard");
	DGWidgetDevice(main_form)->set_background_color(DGWidgetDevice(main_form), DG_RGB_COLOR(255, 139, 0, 255));
	background_image = DGWidgetDevice(main_form)->load_surface_file(DGWidgetDevice(main_form), BACKGROUND);
	DGWidgetDevice(main_form)->set_background_image(DGWidgetDevice(main_form), background_image);
	DGWidgetLayoutAnchor(main_form);

	keyboard = dgw_keyboard_create((dg_widget_p)main_form, 50, 50, 500, 160);

	DGWidgetRender(main_form);
	DGWidgetRender(keyboard);
	DGWidgetInvalidate(keyboard, 0);

	DGWidgetFlip(main_form, 0);
	DGWidgetEventLoop(main_form);
	DGWidgetDestroy(main_form);
	return 0;
}

