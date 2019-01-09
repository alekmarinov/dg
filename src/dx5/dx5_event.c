/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_event.c                                        */
/* Description:   DG/DirectX Event Management                        */
/*                                                                   */
/*********************************************************************/

#include <windows.h>
#include <dg/dg_keys.h>
#include <dg/dg_video.h>
#include "dx5_event.h"

static dg_key_t _key_table[DG_KEY_CODES];

static void dx5_init_keytable()
{
	memset(_key_table, 0, DG_KEY_CODES * sizeof(dg_key_t));

	_key_table[VK_BACK]=KEY_Backspace;
	_key_table[VK_TAB]=KEY_Tab;
	_key_table[VK_CLEAR]=KEY_Clear;
	_key_table[VK_RETURN]=KEY_Return;
	_key_table[VK_MENU]=KEY_Menu;
	_key_table[VK_PAUSE]=KEY_Pause;
	_key_table[VK_ESCAPE]=KEY_Escape;
	_key_table[VK_SPACE]=KEY_Space;
	_key_table[VK_PRIOR]=KEY_Page_Up;
	_key_table[VK_NEXT]=KEY_Page_Down;
	_key_table[VK_END]=KEY_End;
	_key_table[VK_HOME]=KEY_Home;
	_key_table[VK_LEFT]=KEY_Left;
	_key_table[VK_UP]=KEY_Up;
	_key_table[VK_RIGHT]=KEY_Right;
	_key_table[VK_DOWN]=KEY_Down;
	_key_table[VK_SELECT]=KEY_Select;
	_key_table[VK_PRINT]=KEY_Print;
	_key_table[VK_INSERT]=KEY_Insert;
	_key_table[VK_DELETE]=KEY_Delete;
	_key_table[VK_HELP]=KEY_Help;
	_key_table[VK_NUMPAD0]=KEY_0;
	_key_table[VK_NUMPAD1]=KEY_1;
	_key_table[VK_NUMPAD2]=KEY_2;
	_key_table[VK_NUMPAD3]=KEY_3;
	_key_table[VK_NUMPAD4]=KEY_4;
	_key_table[VK_NUMPAD5]=KEY_5;
	_key_table[VK_NUMPAD6]=KEY_6;
	_key_table[VK_NUMPAD7]=KEY_7;
	_key_table[VK_NUMPAD8]=KEY_8;
	_key_table[VK_NUMPAD9]=KEY_9;
	_key_table[VK_MULTIPLY]=KEY_Asterisk;
	_key_table[VK_ADD]=KEY_Plus;
	_key_table[VK_SUBTRACT]=KEY_Minus;
	_key_table[VK_DIVIDE]=KEY_Slash;
	_key_table[VK_F1]=KEY_F1;
	_key_table[VK_F2]=KEY_F2;
	_key_table[VK_F3]=KEY_F3;
	_key_table[VK_F4]=KEY_F4;
	_key_table[VK_F5]=KEY_F5;
	_key_table[VK_F6]=KEY_F6;
	_key_table[VK_F7]=KEY_F7;
	_key_table[VK_F8]=KEY_F8;
	_key_table[VK_F9]=KEY_F9;
	_key_table[VK_F10]=KEY_F10;
	_key_table[VK_F11]=KEY_F11;
	_key_table[VK_F12]=KEY_F12;

#if (_WIN32_WINNT >= 0x0500)
	_key_table[VK_VOLUME_MUTE]=KEY_Mute;
	_key_table[VK_VOLUME_DOWN]=KEY_Volume_Down;
	_key_table[VK_VOLUME_UP]=KEY_Volume_Up;
#endif
	_key_table[VK_PLAY]=KEY_Playpause;
	_key_table[VK_ZOOM]=KEY_Zoom;
}

static int dx5_event_peek(dg_event_manager_p self, dg_event_p event)
{
    MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_CHAR)
		{
			event->type = EVT_TYPE_KEYBOARD;
			event->e.keyboard.key = (int)msg.wParam;
			event->e.keyboard.modifiers = 0;
			event->e.keyboard.key_event = EVT_PRESSED;

			return 1;
		}
		else
		if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)
		{
			int code=(int)msg.wParam;
			if (_key_table[code])
			{
				code=_key_table[code];

				event->type = EVT_TYPE_KEYBOARD;
				event->e.keyboard.key = code;
				event->e.keyboard.modifiers = 0;
				event->e.keyboard.key_event = (msg.message == WM_KEYDOWN)?EVT_PRESSED:EVT_RELEASED;

				PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

				return 1;
			}
		}
		else
		if (msg.message == WM_TIMER)
		{
			event->type = EVT_TYPE_TIMER;
			event->e.custom.n0=event->e.custom.n1=event->e.custom.n2=event->e.custom.n3=0;
			return 1;
		}
		else
		if (msg.message == WM_QUIT)
		{
			event->type = EVT_TYPE_QUIT;
			return 1;
		}
	}
	return 0;
}

dg_event_manager_p dx5_create_event_manager(dg_video_p device)
{
	dg_event_manager_p self = dg_event_manager_master_create();
	self->peek = dx5_event_peek;
	dx5_init_keytable();
	return self;
}
