/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_keys.h                                          */
/* Description:   DG key definitions                                 */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_KEYS_H
#define __DG_KEYS_H

/*!
	\addtogroup dg_keys Key Codes
	<b>This page is under construction</b>
  *  @{
*/

typedef enum
{
	KEY_Backspace=8,          /* 0x08   */
	KEY_Tab,                  /* 0x09   */
	KEY_Return=13,            /* 0x0D   */
	KEY_Escape=27,            /* 0x1B   */
	KEY_Space=32,             /* 0x20   */
	KEY_Exclam,               /* 0x21 ! */
	KEY_Quotedbl,             /* 0x22 " */
	KEY_Hash,                 /* 0x23 # */
	KEY_Dollar,               /* 0x24 $ */
	KEY_Percent,              /* 0x25 % */
	KEY_Ampersand,            /* 0x26 & */
	KEY_Quote,                /* 0x27 ' */
	KEY_Parenleft,            /* 0x28 ( */
	KEY_Parenright,           /* 0x29 ) */
	KEY_Asterisk,             /* 0x2A * */
	KEY_Plus,                 /* 0x2B + */
	KEY_Comma,                /* 0x2C , */
	KEY_Minus,                /* 0x2D - */
	KEY_Period,               /* 0x2E . */
	KEY_Slash,                /* 0x2F / */
	KEY_0=48,                 /* 0x30 0 */
	KEY_1,                    /* 0x31 1 */
	KEY_2,                    /* 0x32 2 */
	KEY_3,                    /* 0x33 3 */
	KEY_4,                    /* 0x34 4 */
	KEY_5,                    /* 0x35 5 */
	KEY_6,                    /* 0x36 6 */
	KEY_7,                    /* 0x37 7 */
	KEY_8,                    /* 0x38 8 */
	KEY_9,                    /* 0x39 9 */
	KEY_Colon,                /* 0x3A : */
	KEY_Semicolon,            /* 0x3B ; */
	KEY_Less,                 /* 0x3C < */
	KEY_Equal,                /* 0x3D = */
	KEY_Greater,              /* 0x3E > */
	KEY_Question,             /* 0x3F ? */
	KEY_At,                   /* 0x40 @ */
	KEY_A,                    /* 0x41 A */
	KEY_B,                    /* 0x42 B */
	KEY_C,                    /* 0x43 C */
	KEY_D,                    /* 0x44 D */
	KEY_E,                    /* 0x45 E */
	KEY_F,                    /* 0x46 F */
	KEY_G,                    /* 0x47 G */
	KEY_H,                    /* 0x48 H */
	KEY_I,                    /* 0x49 I */
	KEY_J,                    /* 0x4A J */
	KEY_K,                    /* 0x4B K */
	KEY_L,                    /* 0x4C L */
	KEY_M,                    /* 0x4D M */
	KEY_N,                    /* 0x4E N */
	KEY_O,                    /* 0x4F O */
	KEY_P,                    /* 0x50 P */
	KEY_Q,                    /* 0x51 Q */
	KEY_R,                    /* 0x52 R */
	KEY_S,                    /* 0x53 S */
	KEY_T,                    /* 0x54 T */
	KEY_U,                    /* 0x55 U */
	KEY_V,                    /* 0x56 V */
	KEY_W,                    /* 0x57 W */
	KEY_X,                    /* 0x58 X */
	KEY_Y,                    /* 0x59 Y */
	KEY_Z,                    /* 0x5A Z */
	KEY_Bracketleft,          /* 0x5B [ */
	KEY_Backslash,            /* 0x5C \ */
	KEY_Bracketright,         /* 0x5D ] */
	KEY_Caret,                /* 0x5E ^ */
	KEY_Underscore,           /* 0x5F _ */
	KEY_Backquote,            /* 0x60 ` */
	KEY_a,                    /* 0x61 a */
	KEY_b,                    /* 0x62 b */
	KEY_c,                    /* 0x63 c */
	KEY_d,                    /* 0x64 d */
	KEY_e,                    /* 0x65 e */
	KEY_f,                    /* 0x66 f */
	KEY_g,                    /* 0x67 g */
	KEY_h,                    /* 0x68 h */
	KEY_i,                    /* 0x69 i */
	KEY_j,                    /* 0x6A j */
	KEY_k,                    /* 0x6B k */
	KEY_l,                    /* 0x6C l */
	KEY_m,                    /* 0x6D m */
	KEY_n,                    /* 0x6E n */
	KEY_o,                    /* 0x6F o */
	KEY_p,                    /* 0x70 p */
	KEY_q,                    /* 0x71 q */
	KEY_r,                    /* 0x72 r */
	KEY_s,                    /* 0x73 s */
	KEY_t,                    /* 0x74 t */
	KEY_u,                    /* 0x75 u */
	KEY_v,                    /* 0x76 v */
	KEY_w,                    /* 0x77 w */
	KEY_x,                    /* 0x78 x */
	KEY_y,                    /* 0x79 y */
	KEY_z,                    /* 0x7A z */
	KEY_Braceleft,            /* 0x7B { */
	KEY_Bar,                  /* 0x7C | */
	KEY_Braceright,           /* 0x7D } */
	KEY_Asciitilde,           /* 0x7E ~ */
	KEY_Delete,               /* 0x7F   */

	KEY_Left,
	KEY_Right,
	KEY_Up,
	KEY_Down,
	KEY_Insert,
	KEY_Home,
	KEY_End,
	KEY_Page_Up,
	KEY_Page_Down,
	KEY_Print,
	KEY_Pause,
	KEY_Ok,
	KEY_Select,
	KEY_Goto,
	KEY_Clear,
	KEY_Power,
	KEY_Power2,
	KEY_Option,
	KEY_Menu,
	KEY_Help,
	KEY_Info,
	KEY_Time,
	KEY_Vendor,
	KEY_Album,
	KEY_Program,
	KEY_Channel,
	KEY_Favorites,
	KEY_EPG,
	KEY_PVR,
	KEY_MHP,
	KEY_Language,
	KEY_Title,
	KEY_Subtitle,
	KEY_Angle,
	KEY_Zoom,
	KEY_Mode,
	KEY_Keyboard,
	KEY_Pc,
	KEY_Screen,
	KEY_Tv,
	KEY_Tv2,
	KEY_Vcr2,
	KEY_Sat,
	KEY_Sat2,
	KEY_CD,
	KEY_Tape,
	KEY_Radio,
	KEY_Tuner,
	KEY_Player,
	KEY_Text,
	KEY_DVD,
	KEY_Aux,
	KEY_MP3,
	KEY_Phone,
	KEY_Audio,
	KEY_Video,
	KEY_Internet,
	KEY_List,
	KEY_Green,
	KEY_Yellow,
	KEY_Channel_Up,
	KEY_Channel_Down,
	KEY_Back,
	KEY_Forward,
	KEY_First,
	KEY_Last,
	KEY_Volume_Up,
	KEY_Volume_Down,
	KEY_Mute,
	KEY_AB,
	KEY_Playpause,
	KEY_PLAY,
	KEY_Stop,
	KEY_Restart,
	KEY_Slow,
	KEY_Fast,
	KEY_Record,
	KEY_Eject,
	KEY_Shuffle,
	KEY_Rewind,
	KEY_Fastforward,
	KEY_Previous,
	KEY_Next,
	KEY_Begin,
	KEY_Digits,
	KEY_Teen,
	KEY_Twen,
	KEY_Break,
	KEY_Exit,
	KEY_Setup,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12
} dg_key_t;

#define DG_KEY_CODES 256

/*! @} */

#endif /* __DG_KEYS_H */
