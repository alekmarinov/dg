/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_utf8.c                                          */
/* Description:   UTF-8 string API                                   */
/*                                                                   */
/*********************************************************************/

#include <string.h>
#include <dg/dg_utf8.h>

static const char utf8_char_sizes[64] =
{
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,1,1,1,1,1,1,1,1
};

int utf8_strlen(const char* p)
{
	register int len = 0;
	while (*p)
	{
		if ((*p & 0xFF) < 0x80)
			p++;
		else
			p += utf8_char_sizes[(*p) & 0x3F];
		len++;
	}
	return len;
}

char* utf8_strpos(const char* p, int position)
{
	register int len = 0;
	while ((*p) && (len++ < position))
	{
		if ((*p & 0xFF) < 0x80)
			p++;
		else
			p += utf8_char_sizes[(*p) & 0x3F];
	}
	return (char*)p;
}

int utf8_strchar(const char* p, int position, char* utf8_char)
{
	int i, utf8_size;
	char* utf8_str = utf8_strpos(p, position);
	utf8_size = ((*utf8_str & 0xFF) >= 0x80)?utf8_char_sizes[(*utf8_str) & 0x3F]:1;
	for (i=0; i<utf8_size; i++)
	{
		*utf8_char++ = *utf8_str++;
	}
	return utf8_size;
}

unsigned int utf8_encode(unsigned int unichar, char* utf8_char)
{
	if (unichar < 0x80)
	{
		utf8_char[0] = (char)unichar;
		return 1;
	}
	else if (unichar < 0x0800)
	{
		utf8_char[0] = 0xC0 | ((unichar >> 6) & 0x1F);
		utf8_char[1] = 0x80 | (unichar & 0x3F);
		return 2;
	}
	else if (unichar < 0x010000)
	{
		utf8_char[0] = 0xE0 | ((unichar >> 12) & 0xF);
		utf8_char[1] = 0x80 | ((unichar >> 6) & 0x3F);
		utf8_char[2] = 0x80 | (unichar & 0x3F);
		return 3;
	}
	else if (unichar < 0x110000)
	{
		utf8_char[0] = 0xF0 | ((unichar >> 18) & 0x7);
		utf8_char[1] = 0x80 | ((unichar >> 12) & 0x3F);
		utf8_char[2] = 0x80 | ((unichar >> 6) & 0x3F);
		utf8_char[3] = 0x80 | (unichar & 0x3F);
		return 4;
	}
	return 0;
}

int utf8_decode(const unsigned char* p, register unsigned int *result)
{
	int len, ilen;
	*result = (unsigned int)(p[0] & 0xFF);

	if ((*result) < 0xC0)
		return 1;

	if ((*result) > 0xFD)
		return (unsigned int) -1;

	len = ilen = utf8_char_sizes[(*result) & 0x3F];

	*result &= 0x7C >> ilen;

	while (--ilen) 
	{
		int c = *(++p);

		if ((c & 0xC0) != 0x80)
			return (unsigned int) -1;

		*result = ((*result) << 6) | (c & 0x3F);
	}

	return len;
}

int utf8_strcpy(char* dst, const char* src, int len)
{
	char* p = utf8_strpos(src, len); 
	int size = (int)(p - src);
	memcpy(dst, src, size);
	dst[size] = 0;
	return size;
}
