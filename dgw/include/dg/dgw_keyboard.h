/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dgw                                                */
/* Filename:      dgw_keyboard.h                                     */
/* Description:   Button widget                                      */
/*                                                                   */
/*********************************************************************/

#ifndef __DGW_KEYBOARD_H
#define __DGW_KEYBOARD_H

#include <dg/dg_widget.h>
#include <dg/dgw_edit.h>

enum
{
	KEYB_BACKSPACE = 0,
	KEYB_SHIFT,
	KEYB_LANGUAGE,
	KEYB_SPACE,
	KEYB_ENTER,
	KEYB_ESC,
	KEYB_COUNT
};

typedef enum
{
	KEYBOARD_ENGLISH = 0,
	KEYBOARD_CYRILLIC,
	KEYBOARD_LANGUAGE_COUNT
} dg_keyboard_language_t;

typedef struct _dgw_key_t
{
	struct _dg_widget_t widget;
	char   text[9];
	int    key;
	int    is_push;
	int    is_selected;
} dgw_key_t, *dgw_key_p;

typedef struct _dgw_keyboard_t
{
	struct _dg_widget_t widget;
	dgw_key_p   *keys;
	int          nkeys;
	char        *text;
	int          options;
	int          caps;
	dgw_edit_p   captured_edit;
	int          hkeys;
	int          vkeys;
	int          key_cursor;
	int          language;

	void (*set_text)(struct _dgw_keyboard_t*, const char* text);
	const char* (*get_text)(struct _dgw_keyboard_t*);
	void (*set_caps)(struct _dgw_keyboard_t*, int);
	int (*is_caps)(struct _dgw_keyboard_t*);
	void (*set_language)(struct _dgw_keyboard_t*, dg_keyboard_language_t);
	dg_keyboard_language_t (*get_language)(struct _dgw_keyboard_t*);
	const char* (*get_language_id)(struct _dgw_keyboard_t*);
	const char* (*get_space_id)(struct _dgw_keyboard_t*);
	const char* (*get_esc_id)(struct _dgw_keyboard_t*);
	void (*set_options)(struct _dgw_keyboard_t*, int);
	int  (*get_options)(struct _dgw_keyboard_t*);
	void (*capture)(struct _dgw_keyboard_t*, dgw_edit_p);
} dgw_keyboard_t, *dgw_keyboard_p;


DG_API dgw_keyboard_p dgw_keyboard_create(dg_widget_p, int, int, int, int);

#endif
