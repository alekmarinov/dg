/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_utf8.h                                          */
/* Description:   UTF-8 string API                                   */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_UTF8_H
#define __DG_UTF8_H

#include <dg/dg.h>

DG_API int utf8_strlen(const char*);
DG_API char* utf8_strpos(const char*, int);
DG_API int utf8_strchar(const char*, int, char*);
DG_API unsigned int utf8_encode(unsigned int, char*);
DG_API int utf8_decode(const unsigned char*, unsigned int *);
DG_API int utf8_strcpy(char*, const char*, int);

#endif
