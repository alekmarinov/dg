/*
	dg_sample_label_1.c
	Test widget library - Label
*/


#include <dg/dgw_form.h>
#include <dg/dgw_label.h>
#include <dg/dgw_style.h>

dgw_form_p main_form;
dgw_label_p label;

int main()
{
	main_form=dgw_form_create(0, 0, 0, 640, 480, "Simple Label");
	DGWidgetLayoutAnchor(main_form);

	label=dgw_label_create((dg_widget_p)main_form, 530, 20, 100, 20, "Hello, Label!");
	DGWidgetLayoutAnchorProps(label, DG_LAYOUT_ANCHOR_RIGHT_TOP);

	DGWidgetRender(main_form);
	DGWidgetRender(label);
	DGWidgetInvalidate(label, 0);

	DGWidgetFlip(main_form, 0);
	DGWidgetEventLoop(main_form);
	DGWidgetDestroy(main_form);
	return 0;
}
