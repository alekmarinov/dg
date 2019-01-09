/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_blit.c                                          */
/* Description:   software blits                                     */
/*                                                                   */
/*********************************************************************/

#include <malloc.h>
#include <string.h>
#include <dg/dg_blit.h>

/*
* copy source pixel buffer to destination pixel buffer
*/
void dg_blit_copy(int bitsPerPixel, unsigned char* dst, unsigned char* src, unsigned int width)
{
	switch (bitsPerPixel)
	{
		case 15: 
		case 16: 
			if (width & 1)
			{
				*(unsigned short*)dst=*(unsigned short*)src;
				dst+=2;
				src+=2;
			}
			width>>=1;
		break;
		case 24: 
			if (width & 1)
			{
				*(unsigned short*)dst=*(unsigned short*)src;
				dst+=2;
				src+=2;
				*dst++=*src++;
			}
			width>>=1;
			if (width>0)
			{
				if (width & 1)
				{
					*(unsigned short*)dst=*(unsigned short*)src;
					dst+=2;
					src+=2;
					*dst++=*src++;
				}
			}
			width>>=1;
			width*=3;
		break;
		case 8: 
			while (width & 3)
			{
				*dst++=*src++;
				width--;
			}
			width>>=2;
		break;
	}
	while (width)
	{
		*(unsigned long*)dst=*(unsigned long*)src;
		dst+=4;
		src+=4;
		width--;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 16-bit pixel with transparency
*/
static void dg_blit_color_key_16(unsigned char* dst, unsigned char* src, unsigned int width, unsigned long color_key)
{
	register unsigned int i=width;
	register unsigned short *pdst=(unsigned short *)dst;
	register unsigned short *psrc=(unsigned short *)src;
	register unsigned short color=(unsigned short)color_key;
	while (i--) if (*psrc != color) *psrc++=*pdst++;
}

/*
* copy source pixel buffer to destination pixel buffer
* 24-bit pixel with transparency
*/
static void dg_blit_color_key_24(unsigned char* dst, unsigned char* src, unsigned int width, unsigned long color_key)
{
	register unsigned int i=width;
	while (i--) 
		if ( (*(unsigned long*)src & 0xFFFFFF) != color_key)
		{
			*(unsigned short*)dst = *(unsigned short*)src;
			dst+=2;
			src+=2;
			*dst++=*src++;
		}
		else
		{
			dst+=3;
			src+=3;
		}
}

/*
* copy source pixel buffer to destination pixel buffer
* 32-bit pixel with transparency
*/
static void dg_blit_color_key_32(unsigned long* dst, unsigned long* src, unsigned int width, unsigned long color_key)
{
	register unsigned int i=width;
	while (i--) 
	{
		if ( *src != color_key) *dst=*src;
		src++;
		dst++;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* with transparency
*/
void dg_blit_color_key(int bitsPerPixel, unsigned char* dst, unsigned char* src, unsigned int width, unsigned long color_key)
{
	switch (bitsPerPixel)
	{
	case 15:
	case 16:
		dg_blit_color_key_16(dst, src, width, color_key); 
		break;
	case 24:
		dg_blit_color_key_24(dst, src, width, color_key); 
		break;
	case 32:
		dg_blit_color_key_32((unsigned long*)dst, (unsigned long*)src, width, color_key); 
		break;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 15-bit with alpha factor
*/
static void dg_blit_alpha_15(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha=(unsigned short)alpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		dTemp = *((unsigned short*)dst);
		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x1F;
		dg = (dTemp >> 5) & 0x1F;
		sr = (sTemp >> 10) & 0x1F;
		dr = (dTemp >> 10) & 0x1F;

		*((unsigned short*)dst) = (unsigned short)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 10);

		dst += 2;
		src += 2;
	}
	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		Result = dTemp;
		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x1F;
		dg = (dTemp >> 5) & 0x1F;
		sr = (sTemp >> 10) & 0x1F;
		dr = (dTemp >> 10) & 0x1F;

		Result = Result & 0xFFFF0000;
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 10);

		sb = (sTemp >> 16) & 0x1F;
		db = (dTemp >> 16) & 0x1F;
		sg = (sTemp >> 21) & 0x1F;
		dg = (dTemp >> 21) & 0x1F;
		sr = (sTemp >> 26) & 0x1F;
		dr = (dTemp >> 26) & 0x1F;

		Result = Result & 0xFFFF;
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 10 ) << 16;

		*((unsigned long*)dst) = Result;
		dst += 4;
		src += 4;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 16-bit with alpha factor
*/
static void dg_blit_alpha_16(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha=(unsigned short)alpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		dTemp = *((unsigned short*)dst);
		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x3F;
		dg = (dTemp >> 5) & 0x3F;
		sr = (sTemp >> 11) & 0x1F;
		dr = (dTemp >> 11) & 0x1F;

		*((unsigned short*)dst) = (unsigned short)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 11);

		dst += 2;
		src += 2;
	}
	/* 5-6-5 color format */
	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		Result = dTemp;

		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x3F;
		dg = (dTemp >> 5) & 0x3F;
		sr = (sTemp >> 11) & 0x1F;
		dr = (dTemp >> 11) & 0x1F;

		Result = Result & 0xFFFF0000;
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 11);

		sb = (sTemp >> 16) & 0x1F;
		db = (dTemp >> 16) & 0x1F;
		sg = (sTemp >> 21) & 0x3F;
		dg = (dTemp >> 21) & 0x3F;
		sr = (sTemp >> 27) & 0x1F;
		dr = (dTemp >> 27) & 0x1F;

		Result = Result & 0xFFFF;
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 11 ) << 16;

		*((unsigned long*)dst) = Result;
		dst += 4;
		src += 4;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 24-bit with alpha factor
