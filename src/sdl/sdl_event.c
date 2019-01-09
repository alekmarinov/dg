/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      sdl_event.c                                        */
/* Description:   DG/SDL Event Management                            */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <SDL.h>
#include <dg/dg_debug.h>
#include <dg/dg_keys.h>
#include <dg/dg_video.h>
#include "sdl_event.h"

static dg_key_t _key_table[SDLK_LAST];

static void sdl_init_keytable()
{
	memset(_key_table, 0, SDLK_LAST * sizeof(dg_key_t));

	_key_table[SDLK_BACKSPACE]=KEY_Backspace;
	_key_table[SDLK_TAB]=KEY_Tab;
	_key_table[SDLK_CLEAR]=KEY_Clear;
	_key_table[SDLK_RETURN]=KEY_Return;
	_key_table[SDLK_PAUSE]=KEY_Pause;
	_key_table[SDLK_ESCAPE]=KEY_Escape;
	_key_table[SDLK_SPACE]=KEY_Space;
	_key_table[SDLK_EXCLAIM]=KEY_Exclam;
	_key_table[SDLK_QUOTEDBL]=KEY_Quotedbl;
	_key_table[SDLK_HASH]=KEY_Hash;
	_key_table[SDLK_DOLLAR]=KEY_Dollar;
	_key_table[SDLK_AMPERSAND]=KEY_Ampersand;
	_key_table[SDLK_QUOTE]=KEY_Quote;
	_key_table[SDLK_LEFTPAREN]=KEY_Parenleft;
	_key_table[SDLK_RIGHTPAREN]=KEY_Parenright;
	_key_table[SDLK_ASTERISK]=KEY_Asterisk;
	_key_table[SDLK_PLUS]=KEY_Plus;
	_key_table[SDLK_COMMA]=KEY_Comma;
	_key_table[SDLK_MINUS]=KEY_Minus;
	_key_table[SDLK_PERIOD]=KEY_Period;
	_key_table[SDLK_SLASH]=KEY_Slash;
	_key_table[SDLK_0]=KEY_0;
	_key_table[SDLK_1]=KEY_1;
	_key_table[SDLK_2]=KEY_2;
	_key_table[SDLK_3]=KEY_3;
	_key_table[SDLK_4]=KEY_4;
	_key_table[SDLK_5]=KEY_5;
	_key_table[SDLK_6]=KEY_6;
	_key_table[SDLK_7]=KEY_7;
	_key_table[SDLK_8]=KEY_8;
	_key_table[SDLK_9]=KEY_9;
	_key_table[SDLK_COLON]=KEY_Colon;
	_key_table[SDLK_SEMICOLON]=KEY_Semicolon;
	_key_table[SDLK_LESS]=KEY_Less;
	_key_table[SDLK_EQUALS]=KEY_Equal;
	_key_table[SDLK_GREATER]=KEY_Greater;
	_key_table[SDLK_QUESTION]=KEY_Question;
	_key_table[SDLK_AT]=KEY_At;
	_key_table[SDLK_LEFTBRACKET]=KEY_Bracketleft;
	_key_table[SDLK_BACKSLASH]=KEY_Backslash;
	_key_table[SDLK_RIGHTBRACKET]=KEY_Bracketright;
	_key_table[SDLK_CARET]=KEY_Caret;
	_key_table[SDLK_UNDERSCORE]=KEY_Underscore;
	_key_table[SDLK_BACKQUOTE]=KEY_Backquote;
	_key_table[SDLK_a]=KEY_a;
	_key_table[SDLK_b]=KEY_b;
	_key_table[SDLK_c]=KEY_c;
	_key_table[SDLK_d]=KEY_d;
	_key_table[SDLK_e]=KEY_e;
	_key_table[SDLK_f]=KEY_f;
	_key_table[SDLK_g]=KEY_g;
	_key_table[SDLK_h]=KEY_h;
	_key_table[SDLK_i]=KEY_i;
	_key_table[SDLK_j]=KEY_j;
	_key_table[SDLK_k]=KEY_k;
	_key_table[SDLK_l]=KEY_l;
	_key_table[SDLK_m]=KEY_m;
	_key_table[SDLK_n]=KEY_n;
	_key_table[SDLK_o]=KEY_o;
	_key_table[SDLK_p]=KEY_p;
	_key_table[SDLK_q]=KEY_q;
	_key_table[SDLK_r]=KEY_r;
	_key_table[SDLK_s]=KEY_s;
	_key_table[SDLK_t]=KEY_t;
	_key_table[SDLK_u]=KEY_u;
	_key_table[SDLK_v]=KEY_v;
	_key_table[SDLK_w]=KEY_w;
	_key_table[SDLK_x]=KEY_x;
	_key_table[SDLK_y]=KEY_y;
	_key_table[SDLK_z]=KEY_z;
	_key_table[SDLK_DELETE]=KEY_Delete;
	_key_table[SDLK_KP0]=KEY_0;
	_key_table[SDLK_KP1]=KEY_1;
	_key_table[SDLK_KP2]=KEY_2;
	_key_table[SDLK_KP3]=KEY_3;
	_key_table[SDLK_KP4]=KEY_4;
	_key_table[SDLK_KP5]=KEY_5;
	_key_table[SDLK_KP6]=KEY_6;
	_key_table[SDLK_KP7]=KEY_7;
	_key_table[SDLK_KP8]=KEY_8;
	_key_table[SDLK_KP9]=KEY_9;
	_key_table[SDLK_KP_PERIOD]=KEY_Period;
	_key_table[SDLK_KP_DIVIDE]=KEY_Slash;
	_key_table[SDLK_KP_MULTIPLY]=KEY_Asterisk;
	_key_table[SDLK_KP_MINUS]=KEY_Minus;
	_key_table[SDLK_KP_PLUS]=KEY_Plus;
	_key_table[SDLK_KP_ENTER]=KEY_Return;
	_key_table[SDLK_KP_EQUALS]=KEY_Equal;
	_key_table[SDLK_UP]=KEY_Up;
	_key_table[SDLK_DOWN]=KEY_Down;
	_key_table[SDLK_RIGHT]=KEY_Right;
	_key_table[SDLK_LEFT]=KEY_Left;
	_key_table[SDLK_INSERT]=KEY_Insert;
	_key_table[SDLK_HOME]=KEY_Home;
	_key_table[SDLK_END]=KEY_End;
	_key_table[SDLK_PAGEUP]=KEY_Page_Up;
	_key_table[SDLK_PAGEDOWN]=KEY_Page_Down;
	_key_table[SDLK_F1]=KEY_F1;
	_key_table[SDLK_F2]=KEY_F2;
	_key_table[SDLK_F3]=KEY_F3;
	_key_table[SDLK_F4]=KEY_F4;
	_key_table[SDLK_F5]=KEY_F5;
	_key_table[SDLK_F6]=KEY_F6;
	_key_table[SDLK_F7]=KEY_F7;
	_key_table[SDLK_F8]=KEY_F8;
	_key_table[SDLK_F9]=KEY_F9;
	_key_table[SDLK_F10]=KEY_F10;
	_key_table[SDLK_F11]=KEY_F11;
	_key_table[SDLK_F12]=KEY_F12;
	_key_table[SDLK_MODE]=KEY_Mode;
	_key_table[SDLK_HELP]=KEY_Help;
	_key_table[SDLK_PRINT]=KEY_Print;
	_key_table[SDLK_BREAK]=KEY_Break;
	_key_table[SDLK_MENU]=KEY_Menu;
	_key_table[SDLK_POWER]=KEY_Power;
}

