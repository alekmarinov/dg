/*
	dg_sample_dialog_1.c
	Test widget library - Child Window with dialog skin
*/


#include <dg/dgw_form.h>
#include <dg/dgw_scrollbar.h>
#include <dg/dgw_style.h>

dgw_form_p main_form;
dgw_scrollbar_p sb_h;
dgw_scrollbar_p sb_v;

int main()
{
	dgw_set_style(DGW_STYLE_WINXP);
	main_form=dgw_form_create(0, 0, 0, 640, 480, "Sample Window");
	DGWidgetLayoutAnchor(main_form);

	sb_v=dgw_scrollbar_create(DGWidget(main_form), SCROLLBAR_VERTICAL);
	DGWidgetLayoutAnchorProps(sb_v, DG_LAYOUT_ANCHOR_TOP_BOTTOM_RIGHT);
	DGWidgetRender(sb_v);
	DGWidgetInvalidate(sb_v, 0);

	sb_h=dgw_scrollbar_create(DGWidget(main_form), SCROLLBAR_HORIZONTAL);
	DGWidgetLayoutAnchorProps(sb_h, DG_LAYOUT_ANCHOR_LEFT_RIGHT_BOTTOM);
	DGWidgetRender(sb_h);
	DGWidgetInvalidate(sb_h, 0);

	DGWidgetFlip(main_form, 0);
	DGWidgetEventLoop(main_form);
	DGWidgetDestroy(main_form);
	return 0;
}