*/
static void dg_blit_alpha_24(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha)
{
	register unsigned int i = width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha=(unsigned short)alpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		sb = sTemp & 0xFF;
		db = dTemp & 0xFF;
		sg = (sTemp >> 8) & 0xFF;
		dg = (dTemp >> 8) & 0xFF;
		sr = (sTemp >> 16) & 0xFF;
		dr = (dTemp >> 16) & 0xFF;

		Result = (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 8 |
			((wAlpha * (sr - dr) >> 8) + dr) << 16);

		*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
		dst += 2;
		*dst = (unsigned char)((Result >> 16) & 0xFF);
		dst++;
		src+=3;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 32-bit with alpha factor
*/
static void dg_blit_alpha_32(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha)
{
	register unsigned int i=width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha=(unsigned short)alpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		sb = sTemp & 0xFF;
		db = dTemp & 0xFF;
		sg = (sTemp >> 8) & 0xFF;
		dg = (dTemp >> 8) & 0xFF;
		sr = (sTemp >> 16) & 0xFF;
		dr = (dTemp >> 16) & 0xFF;

		Result = (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 8 |
			((wAlpha * (sr - dr) >> 8) + dr) << 16);

		*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
		dst += 2;
		*dst = (unsigned char)((Result >> 16) & 0xFF);
		dst += 2;
		src += 4;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 15-bit with 50% alpha factor
*/
static void dg_blit_alpha_fast_15(unsigned char* dst, unsigned char* src, unsigned int width)
{
	int is_odd_width = 0;
	register unsigned int i;
	register unsigned long sTemp, dTemp;
    unsigned long Result;
	
	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		dTemp = *((unsigned short*)dst);
		*((unsigned short*)dst) = (unsigned short)(((sTemp & 0x7BDE) >> 1) + ((dTemp & 0x7BDE) >> 1));
		dst += 2;
		src += 2;
	}
	
	i = width;
	while (i--)
	{
		sTemp  = *((unsigned long*)src);
		dTemp  = *((unsigned long*)dst);
		Result = dTemp;

		Result &= 0xFFFF0000;
		Result |= ((sTemp & 0x7BDE) >> 1) + ((dTemp & 0x7BDE) >> 1);
		Result &= 0xFFFF;
		Result |= ((sTemp & 0x7BDE0000) >> 1) + ((dTemp & 0x7BDE0000) >> 1);

		*((unsigned long*)dst) = Result;
		dst += 4;
		src += 4;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 16-bit with 50% alpha factor
*/
static void dg_blit_alpha_fast_16(unsigned char* dst, unsigned char* src, unsigned int width)
{
	int is_odd_width = 0;
	register unsigned int i;
	register unsigned long sTemp, dTemp;
    unsigned long Result;
	
	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		dTemp = *((unsigned short*)dst);
		*((unsigned short*)dst) = (unsigned short)(((sTemp & 0xF7DE) >> 1) + ((dTemp & 0xF7DE) >> 1));

		dst += 2;
		src += 2;
	}
	
	i = width;
	while (i--)
	{
		sTemp  = *((unsigned long*)src);
		dTemp  = *((unsigned long*)dst);
		Result = dTemp;

		Result &= 0xFFFF0000;
		Result |= ((sTemp & 0xF7DE) >> 1) + ((dTemp & 0xF7DE) >> 1);
		Result &= 0xFFFF;
		Result |= ((sTemp & 0xF7DE0000) >> 1) + ((dTemp & 0xF7DE0000) >> 1);

		*((unsigned long*)dst) = Result;
		dst += 4;
		src += 4;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 24-bit with 50% alpha factor
*/
static void dg_blit_alpha_fast_24(unsigned char* dst, unsigned char* src, unsigned int width)
{
	register unsigned int i;
	register unsigned long sTemp, dTemp;
    unsigned long Result;

	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp  = *((unsigned long*)dst);
		Result = ((sTemp & 0xFEFEFE) >> 1) + ((dTemp & 0xFEFEFE) >> 1);
		*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
		dst += 2;
		*dst = (unsigned char)((Result >> 16) & 0xFF);
		dst++;
		src += 3;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 32-bit with 50% alpha factor
*/
static void dg_blit_alpha_fast_32(unsigned long* dst, unsigned long* src, unsigned int width)
{
	register unsigned int i=width;
	register unsigned int mask=0xFEFEFE;
	while (i--)
		*dst = ((*(dst++)&mask)+(*(src++)&mask))>>1;
}

/*
* copy source pixel buffer to destination pixel buffer
* with alpha factor
*/
void dg_blit_alpha(int bitsPerPixel, unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha)
{
	if (alpha == 128)
	{
		switch (bitsPerPixel)
		{
			case 32: dg_blit_alpha_fast_32((unsigned long*)dst, (unsigned long*)src, width); break;
			case 24: dg_blit_alpha_fast_24(dst, src, width); break;
			case 16: dg_blit_alpha_fast_16(dst, src, width); break;
			case 15: dg_blit_alpha_fast_15(dst, src, width); break;
		}
	}
	else
	{
		switch (bitsPerPixel)
		{
			case 32: dg_blit_alpha_32(dst, src, width, alpha); break;
			case 24: dg_blit_alpha_24(dst, src, width, alpha); break;
			case 16: dg_blit_alpha_16(dst, src, width, alpha); break;
			case 15: dg_blit_alpha_15(dst, src, width, alpha); break;
		}
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 15-bit with alpha factor and color key
*/
static void dg_blit_alpha_color_key_15(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned long colorKey)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha=(unsigned short)alpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		if (sTemp != (colorKey & 0x7FFF))
		{
			dTemp = *((unsigned short*)dst);
			sb = sTemp & 0x1F;
			db = dTemp & 0x1F;
			sg = (sTemp >> 5) & 0x1F;
			dg = (dTemp >> 5) & 0x1F;
			sr = (sTemp >> 10) & 0x1F;
			dr = (dTemp >> 10) & 0x1F;

			*((unsigned short*)dst) = (unsigned short)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 5 |
				((wAlpha * (sr - dr) >> 8) + dr) << 10);
		}
		dst += 2;
		src += 2;
	}
	colorKey=(colorKey & 0xFFFF) | (colorKey << 16);
	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if (sTemp != colorKey)
		{
			dTemp = *((unsigned long*)dst);
			Result = dTemp;
			if ((sTemp & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = sTemp & 0x1F;
				db = dTemp & 0x1F;
				sg = (sTemp >> 5) & 0x1F;
				dg = (dTemp >> 5) & 0x1F;
				sr = (sTemp >> 10) & 0x1F;
				dr = (dTemp >> 10) & 0x1F;

				Result = Result & 0xFFFF0000;
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 10);
			}
			if (((sTemp >> 16) & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = (sTemp >> 16) & 0x1F;
				db = (dTemp >> 16) & 0x1F;
				sg = (sTemp >> 21) & 0x1F;
				dg = (dTemp >> 21) & 0x1F;
				sr = (sTemp >> 26) & 0x1F;
				dr = (dTemp >> 26) & 0x1F;

				Result = Result & 0xFFFF;
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 10 ) << 16;
			}
			*((unsigned long*)dst) = Result;
		}
		dst += 4;
		src += 4;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 16-bit with alpha factor and color key
*/
static void dg_blit_alpha_color_key_16(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned long colorKey)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha=(unsigned short)alpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		if (sTemp != (colorKey & 0xFFFF))
		{
			dTemp = *((unsigned short*)dst);
			sb = sTemp & 0x1F;
			db = dTemp & 0x1F;
			sg = (sTemp >> 5) & 0x3F;
			dg = (dTemp >> 5) & 0x3F;
			sr = (sTemp >> 11) & 0x1F;
			dr = (dTemp >> 11) & 0x1F;

			*((unsigned short*)dst) = (unsigned short)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 5 |
				((wAlpha * (sr - dr) >> 8) + dr) << 11);
		}
		dst += 2;
		src += 2;
	}
	/* 5-6-5 color format */
	colorKey=(colorKey & 0xFFFF) | (colorKey << 16);
	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if (sTemp != colorKey)
		{

			dTemp = *((unsigned long*)dst);
			Result = dTemp;

			if ((sTemp & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = sTemp & 0x1F;
				db = dTemp & 0x1F;
				sg = (sTemp >> 5) & 0x3F;
				dg = (dTemp >> 5) & 0x3F;
				sr = (sTemp >> 11) & 0x1F;
				dr = (dTemp >> 11) & 0x1F;

				Result = Result & 0xFFFF0000;
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 11);
			}
			if (((sTemp >> 16) & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = (sTemp >> 16) & 0x1F;
				db = (dTemp >> 16) & 0x1F;
				sg = (sTemp >> 21) & 0x3F;
				dg = (dTemp >> 21) & 0x3F;
				sr = (sTemp >> 27) & 0x1F;
				dr = (dTemp >> 27) & 0x1F;

				Result = Result & 0xFFFF;
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 11 ) << 16;
			}
			*((unsigned long*)dst) = Result;
		}
		dst += 4;
		src += 4;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 24-bit with alpha factor and color key
*/
static void dg_blit_alpha_color_key_24(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned long colorKey)
{
	register unsigned int i = width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha=(unsigned short)alpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if ((sTemp & 0xFFFFFF) != colorKey)
		{
			dTemp = *((unsigned long*)dst);
			sb = sTemp & 0xFF;
			db = dTemp & 0xFF;
			sg = (sTemp >> 8) & 0xFF;
			dg = (dTemp >> 8) & 0xFF;
			sr = (sTemp >> 16) & 0xFF;
			dr = (dTemp >> 16) & 0xFF;

			Result = (unsigned long)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 8 |
				((wAlpha * (sr - dr) >> 8) + dr) << 16);

			*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
			dst += 2;
			*dst = (unsigned char)((Result >> 16) & 0xFF);
			dst++;
		}
		else
		{
			dst+=3;
		}
		src+=3;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 32-bit with alpha factor and color key
*/
static void dg_blit_alpha_color_key_32(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned long colorKey)
{
	register unsigned int i=width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha=(unsigned short)alpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if ((sTemp & 0xFFFFFF) != colorKey)
		{
			dTemp = *((unsigned long*)dst);
			sb = sTemp & 0xFF;
			db = dTemp & 0xFF;
			sg = (sTemp >> 8) & 0xFF;
			dg = (dTemp >> 8) & 0xFF;
			sr = (sTemp >> 16) & 0xFF;
			dr = (dTemp >> 16) & 0xFF;

			Result = (unsigned long)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 8 |
				((wAlpha * (sr - dr) >> 8) + dr) << 16);

			*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
			dst += 2;
			*dst = (unsigned char)((Result >> 16) & 0xFF);
			dst += 2;
		}
		else
		{
			dst += 4;
		}
		src += 4;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 15-bit with 50% alpha factor and color key
*/
void dg_blit_alpha_fast_color_key_15(unsigned char* dst, unsigned char* src, unsigned int width, unsigned long colorKey)
{
	int is_odd_width = 0;
	register unsigned int i;
	register unsigned long sTemp, dTemp;
    unsigned long Result;
	
	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		if (sTemp != colorKey)
		{
			dTemp = *((unsigned short*)dst);
			*((unsigned short*)dst) = (unsigned short)(((sTemp & 0x7BDE) >> 1) + ((dTemp & 0x7BDE) >> 1));
		}
		dst += 2;
		src += 2;
	}
	
	i = width;
	while (i--)
	{
		sTemp  = *((unsigned long*)src);
		dTemp  = *((unsigned long*)dst);
		Result = dTemp;

		if ((sTemp & 0xFFFF) != colorKey)
		{
			Result &= 0xFFFF0000;
			Result |= ((sTemp & 0x7BDE) >> 1) + ((dTemp & 0x7BDE) >> 1);
		}
		if ((sTemp >> 16) != colorKey)
		{
			Result &= 0xFFFF;
			Result |= ((sTemp & 0x7BDE0000) >> 1) + ((dTemp & 0x7BDE0000) >> 1);
		}

		*((unsigned long*)dst) = Result;
		dst += 4;
		src += 4;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 16-bit with 50% alpha factor and color key
*/
void dg_blit_alpha_fast_color_key_16(unsigned char* dst, unsigned char* src, unsigned int width, unsigned long colorKey)
{
	int is_odd_width = 0;
	register unsigned int i;
	register unsigned long sTemp, dTemp;
    unsigned long Result;
	
	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		if (sTemp != (colorKey & 0xFFFF))
		{
			dTemp = *((unsigned short*)dst);
			*((unsigned short*)dst) = (unsigned short)(((sTemp & 0xF7DE) >> 1) + ((dTemp & 0xF7DE) >> 1));
		}
		dst += 2;
		src += 2;
	}
	
	i = width;
	while (i--)
	{
		sTemp  = *((unsigned long*)src);
		dTemp  = *((unsigned long*)dst);
		Result = dTemp;

		if ((sTemp & 0xFFFF) != (colorKey & 0xFFFF))
		{
			Result &= 0xFFFF0000;
			Result |= ((sTemp & 0xF7DE) >> 1) + ((dTemp & 0xF7DE) >> 1);
		}
		if ((sTemp >> 16) != (colorKey & 0xFFFF))
		{
			Result &= 0xFFFF;
			Result |= ((sTemp & 0xF7DE0000) >> 1) + ((dTemp & 0xF7DE0000) >> 1);
		}

		*((unsigned long*)dst) = Result;
		dst += 4;
		src += 4;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 24-bit with 50% alpha factor and color key
*/
void dg_blit_alpha_fast_color_key_24(unsigned char* dst, unsigned char* src, unsigned int width, unsigned long colorKey)
{
	register unsigned int i;
	register unsigned long sTemp, dTemp;
    unsigned long Result;

	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);

		if ((sTemp &= 0xFFFFFF) != colorKey)
		{
			dTemp  = *((unsigned long*)dst);
			Result = ((sTemp & 0xFEFEFE) >> 1) + ((dTemp & 0xFEFEFE) >> 1);

			*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
			dst += 2;
			*dst = (unsigned char)((Result >> 16) & 0xFF);
			dst++;
		}
		else
		{
			dst += 3;
		}

		src += 3;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 32-bit with 50% alpha factor and color key
*/
void dg_blit_alpha_fast_color_key_32(unsigned long* dst, unsigned long* src, unsigned int width, unsigned long colorKey)
{
	register unsigned int i=width;
	register unsigned int mask=0xFEFEFE;
	while (i--)
		if (*src != colorKey)
			*dst = ((*(dst++)&mask)+(*(src++)&mask))>>1;
}

/*
* copy source pixel buffer to destination pixel buffer
* with alpha factor and color key
*/
void dg_blit_alpha_color_key(int bitsPerPixel, unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned long colorKey)
{
	if (alpha == 128)
	{
		switch (bitsPerPixel)
		{
			case 32: dg_blit_alpha_fast_color_key_32((unsigned long*)dst, (unsigned long*)src, width, colorKey); break;
			case 24: dg_blit_alpha_fast_color_key_24(dst, src, width, colorKey); break;
			case 16: dg_blit_alpha_fast_color_key_16(dst, src, width, colorKey); break;
			case 15: dg_blit_alpha_fast_color_key_15(dst, src, width, colorKey); break;
		}
	}
	else
	{
		switch (bitsPerPixel)
		{
			case 32: dg_blit_alpha_color_key_32(dst, src, width, alpha, colorKey); break;
			case 24: dg_blit_alpha_color_key_24(dst, src, width, alpha, colorKey); break;
			case 16: dg_blit_alpha_color_key_16(dst, src, width, alpha, colorKey); break;
			case 15: dg_blit_alpha_color_key_15(dst, src, width, alpha, colorKey); break;
		}
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 15-bit with alpha channel, alpha factor and color key
*/
static void dg_blit_alpha_channel_alpha_color_key_15(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned char* alphaChannel, unsigned long colorKey)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		if (sTemp != (colorKey & 0x7FFF))
		{
			dTemp = *((unsigned short*)dst);
			sb = sTemp & 0x1F;
			db = dTemp & 0x1F;
			sg = (sTemp >> 5) & 0x1F;
			dg = (dTemp >> 5) & 0x1F;
			sr = (sTemp >> 10) & 0x1F;
			dr = (dTemp >> 10) & 0x1F;
	
			wAlpha = (alpha * *alphaChannel) >> 8;

			*((unsigned short*)dst) = (unsigned short)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 5 |
				((wAlpha * (sr - dr) >> 8) + dr) << 10);
		}
		dst += 2;
		src += 2;
		alphaChannel++;
	}
	colorKey=(colorKey & 0xFFFF) | (colorKey << 16);
	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if (sTemp != colorKey)
		{
			dTemp = *((unsigned long*)dst);
			Result = dTemp;
			if ((sTemp & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = sTemp & 0x1F;
				db = dTemp & 0x1F;
				sg = (sTemp >> 5) & 0x1F;
				dg = (dTemp >> 5) & 0x1F;
				sr = (sTemp >> 10) & 0x1F;
				dr = (dTemp >> 10) & 0x1F;

				Result = Result & 0xFFFF0000;
				wAlpha = (alpha * *alphaChannel) >> 8;
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 10);
			}
			if (((sTemp >> 16) & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = (sTemp >> 16) & 0x1F;
				db = (dTemp >> 16) & 0x1F;
				sg = (sTemp >> 21) & 0x1F;
				dg = (dTemp >> 21) & 0x1F;
				sr = (sTemp >> 26) & 0x1F;
				dr = (dTemp >> 26) & 0x1F;

				Result = Result & 0xFFFF;
				wAlpha = (alpha * alphaChannel[1]) >> 8;
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 10 ) << 16;
			}
			*((unsigned long*)dst) = Result;
		}
		dst += 4;
		src += 4;
		alphaChannel += 2;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 16-bit with alpha channel, alpha factor and color key
*/
static void dg_blit_alpha_channel_alpha_color_key_16(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned char* alphaChannel, unsigned long colorKey)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		if (sTemp != (colorKey & 0xFFFF))
		{
			dTemp = *((unsigned short*)dst);
			sb = sTemp & 0x1F;
			db = dTemp & 0x1F;
			sg = (sTemp >> 5) & 0x3F;
			dg = (dTemp >> 5) & 0x3F;
			sr = (sTemp >> 11) & 0x1F;
			dr = (dTemp >> 11) & 0x1F;

			wAlpha = (alpha * *alphaChannel) >> 8;

			*((unsigned short*)dst) = (unsigned short)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 5 |
				((wAlpha * (sr - dr) >> 8) + dr) << 11);
		}
		dst += 2;
		src += 2;
		alphaChannel++;
	}
	/* 5-6-5 color format */
	colorKey=(colorKey & 0xFFFF) | (colorKey << 16);
	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if (sTemp != colorKey)
		{

			dTemp = *((unsigned long*)dst);
			Result = dTemp;

			if ((sTemp & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = sTemp & 0x1F;
				db = dTemp & 0x1F;
				sg = (sTemp >> 5) & 0x3F;
				dg = (dTemp >> 5) & 0x3F;
				sr = (sTemp >> 11) & 0x1F;
				dr = (dTemp >> 11) & 0x1F;

				Result = Result & 0xFFFF0000;
				wAlpha = (alpha * *alphaChannel) >> 8;
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 11);
			}
			if (((sTemp >> 16) & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = (sTemp >> 16) & 0x1F;
				db = (dTemp >> 16) & 0x1F;
				sg = (sTemp >> 21) & 0x3F;
				dg = (dTemp >> 21) & 0x3F;
				sr = (sTemp >> 27) & 0x1F;
				dr = (dTemp >> 27) & 0x1F;

				Result = Result & 0xFFFF;
				wAlpha = (alpha * alphaChannel[1]) >> 8;
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 11 ) << 16;
			}
			*((unsigned long*)dst) = Result;
		}
		dst += 4;
		src += 4;
		alphaChannel+=2;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 24-bit with alpha channel, alpha factor and color key
*/
static void dg_blit_alpha_channel_alpha_color_key_24(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned char* alphaChannel, unsigned long colorKey)
{
	register unsigned int i = width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if ((sTemp & 0xFFFFFF) != colorKey)
		{
			dTemp = *((unsigned long*)dst);
			sb = sTemp & 0xFF;
			db = dTemp & 0xFF;
			sg = (sTemp >> 8) & 0xFF;
			dg = (dTemp >> 8) & 0xFF;
			sr = (sTemp >> 16) & 0xFF;
			dr = (dTemp >> 16) & 0xFF;

			wAlpha = (alpha * *alphaChannel) >> 8;

			Result = (unsigned long)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 8 |
				((wAlpha * (sr - dr) >> 8) + dr) << 16);

			*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
			dst += 2;
			*dst = (unsigned char)((Result >> 16) & 0xFF);
			dst++;
		}
		else
		{
			dst+=3;
		}
		src+=3;
		alphaChannel++;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 32-bit with alpha channel, alpha factor and color key
*/
static void dg_blit_alpha_channel_alpha_color_key_32(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned char* alphaChannel, unsigned long colorKey)
{
	register unsigned int i=width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if ((sTemp & 0xFFFFFF) != colorKey)
		{
			dTemp = *((unsigned long*)dst);
			sb = sTemp & 0xFF;
			db = dTemp & 0xFF;
			sg = (sTemp >> 8) & 0xFF;
			dg = (dTemp >> 8) & 0xFF;
			sr = (sTemp >> 16) & 0xFF;
			dr = (dTemp >> 16) & 0xFF;

			wAlpha = (alpha * *alphaChannel) >> 8;

			Result = (unsigned long)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 8 |
				((wAlpha * (sr - dr) >> 8) + dr) << 16);

			*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
			dst += 2;
			*dst = (unsigned char)((Result >> 16) & 0xFF);
			dst += 2;
		}
		else
		{
			dst += 4;
		}
		src += 4;
		alphaChannel++;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* with alpha channel, alpha factor and color key
*/
void dg_blit_alpha_channel_alpha_color_key(int bitsPerPixel, unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned char* alphaChannel, unsigned long colorKey)
{
	switch (bitsPerPixel)
	{
		case 32: dg_blit_alpha_channel_alpha_color_key_32(dst, src, width, alpha, alphaChannel, colorKey); break;
		case 24: dg_blit_alpha_channel_alpha_color_key_24(dst, src, width, alpha, alphaChannel, colorKey); break;
		case 16: dg_blit_alpha_channel_alpha_color_key_16(dst, src, width, alpha, alphaChannel, colorKey); break;
		case 15: dg_blit_alpha_channel_alpha_color_key_15(dst, src, width, alpha, alphaChannel, colorKey); break;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 15-bit with alpha channel and alpha factor
*/
static void dg_blit_alpha_channel_alpha_15(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned char* alphaChannel)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		dTemp = *((unsigned short*)dst);
		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x1F;
		dg = (dTemp >> 5) & 0x1F;
		sr = (sTemp >> 10) & 0x1F;
		dr = (dTemp >> 10) & 0x1F;

		wAlpha = (alpha * *alphaChannel) >> 8;

		*((unsigned short*)dst) = (unsigned short)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 10);
		dst += 2;
		src += 2;
		alphaChannel++;
	}

	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		Result = dTemp;
		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x1F;
		dg = (dTemp >> 5) & 0x1F;
		sr = (sTemp >> 10) & 0x1F;
		dr = (dTemp >> 10) & 0x1F;

		Result = Result & 0xFFFF0000;
		wAlpha = (alpha * *alphaChannel) >> 8;
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 10);

		sb = (sTemp >> 16) & 0x1F;
		db = (dTemp >> 16) & 0x1F;
		sg = (sTemp >> 21) & 0x1F;
		dg = (dTemp >> 21) & 0x1F;
		sr = (sTemp >> 26) & 0x1F;
		dr = (dTemp >> 26) & 0x1F;

		Result = Result & 0xFFFF;
		wAlpha = (alpha * alphaChannel[1]) >> 8;
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 10 ) << 16;

		*((unsigned long*)dst) = Result;
		dst += 4;
		src += 4;
		alphaChannel += 2;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 16-bit with alpha channel and alpha factor
*/
static void dg_blit_alpha_channel_alpha_16(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned char* alphaChannel)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		dTemp = *((unsigned short*)dst);
		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x3F;
		dg = (dTemp >> 5) & 0x3F;
		sr = (sTemp >> 11) & 0x1F;
		dr = (dTemp >> 11) & 0x1F;

		wAlpha = (alpha * *alphaChannel) >> 8;

		*((unsigned short*)dst) = (unsigned short)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 11);
		dst += 2;
		src += 2;
		alphaChannel++;
	}
	/* 5-6-5 color format */
	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		Result = dTemp;

		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x3F;
		dg = (dTemp >> 5) & 0x3F;
		sr = (sTemp >> 11) & 0x1F;
		dr = (dTemp >> 11) & 0x1F;

		Result = Result & 0xFFFF0000;
		wAlpha = (alpha * *alphaChannel) >> 8;
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 11);

		sb = (sTemp >> 16) & 0x1F;
		db = (dTemp >> 16) & 0x1F;
		sg = (sTemp >> 21) & 0x3F;
		dg = (dTemp >> 21) & 0x3F;
		sr = (sTemp >> 27) & 0x1F;
		dr = (dTemp >> 27) & 0x1F;

		Result = Result & 0xFFFF;
		wAlpha = (alpha * alphaChannel[1]) >> 8;
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 11 ) << 16;
		*((unsigned long*)dst) = Result;
		dst += 4;
		src += 4;
		alphaChannel+=2;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 24-bit with alpha channel and alpha factor