static int sdl_event_peek(dg_event_manager_p self, dg_event_p event)
{
    SDL_Event msg;

	if (SDL_PollEvent(&msg)==1)
	{
		switch (msg.type)
		{
			case SDL_KEYDOWN: 
			case SDL_KEYUP:
			{
				int code=(int)msg.key.keysym.sym;
				if (_key_table[code])
				{
					code=_key_table[code];
				}
				event->type = EVT_TYPE_KEYBOARD;
				event->e.keyboard.key = code;
				event->e.keyboard.modifiers = 0;
				event->e.keyboard.key_event = (msg.type == SDL_KEYDOWN)?EVT_PRESSED:EVT_RELEASED;
				return 1;
			}
			case SDL_MOUSEMOTION:
			{	
				event->type = EVT_TYPE_MOUSE;
				event->e.mouse.x = msg.motion.x;
				event->e.mouse.y = msg.motion.y;
				event->e.mouse.mouse_event = 0;
				return 1;
			}
			break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{	
				event->type = EVT_TYPE_MOUSE;
				event->e.mouse.x = msg.motion.x;
				event->e.mouse.y = msg.motion.y;
				switch (msg.button.button)
				{
					case SDL_BUTTON_LEFT:   event->e.mouse.mouse_event=EVT_MOUSE_LBUTTON; break;
					case SDL_BUTTON_MIDDLE: event->e.mouse.mouse_event=EVT_MOUSE_MBUTTON; break;
					case SDL_BUTTON_RIGHT:  event->e.mouse.mouse_event=EVT_MOUSE_RBUTTON; break;
				}
				event->e.mouse.mouse_event |= ((msg.type==SDL_MOUSEBUTTONDOWN)?EVT_PRESSED:EVT_RELEASED);
				return 1;
			}
			case SDL_QUIT:
			{
				event->type = EVT_TYPE_QUIT;
				return 1;
			}
		/* FIXME: Capture SDL_VIDEOEXPOSE (to redraw the screen) 
		 * and SDL_VIDEORESIZE (to change the video mode) */
		}
	}
	return 0;
}

dg_event_manager_p sdl_create_event_manager(dg_video_p device)
{
	dg_event_manager_p self = dg_event_manager_create();
	self->peek = sdl_event_peek;
	sdl_init_keytable();
	return self;
}
