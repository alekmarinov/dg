/*
	dg_sample_button_1.c
	Test widget library - Button
*/


#include <stdio.h>
#include <dg/dgw_form.h>
#include <dg/dgw_label.h>
#include <dg/dgw_button.h>
#include <dg/dgw_style.h>

dgw_form_p main_form;
dgw_form_p dialog;
dgw_label_p label;
dgw_button_p button_ok;
dgw_button_p button_cancel;
dgw_button_p button_yes;
dgw_button_p button_care;

int button_event(dgw_button_p button, dg_event_p event)
{
	if (event->type == EVT_TYPE_WIDGET && event->e.widget.widget_event & EVT_WIDGET_STATUS_CHANGED)
	{
		dg_rect_p rect = dg_rect_create(0, 0, 0, 0);
		char text[80];
		int destroyed = 0;
		if (event->e.widget.p1 == BUTTON_STATUS_PRESSED)
		{
			sprintf(text, "Pressed %s", button->get_text(button));
		}
		else
		{
			sprintf(text, "Released %s", button->get_text(button));
			if (button_care == button)
			{
				dg_video_p device = DGWidgetDevice(button);
				DGWidgetRect(dialog, rect);
				DGWidgetDestroy(dialog);
				DGFlip(device, rect);
				destroyed = 1;
			}
		}
		if (!destroyed)
		{
			label->set_text(label, text);
			DGWidgetRender(label);
			DGWidgetInvalidate(label, 0);
			DGWidgetRect(label, rect);
			DGWidgetFlip(label, rect);
		}
		rect->destroy(rect);
		return 1;
	}
	return 0;
}

int main()
{
	main_form = dgw_form_create(0, 0, 0, 640, 480, "Simple Label");
	dialog = dgw_form_create(DGWidget(main_form), 20, 50, 540, 180, "Dialog");
	DGWidgetRender(dialog);
	DGWidgetLayoutAnchor(dialog);

	label = dgw_label_create(DGWidget(dialog), 30, 20, 500, 30, "WARNING, ALL DATA ON NON-REMOVABLE DISK DRIVE C: WILL BE LOST! Proceed with Format (Y/N)?");
	DGWidgetRender(label);

	label = dgw_label_create(DGWidget(dialog), 40, 40, 200, 30, "");
	DGWidgetRender(label);

	button_ok=dgw_button_create(DGWidget(dialog), 10, 130, 100, 20, "OK");
	DGWidgetLayoutAnchorProps(button_ok, DG_LAYOUT_ANCHOR_RIGHT_BOTTOM);
	DGWidget(button_ok)->tab_index = 1;
	DGWidgetSetFocus(button_ok);
	DGWidgetRender(button_ok);
	DGWidgetAddListener(button_ok, button_event, EVT_TYPE_WIDGET, button_ok);
	
	button_cancel=dgw_button_create(DGWidget(dialog), 120, 130, 100, 20, "Yes");
	DGWidgetLayoutAnchorProps(button_cancel, DG_LAYOUT_ANCHOR_RIGHT_BOTTOM);
	DGWidget(button_cancel)->tab_index = 2;
	DGWidgetRender(button_cancel);
	DGWidgetAddListener(button_cancel, button_event, EVT_TYPE_WIDGET, button_cancel);

	button_yes=dgw_button_create(DGWidget(dialog), 230, 130, 100, 20, "Format");
	DGWidgetLayoutAnchorProps(button_yes, DG_LAYOUT_ANCHOR_RIGHT_BOTTOM);
	DGWidget(button_yes)->tab_index = 3;
	DGWidgetRender(button_yes);
	DGWidgetAddListener(button_yes, button_event, EVT_TYPE_WIDGET, button_yes);

	button_care=dgw_button_create(DGWidget(dialog), 340, 130, 100, 20, "Don't care");
	DGWidgetLayoutAnchorProps(button_care, DG_LAYOUT_ANCHOR_RIGHT_BOTTOM);
	DGWidget(button_care)->tab_index = 4;
	DGWidgetRender(button_care);
	DGWidgetAddListener(button_care, button_event, EVT_TYPE_WIDGET, button_care);

	DGWidgetInvalidate(dialog, 0);
	DGWidgetFlip(main_form, 0);
	DGWidgetEventLoop(main_form);
	DGWidgetDestroy(main_form);
	return 0;
}