*/
static void dg_blit_alpha_channel_alpha_24(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned char* alphaChannel)
{
	register unsigned int i = width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		sb = sTemp & 0xFF;
		db = dTemp & 0xFF;
		sg = (sTemp >> 8) & 0xFF;
		dg = (dTemp >> 8) & 0xFF;
		sr = (sTemp >> 16) & 0xFF;
		dr = (dTemp >> 16) & 0xFF;

		wAlpha = (alpha * *alphaChannel) >> 8;

		Result = (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 8 |
			((wAlpha * (sr - dr) >> 8) + dr) << 16);

		*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
		dst += 2;
		*dst = (unsigned char)((Result >> 16) & 0xFF);
		dst++;
		src+=3;
		alphaChannel++;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 32-bit with alpha channel and alpha factor
*/
static void dg_blit_alpha_channel_alpha_32(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned char* alphaChannel)
{
	register unsigned int i=width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		sb = sTemp & 0xFF;
		db = dTemp & 0xFF;
		sg = (sTemp >> 8) & 0xFF;
		dg = (dTemp >> 8) & 0xFF;
		sr = (sTemp >> 16) & 0xFF;
		dr = (dTemp >> 16) & 0xFF;

		wAlpha = (alpha * *alphaChannel) >> 8;

		Result = (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 8 |
			((wAlpha * (sr - dr) >> 8) + dr) << 16);

		*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
		dst += 2;
		*dst = (unsigned char)((Result >> 16) & 0xFF);
		dst += 2;
		src += 4;
		alphaChannel++;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* with alpha channel and alpha factor
*/
void dg_blit_alpha_channel_alpha(int bitsPerPixel, unsigned char* dst, unsigned char* src, unsigned int width, unsigned char alpha, unsigned char* alphaChannel)
{
	switch (bitsPerPixel)
	{
		case 32: dg_blit_alpha_channel_alpha_32(dst, src, width, alpha, alphaChannel); break;
		case 24: dg_blit_alpha_channel_alpha_24(dst, src, width, alpha, alphaChannel); break;
		case 16: dg_blit_alpha_channel_alpha_16(dst, src, width, alpha, alphaChannel); break;
		case 15: dg_blit_alpha_channel_alpha_15(dst, src, width, alpha, alphaChannel); break;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 15-bit with alpha channel and color key
*/
static void dg_blit_alpha_channel_color_key_15(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char* alphaChannel, unsigned long colorKey)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		if (sTemp != (colorKey & 0x7FFF))
		{
			dTemp = *((unsigned short*)dst);
			sb = sTemp & 0x1F;
			db = dTemp & 0x1F;
			sg = (sTemp >> 5) & 0x1F;
			dg = (dTemp >> 5) & 0x1F;
			sr = (sTemp >> 10) & 0x1F;
			dr = (dTemp >> 10) & 0x1F;

			wAlpha = *alphaChannel;
			*((unsigned short*)dst) = (unsigned short)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 5 |
				((wAlpha * (sr - dr) >> 8) + dr) << 10);
		}
		dst += 2;
		src += 2;
		alphaChannel++;
	}
	colorKey=(colorKey & 0xFFFF) | (colorKey << 16);
	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if (sTemp != colorKey)
		{
			dTemp = *((unsigned long*)dst);
			Result = dTemp;
			if ((sTemp & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = sTemp & 0x1F;
				db = dTemp & 0x1F;
				sg = (sTemp >> 5) & 0x1F;
				dg = (dTemp >> 5) & 0x1F;
				sr = (sTemp >> 10) & 0x1F;
				dr = (dTemp >> 10) & 0x1F;

				Result = Result & 0xFFFF0000;
				wAlpha = *alphaChannel;
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 10);
			}
			if (((sTemp >> 16) & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = (sTemp >> 16) & 0x1F;
				db = (dTemp >> 16) & 0x1F;
				sg = (sTemp >> 21) & 0x1F;
				dg = (dTemp >> 21) & 0x1F;
				sr = (sTemp >> 26) & 0x1F;
				dr = (dTemp >> 26) & 0x1F;

				Result = Result & 0xFFFF;
				wAlpha = alphaChannel[1];
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 10 ) << 16;
			}
			*((unsigned long*)dst) = Result;
		}
		dst += 4;
		src += 4;
		alphaChannel += 2;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 16-bit with alpha channel and color key
*/
static void dg_blit_alpha_channel_color_key_16(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char* alphaChannel, unsigned long colorKey)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		if (sTemp != (colorKey & 0xFFFF))
		{
			dTemp = *((unsigned short*)dst);
			sb = sTemp & 0x1F;
			db = dTemp & 0x1F;
			sg = (sTemp >> 5) & 0x3F;
			dg = (dTemp >> 5) & 0x3F;
			sr = (sTemp >> 11) & 0x1F;
			dr = (dTemp >> 11) & 0x1F;

			wAlpha = *alphaChannel;

			*((unsigned short*)dst) = (unsigned short)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 5 |
				((wAlpha * (sr - dr) >> 8) + dr) << 11);
		}
		dst += 2;
		src += 2;
		alphaChannel++;
	}
	/* 5-6-5 color format */
	colorKey=(colorKey & 0xFFFF) | (colorKey << 16);
	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if (sTemp != colorKey)
		{

			dTemp = *((unsigned long*)dst);
			Result = dTemp;

			if ((sTemp & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = sTemp & 0x1F;
				db = dTemp & 0x1F;
				sg = (sTemp >> 5) & 0x3F;
				dg = (dTemp >> 5) & 0x3F;
				sr = (sTemp >> 11) & 0x1F;
				dr = (dTemp >> 11) & 0x1F;

				Result = Result & 0xFFFF0000;
				wAlpha = *alphaChannel;
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 11);
			}
			if (((sTemp >> 16) & 0xFFFF) != (colorKey & 0xFFFF))
			{
				sb = (sTemp >> 16) & 0x1F;
				db = (dTemp >> 16) & 0x1F;
				sg = (sTemp >> 21) & 0x3F;
				dg = (dTemp >> 21) & 0x3F;
				sr = (sTemp >> 27) & 0x1F;
				dr = (dTemp >> 27) & 0x1F;

				Result = Result & 0xFFFF;
				wAlpha = alphaChannel[1];
				Result |= (unsigned long)
					((wAlpha * (sb - db) >> 8) + db |
					((wAlpha * (sg - dg) >> 8) + dg) << 5 |
					((wAlpha * (sr - dr) >> 8) + dr) << 11 ) << 16;
			}
			*((unsigned long*)dst) = Result;
		}
		dst += 4;
		src += 4;
		alphaChannel+=2;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 24-bit with alpha channel and color key
