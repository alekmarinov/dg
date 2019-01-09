/*
	dg_sample_edit_1.c
	Test widget library - Edit
*/


#include <malloc.h>
#include <dg/dgw_form.h>
#include <dg/dgw_label.h>
#include <dg/dgw_button.h>
#include <dg/dgw_edit.h>
#include <dg/dgw_list.h>
#include <dg/dgw_style.h>

#ifdef NDEBUG
#define BACKGROUND "background.jpg"
#else
#define BACKGROUND "c:\\temp\\demo-dg-1\\background.jpg"
#endif
#undef BACKGROUND

dgw_form_p main_form;
dgw_form_p dialog;
dgw_label_p label;
dgw_edit_p edit;
dgw_list_p list;
dgw_button_p button_add;
dgw_button_p button_cancel;

int button_event(dgw_button_p button, dg_event_p event)
{
	if (event->type == EVT_TYPE_WIDGET && event->e.widget.widget_event & EVT_WIDGET_STATUS_CHANGED)
	{
		dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
		int destroyed = 0;
		if (event->e.widget.p1 == BUTTON_STATUS_RELEASED)
		{
			if (button_add == button)
			{
				dgw_list_row_t row;
				row.cells = malloc(1*sizeof(dgw_list_cell_t));
				row.cells[0].text = (char*)edit->get_text(edit);
				row.is_selected = 0;
				row.is_separator = 0;
				list->add_row(list, row);
				free(row.cells);
				DGWidgetRender(list);
				DGWidgetInvalidate(list, 0);
			}
			else if (button_cancel == button)
			{
				dg_video_p device = DGWidgetDevice(dialog);
				DGWidgetRect(dialog, rect);
				DGWidgetDestroy(dialog);
				DGFlip(device, rect);
				destroyed = 1;
			}
		}
		if (!destroyed)
		{
			DGWidgetRect(dialog, rect);
			DGWidgetFlip(dialog, rect);
		}
		rect->destroy(rect);
		return 1;
	}
	return 0;
}

int main()
{
	dgw_list_column_t column;
	dg_surface_p background_image;

	main_form = dgw_form_create(0, 0, 0, 800, 600, "Simple Label");
	DGWidgetDevice(main_form)->set_background_color(DGWidgetDevice(main_form), DG_RGB_COLOR(255, 139, 0, 255));

#ifdef BACKGROUND
	background_image = DGWidgetDevice(main_form)->load_surface_file(DGWidgetDevice(main_form), BACKGROUND);
	DGWidgetDevice(main_form)->set_background_image(DGWidgetDevice(main_form), background_image);
#endif

	dialog = dgw_form_create((dg_widget_p)main_form, 20, 50, 540, 180, "Dialog");
	DGWidgetRender(dialog);
	DGWidgetLayoutAnchor(dialog);

	label = dgw_label_create((dg_widget_p)dialog, 30, 20, 50, 30, "Type text:");
	DGWidgetLayoutAnchorProps(label, DG_LAYOUT_ANCHOR_LEFT_TOP);
	DGWidgetRender(label);

	button_add=dgw_button_create((dg_widget_p)dialog, 10, 130, 100, 20, "Add");
	DGWidgetLayoutAnchorProps(button_add, DG_LAYOUT_ANCHOR_LEFT_BOTTOM);
	DGWidget(button_add)->tab_index = 1;
	DGWidgetRender(button_add);
	DGWidgetAddListener(button_add, button_event, EVT_TYPE_WIDGET, button_add);

	button_cancel=dgw_button_create((dg_widget_p)dialog, 120, 130, 100, 20, "Cancel");
	DGWidgetLayoutAnchorProps(button_cancel, DG_LAYOUT_ANCHOR_LEFT_BOTTOM);
	DGWidget(button_cancel)->tab_index = 2;
	DGWidgetRender(button_cancel);
	DGWidgetAddListener(button_cancel, button_event, EVT_TYPE_WIDGET, button_cancel);

	edit=dgw_edit_create((dg_widget_p)dialog, 80, 20, 100, 30, "");
	DGWidgetLayoutAnchorProps(edit, DG_LAYOUT_ANCHOR_LEFT_TOP);
	DGWidget(edit)->tab_index = 3;
	DGWidgetRender(edit);

	list=dgw_list_create((dg_widget_p)dialog, 250, 10, 250, 120);
	DGWidgetLayoutAnchorProps(list, DG_LAYOUT_ANCHOR_ALL);
	DGWidget(list)->tab_index = 4;
	DGWidgetSetFocus(list);
	column.column_type = LIST_COLUMN_TEXT;
	column.visible = 1;
	column.width = 90;
	list->add_column(list, column);
	DGWidgetRender(list);

	DGWidgetInvalidate(dialog, 0);
	DGWidgetFlip(main_form, 0);

	DGWidgetAnimate(dialog, WIDGET_ANIMATION_FADE_IN, 2000);
	DGWidgetAnimate(label, WIDGET_ANIMATION_FADE_IN, 1000);
	DGWidgetAnimate(edit, WIDGET_ANIMATION_FADE_IN, 1000);
	DGWidgetAnimate(list, WIDGET_ANIMATION_FADE_IN, 1000);
	DGWidgetAnimate(button_add, WIDGET_ANIMATION_FADE_IN, 1000);
	DGWidgetAnimate(button_cancel, WIDGET_ANIMATION_FADE_IN, 1000);

	DGWidgetEventLoop(main_form);
	DGWidgetDestroy(main_form);
	return 0;
}
