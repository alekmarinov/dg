/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      ft_utf8.h                                          */
/* Description:   UTF8 conversion utility                            */
/*                                                                   */
/*********************************************************************/

#ifndef __FT_UTF8_H
#define __FT_UTF8_H

#define UTF8_CHAR_SIZE(c)     (((unsigned char)(c) < 0xC0) ? 1 : utf8_char_sizes[(unsigned char)(c) & 0x3F])
#define UTF8_CHAR(p)          (*(const unsigned char*)(p) < 0xC0 ? \
                               *(const unsigned char*)(p) : utf8_get_char((const unsigned char*)(p)))

/* array of utf8 char sizes */
extern const char utf8_char_sizes[0x40];

/* convert utf8 char to unicode */
unsigned int utf8_get_char(const unsigned char *);

#endif