*/
static void dg_blit_alpha_channel_color_key_24(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char* alphaChannel, unsigned long colorKey)
{
	register unsigned int i = width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if ((sTemp & 0xFFFFFF) != colorKey)
		{
			dTemp = *((unsigned long*)dst);
			sb = sTemp & 0xFF;
			db = dTemp & 0xFF;
			sg = (sTemp >> 8) & 0xFF;
			dg = (dTemp >> 8) & 0xFF;
			sr = (sTemp >> 16) & 0xFF;
			dr = (dTemp >> 16) & 0xFF;

			wAlpha = *alphaChannel;

			Result = (unsigned long)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 8 |
				((wAlpha * (sr - dr) >> 8) + dr) << 16);

			*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
			dst += 2;
			*dst = (unsigned char)((Result >> 16) & 0xFF);
			dst++;
		}
		else
		{
			dst+=3;
		}
		src+=3;
		alphaChannel++;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 32-bit with alpha channel and color key
*/
static void dg_blit_alpha_channel_color_key_32(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char* alphaChannel, unsigned long colorKey)
{
	register unsigned int i=width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		if ((sTemp & 0xFFFFFF) != colorKey)
		{
			dTemp = *((unsigned long*)dst);
			sb = sTemp & 0xFF;
			db = dTemp & 0xFF;
			sg = (sTemp >> 8) & 0xFF;
			dg = (dTemp >> 8) & 0xFF;
			sr = (sTemp >> 16) & 0xFF;
			dr = (dTemp >> 16) & 0xFF;

			wAlpha = *alphaChannel;

			Result = (unsigned long)
				((wAlpha * (sb - db) >> 8) + db |
				((wAlpha * (sg - dg) >> 8) + dg) << 8 |
				((wAlpha * (sr - dr) >> 8) + dr) << 16);

			*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
			dst += 2;
			*dst = (unsigned char)((Result >> 16) & 0xFF);
			dst += 2;
		}
		else
		{
			dst += 4;
		}
		src += 4;
		alphaChannel++;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* with alpha channel and color key
*/
void dg_blit_alpha_channel_color_key(int bitsPerPixel, unsigned char* dst, unsigned char* src, unsigned int width, unsigned char* alphaChannel, unsigned long colorKey)
{
	switch (bitsPerPixel)
	{
		case 32: dg_blit_alpha_channel_color_key_32(dst, src, width, alphaChannel, colorKey); break;
		case 24: dg_blit_alpha_channel_color_key_24(dst, src, width, alphaChannel, colorKey); break;
		case 16: dg_blit_alpha_channel_color_key_16(dst, src, width, alphaChannel, colorKey); break;
		case 15: dg_blit_alpha_channel_color_key_15(dst, src, width, alphaChannel, colorKey); break;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 15-bit with alpha channel
*/
static void dg_blit_alpha_channel_15(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char* alphaChannel)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		dTemp = *((unsigned short*)dst);
		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x1F;
		dg = (dTemp >> 5) & 0x1F;
		sr = (sTemp >> 10) & 0x1F;
		dr = (dTemp >> 10) & 0x1F;

		wAlpha = *alphaChannel;
		*((unsigned short*)dst) = (unsigned short)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 10);
		dst += 2;
		src += 2;
		alphaChannel++;
	}

	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		Result = dTemp;
		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x1F;
		dg = (dTemp >> 5) & 0x1F;
		sr = (sTemp >> 10) & 0x1F;
		dr = (dTemp >> 10) & 0x1F;

		Result = Result & 0xFFFF0000;
		wAlpha = *alphaChannel;
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 10);

		sb = (sTemp >> 16) & 0x1F;
		db = (dTemp >> 16) & 0x1F;
		sg = (sTemp >> 21) & 0x1F;
		dg = (dTemp >> 21) & 0x1F;
		sr = (sTemp >> 26) & 0x1F;
		dr = (dTemp >> 26) & 0x1F;

		Result = Result & 0xFFFF;
		wAlpha = alphaChannel[1];
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 10 ) << 16;

		*((unsigned long*)dst) = Result;
		dst += 4;
		src += 4;
		alphaChannel += 2;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 16-bit with alpha channel
