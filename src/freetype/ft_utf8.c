/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      ft_utf8.c                                          */
/* Description:   UTF8 conversion utility                            */
/*                                                                   */
/*********************************************************************/

#include "ft_utf8.h"

const char utf8_char_sizes[64] =
{
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

unsigned int utf8_get_char(const unsigned char *p)
{
	int len;
	register unsigned int result = p[0];

	if (result < 0xC0)
		return result;

	if (result > 0xFD)
		return (unsigned int) -1;

	len = utf8_char_sizes[result & 0x3F];

	result &= 0x7C >> len;

	while (--len) 
	{
		int c = *(++p);

		if ((c & 0xC0) != 0x80)
			return (unsigned int) -1;

		result = (result << 6) | (c & 0x3F);
	}

	return result;
}
