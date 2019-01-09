/*
	dg_sample_dialog_1.c
	Test widget library - Dialogs
*/

#include <dg/dg_debug.h>
#include <dg/dgw_form.h>
#include <dg/dgw_label.h>
#include <dg/dgw_style.h>

dgw_form_p main_form;

dgw_label_p create_label(dg_widget_p parent, const char* caption, int x, int y, int w, int h)
{
	dgw_label_p label=dgw_label_create(parent, x, y, w, h, caption);
	DGWidgetLayoutAnchorProps(label, DG_LAYOUT_ANCHOR_LEFT_TOP);
	DGWidgetWindow(label)->owner_window=1;
	DGWidgetRender(label);
	DGWidgetInvalidate(label, 0);
	return label;
}

dgw_form_p create_dialog(dg_widget_p parent, const char* caption, int x, int y, int w, int h, int is_owner)
{
	dgw_form_p dialog=dgw_form_create(parent?parent:(dg_widget_p)main_form, x, y, w, h, caption);
	DGWidgetLayoutAnchorProps(dialog, DG_LAYOUT_ANCHOR_LEFT_TOP);
	DGWidgetWindow(dialog)->owner_window=is_owner;
	DGWidgetRender(dialog);
	DGWidgetInvalidate(dialog, 0);
	return dialog;
}

int main()
{
	dgw_form_p dialog;

#if defined(DEBUG) && defined(_WIN32)
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	main_form=dgw_form_create(0, 0, 0, 800, 600, "Sample Window");
	DGWidgetLayoutAnchor(main_form);

	dialog=create_dialog((dg_widget_p)main_form, "Sample Dialog 0 (clipped)", 100, 20, 700, 540, 0);
	DGWidgetWindow(dialog)->clip_children=1;

	create_label((dg_widget_p)create_dialog((dg_widget_p)main_form, "Sample Dialog 3", 20, 400, 320, 240, 1), "This is a simple label", 5, 0, 100, 30);

	create_dialog((dg_widget_p)dialog, "Sample Dialog 0-0", 10, 20, 150, 150, 0);
	create_dialog((dg_widget_p)dialog, "Sample Dialog 0-1", 15, 30, 150, 150, 0);
	create_dialog((dg_widget_p)dialog, "Sample Dialog 0-2", 20, 40, 150, 150, 0);

	dialog=create_dialog((dg_widget_p)dialog, "Sample Dialog 1", 180, 80, 250, 200, 0);
	create_dialog((dg_widget_p)dialog, "Sample Dialog 1-0", 180+10, 80+20, 150, 150, 0);
	create_dialog((dg_widget_p)dialog, "Sample Dialog 1-1", 180+15, 80+30, 150, 150, 0);
	create_dialog((dg_widget_p)dialog, "Sample Dialog 1-2", 180+20, 80+40, 150, 150, 0);

	dialog=create_dialog((dg_widget_p)dialog, "Sample Dialog 2", 80, 180, 300, 250, 0);
	create_dialog((dg_widget_p)dialog, "Sample Dialog 2-0 (owner)", 10, 20, 200, 150, 1);
	create_dialog((dg_widget_p)dialog, "Sample Dialog 2-1 (owner)", 20, 30, 200, 150, 1);
	create_dialog((dg_widget_p)dialog, "Sample Dialog 2-2 (owner)", 30, 40, 200, 150, 1);

	DGWidgetFlip(main_form, 0);
	DGWidgetEventLoop(main_form);
	DGWidgetDestroy(main_form);
	return 0;
}