*/
static void dg_blit_alpha_channel_16(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char* alphaChannel)
{
	int is_odd_width = 0;
	long sr,sg,sb,dr,dg,db;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned int i;
	unsigned short wAlpha;

	/* is odd width ? */
	if (width & 1)
	{
		is_odd_width = 1;
		width--; 
	}

	/* div by 2, processing 2 pixels at a time */
	width>>=1;

	if (is_odd_width)
	{
		sTemp = *((unsigned short*)src);
		dTemp = *((unsigned short*)dst);
		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x3F;
		dg = (dTemp >> 5) & 0x3F;
		sr = (sTemp >> 11) & 0x1F;
		dr = (dTemp >> 11) & 0x1F;

		wAlpha = *alphaChannel;

		*((unsigned short*)dst) = (unsigned short)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 11);
		dst += 2;
		src += 2;
		alphaChannel++;
	}
	/* 5-6-5 color format */
	i = width;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		Result = dTemp;

		sb = sTemp & 0x1F;
		db = dTemp & 0x1F;
		sg = (sTemp >> 5) & 0x3F;
		dg = (dTemp >> 5) & 0x3F;
		sr = (sTemp >> 11) & 0x1F;
		dr = (dTemp >> 11) & 0x1F;

		Result = Result & 0xFFFF0000;
		wAlpha = *alphaChannel;
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 11);

		sb = (sTemp >> 16) & 0x1F;
		db = (dTemp >> 16) & 0x1F;
		sg = (sTemp >> 21) & 0x3F;
		dg = (dTemp >> 21) & 0x3F;
		sr = (sTemp >> 27) & 0x1F;
		dr = (dTemp >> 27) & 0x1F;

		Result = Result & 0xFFFF;
		wAlpha = alphaChannel[1];
		Result |= (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 5 |
			((wAlpha * (sr - dr) >> 8) + dr) << 11 ) << 16;
		*((unsigned long*)dst) = Result;
		dst += 4;
		src += 4;
		alphaChannel+=2;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 24-bit with alpha channel
*/
static void dg_blit_alpha_channel_24(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char* alphaChannel)
{
	register unsigned int i = width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		sb = sTemp & 0xFF;
		db = dTemp & 0xFF;
		sg = (sTemp >> 8) & 0xFF;
		dg = (dTemp >> 8) & 0xFF;
		sr = (sTemp >> 16) & 0xFF;
		dr = (dTemp >> 16) & 0xFF;

		wAlpha = *alphaChannel;

		Result = (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 8 |
			((wAlpha * (sr - dr) >> 8) + dr) << 16);

		*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
		dst += 2;
		*dst = (unsigned char)((Result >> 16) & 0xFF);
		dst++;
		src+=3;
		alphaChannel++;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* 32-bit with alpha channel
*/
static void dg_blit_alpha_channel_32(unsigned char* dst, unsigned char* src, unsigned int width, unsigned char* alphaChannel)
{
	register unsigned int i=width;
	register unsigned long sTemp, dTemp;
	register unsigned long Result;
	register unsigned short wAlpha;
	long sr,sg,sb,dr,dg,db;
	while (i--)
	{
		sTemp = *((unsigned long*)src);
		dTemp = *((unsigned long*)dst);
		sb = sTemp & 0xFF;
		db = dTemp & 0xFF;
		sg = (sTemp >> 8) & 0xFF;
		dg = (dTemp >> 8) & 0xFF;
		sr = (sTemp >> 16) & 0xFF;
		dr = (dTemp >> 16) & 0xFF;

		wAlpha = *alphaChannel;

		Result = (unsigned long)
			((wAlpha * (sb - db) >> 8) + db |
			((wAlpha * (sg - dg) >> 8) + dg) << 8 |
			((wAlpha * (sr - dr) >> 8) + dr) << 16);

		*((unsigned short*)dst) = (unsigned short)(Result & 0xFFFF);
		dst += 2;
		*dst = (unsigned char)((Result >> 16) & 0xFF);
		dst += 2;
		src += 4;
		alphaChannel++;
	}
}

/*
* copy source pixel buffer to destination pixel buffer
* with alpha channel
*/
void dg_blit_alpha_channel(int bitsPerPixel, unsigned char* dst, unsigned char* src, unsigned int width, unsigned char* alphaChannel)
{
	switch (bitsPerPixel)
	{
		case 32: dg_blit_alpha_channel_32(dst, src, width, alphaChannel); break;
		case 24: dg_blit_alpha_channel_24(dst, src, width, alphaChannel); break;
		case 16: dg_blit_alpha_channel_16(dst, src, width, alphaChannel); break;
		case 15: dg_blit_alpha_channel_15(dst, src, width, alphaChannel); break;
	}
}

/*
	FIXME: Use averaged scaling as described in article http://www.ddj.com/184405045

	#define average(a, b)   (PIXEL)(( (int)(a) + (int)(b) ) >> 1)
	void ScaleLineAvg(PIXEL *Target, PIXEL *Source, int SrcWidth, int TgtWidth)
	{
	int NumPixels = TgtWidth;
	int Mid = TgtWidth / 2;
	int E = 0;
	PIXEL p;
	if (TgtWidth > SrcWidth)
		NumPixels--;
	while (NumPixels-- > 0) {
		p = *Source;
		if (E >= Mid)
		p = average(p, *(Source+1));
		*Target++ = p;
		E += SrcWidth;
		if (E >= TgtWidth) {
		E -= TgtWidth;
		Source++;
		} 
	} 
	if (TgtWidth > SrcWidth)
		*Target = *Source;
	}
*/

/* 8-bit line scale */
static int dg_scale_pixel_line_8(unsigned char *dst, int dWidth, unsigned char *src, int sWidth)
{
	int NumPixels = dWidth;
	int IntPart = sWidth / dWidth;
	int FractPart = sWidth % dWidth;
	int E = 0;
	while (NumPixels-- > 0) 
	{
		*dst++=*src;
		src+=IntPart;

		E+=FractPart;
		if (E>=dWidth) 
		{
			E-=dWidth;
			src++;
		}
	}
	return 1;
}

/* 16-bit line scale */
static int dg_scale_pixel_line_16(unsigned short *dst, int dWidth, unsigned short *src, int sWidth)
{
	int NumPixels = dWidth;
	int IntPart = sWidth / dWidth;
	int FractPart = sWidth % dWidth;
	int E = 0;
	while (NumPixels-- > 0) 
	{
		*dst++=*src;
		src+=IntPart;

		E+=FractPart;
		if (E>=dWidth) 
		{
			E-=dWidth;
			src++;
		}
	}
	return 1;
}

/* 24-bit line scale */
static int dg_scale_pixel_line_24(unsigned char *dst, int dWidth, unsigned char *src, int sWidth)
{
	int NumPixels = dWidth;
	int IntPart = sWidth / dWidth;
	int FractPart = sWidth % dWidth;
	int E = 0;
	while (NumPixels-- > 0) 
	{
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];

		dst+=3;
		src+=3*IntPart;

		E += FractPart;
		if (E >= dWidth) 
		{
			E -= dWidth;
			src+=3;
		}
	}
	return 1;
}

/* 32-bit line scale */
static int dg_scale_pixel_line_32(unsigned long *dst, int dWidth, unsigned long *src, int sWidth)
{
	int NumPixels = dWidth;
	int IntPart = sWidth / dWidth;
	int FractPart = sWidth % dWidth;
	int E = 0;
	while (NumPixels-- > 0) 
	{
		*dst++=*src;
		src+=IntPart;

		E+=FractPart;
		if (E>=dWidth) 
		{
			E-=dWidth;
			src++;
		}
	}
	return 1;
}

/*
* scales source pixel line to destination pixel line
*/
static int dg_scale_pixel_line(int bitsPerPixel, unsigned char *dst, int dWidth, unsigned char *src, int sWidth)
{
	switch (bitsPerPixel)
	{
		case 32: return dg_scale_pixel_line_32((unsigned long*)dst, dWidth, (unsigned long*)src, sWidth); break;
		case 24: return dg_scale_pixel_line_24(dst, dWidth, src, sWidth); break;
		case 16: 
		case 15: return dg_scale_pixel_line_16((unsigned short*)dst, dWidth, (unsigned short*)src, sWidth); break;
		case 8:  return dg_scale_pixel_line_8(dst, dWidth, src, sWidth); break;
	}
	return 0;
}

/*
* scales source pixel buffer to destination pixel buffer
*/
int dg_scale_pixel_buffer(int bitsPerPixel, unsigned char* dst, int dstWidth, int dstHeight, unsigned char* src, int srcWidth, int srcHeight)
{
	int IntPart = (srcHeight / dstHeight) * srcWidth;
	int sPitch = (bitsPerPixel * srcWidth) >> 3;
	int dPitch = (bitsPerPixel * dstWidth) >> 3;
	int FractPart = srcHeight % dstHeight;
	unsigned char *PrevSource = 0;
	int NumPixels = dstHeight;
	int E = 0;

	while (NumPixels-- > 0) 
	{
		if (src == PrevSource) 
		{
			dg_blit_copy(bitsPerPixel, dst, dst-dPitch, dstWidth);
		} 
		else 
		{
			dg_scale_pixel_line(bitsPerPixel, dst, dstWidth, src, srcWidth);
			PrevSource = src;
		}
		dst += dPitch;
		src += IntPart;
		E += FractPart;
		if (E >= dstHeight) 
		{
			E -= dstHeight;
			src += sPitch;
		}
	}
	return 1;
}

/*
* scales source pixel line to destination pixel line 
* 16-bit with color key
*/
static int dg_scale_pixel_line_color_key_16(unsigned short *dst, int dWidth, unsigned short *src, int sWidth, unsigned long colorKey)
{
	int NumPixels = dWidth;
	int IntPart = sWidth / dWidth;
	int FractPart = sWidth % dWidth;
	int E = 0;
	while (NumPixels-- > 0) 
	{
		if (*src != (colorKey & 0xFFFF))
		{
			*dst=*src;
		}
		dst++;
		src+=IntPart;

		E+=FractPart;
		if (E>=dWidth) 
		{
			E-=dWidth;
			src++;
		}
	}
	return 1;
}

/*
* scales source pixel line to destination pixel line 
* 24-bit with color key
*/
static int dg_scale_pixel_line_color_key_24(unsigned char *dst, int dWidth, unsigned char *src, int sWidth, unsigned long colorKey)
{
	int NumPixels = dWidth;
	int IntPart = sWidth / dWidth;
	int FractPart = sWidth % dWidth;
	int E = 0;
	while (NumPixels-- > 0) 
	{
		if (*(unsigned short*)src != (colorKey & 0xFFFF) && src[2] != ((colorKey >> 16) & 0xFF))
		{
			*(unsigned short*)dst = *(unsigned short*)src;
			dst[2] = src[2];
		}

		dst+=3;
		src+=3*IntPart;

		E += FractPart;
		if (E >= dWidth) 
		{
			E -= dWidth;
			src+=3;
		}
	}
	return 1;
}

/*
* scales source pixel line to destination pixel line 
* 32-bit with color key
*/
static int dg_scale_pixel_line_color_key_32(unsigned long *dst, int dWidth, unsigned long *src, int sWidth, unsigned long colorKey)
{
	int NumPixels = dWidth;
	int IntPart = sWidth / dWidth;
	int FractPart = sWidth % dWidth;
	int E = 0;
	while (NumPixels-- > 0) 
	{
		if ( (*src & 0xFFFFFF) != (colorKey & 0xFFFFFF) )
		{
			*dst=*src;
		}
		dst++;
		src+=IntPart;

		E+=FractPart;
		if (E>=dWidth) 
		{
			E-=dWidth;
			src++;
		}
	}
	return 1;
}

/*
* scales source pixel line to destination pixel line 
* with color key
*/
static int dg_scale_pixel_line_color_key(int bitsPerPixel, unsigned char *dst, int dWidth, unsigned char *src, int sWidth, unsigned long colorKey)
{
	switch (bitsPerPixel)
	{
		case 32: return dg_scale_pixel_line_color_key_32((unsigned long*)dst, dWidth, (unsigned long*)src, sWidth, colorKey); break;
		case 24: return dg_scale_pixel_line_color_key_24(dst, dWidth, src, sWidth, colorKey); break;
		case 16: 
		case 15: return dg_scale_pixel_line_color_key_16((unsigned short*)dst, dWidth, (unsigned short*)src, sWidth, colorKey); break;
	}
	return 0;
}

/*
* scales source pixel buffer to destination pixel buffer
* with color key
*/
int dg_scale_pixel_buffer_color_key(int bitsPerPixel, unsigned char* dst, int dstWidth, int dstHeight, unsigned char* src, int srcWidth, int srcHeight, unsigned long color_key)
{
	int IntPart = (srcHeight / dstHeight) * srcWidth;
	int sPitch = (bitsPerPixel * srcWidth) >> 3;
	int dPitch = (bitsPerPixel * dstWidth) >> 3;
	int FractPart = srcHeight % dstHeight;
	unsigned char *PrevSource = 0;
	int NumPixels = dstHeight;
	int E = 0;

	while (NumPixels-- > 0) 
	{
		if (src == PrevSource) 
		{
			dg_blit_color_key(bitsPerPixel, dst, dst-dPitch, dstWidth, color_key);
		} 
		else 
		{
			dg_scale_pixel_line_color_key(bitsPerPixel, dst, dstWidth, src, srcWidth, color_key);
			PrevSource = src;
		}
		dst += dPitch;
		src += IntPart;
		E += FractPart;
		if (E >= dstHeight) 
		{
			E -= dstHeight;
			src += sPitch;
		}
	}
	return 1;
}

/*
* blit and scaled source surface rectangle into destination surface rectangle
*/
static int dg_surface_blit_scale(dg_surface_p dSurface, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect)
{
	int result=1;
	dg_video_p device=dSurface->device;
	unsigned char *pSrc, *pDst;
	unsigned long sPitch, dPitch;

	if (sSurface->lock(sSurface, 0))
	{
		pSrc=(unsigned char *)sSurface->pixels;
		sPitch=sSurface->pitch;
		if (dSurface->lock(dSurface, 0))
		{
			int bpp=(7+device->pixel_format.bitsPerPixel)>>3;
			int NumPixels = dstRect->h;
			int IntPart = (srcRect->h / dstRect->h) * srcRect->w * bpp;
			int FractPart = srcRect->h % dstRect->h;
			int E = 0;
			unsigned char *PrevSource = 0;

			pDst=(unsigned char *)dSurface->pixels;
			dPitch=dSurface->pitch;

			pSrc += srcRect->y * sPitch + srcRect->x * bpp;
			pDst += dstRect->y * dPitch + dstRect->x * bpp;

			while (NumPixels-- > 0) 
			{
				if (pSrc == PrevSource) 
				{
					dg_blit_copy(device->pixel_format.bitsPerPixel, pDst, pDst-dPitch, dstRect->w);
				} 
				else 
				{
					dg_scale_pixel_line(device->pixel_format.bitsPerPixel, pDst, dstRect->w, pSrc, srcRect->w);
					PrevSource = pSrc;
				}
				pDst += dPitch;
				pSrc += IntPart;
				E += FractPart;
				if (E >= dstRect->h) 
				{
					E -= dstRect->h;
					pSrc += sPitch;
				}
			}
			dSurface->unlock(dSurface);
		}
		else
		{
			result=0;
		}
		sSurface->unlock(sSurface);
	}
	else
	{
		result=0;
	}
	return result;
}

/* 
* blit and scaled source surface rectangle into destination surface rectangle with color key
*/
static int dg_surface_blit_scale_color_key(dg_surface_p dSurface, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect, unsigned long colorKey)
{
	int result=1;
	dg_video_p device=dSurface->device;
	unsigned char *pSrc, *pDst;
	unsigned long sPitch, dPitch;

	if (sSurface->lock(sSurface, 0))
	{
		pSrc=(unsigned char *)sSurface->pixels;
		sPitch=sSurface->pitch;
		if (dSurface->lock(dSurface, 0))
		{
			int bpp=(7+device->pixel_format.bitsPerPixel)>>3;
			int NumPixels = dstRect->h;
			int IntPart = (srcRect->h / dstRect->h) * srcRect->w * bpp;
			int FractPart = srcRect->h % dstRect->h;
			int E = 0;
			unsigned char *PrevSource = 0;

			pDst=(unsigned char *)dSurface->pixels;
			dPitch=dSurface->pitch;

			pSrc += srcRect->y * sPitch + srcRect->x * bpp;
			pDst += dstRect->y * dPitch + dstRect->x * bpp;

			while (NumPixels-- > 0) 
			{
				if (pSrc == PrevSource) 
				{
					dg_blit_color_key(device->pixel_format.bitsPerPixel, pDst, pDst-dPitch, dstRect->w, colorKey);
				} 
				else 
				{
					dg_scale_pixel_line_color_key(device->pixel_format.bitsPerPixel, pDst, dstRect->w, pSrc, srcRect->w, colorKey);
					PrevSource = pSrc;
				}
				pDst += dPitch;
				pSrc += IntPart;
				E += FractPart;
				if (E >= dstRect->h) 
				{
					E -= dstRect->h;
					pSrc += sPitch;
				}
			}
			dSurface->unlock(dSurface);
		}
		else
		{
			result=0;
		}
		sSurface->unlock(sSurface);
	}
	else
	{
		result=0;
	}
	return result;
}

/* 
* blit surface from a source surface rectangle into destination surface rectangle
*/
int dg_surface_blit(dg_surface_p dSurface, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect, unsigned char alpha, unsigned char* alphaChannel, dg_color_t colorKey, int options)
{
	int result=1;
	dg_surface_p sSurfaceStretched=0;
	dg_rect_p sRectStretched;
	unsigned char *pSrc, *pDst;
	unsigned long sPitch, dPitch;
	dg_video_p device=dSurface->device;
	int isStretched=0;
	unsigned char _alpha=0;
	unsigned char* _alphaChannel=0;
	unsigned long _colorKey=0;
	int isRmSrcRect=0, isRmDstRect=0;

	if (!srcRect)
	{
		srcRect = dg_rect_create(0, 0, sSurface->width, sSurface->height);
		isRmSrcRect = 1;
	}

	if (!dstRect)
	{
		dstRect = dg_rect_create(0, 0, dSurface->width, dSurface->height);
		isRmDstRect = 1;
	}

	isStretched=dstRect->w != srcRect->w || dstRect->h != srcRect->h;

	if (options == DG_OPT_BLIT_DEFAULT)
	{
		options = 0;
		if (sSurface->options & DG_OPT_SURFACE_COLOR_KEY)
		{
			options|=DG_OPT_BLIT_COLOR_KEY;
			_colorKey = device->pack_color(device, sSurface->color_key);
		}
		if (sSurface->options & DG_OPT_SURFACE_ALPHA)
		{
			options|=DG_OPT_BLIT_ALPHA;
			_alpha = sSurface->alpha;
		}
		if (sSurface->options & DG_OPT_SURFACE_ALPHA_CHANNEL)
		{
			options|=DG_OPT_BLIT_ALPHA_CHANNEL;
			_alphaChannel = sSurface->alpha_channel;
		}

		if (!options) 
		{
			options=DG_OPT_BLIT_COPY;
		}
	}
	else
	{
		_alpha=alpha;
		_alphaChannel=alphaChannel;
		_colorKey=device->pack_color(device, colorKey);
	}

	if (isStretched)
	{
		dg_color_t dummy={0};

		if (options & DG_OPT_BLIT_COPY)
		{
			int result;
			if (options & DG_OPT_BLIT_COLOR_KEY)
			{
				result = dg_surface_blit_scale_color_key(dSurface, dstRect, sSurface, srcRect, _colorKey);
			}
			else
			{
				result = dg_surface_blit_scale(dSurface, dstRect, sSurface, srcRect);
			}
			if (isRmSrcRect)
				srcRect->destroy(srcRect);
			if (isRmDstRect)
				dstRect->destroy(dstRect);
			return result;
		}

		/* 
			Alpha blending 
			sSurface and dSurface surface sizes differs, so we are creating a new surface sSurfaceStretched 
			with size matching the dSurface surface and then stretch blit the sSurface to sSurfaceStretched.
		*/
		sRectStretched=dg_rect_create(0, 0, dstRect->w, dstRect->h);
		sSurfaceStretched=device->create_surface(device, sRectStretched->w, sRectStretched->h, 0);
		if (!sSurfaceStretched)
		{
			LOG_IMPORTANT("dg_surface_blit: Unable to create video surface");
			sRectStretched->destroy(sRectStretched);
			if (isRmSrcRect)
				srcRect->destroy(srcRect);
			if (isRmDstRect)
				dstRect->destroy(dstRect);
			return 0;
		}

		/* stretch blit sSurface to sSurfaceStretched */
		if (!sSurfaceStretched->blit(sSurfaceStretched, sRectStretched, sSurface, srcRect, 0, 0, dummy, DG_OPT_BLIT_COPY))
		{
			sSurfaceStretched->destroy(sSurfaceStretched);
			sRectStretched->destroy(sRectStretched);
			if (isRmSrcRect)
				srcRect->destroy(srcRect);
			if (isRmDstRect)
				dstRect->destroy(dstRect);
			return 0;
		}

		/* stretch alpha channel */
		if (sSurface->options & DG_OPT_SURFACE_ALPHA_CHANNEL)
		{
			unsigned char* stretchedAlphaChannel = (unsigned char*)malloc(2*dstRect->w * dstRect->h);
			dg_scale_pixel_buffer(8, stretchedAlphaChannel, dstRect->w, dstRect->h, sSurface->alpha_channel, sSurface->width, sSurface->height);
			sSurfaceStretched->set_alpha_channel(sSurfaceStretched, stretchedAlphaChannel);
			free(stretchedAlphaChannel);
			_alphaChannel=sSurfaceStretched->alpha_channel;
		}

		/* change sSurface with the sSurfaceStretched */
		sSurface=sSurfaceStretched;
		srcRect=sRectStretched;
	}

	assert(srcRect->w == dstRect->w);
	assert(srcRect->h == dstRect->h);

	if (sSurface->lock(sSurface, 0))
	{
		pSrc=(unsigned char *)sSurface->pixels;
		sPitch=sSurface->pitch;
		if (dSurface->lock(dSurface, 0))
		{
			int bpp=(7+device->pixel_format.bitsPerPixel)>>3; /* bytes per pixel */
			int width=dstRect->w;                             /* blitted rectangle width */
			int height=dstRect->h;                            /* blitted rectangle height */
			unsigned char* dstAlphaChannel;                   /* destination alpha channel */

			pDst=(unsigned char *)dSurface->pixels;
			dPitch=dSurface->pitch;

			pSrc += srcRect->y * sPitch + srcRect->x * bpp;
			pDst += dstRect->y * dPitch + dstRect->x * bpp;

			if (options & DG_OPT_BLIT_ALPHA_CHANNEL)
			{
				_alphaChannel += srcRect->y * sSurface->width + srcRect->x;

				if (dSurface->options & DG_OPT_SURFACE_ALPHA_CHANNEL)
				{
					dstAlphaChannel = dSurface->alpha_channel;
					dstAlphaChannel += dstRect->y * dSurface->width + dstRect->x;
				}
			}

			while (height>0)
			{
				if (options & DG_OPT_BLIT_ALPHA_CHANNEL)
				{
					if (dSurface->options & DG_OPT_SURFACE_ALPHA_CHANNEL)
					{
						/* blend destination surface with its attached alpha channel */
						unsigned char* dst = pDst;
						int i;
						for (i=0; i<width; i++)
						{
							unsigned long c=0;
							/* blend dst color */
							switch (bpp)
							{
							case 2: c = *((unsigned short*)dst); break;
							case 3: c = *((unsigned short*)dst);
									c |= ((dst[2])<<16);
									break;
							case 4: c = *((unsigned long*)dst); break;
							}
							c = dg_pixel_unpack(&device->pixel_format, c);
							c = DG_RGB_COLOR( 
								(DG_GET_RED(c) * dstAlphaChannel[i]) >> 8,
								(DG_GET_GREEN(c) * dstAlphaChannel[i]) >> 8,
								(DG_GET_BLUE(c) * dstAlphaChannel[i]) >> 8,
								0);
							c = dg_pixel_pack(&device->pixel_format, c);
							
							switch (device->pixel_format.bitsPerPixel)
							{
							case 2: *((unsigned short*)dst) = (unsigned short)(c & 0xFFFF); break;
							case 3: *((unsigned short*)dst) = (unsigned short)(c & 0xFFFF);
									dst[2] = (unsigned char)((c >> 16) & 0xFF);
									break;
							case 4: *((unsigned long*)dst) = c; break;
							dst+=bpp;
							}
							dstAlphaChannel[i] = _alphaChannel[i];
						}
					}

					if (options & DG_OPT_BLIT_ALPHA)
					{
						if (options & DG_OPT_BLIT_COLOR_KEY)
						{
							/* blit alpha channel + alpha + color key */
							dg_blit_alpha_channel_alpha_color_key(device->pixel_format.bitsPerPixel, pDst, pSrc, width, _alpha, _alphaChannel, _colorKey);
						}
						else
						{
							/* blit alpha channel + alpha */
							dg_blit_alpha_channel_alpha(device->pixel_format.bitsPerPixel, pDst, pSrc, width, _alpha, _alphaChannel);
						}
					}
					else
					{
						if (options & DG_OPT_BLIT_COLOR_KEY)
						{
							/* blit alpha channel + color key */
							dg_blit_alpha_channel_color_key(device->pixel_format.bitsPerPixel, pDst, pSrc, width, _alphaChannel, _colorKey);
						}
						else
						{
							/* blit alpha channel */
							dg_blit_alpha_channel(device->pixel_format.bitsPerPixel, pDst, pSrc, width, _alphaChannel);
						}
					}

					if (dSurface->options & DG_OPT_SURFACE_ALPHA_CHANNEL)
					{
						dstAlphaChannel+=dSurface->width;
					}
					_alphaChannel+=sSurface->width;
				}
				else
				{
					if (options & DG_OPT_BLIT_ALPHA)
					{
						if (options & DG_OPT_BLIT_COLOR_KEY)
						{
							/* blit alpha + color key */
							dg_blit_alpha_color_key(device->pixel_format.bitsPerPixel, pDst, pSrc, width, _alpha, _colorKey);
						}
						else
						{
							/* blit alpha */
							dg_blit_alpha(device->pixel_format.bitsPerPixel, pDst, pSrc, width, _alpha);
						}
					}
					else
					{
						if (options & DG_OPT_BLIT_COLOR_KEY)
						{
							/* blit color key */
							dg_blit_color_key(device->pixel_format.bitsPerPixel, pDst, pSrc, width, _colorKey);
						}
						else
						{
							/* blit */
							dg_blit_copy(device->pixel_format.bitsPerPixel, pDst, pSrc, width);
						}
					}
				}

				pSrc+=sPitch;
				pDst+=dPitch;
				height--;
			}

			dSurface->unlock(dSurface);
		}
		else
		{
			result=0;
		}
		sSurface->unlock(sSurface);
	}
	else
	{
		result=0;
	}

	/* Free the temporary surface if used */
	if (sSurfaceStretched)
	{
		sRectStretched->destroy(sRectStretched);
		sSurfaceStretched->destroy(sSurfaceStretched);
	}
	if (isRmSrcRect)
		srcRect->destroy(srcRect);
	if (isRmDstRect)
		dstRect->destroy(dstRect);
	return result;
}
